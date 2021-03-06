
#ifndef PYLITE_TYPES_TUPLE_H
#define PYLITE_TYPES_TUPLE_H

#include "../object.h"

typedef struct PyLiteTupleObject {
    PyLiteObject_HEAD;
    pl_int_t ob_size;
    PyLiteObject **ob_val;
} PyLiteTupleObject;

pl_bool_t pylt_obj_tuple_eq(PyLiteInterpreter *I, PyLiteTupleObject *self, PyLiteObject *other);

struct PyLiteStrObject* pylt_obj_tuple_to_str(PyLiteInterpreter *I, PyLiteTupleObject *self);

PyLiteObject* pylt_obj_tuple_getitem(PyLiteInterpreter *I, PyLiteTupleObject *self, int index);
PyLiteTupleObject* pylt_obj_tuple_slice(PyLiteInterpreter *I, PyLiteTupleObject *self, pl_int_t *pstart, pl_int_t *pend, pl_int_t step);

PyLiteTupleObject* pylt_obj_tuple_new(PyLiteInterpreter *I, pl_int_t size);
PyLiteTupleObject* pylt_obj_tuple_new_with_data(PyLiteInterpreter *I, pl_int_t len, void *data);

void pylt_obj_tuple_rfree(PyLiteInterpreter *I, PyLiteTupleObject *self);
void pylt_obj_tuple_free(PyLiteInterpreter *I, PyLiteTupleObject *self);

#endif
