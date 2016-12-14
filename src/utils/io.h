﻿
#ifndef PYLITE_CORE_IO_H
#define PYLITE_CORE_IO_H

#include "misc.h"

typedef struct {
    int fileno;
    int flags;
    pl_uint_t current;
    pl_uint_t size;
} PyLiteFile;


FILE* pl_io_fopen(PyLiteInterpreter *I, PyLiteStrObject *fn, PyLiteStrObject *mode);
PyLiteFile* pl_io_file_new(PyLiteInterpreter *I, PyLiteStrObject *fn, PyLiteStrObject *mode);
PyLiteFile* pl_io_file_new_with_cfile(PyLiteInterpreter *I, FILE *fp);

void pl_io_init(PyLiteInterpreter *I);

#endif
