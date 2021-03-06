
#ifndef PYLITE_TYPES_NONE_H
#define PYLITE_TYPES_NONE_H

#include "../object.h"

typedef struct PyLiteNoneObject {
    PyLiteObject_HEAD;
} PyLiteNoneObject;

pl_int_t pylt_obj_none_cmp(PyLiteInterpreter *I, PyLiteNoneObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_none_eq(PyLiteInterpreter *I, PyLiteNoneObject *self, PyLiteObject *other);
pl_uint32_t pylt_obj_none_hash(PyLiteInterpreter *I, PyLiteNoneObject *obj);

PyLiteNoneObject* pylt_obj_none_new(PyLiteInterpreter *I);

PyLiteNoneObject PyLiteNone;

#endif
