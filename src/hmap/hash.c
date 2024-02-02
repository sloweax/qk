#include "../../include/qk.h"
#include "../../include/qk/hmap.h"
#include <string.h>

QKAPI size_t qk_hmap_hash_str(const void* s)
{
    const unsigned char *str = s;
    size_t hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

QKAPI int qk_hmap_cmp_str(const void *a, const void *b)
{
    return strcmp(a, b);
}
