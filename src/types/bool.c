﻿
#include "bool.h"
#include "number.h"

PyLiteBoolObject PyLiteTrue = { PYLT_OBJ_TYPE_BOOL, -1 };
PyLiteBoolObject PyLiteFalse = { PYLT_OBJ_TYPE_BOOL, 0 };

uint32_t pylt_obj_bool_hash(PyLiteBoolObject *obj) {
    return obj->ob_val;
}

uint32_t pylt_obj_bool_eq(PyLiteBoolObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
    case PYLT_OBJ_TYPE_INT:
        return self->ob_val == castint(other)->ob_val;
    case PYLT_OBJ_TYPE_FLOAT:
        return self->ob_val == castfloat(other)->ob_val;
    case PYLT_OBJ_TYPE_BOOL:
        return self->ob_val == castbool(other)->ob_val;
    default:
        return 0;
    }
}

uint32_t pylt_obj_bool_cmp(PyLiteBoolObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
    case PYLT_OBJ_TYPE_INT:
        if (self->ob_val < castint(other)->ob_val) return -1;
        if (self->ob_val > castint(other)->ob_val) return 1;
        else return 0;
    case PYLT_OBJ_TYPE_FLOAT:
        if (self->ob_val < castfloat(other)->ob_val) return -1;
        if (self->ob_val > castfloat(other)->ob_val) return 1;
        else return 0;
    case PYLT_OBJ_TYPE_BOOL:
        if (self->ob_val < castbool(other)->ob_val) return -1;
        if (self->ob_val > castbool(other)->ob_val) return 1;
        else return 0;
    default:
        return -2;
    }
}

PyLiteBoolObject* pylt_obj_bool_new(bool val) {
    return (val == false) ? &PyLiteTrue : &PyLiteFalse;
}
