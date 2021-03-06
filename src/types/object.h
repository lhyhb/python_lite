
#ifndef PYLITE_TYPES_OBJECT_H
#define PYLITE_TYPES_OBJECT_H

#include "../utils/config.h"
#include "../deps/khash_obj.h"

// Object

typedef struct PyLiteObject {
    uint32_t ob_type;
    uint32_t ob_flags;
} PyLiteObject;

#define PyLiteObject_HEAD \
    uint32_t ob_type; \
    uint32_t ob_flags

#define PYLT_OBJ_FLAG_CANFREE  1
#define PYLT_OBJ_FLAG_STATIC   2
#define PYLT_OBJ_FLAG_WATCH    4

/** All objects managed by GC. But by default,
    GC can't release object, otherwise with PYLT_OBJ_FLAG_CANFREE flag.
    */
#define PyLiteObject_init(I, obj, typestruct, typecode) \
    struct typestruct *obj = (struct typestruct*)pylt_malloc(I, sizeof(struct typestruct)); \
    obj->ob_type = typecode; \
    obj->ob_flags = 0; \
    pylt_gc_add(I, (PyLiteObject*)(obj))


enum PyLiteObjectTypeCode {
    PYLT_OBJ_TYPE_OBJ = 1,
    PYLT_OBJ_TYPE_INT,
    PYLT_OBJ_TYPE_FLOAT,
    PYLT_OBJ_TYPE_BOOL,

    PYLT_OBJ_TYPE_STR,  // 5
    PYLT_OBJ_TYPE_BYTES,
    PYLT_OBJ_TYPE_SET,
    PYLT_OBJ_TYPE_LIST,
    PYLT_OBJ_TYPE_TUPLE,
    PYLT_OBJ_TYPE_DICT,

    PYLT_OBJ_TYPE_TYPE,  // 11
    PYLT_OBJ_TYPE_UNUSUAL,

    PYLT_OBJ_TYPE_MODULE, // 13
    PYLT_OBJ_TYPE_FUNCTION,
    PYLT_OBJ_TYPE_CFUNCTION,
    PYLT_OBJ_TYPE_CODE,

    PYLT_OBJ_TYPE_ITER, // 17
    PYLT_OBJ_TYPE_PROP,
    PYLT_OBJ_TYPE_NONE,
    PYLT_OBJ_TYPE_CPTR,

    PYLT_OBJ_TYPE_RANGE, // 21

    PYLT_OBJ_TYPE_BASE_EXCEPTION, // 22

    PYLT_OBJ_TYPE_USERCLASS, // 23
};

#define PYLT_OBJ_BUILTIN_TYPE_NUM PYLT_OBJ_TYPE_BASE_EXCEPTION


// Object methods

pl_int_t pylt_obj_cmp(PyLiteInterpreter *I, PyLiteObject *a, PyLiteObject *b);
pl_bool_t pylt_obj_eq(PyLiteInterpreter *I, PyLiteObject *a, PyLiteObject *b);
pl_uint32_t pylt_obj_hash(PyLiteInterpreter *I, PyLiteObject *obj);
pl_int_t pylt_obj_len(PyLiteInterpreter *I, PyLiteObject *obj);

pl_bool_t pylt_obj_hashable(PyLiteInterpreter *I, PyLiteObject *obj);
pl_bool_t pylt_obj_iterable(PyLiteInterpreter *I, PyLiteObject *obj);
pl_bool_t pylt_obj_istrue(PyLiteInterpreter *I, PyLiteObject *obj);

PyLiteObject* pylt_obj_getattr_ex(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject* key, PyLiteObject* _default, pl_bool_t *p_at_type);
PyLiteObject* pylt_obj_getattr(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject* key, pl_bool_t *p_at_type);
pl_bool_t pylt_obj_setattr(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject* key, PyLiteObject* value);
pl_bool_t pylt_obj_delattr(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject* key);

//PyLiteObject* pylt_obj_getitem(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject* key);
//pl_bool_t pylt_obj_setitem(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject* key, PyLiteObject* value);

pl_bool_t pylt_obj_has(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject *obj, pl_bool_t *is_valid);

PyLiteObject* pylt_obj_op_unary(PyLiteInterpreter *I, int op, PyLiteObject *obj);
PyLiteObject* pylt_obj_op_binary(PyLiteInterpreter *I, int op, PyLiteObject *a, PyLiteObject *b);

// Type cast

struct PyLiteIntObject;
struct PyLiteFloatObject;
struct PyLiteBoolObject;

struct PyLiteStrObject;
struct PyLiteBytesObject;
struct PyLiteSetObject;
struct PyLiteListObject;
struct PyLiteTupleObject;
struct PyLiteDictObject;
struct PyLiteTypeObject;
struct PyLiteUnusualObject;

struct PyLiteCodeObject;
struct PyLiteModuleObject;
struct PyLiteFunctionObject;
struct PyLiteCFunctionObject;

struct PyLiteIterObject;
struct PyLitePropertyObject;
struct PyLiteNoneObject;
struct PyLiteCPtrObject;

struct PyLiteRangeObject;
struct PyLiteBaseExceptionObject;

struct PyLiteCustomObject;


