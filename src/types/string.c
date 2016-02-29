﻿
#include "bool.h"
#include "string.h"
#include "../state.h"

pl_int_t pylt_obj_str_ccmp(PyLiteState *state, PyLiteStrObject *self, PyLiteObject *other) {
    return false;
}

pl_bool_t pylt_obj_str_ceq(PyLiteState *state, PyLiteStrObject *self, PyLiteObject *other) {
    unsigned int i;
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_STR:
            if (self->ob_hash != caststr(other)->ob_hash)
                return false;
            if (self->ob_size != caststr(other)->ob_size)
                return false;
            for (i = 0; i < self->ob_size; i++) {
                if (self->ob_val[i] != caststr(other)->ob_val[i]) return false;
            }
            return true;
        default:
            return false;
    }
}

pl_uint32_t pylt_obj_str_chash(PyLiteState *state, PyLiteStrObject *obj) {
    return caststr(obj)->ob_hash;
}

// BKDR Hash
pl_uint32_t pylt_obj_str_cforcehash(PyLiteState *state, PyLiteStrObject *obj) {
    register size_t hash = 0;
    for (unsigned int i = 0; i < obj->ob_size; i++) {
        hash = hash * 131 + obj->ob_val[i];
    }
    return (hash & 0x7FFFFFFF);
}

#define _isnumber(c) (c >= '0' && c <= '9')
#define _ishex(c) (_isnumber(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))

_INLINE static uint8_t _hex(uint32_t code) {
    if (code >= '0' && code <= '9') return code - '0';
    else if (code >= 'A' && code <= 'F') return code - 'A' + 10;
    else if (code >= 'a' && code <= 'f') return code - 'a' + 10;
    return 255;
}

_INLINE static uint8_t _oct(uint32_t code) {
    if (code >= '0' && code <= '7') return code - '0';
    return 255;
}

_INLINE static
int _read_x_int(uint32_t *p, int *pnum, uint8_t(*func)(uint32_t code), int max_size) {
    uint32_t *e = p + max_size;
    int ret = 0, num = 0, val = (int)pow(10, e - p - 1);

    do {
        ret += (*func)(*p++) * val;
        val /= 10;
        num++;
    } while (p != e);

    *pnum = num;
    return ret;
}


PyLiteStrObject* pylt_obj_str_new(PyLiteState *state, uint32_t *str, int size, bool is_raw) {
    PyLiteStrObject *obj = pylt_realloc(NULL, sizeof(PyLiteStrObject)), *obj2;
    obj->ob_type = PYLT_OBJ_TYPE_STR;
    obj->ob_val = pylt_realloc(NULL, sizeof(uint32_t)*size + 1);
    if (is_raw) {
        obj->ob_size = size;
        for (int i = 0; i < size; i++) {
            obj->ob_val[i] = str[i];
        }
        obj->ob_val[size] = '\0';
    } else {
        int pos = 0, num;
        for (int i = 0; i < size;) {
            switch (str[i]) {
                case '\\':
                    if (++i >= size) goto _def;
                    switch (str[i]) {
                        case 'a': obj->ob_val[pos++] = 7; i++; break;
                        case 'b': obj->ob_val[pos++] = 8; i++; break;
                        case 'f': obj->ob_val[pos++] = 12; i++; break;
                        case 'n': obj->ob_val[pos++] = 10; i++; break;
                        case 'r': obj->ob_val[pos++] = 13; i++; break;
                        case 't': obj->ob_val[pos++] = 9; i++; break;
                        case 'v': obj->ob_val[pos++] = 11; break;
                        case '\\': obj->ob_val[pos++] = '\\'; break;
                        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
                            obj->ob_val[pos++] = _read_x_int(str + i, &num, _oct, min(size - i, 3));
                            i += num;
                            break;
                        case 'x':
                            i++;
                            if ((size - i >= 2) && (_ishex(str[i]) && _ishex(str[i + 1]))) {
                                obj->ob_val[pos++] = _hex(str[i]) * 16 + _hex(str[i + 1]);
                            } else {
                                pylt_obj_str_free(state, obj);
                                return NULL;
                            } 
                            i += 2;
                            break;
                        default:
                            if (str[++i] >= 0x80) {
                                // error
                                return NULL;
                            }
                            obj->ob_val[pos++] = '\\';
                            obj->ob_val[pos++] = str[i];
                            break;
                    }
                default: _def :
                    obj->ob_val[pos++] = str[i++];
            }
        }
        obj->ob_size = pos;
        obj->ob_val[pos] = '\0';
        pylt_realloc(obj->ob_val, sizeof(uint32_t)*pos + 1);
    }
    obj->ob_hash = pylt_obj_str_cforcehash(state, obj);
    obj2 = caststr(pylt_obj_set_has(state, state->cache_str, castobj(obj)));
    if (obj2) {
        pylt_obj_str_free(state, obj);
        return obj2;
    } else {
        pylt_obj_set_add(state, state->cache_str, castobj(obj));
    }
    return obj;
}

void pylt_obj_str_free(PyLiteState *state, PyLiteStrObject *self) {
    pylt_free(self->ob_val);
    pylt_free(self);
}