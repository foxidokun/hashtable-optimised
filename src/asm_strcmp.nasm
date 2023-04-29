global asm_strcmp_noinline
global list_find_asm

segment .text

asm_strcmp_noinline:
        vmovdqa ymm0, YWORD [rdi]

        xor     rax, rax
        vptest  ymm0, YWORD [rsi]
        seta    al

        vzeroupper ; https://www.agner.org/optimize/calling_conventions.pdf page 14
        ret

; list_find_asm(double_node_t*, char const*):
list_find_asm:
        vmovdqa ymm0, yword  [rsi] ; Load key to ymm0
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
        vzeroupper                              ; VEX meme
        ret                                     ; Goodbye