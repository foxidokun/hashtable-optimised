global crc32_intrin_hash

segment .text

; uint64_t crc32_intrin_hash(const char obj[32])
crc32_intrin_hash:
    xor rax, rax
    crc32 rax, QWORD [rdi]
    crc32 rax, QWORD [rdi+8]
    crc32 rax, QWORD [rdi+16]
    crc32 rax, QWORD [rdi+24]
    ret

asm_strcmp_noinline:
        vmovdqa ymm0, YWORD [rdi]

        xor     rax, rax
        vptest  ymm0, YWORD [rsi]
        seta    al

        vzeroupper ; https://www.agner.org/optimize/calling_conventions.pdf page 14
        ret