#define cast(t, exp)    ((t)(exp))
#define castobj(i)      cast(struct PyLiteObject*, (i))
#define castint(i)      cast(struct PyLiteIntObject*, (i))
#define castfloat(i)    cast(struct PyLiteFloatObject*, (i))
#define castbool(i)     cast(struct PyLiteBoolObject*, (i))

#define caststr(i)      cast(struct PyLiteStrObject*, (i))
#define castbytes(i)    cast(struct PyLiteBytesObject*, (i))
#define castset(i)      cast(struct PyLiteSetObject*, (i))
#define castlist(i)     cast(struct PyLiteListObject*, (i))
#define casttuple(i)    cast(struct PyLiteTupleObject*, (i))
#define castdict(i)     cast(struct PyLiteDictObject*, (i))
#define casttype(i)     cast(struct PyLiteTypeObject*, (i))
#define castunusual(i)  cast(struct PyLiteUnusualObject*, (i))

#define castmod(i)      cast(struct PyLiteModuleObject*, (i))
#define castfunc(i)     cast(struct PyLiteFunctionObject*, (i))
#define castcfunc(i)    cast(struct PyLiteCFunctionObject*, (i))
#define castcode(i)     cast(struct PyLiteCodeObject*, (i))

#define castiter(i)     cast(struct PyLiteIterObject*, (i))
#define castprop(i)     cast(struct PyLitePropertyObject*, (i))
#define castnone(i)     cast(struct PyLiteNoneObject*, (i))
#define castcptr(i)     cast(struct PyLiteCPtrObject*, (i))

#define castrange(i)    cast(struct PyLiteRangeObject*, (i))
#define castexcept(i)   cast(struct PyLiteBaseExceptionObject*, (i))

#define castcustom(i)   cast(struct PyLiteCustomObject*, (i))

#define dcast(type, i)  cast##type(pylt_obj_getbase(i))

#define pl_isint(i)        (i->ob_type == PYLT_OBJ_TYPE_INT)
#define pl_isflt(i)        (i->ob_type == PYLT_OBJ_TYPE_FLOAT)
#define pl_isstr(i)        (i->ob_type == PYLT_OBJ_TYPE_STR)
#define pl_isbytes(i)      (i->ob_type == PYLT_OBJ_TYPE_BYTES)
#define pl_iset(i)         (i->ob_type == PYLT_OBJ_TYPE_SET)
#define pl_islist(i)       (i->ob_type == PYLT_OBJ_TYPE_LIST)
#define pl_istuple(i)      (i->ob_type == PYLT_OBJ_TYPE_TUPLE)
#define pl_isdict(i)       (i->ob_type == PYLT_OBJ_TYPE_DICT)
#define pl_istype(i)       (i->ob_type == PYLT_OBJ_TYPE_TYPE)
#define pl_isunusual(i)    (i->ob_type == PYLT_OBJ_TYPE_UNUSUAL)
#define pl_isiter(i)       (i->ob_type == PYLT_OBJ_TYPE_ITER)
#define pl_isnone(i)       (i->ob_type == PYLT_OBJ_TYPE_NONE)
#define pl_isnum(i)        ((i->ob_type == PYLT_OBJ_TYPE_INT) || (i->ob_type == PYLT_OBJ_TYPE_FLOAT))
#define pl_isstrkind(i)    ((i->ob_type == PYLT_OBJ_TYPE_STR) || (i->ob_type == PYLT_OBJ_TYPE_BYTES))
#define pl_ismod(i)        (i->ob_type == PYLT_OBJ_TYPE_MODULE)
#define pl_iscode(i)       (i->ob_type == PYLT_OBJ_TYPE_CODE)
#define pl_iscustom(i)     (i->ob_type >= PYLT_OBJ_TYPE_USERCLASS)

#define pl_iscustomtype(i) (pl_istype(i)) && (((struct PyLiteTypeObject*)i)->ob_reftype >= PYLT_OBJ_TYPE_USERCLASS)

/** get base object of custom class object. get itself of builtin type's object */
PyLiteObject* pylt_obj_getbase(PyLiteObject *obj);
struct PyLiteStrObject* pylt_obj_to_str(PyLiteInterpreter *I, PyLiteObject *obj);
struct PyLiteStrObject* pylt_obj_to_repr(PyLiteInterpreter *I, PyLiteObject *obj);

void pylt_obj_free(PyLiteInterpreter *I, PyLiteObject *obj);
void pylt_obj_rfree(PyLiteInterpreter *I, PyLiteObject *obj);

// Others

typedef PyLiteObject* (*PyLiteObjUnaryOpFunc)(PyLiteInterpreter *I, PyLiteObject *obj);
typedef PyLiteObject* (*PyLiteObjBinaryOpFunc)(PyLiteInterpreter *I, PyLiteObject *a, PyLiteObject *b);

typedef PyLiteObject* (*PyLiteCFunctionPtr)(PyLiteInterpreter *I, int argc, PyLiteObject **args);
typedef PyLiteObject* (*PyLiteIterFunc)(PyLiteInterpreter *I, struct PyLiteIterObject *iter);

typedef pl_bool_t (*PyLiteModuleRegisterFunc)(PyLiteInterpreter *I);
typedef struct PyLiteModuleObject* (*PyLiteModuleLoaderFunc)(PyLiteInterpreter *I);

#endif
