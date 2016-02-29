﻿
#ifndef PYLITE_PARSER_H
#define PYLITE_PARSER_H

#include "lexer.h"
#include "lib/kvec.h"
#include "types/object.h"
#include "types/func.h"

typedef struct ParserState {
    LexState *ls;
    PyLiteState* state;
    kvec_t(PyLiteFunctionObject*) func_stack;
    PyLiteFunctionObject* func;
} ParserState;

#define kv_pushobj(v, x) kv_push(PyLiteObject*, (v), (x))
#define kv_pushbc(v, x) kv_push(size_t, (v), (x))

void parse(ParserState *ps);
void pylt_parser_init(PyLiteState* state, ParserState *ps, LexState *ls);

// SyntaxError: invalid syntax
#define PYLT_ERR_PARSER_INVALID_SYNTAX -1
// SyntaxError: (value error) invalid escape
#define PYLT_ERR_PARSER_BYTES_INVALID_ESCAPE -2

#endif