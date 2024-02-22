#include "../../include/qk/buf.h"
#include <fcntl.h>
#include <unistd.h>

QKAPI int qk_buf_read(qk_buf *b, int fd)
{
    char buf[4096];
    ssize_t rd;
    int r;

    while ((rd = read(fd, buf, sizeof(buf)))) {
        if (rd == -1) return QK_ERRNO;
        r = qk_buf_cat(b, buf, rd);
        if (r != QK_OK) return r;
    }

    return QK_OK;
}

QKAPI int qk_buf_read_path(qk_buf *b, const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1) return QK_ERRNO;
    int r = qk_buf_read(b, fd);
    if (close(fd) != 0) return r == QK_OK ? QK_ERRNO : r;
    return r;
}

