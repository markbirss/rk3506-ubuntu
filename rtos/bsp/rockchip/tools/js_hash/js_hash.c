#include <stdint.h>
#include <stddef.h>

uint32_t JS_Hash(uint8_t *buf, size_t len)
{
    uint32_t hash = 0x47C6A7E6;
    for (size_t i = 0; i < len; ++i)
    {
        hash ^= ((hash << 5) + buf[i] + (hash >> 2));
    }
    return hash;
}

