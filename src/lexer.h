﻿
#ifndef PYLITE_LEXER_H
#define PYLITE_LEXER_H

#include "utils.h"
#include "io.h"

#define FIRST_TOKEN    128

typedef enum TokenKind {
    // Common
    TK_END = FIRST_TOKEN,
    TK_COMMENTS,
    TK_NEWLINE,
    TK_INDENT,
    TK_DEDENT,

    TK_NAME,
    TK_INT,
    TK_FLOAT,
    TK_STRING,
    TK_BYTES,

    // Keywords
    TK_KW_FALSE, TK_KW_NONE, TK_KW_TRUE, TK_KW_AND, TK_KW_AS, TK_KW_ASSERT, TK_KW_BREAK,
    TK_KW_CLASS, TK_KW_CONTINUE, TK_KW_DEF, TK_KW_DEL, TK_KW_ELIF, TK_KW_ELSE, TK_KW_EXCEPT,
    TK_KW_FINALLY, TK_KW_FOR, TK_KW_FROM, TK_KW_GLOBAL, TK_KW_IF, TK_KW_IMPORT, TK_KW_IN,
    TK_KW_IS, TK_KW_LAMBDA, TK_KW_NONLOCAL, TK_KW_NOT, TK_KW_OR, TK_KW_PASS, TK_KW_RAISE,
    TK_KW_RETURN, TK_KW_TRY, TK_KW_WHILE, TK_KW_WITH, TK_KW_YIELD,

    // Operators
    // **      //      <<      >>      <=      >=      ==      !=
    TK_OP_POW, TK_OP_FLOORDIV, TK_OP_LSHIFT, TK_OP_RSHIFT, TK_OP_LE, TK_OP_GE, TK_OP_EQ, TK_OP_NE,

    // Delimiters
    // ->      
    // +=      -=      *=      /=      //=     %=      @=
    // &=      |=      ^=      >>=     <<=     **=
    TK_DE_RET_TYPE,
    TK_DE_PLUS_EQ, TK_DE_MINUS_EQ, TK_DE_MUL_EQ, TK_DE_DIV_EQ, TK_DE_FLOORDIV_EQ, TK_DE_MOD_EQ, TK_DE_MATMUL_EQ,
    TK_DE_BITAND_EQ, TK_DE_BITOR_EQ, TK_DE_XOR_EQ, TK_DE_RSHIFT_EQ, TK_DE_LSHIFT_EQ, TK_DE_POW_EQ,

} TokenKind;

typedef union TokenInfo {
    uint32_t i32;
    float f32;
} TokenInfo;

typedef struct Token {
    uint32_t val;
    TokenInfo info;
} Token;

typedef struct IndentInfo {
    struct IndentInfo *prev;
    int val;
} IndentInfo;

typedef struct LexState {
    int linenumber;
    Token token; /* current token */
    uint32_t current; /* current character */
    FileReader* fr;

    int current_indent;
    IndentInfo *indent; /* indent stack */
    IndentInfo *indent_used;
} LexState;

void pyltL_init(LexState *ls, FileReader *fr);
void pyltL_next(LexState *ls);

const char* pyltL_get_token_name(uint32_t token);

#endif
