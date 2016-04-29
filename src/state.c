﻿
#include "state.h"

void pylt_state_err(PyLiteState *state) {
    exit(-1);
}

void pylt_state_init(PyLiteState *state) {
    kv_init(state->cls_base);
    state->modules = pylt_obj_table_new(state);
    state->cache_str = pylt_obj_set_new(state);
    state->cache_bytes = pylt_obj_set_new(state);
    state->error_code = 0;
}
