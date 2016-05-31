﻿
#include "bool.h"
#include "bytes.h"
#include "set.h"
#include "../state.h"

static PyLiteBytesObject* hash_and_check_cache(PyLiteState *state, PyLiteBytesObject *obj) {
    PyLiteBytesObject *obj2;
    obj->ob_hash = pylt_obj_bytes_forcehash(state, obj);
    obj2 = castbytes(pylt_obj_set_has(state, state->cache_bytes, castobj(obj)));
    if (obj2) {
        pylt_obj_bytes_free(state, obj);
        return obj2;
    } else {
        pylt_obj_set_add(state, state->cache_bytes, castobj(obj));
    }
    return obj;
}

pl_int_t pylt_obj_bytes_cmp(PyLiteState *state, PyLiteBytesObject *self, PyLiteObject *other) {
    return false;
}

pl_bool_t pylt_obj_bytes_eq(PyLiteState *state, PyLiteBytesObject *self, PyLiteObject *other) {
    unsigned int i;
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_BYTES:
            if (self->ob_hash != castbytes(other)->ob_hash)
                return false;
            if (self->ob_size != castbytes(other)->ob_size)
                return false;
            for (i = 0; i < self->ob_size; i++) {
                if (self->ob_val[i] != castbytes(other)->ob_val[i]) return false;
            }
            return true;
        default:
            return false;
    }
}

pl_uint32_t pylt_obj_bytes_hash(PyLiteState *state, PyLiteBytesObject *obj) {
    return castbytes(obj)->ob_hash;
}

// BKDR Hash
pl_uint32_t pylt_obj_bytes_forcehash(PyLiteState *state, PyLiteBytesObject *obj) {
    register size_t hash = 0;
    for (unsigned int i = 0; i < obj->ob_size; i++) {
        hash = hash * 131 + obj->ob_val[i];
    }
    return (hash & 0x7FFFFFFF);
}

PyLiteBytesObject* pylt_obj_bytes_getitem(PyLiteState *state, PyLiteBytesObject *self, int index) {
    char buf[1];
    int len = self->ob_size;
    if (index < 0) index += len;
    if (index >= len) return NULL;
    else {
        buf[0] = self->ob_val[index];
        return pylt_obj_bytes_new(state, buf, 1, true);
    }
    return NULL;
}

PyLiteObject* pylt_obj_bytes_mul(PyLiteState *state, PyLiteBytesObject *self, PyLiteObject *other) {
    PyLiteBytesObject *obj;
    if (other->ob_type != PYLT_OBJ_TYPE_INT) return NULL;

    obj = pylt_realloc(NULL, sizeof(PyLiteBytesObject));
    obj->ob_type = PYLT_OBJ_TYPE_BYTES;
    obj->ob_size = self->ob_size * castint(other)->ob_val;
    obj->ob_val = pylt_realloc(NULL, sizeof(uint8_t) * (obj->ob_size + 1));
    if (!obj->ob_val); // Memory Error

    for (int i = 0; i < castint(other)->ob_val; ++i) {
        memcpy(obj->ob_val + i * self->ob_size, self->ob_val, self->ob_size * sizeof(uint8_t));
    }

    obj->ob_val[obj->ob_size] = '\0';
    return castobj(hash_and_check_cache(state, obj));
}

PyLiteObject* pylt_obj_bytes_plus(PyLiteState *state, PyLiteBytesObject *self, PyLiteObject *other) {
    PyLiteBytesObject *obj;
    if (other->ob_type != PYLT_OBJ_TYPE_BYTES) return NULL;

    obj = pylt_realloc(NULL, sizeof(PyLiteBytesObject));
    obj->ob_type = PYLT_OBJ_TYPE_BYTES;
    obj->ob_size = self->ob_size + castbytes(other)->ob_size;
    obj->ob_val = pylt_realloc(NULL, sizeof(uint8_t) * (obj->ob_size + 1));
    if (!obj->ob_val); // Memory Error

    memcpy(obj->ob_val, self->ob_val, self->ob_size * sizeof(uint8_t));
    memcpy(obj->ob_val + self->ob_size, caststr(other)->ob_val, caststr(other)->ob_size * sizeof(uint8_t));

    obj->ob_val[obj->ob_size] = '\0';
    return castobj(hash_and_check_cache(state, obj));
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
int _read_x_int(const char *p, int *pnum, uint8_t(*func)(uint32_t code), int max_size) {
    const char *e = p + max_size;
    int ret = 0, num = 0, val = (int)pow(10, e - p - 1);

    do {
        ret += (*func)(*p++) * val;
        val /= 10;
        num++;
    } while (p != e);

    *pnum = num;
    return ret;
}


PyLiteBytesObject* pylt_obj_bytes_new(PyLiteState *state, const char* str, int size, bool is_raw) {
    PyLiteBytesObject *obj = pylt_realloc(NULL, sizeof(PyLiteBytesObject));
    obj->ob_type = PYLT_OBJ_TYPE_BYTES;
    obj->ob_val = pylt_realloc(NULL, sizeof(uint8_t) * (size + 1));
    if (is_raw) {
        obj->ob_val[size] = '\0';
        obj->ob_size = size;
        memcpy(obj->ob_val, str, size);
    } else {
        int pos = 0, num;
        for (int i = 0; i < size; ) {
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
                                pylt_obj_bytes_free(state, obj);
                                return NULL;
                            }
                            i += 2;
                            break;
                        default:
                            obj->ob_val[pos++] = '\\';
                            obj->ob_val[pos++] = str[++i];
                            break;
                    }
                default: _def:
                    obj->ob_val[pos++] = str[i++];
            }
        }
        obj->ob_size = pos;
        obj->ob_val[pos] = '\0';
        obj->ob_val = pylt_realloc(obj->ob_val, sizeof(uint8_t)*pos + 1);
    }
    return hash_and_check_cache(state, obj);
}

PyLiteBytesObject* pylt_obj_bytes_new_empty(PyLiteState *state) {
    return pylt_obj_bytes_new(state, NULL, 0, true);
}

void pylt_obj_bytes_free(PyLiteState *state, PyLiteBytesObject *self) {
    pylt_free(self->ob_val);
    pylt_free(self);
}

pl_int_t pylt_obj_bytes_index_full(PyLiteState *state, PyLiteBytesObject *self, PyLiteBytesObject *sub, pl_int_t start, pl_int_t end) {
    pl_int_t i, j, k;
    pl_int_t len_self = (pl_int_t)self->ob_size;

    if (start < 0) start += len_self;
    start = max(0, start);
    if (start >= len_self) return -2;

    if (end < 0) end += len_self;
    end = min(len_self, end);
    if (end <= start) return -2;

    for (i = start; i < end; ++i) {
        if (self->ob_val[i] == sub->ob_val[0]) {
            k = 1;
            j = i + 1;
            while ((j < end) && (k < (pl_int_t)sub->ob_size)) {
                if (self->ob_val[j] != sub->ob_val[k]) {
                    break;
                }
                ++j; ++k;
            }
            if (k == sub->ob_size) {
                return i;
            }
        }
    }
    return -1;
}

pl_int_t pylt_obj_bytes_index(PyLiteState *state, PyLiteBytesObject *self, PyLiteBytesObject *sub) {
    return pylt_obj_bytes_index_full(state, self, sub, 0, self->ob_size);
}
