﻿
#include "vm.h"
#include "state.h"
#include "debug.h"
#include "lib/kvec.h"
#include "types/all.h"
#include "mods/builtin.h"

const char* op_vals[] = {
    "or",
    "and",
    "not",
    "in", "not in", "is", "is not", "<", "<=", ">", ">=", "!=", "==",
    "|",
    "^",
    "&",
    "<<", ">>",
    "+", "-",
    "*", "@", "/", "//", "%",
    "+x", "-x", "~x",
    "**",
};

const char* pylt_vm_get_op_name(int op) {
    return op_vals[op];
}

int token_to_op_val(uint32_t tk) {
    switch (tk) {
        case TK_KW_OR: return OP_OR;
        case TK_KW_AND: return OP_AND;
        case TK_KW_NOT: return OP_NOT;

        case TK_KW_IN: return OP_IN; case TK_KW_IS: return OP_IS; case '<': return OP_LT; case TK_OP_LE: return OP_LE; 
        case '>': return OP_GT; case TK_OP_GE: return OP_GE; case TK_OP_NE: return OP_NE; case TK_OP_EQ: return OP_EQ;

        case '|': return OP_BITOR; case '^': return OP_BITXOR; case '&': return OP_BITAND;
        case TK_OP_LSHIFT: OP_LSHIFT; case TK_OP_RSHIFT: return OP_RSHIFT;
        case '+': return OP_PLUS; case '-': return OP_MINUS;
        case '*': return OP_MUL; case '@': return OP_MATMUL; case '/': return OP_DIV; case TK_OP_FLOORDIV: return OP_FLOORDIV; case '%': return OP_MOD;
        /*case '+': return OP_POS; case '-': return OP_NEG;*/  case '~': return OP_BITNOT;
        case TK_OP_POW: return OP_POW;
    }
    return 0;
}

int token_de_to_op_val(uint32_t tk) {
    switch (tk) {
        case TK_DE_PLUS_EQ: return OP_PLUS;
        case TK_DE_MINUS_EQ: return OP_MINUS;
        case TK_DE_MUL_EQ: return OP_MUL;
        case TK_DE_DIV_EQ: return OP_DIV;
        case TK_DE_FLOORDIV_EQ: return OP_FLOORDIV;
        case TK_DE_MOD_EQ: return OP_MOD;
        case TK_DE_MATMUL_EQ: return OP_MATMUL;
        case TK_DE_BITAND_EQ: return OP_BITAND;
        case TK_DE_BITOR_EQ: return OP_BITOR;
        case TK_DE_BITXOR_EQ: return OP_BITXOR;
        case TK_DE_RSHIFT_EQ: return OP_RSHIFT;
        case TK_DE_LSHIFT_EQ: return OP_LSHIFT;
        case TK_DE_POW_EQ: return OP_POW;
    }
    return 0;
}

void pylt_vm_init(struct PyLiteState *state, PyLiteVM* vm) {
    PyLiteFrame *frame;
    PyLiteTable *locals;
    PyLiteModuleObject *mod;

    kv_init(vm->stack);
    kv_init(vm->frames);

    // first frame
    kv_pushp(PyLiteFrame, vm->frames);
    frame = &kv_A(vm->frames, 0);
    frame->func = NULL;
    kv_init(frame->var_tables);

    // built-in
    kv_push(PyLiteTable*, frame->var_tables, pylt_obj_table_new(state));
    locals = kv_top(frame->var_tables);
    mod = pylt_mods_builtins_register(state);

    pl_int_t k = kho_begin(mod->attrs);
    while (k != kho_end(mod->attrs)) {
        if (kho_exist(mod->attrs, k)) {
            pylt_obj_table_set(locals, kho_key(mod->attrs, k), kho_val(mod->attrs, k));
        }
        ++k;
    }

    // first local
    kv_push(PyLiteTable*, frame->var_tables, pylt_obj_table_new(state));
}

