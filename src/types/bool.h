
#ifndef PYLITE_TYPES_BOOL_H
#define PYLITE_TYPES_BOOL_H

#include "object.h"

typedef struct PyLiteBoolObject {
    PyLiteObject_HEAD;
    bool ob_val;
} PyLiteBoolObject;

uint32_t pylt_obj_bool_hash(PyLiteBoolObject *obj);
uint32_t pylt_obj_bool_eq(PyLiteBoolObject *self, PyLiteObject *other);
uint32_t pylt_obj_bool_cmp(PyLiteBoolObject *self, PyLiteObject *other);

#endif
