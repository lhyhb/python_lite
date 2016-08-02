﻿
#include "extra.h"
#include "../types/all.h"

PyLiteObject* pylt_cls_method_function_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
	return NULL;
}

PyLiteObject* pylt_prop_function_parameters_get(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
	PyLiteFunctionInfo *info = pylt_obj_func_get_info(I, args[0]);
	return castobj(info->params);
}

PyLiteObject* pylt_prop_function_defaults_get(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
	PyLiteFunctionInfo *info = pylt_obj_func_get_info(I, args[0]);
	return castobj(info->defaults);
}

PyLiteObject* pylt_cls_method_range_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pl_int_t start, end;
    if (castnone(args[2]) == &PyLiteNone) {
        start = 0;
        end = dcast(int, args[1])->ob_val;
    } else {
        start = dcast(int, args[1])->ob_val;
        end = dcast(int, args[2])->ob_val;
    }

    return pylt_obj_typecast(
        I,
        dcast(type, args[0]),
        castobj(pylt_obj_range_new(I, start, end, dcast(int, args[3])->ob_val))
    );
}

PyLiteObject* pylt_cls_method_base_exception_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return pylt_obj_typecast(
        I, 
        dcast(type, args[0]),
        castobj(pylt_obj_exception_new(I, dcast(tuple, args[1])))
    );
}

PyLiteObject* pylt_prop_base_exception_args_get(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(dcast(except, args[1])->args);
}
