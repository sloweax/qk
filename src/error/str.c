#include "../../include/qk/error.h"
#include <string.h>
#include <errno.h>

QKAPI char *qk_error_str(int e)
{
    switch (e) {
    case QK_ERRNO:     return strerror(errno);
    case QK_OK:        return "OK";
    case QK_INVALID:   return "Invalid argument";
    default:           return "?";
    }
}
