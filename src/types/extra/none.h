
#ifndef PYLITE_TYPES_NONE_H
#define PYLITE_TYPES_NONE_H

#include "../object.h"

typedef struct PyLiteNoneObject {
    PyLiteObject_HEAD;
} PyLiteNoneObject;

pl_int_t pylt_obj_none_cmp(PyLiteState *state, PyLiteNoneObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_none_eq(PyLiteState *state, PyLiteNoneObject *self, PyLiteObject *other);
pl_uint32_t pylt_obj_none_hash(PyLiteState *state, PyLiteNoneObject *obj);

PyLiteNoneObject* pylt_obj_none_new(PyLiteState *state);

PyLiteNoneObject PyLiteNone;

#endif
