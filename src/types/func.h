
#ifndef PYLITE_TYPES_FUNC_H
#define PYLITE_TYPES_FUNC_H

#include "object.h"
#include "../lib/kvec.h"

typedef struct PyLiteFunctionObject {
    PyLiteObject_HEAD;
    kvec_t(PyLiteObject*) const_val;
    kvec_t(size_t) opcodes;
    // locals
    // upvalue
} PyLiteFunctionObject;

#endif
