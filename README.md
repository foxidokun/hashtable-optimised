# Исследование хештаблиц

В данной работе была написана хештаблица методом списков и исследованы два ее ключевых атрибута:
хеш-функции и функция поиска.

## Часть 1. Исследование хеш-функций
Хеш-функция имеет сигнатуру `uint64_t func(const char *str)` и считает хеш только C-like строк (с терминирующим нулем).

Для каждой функции построим графики и посчитаем дисперсию длин цепочек, а также время работы хеш-функции.
За время работы будем считать подсчет хеша от всех слов в словаре 10 раз (`common.h::HASH_REPEAT_NUM`), не вставляя элементы в действительности.
Такое измерение было проведено три раза и усреднено для каждого хеша.

#### Константа
Хеш-функция возвращает заранее зафиксированное значение вне зависимости от ввода.
```cpp
uint64_t const_hash(const char *) {
    return 22801337;
}
```

![const_hash.png](images%2Fconst_hash.png.jpg)

Стандартное отклонение: 866

#### Длина строки
Хеш-функция возвращает длину строки.
```cpp
uint64_t strlen_hash(const char *obj) {
    return strlen(obj);
}
```

![strlen_hash.png](images%2Fstrlen_hash.png.jpg)
![strlen_hash_detailed.png](images%2Fstrlen_hash_detailed.png.jpg)
_Второй график является приближением ненулевого участка первого._

Стандартное отклонение: 223

#### Первый символ
Хеш-функция возвращает ASCII код первого символа.
```cpp
uint64_t first_char_hash(const char *obj) {
    return (uint64_t) obj[0];
}
```

![first_char_hash.png](images%2Ffirst_char_hash.png.jpg)
![first_char_hash_detailed.png](images%2Ffirst_char_hash_detailed.png.jpg)
_Второй график является приближением ненулевого участка первого._ 

Стандартное отклонение: 197

#### Сумма букв
Хеш-функция возвращает сумму ASCII кодов всех букв в строке.
```cpp
uint64_t sum_hash(const char *obj) {
    uint64_t hash = 0;
    unsigned char c = 0;

    while ((c = (unsigned char) *obj++)) {
        hash += c;
    }

    return hash;
}
```

![sum_hash.png](images%2Fsum_hash.png)

Стандартное отклонение: 23.3

#### ROR-hash
Хеш-функция суммирует байты строки, выполняя при этом циклический сдвиг вправо после каждого суммирования.
```cpp
uint64_t ror_hash(const char *obj) {
    uint64_t hash = 0;
    unsigned char c = 0;

    while ((c = (unsigned char) *obj++)) {
        hash = ror(hash) + c;
    }

    return hash;
}
```

![ror_hash.png](images%2Fror_hash.png.jpg)

Стандартное отклонение: 8.1 \
Время работы: 7.89 ± 0.08 мс

#### ROL-hash
Хеш-функция суммирует байты строки, выполняя при этом циклический сдвиг влево после каждого суммирования.

```cpp
uint64_t rol_hash(const char *obj) {
    uint64_t hash = 0;
    unsigned char c = 0;

    while ((c = (unsigned char) *obj++)) {
        hash = rol(hash) + c;
    }

    return hash;
}
```

![rol_hash.png](images%2Frol_hash.png.jpg)

Стандартное отклонение: 5.1 \
Время работы: 7.90 ± 0.04 мс

#### GNU-hash
```cpp
uint64_t gnu_hash(const char *obj)
{
    uint64_t hash = 5381;
    unsigned char c = 0;

    while ((c = (unsigned char) *obj++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}
```

![gnu_hash.png](images%2Fgnu_hash.png.jpg)

Стандартное отклонение: 3.6 \
Время работы: 8.390 ± 0.010 мс

#### CRC32-hash
```cpp
uint64_t crc32_hash(const char *obj) {
    unsigned char byte = 0;
    unsigned int crc = 0xFFFFFFFF, mask = 0;

    while ((byte = (unsigned char) *obj++)) {
        crc = crc ^ byte;

        for (int j = 7; j >= 0; j--) {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
    }
    return ~crc;
}
```

![crc32_hash.png](images%2Fcrc32_hash.png.jpg)

Стандартное отклонение: 3.6 \
Время работы: 64.73 ± 0.17 мс

Однако если написать CRC32 хеш с использованием интринсиков, то время работы сократиться до `0.942 ± 0.025 мс`

### Анализ распределений

Ожидаемо, константная хеш-функция имеет самое плохое распределение, по сути превращая хештаблицу в связный список. Хеш-функции
длины строки, первого символа и суммы также имеют плохое распределение, не покрывающее весь диапазон значений из-за ограничений 
естественного языка: малого уникального количества букв и ограниченной длины слова.

