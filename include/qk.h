#pragma once

#ifndef QK_MALLOC
#define QK_MALLOC(sz) malloc(sz)
#endif
#ifndef QK_FREE
#define QK_FREE(p)    free(p)
#endif
#ifndef QK_REALLOC
#define QK_REALLOC(p, sz)    realloc(p, sz)
#endif

#ifndef QKAPI
#define QKAPI extern
#endif