void pylt_vm_call_func(PyLiteState* state, PyLiteFunctionObject *func) {
    PyLiteFrame *frame;
    PyLiteVM *vm = &state->vm;
    int index = kv_size(vm->frames);

    kv_pushp(PyLiteFrame, vm->frames);
    frame = &kv_A(vm->frames, index);
    frame->func = func;
    frame->code = &func->code;
    kv_init(frame->var_tables);

    if (index) {
        kv_copy1(PyLiteTable*, frame->var_tables, kv_A(vm->frames, index - 1).var_tables);
    }

    kv_push(PyLiteTable*, frame->var_tables, pylt_obj_table_new(state));
}


void pylt_vm_load_code(PyLiteState* state, PyLiteCodeSnippetObject *code) {
    PyLiteVM *vm = &state->vm;
    PyLiteFrame *frame = &kv_top(vm->frames);
    frame->func = NULL;
    frame->code = code;
}

// 函数调用检查
//  1 为当前的参数个数小于最少参数个数
//  2 为当前的参数个数多于最少参数个数
// -1 当前对象并非函数类型
int func_call_check(PyLiteObject *obj, int params_num, int *pnum) {
    int params_num_min, params_num_max;

    if (obj->ob_type == PYLT_OBJ_TYPE_FUNCTION) {
        params_num_min = castfunc(obj)->minimal;
        params_num_max = castfunc(obj)->length;
    } else if (obj->ob_type == PYLT_OBJ_TYPE_CFUNCTION) {
        params_num_min = castcfunc(obj)->minimal;
        params_num_max = castcfunc(obj)->length;
    } else return -1;

    if (params_num < params_num_min) {
        if (pnum) *pnum = params_num_min; 
        return 1; 
    }

    if (params_num > params_num_max) {
        if (pnum) *pnum = params_num_max;
        return 2;
    }

    return 0;
}

