
#ifndef PYLITE_TYPES_RANGE_H
#define PYLITE_TYPES_RANGE_H

#include "../object.h"

typedef struct PyLiteRangeObject {
    PyLiteObject_HEAD;
    pl_int_t start;
    pl_int_t stop;
    pl_int_t step; // not zero
} PyLiteRangeObject;

PyLiteRangeObject* pylt_obj_range_new(PyLiteState *state, pl_int_t start, pl_int_t stop, pl_int_t step);

#endif
