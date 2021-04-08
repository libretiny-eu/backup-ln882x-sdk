
#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>

uint32_t GetCrc32(uint8_t *ptr, int len);

typedef struct {
    uint32_t crc;
} crc32_ctx_t;

void crc32_init(crc32_ctx_t* ctx);
void crc32_update(crc32_ctx_t* ctx, uint8_t* data, uint32_t len);
uint32_t crc32_final(crc32_ctx_t* ctx);



#endif


