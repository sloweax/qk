#include "../../include/qk/error.h"
#include <string.h>
#include <errno.h>

QKAPI char *qk_str_error(int e)
{
    switch (e) {
    case QK_ERRNO:         return strerror(errno);
    case QK_OK:            return "OK";
    case QK_ERR_INVALID:   return "Invalid argument";
    case QK_ERR_OVERFLOW:  return "Overflow";
    case QK_ERR_UNDERFLOW: return "Underflow";
    default:               return "?";
    }
}