rol/ror и прочие хеши не имеют таких ограничений и покрывают весь возможный диапазон значений, что и объясняет, почему их отклонения меньше.

С хеш-функциями rol/ror связана интересная особенность компилятора: хоть код и был написан на C без
использования ассемблера, компилятор даже с уровнем оптимизации `-O1` превратил 
```cpp
uint64_t rol(uint64_t byte) {
    return ((byte << 1)) | (byte >> 63);
}
```
в
```asm
rol(unsigned long):
    mov     rax, rdi
    rol     rax
    ret
```
как видно в Godbolt:
![godbolt-rol.png](images%2Fscreenshots%2Fgodbolt-rol.png)

Однако в хеш-функциях нас интересует не только их распределение, но и скорость работы, так как это очень сильно влияет
на скорость работы хеш-таблицы в целом (это будет показано далее). Построим график отклонения от времени работы для всех
функций, имеющих адекватное время работы.

![time_disturb.png](images%2Ftime_disturb.png)

Как видно, без учета векторной реализации `crc32` самым оптимальным хешем является хеш `gnu`, однако
для более честного измерения ассемблерных оптимизаций далее мы будем использовать `crc32` из-за наличия возможности
аппаратного ускорения.

## Часть 2. Ускорение поиска

### Методика измерений
Тесты проводились на AMD Ryzen 7 4800H с версией компилятора GCC v12.2.1. Для чистоты измерений во время замеров на ноутбуке не было запущено никаких других приложений,
планировщик CPU был выставлен в perfomance (процессор зафиксирован на максимальной частоте) и программе был выдан максимальный приоритет.

Все версии программы собирались с флагами `-O2 -mavx2 -DNDEBUG`. Для версий `v1...v5` размер хештаблицы 
был равен размеру из части 1 (`7607` цепочек, средняя длина цепочки — `10` элементов).

### v1. Примитивная версия
Для начала измерим примитивную версию хештаблицы, где узлами списка являются структуры
```c++
struct node_t {
    char key[32];
    char *value;
    node_t *next;
};
```

Размер ключа в 32 байта обусловлен дальнейшим ходом работы, но теоретически там мог стоять любой размер, лишь бы
он вмещал максимально длинное слово в словаре.

Поиск всех ключей в данной таблице занимает `13.7 ± 0.2 ms`.

### v2. Оптимизируем память
Прежде чем перейти к суровым оптимизациям, заметим, что мы не оптимально используем кеш. Проверить это можно с помощью Cachegring:
![v1cachegrind.png](images%2Fscreenshots%2Fv1cachegrind.png)

Вспомним, что кеш линия имеет размер 64 байта, а значит можно оптимизировать локальность строк, храня две ноды в цепочке вместе
в структуре
```c++
struct alignas(64) double_node_t {
    char key1[32]; //32
    char key2[32]; //32

    char *value1;        //8
    char *value2;        //8
    double_node_t *next; //8
};
```
У нее строго указано выравнивание на 64 байта, чтобы обе строки точно оказались в одной кеш линии.

Результат такой оптимизации: поиск всех ключей занимает `10.72 ± 0.12 ms`, количество промахов кеша сократилось до
`9% (10.2% read + 0.2% write)`

| Версия             | Время           | Абсолютное ускорение | Относительное ускорение |
|--------------------|-----------------|----------------------|-------------------------|
| v1: baseline       | 13.7 ± 0.2 ms   | baseline             | baseline                |
| v2: cache-friendly | 10.72 ± 0.12 ms | 1.28 ± 0.03          | 1.28 ± 0.03             |

### v3. CRC на интринсиках
Посмотрим на граф вызовов функции поиска:
![v2flame.png](images%2Fscreenshots%2Fv2flame.png)

| Функция     | Время работы (%) |
|-------------|------------------|
| crc32 hash  | 78%              |
| strcmp_avx2 | 21%              |


Как легко заметить, crc хеш занимает большую часть времени поиска, а потому начнем оптимизации с него.
Оптимизировать его будем, используя векторные SSE инструкции:

```c++
uint64_t crc32_intrin_hash(const char *obj) {
    uint64_t hash = 0;

    hash = _mm_crc32_u64(hash, *((const uint64_t *)obj + 0));
    hash = _mm_crc32_u64(hash, *((const uint64_t *)obj + 1));
    hash = _mm_crc32_u64(hash, *((const uint64_t *)obj + 2));
    hash = _mm_crc32_u64(hash, *((const uint64_t *)obj + 3));

    return hash;
}
```

