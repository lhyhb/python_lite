
#include "range.h"
#include "../../utils/misc.h"

PyLiteRangeObject* pylt_obj_range_new(PyLiteInterpreter *I, pl_int_t start, pl_int_t stop, pl_int_t step) {
    PyLiteRangeObject *obj = pylt_malloc(I, sizeof(PyLiteRangeObject));
    obj->ob_type = PYLT_OBJ_TYPE_RANGE;
    obj->ob_flags = 0;
    obj->start = start;
    obj->stop = stop;
    obj->step = step;
    return obj;
}

pl_int_t pylt_obj_range_itertimes(PyLiteInterpreter *I, PyLiteRangeObject* range) {
    return max(0, (pl_int_t)ceil((range->stop - range->start) / range->step));
}

void pylt_obj_range_free(PyLiteInterpreter *I, PyLiteRangeObject* self) {
    pylt_free_ex(I, self);
}
