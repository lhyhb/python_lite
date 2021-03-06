﻿
#include "bool.h"
#include "bytes.h"
#include "set.h"
#include "../../api.h"
#include "../../intp.h"
#include "../../utils/misc.h"

// 将 index 转为标准形式并约束到可用范围
#define index_fix(__index) \
    if (__index < 0) __index += self->ob_size; \
    if (__index < 0) __index = 0; \
    else if (__index >= (pl_int_t)self->ob_size) __index = self->ob_size;


static PyLiteBytesObject* hash_and_check_cache(PyLiteInterpreter *I, PyLiteBytesObject *obj) {
    obj->ob_hash = pylt_obj_bytes_forcehash(I, obj);
    return (I) ? pylt_gc_cache_bytes_add(I, obj) : obj;
}

pl_int_t pylt_obj_bytes_cmp(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteObject *other) {
    return false;
}

pl_bool_t pylt_obj_bytes_eq(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteObject *other) {
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

pl_uint32_t pylt_obj_bytes_hash(PyLiteInterpreter *I, PyLiteBytesObject *obj) {
    return castbytes(obj)->ob_hash;
}

// BKDR Hash
pl_uint32_t pylt_obj_bytes_forcehash(PyLiteInterpreter *I, PyLiteBytesObject *obj) {
    register size_t hash = 0;
    for (unsigned int i = 0; i < obj->ob_size; i++) {
        hash = hash * 131 + obj->ob_val[i];
    }
    return (hash & 0x7FFFFFFF);
}

PyLiteBytesObject* pylt_obj_bytes_getitem(PyLiteInterpreter *I, PyLiteBytesObject *self, int index) {
    char buf[1];
    int len = self->ob_size;
    if (index < 0) index += len;
    if (index >= len) return NULL;
    else {
        buf[0] = self->ob_val[index];
        return pylt_obj_bytes_new(I, buf, 1, true);
    }
    return NULL;
}

PyLiteObject* pylt_obj_bytes_mul(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteObject *other) {
    PyLiteBytesObject *obj;
    if (other->ob_type != PYLT_OBJ_TYPE_INT) return NULL;

    obj = pylt_malloc(I, sizeof(PyLiteBytesObject));
    obj->ob_type = PYLT_OBJ_TYPE_BYTES;
    obj->ob_size = self->ob_size * castint(other)->ob_val;
    obj->ob_val = pylt_malloc(I, sizeof(uint8_t) * (obj->ob_size + 1));
    if (!obj->ob_val); // Memory Error

    for (int i = 0; i < castint(other)->ob_val; ++i) {
        memcpy(obj->ob_val + i * self->ob_size, self->ob_val, self->ob_size * sizeof(uint8_t));
    }

    obj->ob_val[obj->ob_size] = '\0';
    return castobj(hash_and_check_cache(I, obj));
}

PyLiteObject* pylt_obj_bytes_plus(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteObject *other) {
    PyLiteBytesObject *obj;
    if (other->ob_type != PYLT_OBJ_TYPE_BYTES) return NULL;

    obj = pylt_malloc(I, sizeof(PyLiteBytesObject));
    obj->ob_type = PYLT_OBJ_TYPE_BYTES;
    obj->ob_size = self->ob_size + castbytes(other)->ob_size;
    obj->ob_val = pylt_malloc(I, sizeof(uint8_t) * (obj->ob_size + 1));
    if (!obj->ob_val); // Memory Error

    memcpy(obj->ob_val, self->ob_val, self->ob_size * sizeof(uint8_t));
    memcpy(obj->ob_val + self->ob_size, caststr(other)->ob_val, caststr(other)->ob_size * sizeof(uint8_t));

    obj->ob_val[obj->ob_size] = '\0';
    return castobj(hash_and_check_cache(I, obj));
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
int _read_x_int(const char *p, int n, uint8_t(*func)(uint32_t code), int *pnum, int max_size) {
    const char *e = p + max_size;
    int ret = 0, num = 0, val = (int)pow(n, e - p - 1);

    do {
        uint8_t c = (*func)(*p++);
        if (c == 0xff) {
            ret = (int)(ret * pow(n, -max_size + num));
            break;
        }
        ret += c * val;
        val /= n;
        num++;
    } while (p != e);

    *pnum = num;
    return ret;
}

PyLiteBytesObject* pylt_obj_bytes_new(PyLiteInterpreter *I, const char* str, int size, bool is_raw) {
    PyLiteObject_init(I, obj, PyLiteBytesObject, PYLT_OBJ_TYPE_BYTES);

    obj->ob_val = pylt_malloc(I, sizeof(uint8_t) * (size + 1));
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
                        case 'v': obj->ob_val[pos++] = 11; i++; break;
                        case '\'': obj->ob_val[pos++] = '\''; i++; break;
                        case '\"': obj->ob_val[pos++] = '\"'; i++; break;
                        case '\\': obj->ob_val[pos++] = '\\'; i++; break;
                        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
                            obj->ob_val[pos++] = _read_x_int(str + i, 8, _oct, &num, min(size - i, 3));
                            i += num;
                            break;
                        case 'x':
                            i++;
                            if ((size - i >= 2) && (_ishex(str[i]) && _ishex(str[i + 1]))) {
                                obj->ob_val[pos++] = _hex(str[i]) * 16 + _hex(str[i + 1]);
                            } else {
                                pylt_obj_bytes_release(I, obj);
                                return NULL;
                            }
                            i += 2;
                            break;
                        default:
                            obj->ob_val[pos++] = '\\';
                            obj->ob_val[pos++] = str[i++];
                            break;
                    }
                    break;
                default: _def:
                    obj->ob_val[pos++] = str[i++];
            }
        }
        if (pos != size) {
            obj->ob_val = pylt_realloc(I, obj->ob_val, sizeof(uint8_t)*pos, sizeof(uint8_t)*pos + 1);
        }
        obj->ob_size = pos;
        obj->ob_val[pos] = '\0';
    }
    return hash_and_check_cache(I, obj);
}

