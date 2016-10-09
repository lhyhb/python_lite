
#ifndef PYLITE_WATCH_H
#define PYLITE_WATCH_H

#include "../intp.h"
#include "../types/object.h"

void pl_obj_watch(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject *func);
void pl_obj_unwatch(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject *func);
void pl_obj_unwatch_all(PyLiteInterpreter *I, PyLiteObject *obj);

#endif