void pylt_vm_run(PyLiteState* state, PyLiteCodeSnippetObject *code) {
    PyLiteObject *a, *b, *c, *ret;
    PyLiteVM *vm = &state->vm;
    PyLiteTable *locals;
    uintptr_t op;
    uintptr_t tmp;
    int num;

    if (!code) return;
    pylt_vm_load_code(state, code);
    locals = kv_top(kv_top(vm->frames).var_tables);

    //for (unsigned int i = 0; i < kv_size(code->opcodes); i++) {
    unsigned int i = 0;
    while (true) {
        switch (kv_A(code->opcodes, i)) {
            case BC_LOADCONST:
                //printf("   %-15s %d\n", "LOADCONST", );
                //printf("%d\n", kv_A(code->opcodes, ++i));
                kv_push(uintptr_t, state->vm.stack, (uintptr_t)kv_A(code->const_val, kv_A(code->opcodes, ++i)-1));
                break;
            case BC_SET_VAL:
                pylt_obj_table_set(locals, castobj(kv_A(code->opcodes, ++i)), castobj(kv_top(state->vm.stack)));
                break; 
            case BC_LOAD_VAL:
                a = castobj(kv_A(code->opcodes, ++i));
                ret = pylt_obj_table_get(locals, a);

                if (!ret) {
                    for (int j = kv_size(kv_top(vm->frames).var_tables) - 2; j >= 0; --j) {
                        ret = pylt_obj_table_get(kv_A(kv_top(vm->frames).var_tables, j), a);
                        if (ret) break;
                    }
                }

                if (!ret) {
                    printf("NameError: name '");
                    debug_print_obj(castobj(kv_A(code->opcodes, i)));
                    printf("' is not defined\n");
                    return;
                }
                kv_push(uintptr_t, state->vm.stack, (uintptr_t)ret);
                break;
            case BC_OPERATOR:
                //printf("   %-15s %s\n", "OPERATOR", get_op_name(kv_A(code->opcodes, ++i)));
                op = kv_A(code->opcodes, ++i);
                switch (op) {
                    case OP_LT: case OP_LE: case OP_GT: case OP_GE: case OP_NE: case OP_EQ:
                    case OP_BITOR: case OP_BITXOR: case OP_BITAND: case OP_LSHIFT: case OP_RSHIFT:
                    case OP_PLUS: case OP_MINUS: case OP_MUL : case OP_MATMUL: case OP_DIV: case OP_FLOORDIV: case OP_MOD: case OP_POW :
                        b = castobj(kv_pop(state->vm.stack));
                        a = castobj(kv_pop(state->vm.stack));
                        ret = pylt_obj_op_binary(state, op, a, b);
                        if (!ret) {
                            printf("TypeError: unsupported operand type(s) for %s: '%s' and '%s'\n", pylt_vm_get_op_name(op), pylt_obj_type_name_cstr(state, a), pylt_obj_type_name_cstr(state, b));
                            return;
                        }
                        kv_push(uintptr_t, state->vm.stack, (uintptr_t)ret);
                        break;
                    default:
                        a = castobj(kv_pop(state->vm.stack));
                        ret = pylt_obj_op_unary(state, op, a);
                        if (!ret) {
                            printf("TypeError: bad operand type for unary %s: '%s'\n", pylt_vm_get_op_name(op), pylt_obj_type_name_cstr(state, a));
                            return;
                        }
                        kv_push(uintptr_t, state->vm.stack, (uintptr_t)ret);
                        break;
                }
                break;
            case BC_NEW_OBJ:
                switch (kv_A(code->opcodes, ++i)) {
                    case PYLT_OBJ_TYPE_ITER:
                        ret = castobj(pylt_obj_iter_new(state, castobj(kv_pop(state->vm.stack))));
                        kv_push(uintptr_t, state->vm.stack, (uintptr_t)ret);
                        break;
                    case PYLT_OBJ_TYPE_FUNCTION:
                        c = castobj(kv_pop(state->vm.stack)); // params
                        b = castobj(kv_pop(state->vm.stack)); // code
                        a = castobj(kv_pop(state->vm.stack)); // name
                        ret = castobj(pylt_obj_func_new(state));
                        
                        memcpy(&castfunc(ret)->code, b, sizeof(PyLiteCodeSnippetObject));

                        castfunc(ret)->length = castlist(c)->ob_size;
                        castfunc(ret)->minimal = castlist(c)->ob_size;
                        castfunc(ret)->names = castlist(c)->ob_val;
                        castfunc(ret)->defaults = NULL;
                        castfunc(ret)->doc = NULL;

                        pylt_obj_table_set(locals, a, ret);
                        break;
                }
                break;
            case BC_NEW_OBJ_EXTRA:
                switch (kv_A(code->opcodes, ++i)) {
                    case PYLT_OBJ_TYPE_SET:
                        ret = castobj(pylt_obj_set_new(state));
                        tmp = kv_A(code->opcodes, ++i);
                        for (unsigned int j = 0; j < tmp; j++) {
                            pylt_obj_set_add(state, castset(ret), castobj(kv_pop(state->vm.stack)));
                        }
                        kv_push(uintptr_t, state->vm.stack, (uintptr_t)ret);
                        break;
                    case PYLT_OBJ_TYPE_LIST:
                        tmp = kv_A(code->opcodes, ++i);
                        ret = castobj(pylt_obj_list_new_with_size(state, tmp));
                        for (unsigned int j = tmp; j > 0; --j) {
                            pylt_obj_list_append(state, castlist(ret), castobj(kv_A(state->vm.stack, kv_size(state->vm.stack) - j)));
                        }
                        kv_popn(state->vm.stack, tmp);
                        kv_push(uintptr_t, state->vm.stack, (uintptr_t)ret);
                        break;
                    case PYLT_OBJ_TYPE_DICT:
                        ++i;
                        ret = castobj(pylt_obj_dict_new(state));
                        break;
                } 
                break;
            case BC_CALL:
                tmp = kv_A(code->opcodes, ++i); // 参数个数
                a = castobj(kv_A(code->opcodes, ++i)); // 函数名

                for (int j = kv_size(kv_top(vm->frames).var_tables) - 1; j >= 0; --j) {
                    ret = pylt_obj_table_get(kv_A(kv_top(vm->frames).var_tables, j), a); // 函数对象
                    if (ret) {
                        // check
                        switch (func_call_check(ret, tmp, &num)) {
                            case 1:
                                printf("TypeError: ");
                                debug_print_obj(a);
                                printf("() missing %d required positional argument\n", num - tmp);
                                return;
                            case 2:
                                printf("TypeError: ");
                                debug_print_obj(a);
                                printf("() takes %d positional arguments but %d were given\n", num, tmp);
                                return;
                        }

                        // set locals
                        if (ret->ob_type == PYLT_OBJ_TYPE_FUNCTION) {
                            kv_top(vm->frames).code_pointer_slot = i;
                            pylt_vm_call_func(state, castfunc(ret));
                            code = kv_top(vm->frames).code;
                            locals = kv_top(kv_top(vm->frames).var_tables);
                            if (castfunc(ret)->length > 0) {
                                for (int k = tmp - 1; k >= 0; --k) {
                                    pylt_obj_table_set(locals, castfunc(ret)->names[k], castobj(kv_pop(state->vm.stack)));
                                }
                            }
                            i = -1;
                        } else if (ret->ob_type == PYLT_OBJ_TYPE_CFUNCTION) {
                            ret = castcfunc(ret)->func(state, tmp, (PyLiteObject**)(&kv_topn(vm->stack, tmp-1)));
                            kv_popn(vm->stack, tmp);
                            kv_push(uintptr_t, state->vm.stack, (uintptr_t)pylt_obj_none_new(state));
                        }
                        break;
                    }
                }
                break;
            case BC_RET:
                kv_pop(vm->frames);
                code = kv_top(vm->frames).code;
                locals = kv_top(kv_top(vm->frames).var_tables);
                i = kv_top(vm->frames).code_pointer_slot;
                //kv_push(uintptr_t, state->vm.stack, (uintptr_t)pylt_obj_int_new(state, 111));
                break;
            case BC_TEST:
                //debug_print_obj(castobj(kv_top(state->vm.stack)));
                if (!pylt_obj_cistrue(state, castobj(kv_pop(state->vm.stack)))) {
                    i += kv_A(code->opcodes, ++i);
                } else ++i;
                break;
            case BC_JMP:
                i += kv_A(code->opcodes, ++i);
                break;
            case BC_JMP_BACK:
                i -= kv_A(code->opcodes, ++i);
                break;
            case BC_FORITER:
                a = pylt_obj_iter_next(state, castiter(kv_top(state->vm.stack)));
                if (a == NULL) i += kv_A(code->opcodes, ++i) - 1;
                else {
                    ++i;
                    kv_push(uintptr_t, state->vm.stack, (uintptr_t)a);
                }
                break;
            case BC_DEL_FORCE:
                pylt_free(castobj(kv_pop(state->vm.stack)));
                break;
            case BC_POP:
                kv_pop(state->vm.stack);
                break;
            case BC_GET_ITEM:
                b = castobj(kv_pop(state->vm.stack));
                a = castobj(kv_pop(state->vm.stack));
                ret = pylt_obj_getitem(state, a, b);
                if (!ret) {
                    printf("KeyError: ");
                    debug_print_obj(b);
                    putchar('\n');
                    return;
                } else {
                    kv_push(uintptr_t, state->vm.stack, (uintptr_t)ret);
                }
                break;
            case BC_SET_ITEM:
                ret = castobj(kv_pop(state->vm.stack));
                b = castobj(kv_pop(state->vm.stack));
                a = castobj(kv_pop(state->vm.stack));
                if (!pylt_obj_setitem(state, ret, a, b)) {
                    printf("IndexError: list assignment index out of range\n");
                    return;
                }
                break;
            case BC_PRINT:
                if (kv_size(state->vm.stack) != 0) {
                    debug_print_obj(castobj(kv_top(state->vm.stack)));
                    putchar('\n');
                }
                printf("locals: {");
                for (khiter_t it = kho_begin(locals); it < kho_end(locals); ++it) {
                    if (!kho_exist(locals, it)) continue;
                    debug_print_obj(kho_key(locals, it));
                    printf(": ");
                    debug_print_obj(kho_value(locals, it));
                    printf(", ");
                }
                printf("}");
                putchar('\n');
            case BC_HALT:
                goto _end;
                break;
        }
        i++;
    }
_end:;
}