Поскольку все ключи выровнены на 32 байта, то obj удовлетворяет требованиям выравнивания для uint64_t.
Заменой хеш-функции на интринсики мы ускорили поиск всех ключей до `8.0 ± 0.3 ms`, таким образом:

| Версия             | Время           | Абсолютное ускорение | Относительное ускорение |
|--------------------|-----------------|----------------------|-------------------------|
| v1: baseline       | 13.7 ± 0.2 ms   | baseline             | baseline                |
| v2: cache-friendly | 10.72 ± 0.12 ms | 1.28 ± 0.03          | 1.28 ± 0.03             |
| v3: Hardware hash  | 8.0 ± 0.3 ms    | 1.71 ± 0.08          | 1.34 ± 0.07             |

Ожидаемо, после такой оптимизации функция расчета хеша практически скрылась с графа вызовов, как видно на графе вызовов
в следующем разделе.

### v4&5. strcmp на ассемблере


![v3flame.png](images%2Fscreenshots%2Fv3flame.png)

| Функция     | Время работы (%) |
|-------------|------------------|
| strcmp_avx2 | 87%              |
| crc32 hash  | 2%               |

Ускорим функцию strcmp, написав ее на ассемблере с учетом известной нам длины ключа (32 байта):
```asm
asm_strcmp_noinline:
       vmovdqa ymm0, YWORD [rdi]
       
       xor     rax, rax
       vptest  ymm0, YWORD [rsi]
       
       seta    al
       vzeroupper ; https://www.agner.org/optimize/calling_conventions.pdf page 14
       ret
```

Это сократило время поиска до `6.16 ± 0.16 ms`. Однако, из-за того что написанная на асме функция находится
в другом файле, компилятор не может ее заинлайнить. А инлайн этой функции должен быть выгоден, так как это короткая функция, вызываемая
в горячем цикле.

Перепишем ее на ассемблерных вставках, добавив атрибут always_inline для гарантированного инлайна:
```c++
static  __attribute__ ((always_inline)) int asm_strcmp_inline(const char str1[KEY_SIZE], const char str2[KEY_SIZE]) {
    int res;

    asm inline (".intel_syntax noprefix\n"
        "        vmovdqa ymm0, YMMWORD PTR [%1]\n"  // Load aligned str1
        "        xor     %0, %0\n"                  // Zero return value
        "\n"
        "        vptest  ymm0, YMMWORD PTR [%2]\n"  // test two strings
        "        seta    %b0\n"                     // set return value to planned
        "\n"
        "        vzeroupper\n"                      // https://www.agner.org/optimize/calling_conventions.pdf page 14
        ".att_syntax prefix\n"
        :  "=&r" (res) : "r" (str1), "r" (str2) : "ymm0", "cc");

    return res;
}
```

Экономия на вызовах функции позволила сократить время поиска до `5.1 ± 0.3 ms`:

| Версия              | Время           | Абсолютное ускорение | Относительное ускорение |
|---------------------|-----------------|----------------------|-------------------------|
| v1: baseline        | 13.7  ± 0.2 ms  | baseline             | baseline                |
| v2: cache-friendly  | 10.72 ± 0.12 ms | 1.28 ± 0.03          | 1.28 ± 0.03             |
| v3: Hardware hash   | 8.0   ± 0.3 ms  | 1.71 ± 0.08          | 1.34 ± 0.07             |
| v4: strcmp noinline | 6.16  ± 0.16 ms | 2.22 ± 0.09          | 1.29 ± 0.08             |
| v5: strcmp inline   | 5.1   ± 0.3 ms  | 2.69 ± 0.20          | 1.21 ± 0.10             |

### v6. Оптимизация функции поиска
Как видно из распределения времени в новой программе, оптимизировать осталось только функцию поиска в списке.
![v5flame.png](images%2Fscreenshots%2Fv5flame.png)

| Функция                                | Время работы (%) |
|----------------------------------------|------------------|
| asm_strcmp_inline [inlined]            | 36.5%            |
| list_find [inlined] (без учета strcmp) | 24.9%            |
| crc32 hash                             | 8.48%            |

Перепишем ее на ассемблере, вручную инлайня strcmp:
```asm
; list_find_asm(double_node_t* node, char const* key):
list_find_asm:
        vmovdqa ymm0, yword  [rsi]              ; Load key to ymm0
        jmp     .compares

.next_step:
        mov     rdi, qword  [rdi + 80]          ; node = node->next
        test    rdi, rdi                        ; if (!node) return nullptr;
        jz      .ret_null
.compares:
        vptest  ymm0, yword  [rdi]              ; Test with key1
        jbe     .ret_val1                       ; Equal => return val1
        mov     rax, qword  [rdi + 72]          ; Load val2
        test    rax, rax                        ; Test if null
        je      .ret_null                       ; Return null if null
        vptest  ymm0, yword  [rdi + 32]         ; Compare with key
        ja      .next_step                      ; Not equal => next step
        jmp .exit
.ret_null:
        xor     eax, eax                        ; Return null
        jmp .exit
.ret_val1:
        mov     rax, qword  [rdi + 64]          ; Return value1
.exit:
        vzeroupper                              ; https://www.agner.org/optimize/calling_conventions.pdf page 14
        ret                                     ; Goodbye
```

