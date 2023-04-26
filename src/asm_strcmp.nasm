global asm_strcmp_noinline

segment .text

asm_strcmp_noinline:
        vmovdqa ymm0, YWORD [rdi]

        xor     rax, rax
        vptest  ymm0, YWORD [rsi]
        seta    al

        vzeroupper ; https://www.agner.org/optimize/calling_conventions.pdf page 14
        ret
