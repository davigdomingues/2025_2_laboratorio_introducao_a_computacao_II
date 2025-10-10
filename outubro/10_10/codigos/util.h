#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

/************************** Hashing com CRC32  *********************************/
// ---------- CRC32 ----------
uint32_t crc32_table[256];

void init_crc32() {
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
    const unsigned char *p = (const unsigned char *)buf;
    crc = crc ^ ~0U;
    while (size--)
        crc = crc32_table[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
    return crc ^ ~0U;
}

/************************** Gerador de números aleatórios  *********************************/
// Gerador Xorshift32
int get_random(int *state, int max) {
    uint32_t x = (uint32_t) *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return (int)((x % max) + 1);
}