Однако такое переписывание не дало никакого выигрыша по времени:

| Версия              | Время           | Абсолютное ускорение | Относительное ускорение |
|---------------------|-----------------|----------------------|-------------------------|
| v1: baseline        | 13.7  ± 0.2 ms  | baseline             | baseline                |
| v2: cache-friendly  | 10.72 ± 0.12 ms | 1.28 ± 0.03          | 1.28 ± 0.03             |
| v3: Hardware hash   | 8.0   ± 0.3 ms  | 1.71 ± 0.08          | 1.34 ± 0.06             |
| v4: strcmp noinline | 6.16  ± 0.16 ms | 2.22 ± 0.09          | 1.29 ± 0.07             |
| v5: strcmp inline   | 5.1   ± 0.2 ms  | 2.69 ± 0.14          | 1.21 ± 0.08             |
| v6: asm find        | 4.9   ± 0.1 ms  | 2.79 ± 0.14          | 1.04 ± 0.08             |

Поскольку ускорение от данной оптимизации в пределах погрешности, решено от нее отказаться из-за минусов, связанных с 
ассемблерными оптимизациями: меньшая читаемость, сложнее сопровождать, непереносимый код.

Поскольку больше не осталось не оптимизированных функций, занимающих > 3% времени, а также поскольку последняя оптимизация
не дала заметного прироста, оптимизация функции поиска считается завершенной.

### Бонус. Подходящий размер хештаблицы

Хоть в условии задачи было специально зафиксировано, что хештаблица должна иметь длинные цепочки,
в реальности в хештаблицах используют заселенность не более 0.75.

Как видно из отчета perf, вычисление хеша и сравнение строк уже не являются основными временными затратами в функции find.

Поскольку мы уже оптимизировали до предела хеш-функцию и strcmp, остается только оптимизировать саму
хештаблицу: сократим длину цепочек. Расширив таблицу так, что средняя длина цепочки теперь `0.75` элемента, получаем
время поиска `2.02 ± 0.08 ms`. Итого

| Версия              | Время           | Абсолютное ускорение | Относительное ускорение |
|---------------------|-----------------|----------------------|-------------------------|
| v1: baseline        | 13.7  ± 0.2 ms  | baseline             | baseline                |
| v2: cache-friendly  | 10.72 ± 0.12 ms | 1.28 ± 0.03          | 1.28 ± 0.03             |
| v3: Hardware hash   | 8.0   ± 0.3 ms  | 1.71 ± 0.08          | 1.34 ± 0.06             |
| v4: strcmp noinline | 6.16  ± 0.16 ms | 2.22 ± 0.09          | 1.29 ± 0.07             |
| v5: strcmp inline   | 5.1   ± 0.2 ms  | 2.69 ± 0.14          | 1.21 ± 0.08             |
| bonus: low density  | 2.02  ± 0.08 ms | 6.8 ± 0.4            | 2.52 ± 0.25             |

### Конец оптимизаций
Дальнейшие оптимизации будут неэффективны, так как все основные составляющие функции поиска уже
оптимизированы максимально эффективными способами. Дальнейшее увеличение размера хештаблицы смысла не имеет, так как 
и при текущем размере цепочек длиннее 2х элементов не бывает.
![img.png](images%2Fscreenshots%2Fv6perf.png)

### Замечания
Если собрать финальную версию программы без оптимизаций `v4` и `v5` (без собственной реализации strcmp) с флагом `-flto`, то можно 
заметить отсутствие strcmp в списке функций perf'a. Это означает, что эта функция была заинлайнена. С учетом того, что 
компилятор и так использовал avx2-реализацию strcmp, это по сути эквивалентно проделанным мной оптимизациям.

### Выводы
Благодаря грамотному подходу к оптимизациям, исходная функция поэтапно была ускорена в 6.8 раз. Однако при этом стоит понимать,
что в реальном мире оптимизации v4 и v5 достижимы флагами компилятора, так как `-flto` инлайнит стандартную оптимизированную функцию,
а оптимизации v3 и v6 возможны только со стороны программиста, отчего важно понимать, когда, как и где стоит оптимизировать.