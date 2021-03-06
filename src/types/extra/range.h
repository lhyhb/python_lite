
#ifndef PYLITE_TYPES_RANGE_H
#define PYLITE_TYPES_RANGE_H

#include "../object.h"

typedef struct PyLiteRangeObject {
    PyLiteObject_HEAD;
    pl_int_t start;
    pl_int_t stop;
    pl_int_t step; // not zero
} PyLiteRangeObject;

PyLiteRangeObject* pylt_obj_range_new(PyLiteInterpreter *I, pl_int_t start, pl_int_t stop, pl_int_t step);
pl_int_t pylt_obj_range_itertimes(PyLiteInterpreter *I, PyLiteRangeObject* range);

void pylt_obj_range_rfree(PyLiteInterpreter *I, PyLiteRangeObject* self);
void pylt_obj_range_free(PyLiteInterpreter *I, PyLiteRangeObject* self);

#endif
