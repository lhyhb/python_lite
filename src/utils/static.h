
#ifndef PYLITE_STATIC_H
#define PYLITE_STATIC_H

#include "config.h"

typedef struct PyLiteStrObject PyLiteStrObject;

struct PyLiteStaticObjectsType {
    struct {
        PyLiteStrObject *__base__;
        PyLiteStrObject *__call__;
        PyLiteStrObject *__del__;
        PyLiteStrObject *__new__;
        PyLiteStrObject *__import__;
        PyLiteStrObject *__init__;
        PyLiteStrObject *__str__;
        PyLiteStrObject *__repr__;

        PyLiteStrObject *__add__;
        PyLiteStrObject *__div__;
        PyLiteStrObject *__floordiv__;
        PyLiteStrObject *__mul__;
        PyLiteStrObject *__neg__;
        PyLiteStrObject *__pos__;
        PyLiteStrObject *__pow__;
        PyLiteStrObject *__sub__;

        PyLiteStrObject *__lshift__;
        PyLiteStrObject *__rshift__;

        PyLiteStrObject *__hash__;
        PyLiteStrObject *__iter__;
        PyLiteStrObject *__cmp__;
        PyLiteStrObject *__eq__;
        PyLiteStrObject *__setattr__;
        PyLiteStrObject *__getattr__;
        PyLiteStrObject *__delattr__;
        PyLiteStrObject *__setitem__;
        PyLiteStrObject *__getitem__;
        PyLiteStrObject *__delitem__;

        PyLiteStrObject *__defaults__;
        PyLiteStrObject *__parameters__;
        PyLiteStrObject *__args_types__;

        // builtins
        PyLiteStrObject *abs;
        PyLiteStrObject *all;
        PyLiteStrObject *any;
        PyLiteStrObject *ascii;
        PyLiteStrObject *bin;
        PyLiteStrObject *bytearray;
        PyLiteStrObject *callable;
        PyLiteStrObject *chr;
        PyLiteStrObject *classmethod;
        PyLiteStrObject *compile;
        PyLiteStrObject *delattr;
        PyLiteStrObject *dir;
        PyLiteStrObject *divmod;
        PyLiteStrObject *enumerate;
        PyLiteStrObject *eval;
        PyLiteStrObject *exec;
        PyLiteStrObject *filter;
        PyLiteStrObject *format;
        PyLiteStrObject *frozenset;
        PyLiteStrObject *getattr;
        PyLiteStrObject *globals;
        PyLiteStrObject *hasattr;
        PyLiteStrObject *hash;
        PyLiteStrObject *help;
        PyLiteStrObject *hex;
        PyLiteStrObject *id;
        PyLiteStrObject *input;
        PyLiteStrObject *isinstance;
        PyLiteStrObject *issubclass;
        PyLiteStrObject *iter;
        PyLiteStrObject *len;
        PyLiteStrObject *locals;
        PyLiteStrObject *map;
        PyLiteStrObject *max;
        PyLiteStrObject *memoryview;
        PyLiteStrObject *min;
        PyLiteStrObject *next;
        PyLiteStrObject *oct;
        PyLiteStrObject *open;
        PyLiteStrObject *ord;
        PyLiteStrObject *pow;
        PyLiteStrObject *print;
        PyLiteStrObject *repr;
        PyLiteStrObject *reversed;
        PyLiteStrObject *round;
        PyLiteStrObject *setattr;
        PyLiteStrObject *slice;
        PyLiteStrObject *sorted;
        PyLiteStrObject *staticmethod;
        PyLiteStrObject *sum;
        PyLiteStrObject *super;
        PyLiteStrObject *vars;
        PyLiteStrObject *zip;

        // classes
        PyLiteStrObject *object;
        PyLiteStrObject *int_;
        PyLiteStrObject *float_;
        PyLiteStrObject *bool_;
        PyLiteStrObject *str;
        PyLiteStrObject *bytes;
        PyLiteStrObject *set;
        PyLiteStrObject *list;
        PyLiteStrObject *tuple;
        PyLiteStrObject *dict;
        PyLiteStrObject *type;
        PyLiteStrObject *unusual;

        PyLiteStrObject *module;
        PyLiteStrObject *function;
        PyLiteStrObject *cfunction;
        PyLiteStrObject *code;
        PyLiteStrObject *iterator;
        PyLiteStrObject *property_;
        PyLiteStrObject *NoneType;
        PyLiteStrObject *cpointer;
        PyLiteStrObject *range;

        // object
        PyLiteStrObject *mro;

        // str
        PyLiteStrObject *join;
        PyLiteStrObject *sub;
        PyLiteStrObject *start;
        PyLiteStrObject *end;
        PyLiteStrObject *startswith;
        PyLiteStrObject *endswith;

        // int
        PyLiteStrObject *is_integer;

        // set
        PyLiteStrObject *add;
        PyLiteStrObject *clear;
        PyLiteStrObject *copy;
        PyLiteStrObject *pop;
        PyLiteStrObject *remove;

        // list
        PyLiteStrObject *append;
        PyLiteStrObject *count;
        PyLiteStrObject *index;
        PyLiteStrObject *extend;
        PyLiteStrObject *insert;
        PyLiteStrObject *reverse;

        // tuple

        // dict
        PyLiteStrObject *items;

