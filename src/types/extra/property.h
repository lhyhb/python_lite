
#ifndef PYLITE_TYPES_PROPERTY_H
#define PYLITE_TYPES_PROPERTY_H

#include "../object.h"
#include "function.h"

typedef struct PyLitePropertyObject {
    PyLiteObject_HEAD;
    union {
        PyLiteCFunctionObject *cfunc;
        PyLiteFunctionObject *pyfunc;
        PyLiteObject *func;
    } fget;
    union {
        PyLiteCFunctionObject *cfunc;
        PyLiteFunctionObject *pyfunc;
        PyLiteObject *func;
    } fset;
} PyLitePropertyObject;

PyLitePropertyObject* pylt_obj_property_new(PyLiteState *state, PyLiteObject *fget, PyLiteObject *fset);
void pylt_obj_property_free(PyLiteState *state, PyLitePropertyObject *self);

#endif
