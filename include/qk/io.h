#pragma once

#include "../qk.h"
#include "alloc.h"
#include <fcntl.h>
#include <sys/types.h>

typedef struct qk_io qk_io;

typedef struct {
    ssize_t (*read)(qk_io *io, void *buf, size_t sz);
    ssize_t (*write)(qk_io *io, const void *buf, size_t sz);
    int (*close)(qk_io *io);
} qk_io_vtable;

struct qk_io {
    qk_io_vtable *vtable;
};

typedef struct {
    qk_io io;
    qk_allocator *allocator;
    const char *path;
    int fd;
    int flags;
    int mode;
} qk_io_file;

// QKAPI int qk_io_file_close(qk_io *io);
QKAPI int qk_io_file_init(qk_io_file *f, const char *path, int flags, int mode);
QKAPI qk_io *qk_io_file_open(qk_allocator *alloc, const char *path, int flags, int mode);
// QKAPI ssize_t qk_io_file_read(qk_io *f, void *buf, size_t sz);
// QKAPI ssize_t qk_io_file_write(qk_io *f, const void *buf, size_t sz);