        // unusual
        PyLiteStrObject *useless;
        PyLiteStrObject *NotImplemented;
        PyLiteStrObject *param_args;
        PyLiteStrObject *param_kwargs;
        PyLiteStrObject *param_undefined;

        // prop
        PyLiteStrObject *fget;
        PyLiteStrObject *fset;

        PyLiteStrObject *None;
        PyLiteStrObject *True;
        PyLiteStrObject *False;

        // misc
        PyLiteStrObject *cls;
        PyLiteStrObject *self;
        PyLiteStrObject *args;
        PyLiteStrObject *kwargs;
        PyLiteStrObject *param1;
        PyLiteStrObject *param2;
        PyLiteStrObject *param3;
        PyLiteStrObject *param4;
        PyLiteStrObject *default_;
        PyLiteStrObject *__cobj__;

        PyLiteStrObject *x;
        PyLiteStrObject *y;

        // modules
        PyLiteStrObject *math;
        PyLiteStrObject *builtins;
        PyLiteStrObject *cio;

        // module - io
        PyLiteStrObject *io;
        PyLiteStrObject *BaseIO;
        PyLiteStrObject *BytesIO;
        PyLiteStrObject *TextIO;
        PyLiteStrObject *UnsupportedOperation;
        PyLiteStrObject *encoding;

        // module - sys
        PyLiteStrObject *sys;
        PyLiteStrObject *stdin_;
        PyLiteStrObject *stdout_;
        PyLiteStrObject *stderr_;

        PyLiteStrObject *os;
        PyLiteStrObject *pltypes;

        // module - exceptions
        PyLiteStrObject *BaseException;
        PyLiteStrObject *SystemExit;
        PyLiteStrObject *KeyboardInterrupt;
        PyLiteStrObject *GeneratorExit;
        PyLiteStrObject *Exception;
        PyLiteStrObject *StopIteration;
        PyLiteStrObject *StopAsyncIteration;
        PyLiteStrObject *ArithmeticError;
        PyLiteStrObject *FloatingPointError;
        PyLiteStrObject *OverflowError;
        PyLiteStrObject *ZeroDivisionError;
        PyLiteStrObject *AssertionError;
        PyLiteStrObject *AttributeError;
        PyLiteStrObject *BufferError;
        PyLiteStrObject *EOFError;
        PyLiteStrObject *ImportError;
        PyLiteStrObject *LookupError;
        PyLiteStrObject *IndexError;
        PyLiteStrObject *KeyError;
        PyLiteStrObject *MemoryError;
        PyLiteStrObject *NameError;
        PyLiteStrObject *OSError;
        PyLiteStrObject *BlockingIOError;
        PyLiteStrObject *ChildProcessError;
        PyLiteStrObject *ConnectionError;
        PyLiteStrObject *BrokenPipeError;
        PyLiteStrObject *ConnectionAbortedError;
        PyLiteStrObject *ConnectionRefusedError;
        PyLiteStrObject *ConnectionResetError;
        PyLiteStrObject *FileExistsError;
        PyLiteStrObject *FileNotFoundError;
        PyLiteStrObject *InterruptedError;
        PyLiteStrObject *IsADirectoryError;
        PyLiteStrObject *NotADirectoryError;
        PyLiteStrObject *PermissionError;
        PyLiteStrObject *ProcessLookupError;
        PyLiteStrObject *TimeoutError;
        PyLiteStrObject *ReferenceError;
        PyLiteStrObject *RuntimeError;
        PyLiteStrObject *NotImplementedError;
        PyLiteStrObject *RecursionError;
        PyLiteStrObject *SyntaxError;
        PyLiteStrObject *IndentationError;
        PyLiteStrObject *TabError;
        PyLiteStrObject *SystemError;
        PyLiteStrObject *TypeError;
        PyLiteStrObject *ValueError;
        PyLiteStrObject *UnicodeError;
        PyLiteStrObject *UnicodeDecodeError;
        PyLiteStrObject *UnicodeEncodeError;
        PyLiteStrObject *UnicodeTranslateError;
        PyLiteStrObject *Warning;
        PyLiteStrObject *DeprecationWarning;
        PyLiteStrObject *PendingDeprecationWarning;
        PyLiteStrObject *RuntimeWarning;
        PyLiteStrObject *SyntaxWarning;
        PyLiteStrObject *UserWarning;
        PyLiteStrObject *FutureWarning;
        PyLiteStrObject *ImportWarning;
        PyLiteStrObject *UnicodeWarning;
        PyLiteStrObject *BytesWarning;
        PyLiteStrObject *ResourceWarning;

        // template
        PyLiteStrObject *TMPL_OBJECT_TO_STR;
        PyLiteStrObject *TMPL_CLASS_TO_STR;
        PyLiteStrObject *TMPL_MODULE_TO_STR;
        PyLiteStrObject *TMPL_FUNCTION_TO_STR;
        PyLiteStrObject *TMPL_CFUNCTION_TO_STR;

        PyLiteStrObject *TMPL_EMPTY_STR;
        PyLiteStrObject *TMPL_EMPTY_SET;
        PyLiteStrObject *TMPL_EMPTY_LIST;
        PyLiteStrObject *TMPL_EMPTY_TUPLE;
        PyLiteStrObject *TMPL_EMPTY_DICT;

        PyLiteStrObject *TMPL_LAMBDA_FUNC_NAME;
    } str;
} pl_static;

void pylt_static_objs_init(PyLiteInterpreter *I);

#endif
