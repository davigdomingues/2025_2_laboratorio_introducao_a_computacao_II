#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

/*
    util.h
    - Rotinas auxiliares compartilhadas: CRC32 (hash) e gerador pseudoaleatório xorshift32.
    - Fornece funções usadas pelo programa principal para verificação (hash) e geração de entradas.
*/

/************************** Hashing com CRC32  *********************************/
// ---------- CRC32 ----------
// Tabela de 256 entradas para o CRC-32 (polinômio padrão 0xEDB88320, refletido)
uint32_t crc32_table[256];

void init_crc32() {
    // Pré-computa a tabela do CRC-32 para acelerar o processamento byte a byte.
    uint32_t poly = 0xEDB88320;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (int j = 0; j < 8; j++) {
            if (c & 1)
                c = poly ^ (c >> 1);
            else
                c >>= 1;
        }
        crc32_table[i] = c;
    }
}

uint32_t crc32(uint32_t crc, const void *buf, size_t size) {
    // Calcula o CRC-32 (refletido) do buffer 'buf' com tamanho 'size' bytes.
    // 'crc' permite encadear chamadas (iniciar com 0 para hash simples).
    const unsigned char *p = (const unsigned char *)buf;
    crc = crc ^ ~0U;
    while (size--)
        crc = crc32_table[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
    return crc ^ ~0U;
}

/************************** Gerador de números aleatórios  *********************************/
// Gerador Xorshift32
int get_random(int *state, int max) {
    // Gera valores pseudoaleatórios em [1..max] usando xorshift32.
    // Pré-condição: 'max' > 0 para evitar divisão/modulo por zero.
    // Observação: há viés modular quando 2^32 não é múltiplo de 'max', aceitável na maioria dos usos simples.
    uint32_t x = (uint32_t) *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x; // atualiza o estado externo (reentrante via ponteiro)
    return (int)((x % max) + 1);
}