PyLiteBytesObject* pylt_obj_bytes_new_empty(PyLiteInterpreter *I) {
    return pylt_obj_bytes_new(I, NULL, 0, true);
}

pl_int_t pylt_obj_bytes_index_full(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteBytesObject *sub, pl_int_t start, pl_int_t end) {
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

pl_int_t pylt_obj_bytes_index(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteBytesObject *sub) {
    return pylt_obj_bytes_index_full(I, self, sub, 0, self->ob_size);
}

PyLiteBytesObject* pylt_obj_bytes_slice(PyLiteInterpreter *I, PyLiteBytesObject *self, pl_int_t *pstart, pl_int_t *pend, pl_int_t step) {
    pl_int_t start, end;
    if (step == 0) return NULL;
    start = pstart ? *pstart : (step > 0) ? 0 : self->ob_size;
    end = pend ? *pend : (step > 0) ? self->ob_size : 0;

    index_fix(start);
    index_fix(end);

    if (step < 0) {
        start -= 1;
        if (!pend) end -= 1;
    }

    pl_int_t count = (pl_int_t)ceil(abs(end - start) / abs(step));
    uint8_t *buf = pylt_malloc(I, count * sizeof(uint8_t));

    if (step == 1) {
        memcpy(buf, self->ob_val + start, count * sizeof(uint8_t));
    } else {
        pl_int_t cur_index = start;
        for (pl_int_t i = 0; i < count; ++i) {
            buf[i] = self->ob_val[cur_index];
            cur_index += step;
        }
    }

    PyLiteBytesObject *str = pylt_obj_bytes_new(I, buf, count, true);
    pylt_free(I, buf, count * sizeof(uint8_t));
    return str;
}

struct PyLiteStrObject* pylt_obj_bytes_to_str(PyLiteInterpreter *I, PyLiteBytesObject *self) {
    uint32_t *data;
    pl_uint_t i, j = 0;
    int quote_count = 0;

    for (i = 0; i < self->ob_size; ++i) {
        if (self->ob_val[i] == '\'') quote_count++;
    }

    data = pylt_malloc(I, (self->ob_size + quote_count + 3) * sizeof(uint32_t));
    data[0] = 'b';
    data[1] = '\'';
    j = 2;

    for (i = 0; i < self->ob_size; ++i) {
        if (self->ob_val[i] == '\'') {
            data[j++] = '\\';
        }
        data[j++] = self->ob_val[i];
    }

    data[j++] = '\'';

    PyLiteStrObject *str = pylt_obj_str_new(I, data, j, true);
    pylt_free(I, data, (self->ob_size + quote_count + 3) * sizeof(uint32_t));
    return str;
}

void pylt_obj_bytes_release(PyLiteInterpreter *I, PyLiteBytesObject *self) {
    pylt_free(I, self->ob_val, self->ob_size * sizeof(uint8_t));
    pylt_free_ex(I, self);
}
