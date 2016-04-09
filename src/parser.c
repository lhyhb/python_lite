﻿
#include "state.h"
#include "parser.h"
#include "debug.h"
#include "vm.h"
#include "types/all.h"

void func_push(ParserState *ps);
ParserInfo* func_pop(ParserState *ps);

void parse_t(ParserState *ps);

void parse_stmts(ParserState *ps);
void parse_block(ParserState *ps);

void parse_expr(ParserState *ps);
void parse_expr1(ParserState *ps);
void parse_expr2(ParserState *ps);
void parse_expr3(ParserState *ps);
void parse_expr4(ParserState *ps);
void parse_expr5(ParserState *ps);
void parse_expr6(ParserState *ps);
void parse_expr7(ParserState *ps);
void parse_expr8(ParserState *ps);
void parse_expr9(ParserState *ps);
void parse_expr10(ParserState *ps);

bool parse_try_index(ParserState *ps);

bool parse_try_t(ParserState *ps);
bool parse_try_expr(ParserState *ps);
bool parse_try_expr10(ParserState *ps);

void parse_left_value(ParserState *ps);
void parse_func(ParserState *ps);

void next(ParserState *ps) {
    pylt_lex_next(ps->ls);
}

void error(ParserState *ps, int code) {
    Token *tk = &(ps->ls->token);
    printf("ERROR at line [%d]\n", ps->ls->linenumber);
    //if (tk->val == TK_INT) raw_str_print(&(tk->str));
    const char *name = pylt_lex_get_token_name(tk->val);
    if (name) printf("%s", name);
    else putchar(tk->val);
    putchar('\n');
    switch (code) {
        case PYLT_ERR_PARSER_INVALID_SYNTAX:
            printf("SyntaxError: invalid syntax\n");
            break;
        case PYLT_ERR_PARSER_BYTES_INVALID_ESCAPE:
            printf("SyntaxError: (value error) invalid escape\n");
            break;
        case PYLT_ERR_PARSER_BREAK_OUTSIDE_LOOP:
            printf("SyntaxError: 'break' outside loop\n");
            break;
        case PYLT_ERR_PARSER_CONTINUE_OUTSIDE_LOOP:
            printf("SyntaxError: 'continue' not properly in loop\n");
            break;
    }
    system("pause");
    exit(-1);
}

void print_tk(Token *tk) {
    const char *name = pylt_lex_get_token_name(tk->val);
    if (name) printf("%s", name);
    else putchar(tk->val);
    putchar('\n');
}

void print_tk_val(int tk_val) {
    const char *name = pylt_lex_get_token_name(tk_val);
    if (name) printf("%s", name);
    else putchar(tk_val);
    putchar('\n');
}

static _INLINE
void ACCEPT(ParserState *ps, int token) {
    if (ps->ls->token.val != token) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
    next(ps);
}

void parse_number(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    if (tk->val == '+' || tk->val == '-') {
        next(ps);
    }
}

void parse_op(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '+': case '-': case '*': case '/': case '%': case '@': case '>': case '<':
        case TK_OP_FLOORDIV: case TK_OP_POW: case TK_OP_GE: case TK_OP_LE: case TK_OP_EQ :
            next(ps);
    }
}

PyLiteObject* parse_get_consttype(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    PyLiteObject *obj;

    switch (tk->val) {
        case TK_KW_TRUE:
            next(ps);
            return castobj(pylt_obj_bool_new(ps->state, true));
        case TK_KW_FALSE:
            next(ps);
            return castobj(pylt_obj_bool_new(ps->state, false));
        case TK_INT: case TK_FLOAT: case TK_STRING: case TK_BYTES:
            obj = tk->obj;
            next(ps);
            return obj;
        default:
            return NULL;
    }
}

int parse_mutabletype(ParserState *ps, int *ptimes) {
    Token *tk = &(ps->ls->token);
    //PyLiteObject *obj, *obj2;
    int tmp = 0;

    switch (tk->val) {
        case '[':
            next(ps);
            if (tk->val == ']') {
                if (ptimes) *ptimes = 0;
                return PYLT_OBJ_TYPE_LIST;
            } else {
                tmp = 0;
                while (true) {
                    parse_expr(ps);
                    tmp++;
                    if (tk->val == ',') next(ps);
                    else if (tk->val == ']') break;
                    else return 0;
                }
                next(ps);
                if (ptimes) *ptimes = tmp;
                return PYLT_OBJ_TYPE_LIST;
            }
            break;
        case '{':
            next(ps);
            if (tk->val == '}') {
                if (ptimes) *ptimes = 0;
                return PYLT_OBJ_TYPE_DICT;
            }

            parse_expr(ps);
            tmp = 1;
            switch (tk->val) {
                case ',': // set
                    next(ps);
                    while (true) {
                        if (!parse_try_expr(ps)) break;
                        tmp++;
                        if (tk->val != ',') break;
                        else next(ps);
                    }
                case '}':
                    ACCEPT(ps, '}');
                    if (ptimes) *ptimes = tmp;
                    return PYLT_OBJ_TYPE_SET;
                case ':': // dict
                    break;
            }
            break;
    }
    return 0;
}

bool parse_basetype(ParserState *ps) {
    int ret, times;
    PyLiteObject *obj = parse_get_consttype(ps);

    if (obj) {
        kv_pushobj(ps->info->code->const_val, obj);
        kv_pushbc(ps->info->code->opcodes, BC_LOADCONST);
        kv_pushbc(ps->info->code->opcodes, kv_size(ps->info->code->const_val));
        return true;
    } else {
        ret = parse_mutabletype(ps, &times);
        if (ret) {
            // NEW_OBJ TYPE EXTRA
            kv_pushbc(ps->info->code->opcodes, BC_NEW_OBJ_EXTRA);
            kv_pushbc(ps->info->code->opcodes, ret);
            kv_pushbc(ps->info->code->opcodes, times);
        }
        return ret != 0;
    }
}

/*
T ->    ( EXPR ) |
        not EXPR |
        BASETYPE |
        IDENT
*/
bool parse_try_t(ParserState *ps) {
    int num, tk_val;
    Token *tk = &(ps->ls->token);
    PyLiteObject *obj;
    bool is_success;

    switch (tk->val) {
        case TK_NAME:
            obj = tk->obj;
            next(ps);
            is_success = parse_try_index(ps);
            switch (tk->val) {
                case '=':
                    next(ps);
                    if (is_success) {
                        //printf("%d\n", kv_top(ps->info->code->opcodes));
                        //kv_top(ps->info->code->opcodes) = BC_SET_ITEM;
                        kv_pop(ps->info->code->opcodes);
                    }
                    //kv_pop(ps->info->code->opcodes);
                    //kv_pushbc(ps->info->code->opcodes, BC_SET_ITEM);
                    parse_expr(ps);
                    if (is_success) {
                        kv_pushbc(ps->info->code->opcodes, BC_LOAD_VAL);
                        kv_pushbc(ps->info->code->opcodes, (uintptr_t)obj);
                        kv_pushbc(ps->info->code->opcodes, BC_SET_ITEM);
                    } else {
                        kv_pushbc(ps->info->code->opcodes, BC_SET_VAL);
                        kv_pushbc(ps->info->code->opcodes, (uintptr_t)obj);
                    }
                    break;
                case TK_DE_PLUS_EQ: case TK_DE_MINUS_EQ:  case TK_DE_MUL_EQ: case TK_DE_DIV_EQ:
                case TK_DE_FLOORDIV_EQ: case TK_DE_MOD_EQ: case TK_DE_MATMUL_EQ:
                case TK_DE_BITAND_EQ: case TK_DE_BITOR_EQ: case TK_DE_BITXOR_EQ:
                case TK_DE_RSHIFT_EQ: case TK_DE_LSHIFT_EQ: case TK_DE_POW_EQ:
                    kv_pushbc(ps->info->code->opcodes, BC_LOAD_VAL);
                    kv_pushbc(ps->info->code->opcodes, (uintptr_t)obj);
                    num = token_de_to_op_val(tk->val);
                    next(ps);
                    parse_expr(ps);
                    kv_pushbc(ps->info->code->opcodes, BC_OPERATOR);
                    kv_pushbc(ps->info->code->opcodes, num);
                    kv_pushbc(ps->info->code->opcodes, BC_SET_VAL);
                    kv_pushbc(ps->info->code->opcodes, (uintptr_t)obj);
                    break;
                case '(':
                    next(ps);
                    num = 0;
                    while (true) {
                        if (!parse_try_expr(ps)) break;
                        num++;
                        if (tk->val != ',') break;
                        else next(ps);
                    }
                    kv_pushbc(ps->info->code->opcodes, BC_CALL);
                    kv_pushbc(ps->info->code->opcodes, (uintptr_t)num);
                    kv_pushbc(ps->info->code->opcodes, (uintptr_t)obj);
                    ACCEPT(ps, ')');
                    break;
                case '[':
                    if (!parse_try_index(ps)) {
                        error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
                    }
                    if (tk->val == '=') {
                        kv_pop(ps->info->code->opcodes);
                        next(ps);
                        parse_expr(ps);
                        kv_pushbc(ps->info->code->opcodes, BC_SET_ITEM);
                        kv_pushbc(ps->info->code->opcodes, (uintptr_t)obj);
                    }
                    break;
                default:
                    kv_pushbc(ps->info->code->opcodes, BC_LOAD_VAL);
                    kv_pushbc(ps->info->code->opcodes, (uintptr_t)obj);
            }
            break;
        case '(':
            next(ps);
            parse_expr(ps);
            ACCEPT(ps, ')');
            break;
        case '+': case '-':
            tk_val = tk->val;
            next(ps);
            parse_expr10(ps);
            //print_tk_val(tk_val);
            kv_pushbc(ps->info->code->opcodes, BC_OPERATOR);
            kv_pushbc(ps->info->code->opcodes, tk_val == '+' ? OP_POS : OP_NEG);
            break;
        case '~':
            next(ps);
            parse_expr10(ps);
            print_tk_val('~');
            kv_pushbc(ps->info->code->opcodes, BC_OPERATOR);
            kv_pushbc(ps->info->code->opcodes, token_to_op_val('~'));
            break;
        case TK_KW_NOT:
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            parse_expr5(ps);
            parse_expr4(ps);
            parse_expr3(ps);
            print_tk_val(TK_KW_NOT);
            kv_pushbc(ps->info->code->opcodes, BC_OPERATOR);
            kv_pushbc(ps->info->code->opcodes, token_to_op_val(TK_KW_NOT));
            break;
        default:
            return parse_basetype(ps);
    }
    return true;
}

static _INLINE void parse_t(ParserState *ps) {
    if (!parse_try_t(ps)) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
}

static _INLINE bool parse_try_index(ParserState *ps) {
    if (ps->ls->token.val == '[') {
        next(ps);
        parse_expr(ps);
        ACCEPT(ps, ']');
        kv_pushbc(ps->info->code->opcodes, BC_GET_ITEM);
        return true;
    }
    return false;
}

/* EXPR -> EXPR10 ... EXPR1 */
static _INLINE bool parse_try_expr(ParserState *ps) {
    if (!parse_try_expr10(ps)) return false;
    parse_expr9(ps);
    parse_expr8(ps);
    parse_expr7(ps);
    parse_expr6(ps);
    parse_expr5(ps);
    parse_expr4(ps);
    parse_expr3(ps);
    parse_expr2(ps);
    parse_expr1(ps);
    parse_try_index(ps);
    return true;
}

void parse_expr(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    if (!parse_try_expr(ps)) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
    if (tk->val == '.') {
        next(ps);
        ACCEPT(ps, TK_NAME);
    }
}

/* OR EXPR10 ... EXPR1 | ε */
_INLINE void parse_expr1(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_KW_OR:
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            parse_expr5(ps);
            parse_expr4(ps);
            parse_expr3(ps);
            parse_expr2(ps);
            print_tk_val(TK_KW_OR);
            kv_pushbc(ps->info->code->opcodes, BC_OPERATOR);
            kv_pushbc(ps->info->code->opcodes, OP_OR);
            parse_expr1(ps);
            break;
    }
}

/* AND EXPR10 ... EXPR2 | ε */
_INLINE void parse_expr2(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_KW_AND:
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            parse_expr5(ps);
            parse_expr4(ps);
            parse_expr3(ps);
            print_tk_val(TK_KW_AND);
            kv_pushbc(ps->info->code->opcodes, BC_OPERATOR);
            kv_pushbc(ps->info->code->opcodes, OP_AND);
            parse_expr2(ps);
            break;
    }
}


/* (<|<=|>|>=|!=|==|in|not in|is|is not) EXPR10 ... EXPR3 | ε */
_INLINE void parse_expr3(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    int tk_val, op_val;
    switch (tk->val) {
        case '<': case TK_OP_LE: case '>': case TK_OP_GE: case TK_OP_NE: case TK_OP_EQ: case TK_KW_IN:
            tk_val = tk->val;
            op_val = token_to_op_val(tk->val);
            next(ps);
            goto success;
        case TK_KW_IS:
            next(ps);
            if (tk->val == TK_KW_NOT) {
                op_val = OP_IS_NOT;
                next(ps);
            } else op_val = OP_IS;
            goto success;
        case TK_KW_NOT:
            next(ps);
            ACCEPT(ps, TK_KW_IN);
            op_val = OP_NOT_IN;
            goto success;
    }
    return;
success:
    parse_expr10(ps);
    parse_expr9(ps);
    parse_expr8(ps);
    parse_expr7(ps);
    parse_expr6(ps);
    parse_expr5(ps);
    parse_expr4(ps);
    //printf("%s\n", pylt_vm_get_op_name(op_val));
    kv_pushbc(ps->info->code->opcodes, BC_OPERATOR);
    kv_pushbc(ps->info->code->opcodes, op_val);
    parse_expr3(ps);
}


/* | EXPR10 ... EXPR4 | ε */
_INLINE void parse_expr4(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '|':
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            parse_expr5(ps);
            print_tk_val('|');
            kv_pushbc(ps->info->code->opcodes, BC_OPERATOR);
            kv_pushbc(ps->info->code->opcodes, OP_BITOR);
            parse_expr4(ps);
            break;
    }
}


/* ^ EXPR10 ... EXPR5 | ε */
_INLINE void parse_expr5(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '^':
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            print_tk_val('^');
            kv_pushbc(ps->info->code->opcodes, BC_OPERATOR);
            kv_pushbc(ps->info->code->opcodes, OP_BITXOR);
            parse_expr5(ps);
            break;
    }
}



/* & EXPR10 ... EXPR6 | ε */
_INLINE void parse_expr6(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '&':
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            print_tk_val('&');
            kv_pushbc(ps->info->code->opcodes, BC_OPERATOR);
            kv_pushbc(ps->info->code->opcodes, OP_BITAND);
            parse_expr6(ps);
            break;
    }
}


/* (<<|>>) EXPR10 ... EXPR7 | ε */
_INLINE void parse_expr7(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    int tk_val;
    switch (tk->val) {
        case TK_OP_LSHIFT: case TK_OP_RSHIFT:
            tk_val = tk->val;
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            print_tk_val(tk_val);
            kv_pushbc(ps->info->code->opcodes, BC_OPERATOR);
            kv_pushbc(ps->info->code->opcodes, token_to_op_val(tk_val));
            parse_expr7(ps);
            break;
    }
}


/* (+|-) EXPR10 ... EXPR8 | ε */
_INLINE void parse_expr8(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    int tk_val;
    switch (tk->val) {
        case '+': case '-':
            tk_val = tk->val;
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            print_tk_val(tk_val);
            kv_pushbc(ps->info->code->opcodes, BC_OPERATOR);
            kv_pushbc(ps->info->code->opcodes, token_to_op_val(tk_val));
            parse_expr8(ps);
            break;
    }
}


/* (*|@|/|//|%) EXPR10 ... EXPR9 | ε */
_INLINE void parse_expr9(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    int tk_val;
    switch (tk->val) {
        case '*': case '@': case '/': case TK_OP_FLOORDIV: case '%':
            tk_val = tk->val;
            next(ps);
            parse_expr10(ps);
            print_tk_val(tk_val);
            kv_pushbc(ps->info->code->opcodes, BC_OPERATOR);
            kv_pushbc(ps->info->code->opcodes, token_to_op_val(tk_val));
            parse_expr9(ps);
            break;
    }
}

/* T (** T | ε) */
_INLINE bool parse_try_expr10(ParserState *ps) {
    if (!parse_try_t(ps)) return false;
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_OP_POW:
            next(ps);
            parse_t(ps);
            print_tk_val(TK_OP_POW);
            kv_pushbc(ps->info->code->opcodes, BC_OPERATOR);
            kv_pushbc(ps->info->code->opcodes, OP_POW);
            break;
    }
    return true;
}

_INLINE void parse_expr10(ParserState *ps) {
    if (!parse_try_expr10(ps)) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
}

void parse_block(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    ACCEPT(ps, TK_INDENT);
    parse_stmts(ps);
    ACCEPT(ps, TK_DEDENT);
}


void parse_single_left_value(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    if (tk->val == TK_NAME) {
        next(ps);
        if (tk->val == '[') {
            if (!parse_try_index(ps)) {
                error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            }
            if (tk->val == '=') {
                kv_pop(ps->info->code->opcodes);
                next(ps);
                parse_expr(ps);
                //kv_pushbc(ps->info->code->opcodes, BC_SET_ITEM);
                //kv_pushbc(ps->info->code->opcodes, (uintptr_t)obj);
            }
        }
    }
}

void parse_left_value(ParserState *ps) {

}

void loop_control_replace(ParserState *ps, int start_pos) {
    for (unsigned int i = start_pos; i < kv_size(ps->info->code->opcodes);) {
        switch (kv_A(ps->info->code->opcodes, i)) {
            case BC_NEW_OBJ_EXTRA:i += 3; break;
            case BC_POP:
            case BC_PRINT:
            case BC_GET_ITEM:
            case BC_DEL_FORCE:
                i += 1; break;
            case BC_FAKE_BREAK:
                if (kv_A(ps->info->code->opcodes, i + 1) == ps->info->loop_depth + 1) {
                    kv_A(ps->info->code->opcodes, i) = BC_JMP;
                    kv_A(ps->info->code->opcodes, i + 1) = kv_size(ps->info->code->opcodes) - i;
                }
                break;
            case BC_FAKE_CONTINUE:
                if (kv_A(ps->info->code->opcodes, i + 1) == ps->info->loop_depth + 1) {
                    kv_A(ps->info->code->opcodes, i) = BC_JMP;
                    kv_A(ps->info->code->opcodes, i + 1) = kv_size(ps->info->code->opcodes) - i - 2;
                }
                break;
            default: i += 2; break;
        }
    }
}

void parse_func(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    PyLiteObject *func_name;
    ParserInfo *info;

    ACCEPT(ps, TK_KW_DEF);
    if (tk->val != TK_NAME) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
    func_name = tk->obj;
    next(ps);

    ACCEPT(ps, '(');
    
    PyLiteListObject* lst = pylt_obj_list_new(ps->state);
    
    while (true) {
        if (tk->val != TK_NAME) break;
        pylt_obj_list_append(ps->state, lst, tk->obj);
        next(ps);
        if (tk->val != ',') break;
        next(ps);
    }

    ACCEPT(ps, ')');
    ACCEPT(ps, ':');
    ACCEPT(ps, TK_NEWLINE);

    func_push(ps);
    ACCEPT(ps, TK_INDENT);
    parse_stmts(ps);
    ACCEPT(ps, TK_DEDENT);
    kv_pushobj(ps->info->code->const_val, castobj(pylt_obj_none_new(ps->state)));
    kv_pushbc(ps->info->code->opcodes, BC_LOADCONST);
    kv_pushbc(ps->info->code->opcodes, kv_size(ps->info->code->const_val));
    kv_pushbc(ps->info->code->opcodes, BC_RET);
    info = func_pop(ps);

    // name
    kv_pushobj(ps->info->code->const_val, func_name);
    kv_pushbc(ps->info->code->opcodes, BC_LOADCONST);
    kv_pushbc(ps->info->code->opcodes, kv_size(ps->info->code->const_val));

    // code
    kv_pushobj(ps->info->code->const_val, castobj(info->code));
    kv_pushbc(ps->info->code->opcodes, BC_LOADCONST);
    kv_pushbc(ps->info->code->opcodes, kv_size(ps->info->code->const_val));

    // params
    kv_pushobj(ps->info->code->const_val, castobj(lst));
    kv_pushbc(ps->info->code->opcodes, BC_LOADCONST);
    kv_pushbc(ps->info->code->opcodes, kv_size(ps->info->code->const_val));

    kv_pushbc(ps->info->code->opcodes, BC_NEW_OBJ);
    kv_pushbc(ps->info->code->opcodes, PYLT_OBJ_TYPE_FUNCTION);
}

void parse_stmt(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    PyLiteObject *obj;
    int tmp, tmp2, tmp3;
    int final_pos;

    switch (tk->val) {
        case TK_KW_IF:
            next(ps);
            parse_expr(ps);
            ACCEPT(ps, ':');
            kv_pushbc(ps->info->code->opcodes, BC_TEST);
            kv_pushbc(ps->info->code->opcodes, 0);
            tmp = tmp3 = kv_size(ps->info->code->opcodes);
            if (tk->val == TK_NEWLINE) {
                next(ps);
                parse_block(ps);
            } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            // TODO: stmt

            kv_A(ps->info->code->opcodes, tmp - 1) = kv_size(ps->info->code->opcodes) - tmp;

            tmp2 = 0;
            while (tk->val == TK_KW_ELIF) {
                next(ps);

                // jmp X
                kv_A(ps->info->code->opcodes, tmp - 1) += 2;
                kv_pushbc(ps->info->code->opcodes, BC_JMP);
                kv_pushbc(ps->info->code->opcodes, 0);
                tmp2 = kv_size(ps->info->code->opcodes);

                parse_expr(ps);
                ACCEPT(ps, ':');
                // test X
                kv_pushbc(ps->info->code->opcodes, BC_TEST);
                kv_pushbc(ps->info->code->opcodes, 0);
                tmp = kv_size(ps->info->code->opcodes);

                if (tk->val == TK_NEWLINE) {
                    next(ps);
                    parse_block(ps);
                } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);

                // write X for jmp X
                kv_A(ps->info->code->opcodes, tmp2 - 1) = kv_size(ps->info->code->opcodes) - tmp2 + 2;
                // write X for test X
                kv_A(ps->info->code->opcodes, tmp - 1) = kv_size(ps->info->code->opcodes) - tmp;
            }

            if (tk->val == TK_KW_ELSE) {
                next(ps);
                ACCEPT(ps, ':');
                kv_A(ps->info->code->opcodes, tmp - 1) += 2;
                // jmp X
                kv_pushbc(ps->info->code->opcodes, BC_JMP);
                kv_pushbc(ps->info->code->opcodes, 0);
                tmp = kv_size(ps->info->code->opcodes);

                if (tk->val == TK_NEWLINE) {
                    next(ps);
                    parse_block(ps);
                } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);

                // write X for jmp X
                kv_A(ps->info->code->opcodes, tmp - 1) = kv_size(ps->info->code->opcodes) - tmp;
            } else {
                // fix for last elif
                if (tmp2) {
                    kv_A(ps->info->code->opcodes, tmp - 1) = kv_size(ps->info->code->opcodes) - tmp;
                }
            }

            // new code pos
            final_pos = kv_size(ps->info->code->opcodes);
            // tmp3 <- pos of first jmp (if exists)
            tmp3 += kv_A(ps->info->code->opcodes, tmp3 - 1);

            // change every jmp point
            while (tmp3 != final_pos) {
                // tmp2 <- pos of next jmp (if exists) or block end
                tmp2 = tmp3 + kv_A(ps->info->code->opcodes, tmp3 - 1);
                kv_A(ps->info->code->opcodes, tmp3 - 1) = final_pos - tmp3;
                tmp3 = tmp2;
            }
            return;
        case TK_KW_WHILE:
            next(ps);
            tmp = kv_size(ps->info->code->opcodes);
            parse_expr(ps);
            ACCEPT(ps, ':');

            kv_pushbc(ps->info->code->opcodes, BC_TEST);
            kv_pushbc(ps->info->code->opcodes, 0);
            tmp2 = kv_size(ps->info->code->opcodes);

            if (tk->val == TK_NEWLINE) {
                next(ps);
                ++ps->info->loop_depth;
                parse_block(ps);
                --ps->info->loop_depth;
            } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            loop_control_replace(ps, tmp2);

            kv_pushbc(ps->info->code->opcodes, BC_JMP_BACK);
            kv_pushbc(ps->info->code->opcodes, (kv_size(ps->info->code->opcodes) - tmp + 1));
            kv_A(ps->info->code->opcodes, tmp2 - 1) = kv_size(ps->info->code->opcodes) - tmp2;
            return;
        case TK_KW_BREAK:
            next(ps);
            if (ps->info->loop_depth == 0) error(ps, PYLT_ERR_PARSER_BREAK_OUTSIDE_LOOP);
            else {
                kv_pushbc(ps->info->code->opcodes, BC_FAKE_BREAK);
                kv_pushbc(ps->info->code->opcodes, ps->info->loop_depth);
            }
            break;
        case TK_KW_CONTINUE:
            next(ps);
            if (ps->info->loop_depth == 0) error(ps, PYLT_ERR_PARSER_CONTINUE_OUTSIDE_LOOP);
            else {
                kv_pushbc(ps->info->code->opcodes, BC_FAKE_CONTINUE);
                kv_pushbc(ps->info->code->opcodes, ps->info->loop_depth);
            }
            break;
        case TK_KW_FOR:
            next(ps);
            if (tk->val == TK_NAME) {
                obj = tk->obj;
                next(ps);
                ACCEPT(ps, TK_KW_IN);
                parse_expr(ps);
                ACCEPT(ps, ':');
                kv_pushbc(ps->info->code->opcodes, BC_NEW_OBJ);
                kv_pushbc(ps->info->code->opcodes, PYLT_OBJ_TYPE_ITER);
                tmp = kv_size(ps->info->code->opcodes);
                kv_pushbc(ps->info->code->opcodes, BC_FORITER);
                kv_pushbc(ps->info->code->opcodes, 0);
                kv_pushbc(ps->info->code->opcodes, BC_SET_VAL);
                kv_pushbc(ps->info->code->opcodes, (uintptr_t)obj);
                kv_pushbc(ps->info->code->opcodes, BC_POP);

                ++ps->info->loop_depth;
                ACCEPT(ps, TK_NEWLINE);
                parse_block(ps);
                --ps->info->loop_depth;
                loop_control_replace(ps, tmp);

                kv_A(ps->info->code->opcodes, tmp + 1) = kv_size(ps->info->code->opcodes) - tmp + 1;
                kv_pushbc(ps->info->code->opcodes, BC_JMP_BACK);
                kv_pushbc(ps->info->code->opcodes, (kv_size(ps->info->code->opcodes) - tmp + 1));

                kv_pushbc(ps->info->code->opcodes, BC_DEL_FORCE);
            } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            return;
        case TK_KW_PASS:
            next(ps);
            break;
        case TK_KW_DEF:
            parse_func(ps);
            return;
        case TK_KW_RETURN:
            next(ps);
            if (!parse_try_expr(ps)) {
                kv_pushobj(ps->info->code->const_val, castobj(pylt_obj_none_new(ps->state)));
                kv_pushbc(ps->info->code->opcodes, BC_LOADCONST);
                kv_pushbc(ps->info->code->opcodes, kv_size(ps->info->code->const_val));
            }
            kv_pushbc(ps->info->code->opcodes, BC_RET);
            break;
        default:
            parse_expr(ps);
            kv_pushbc(ps->info->code->opcodes, BC_POP);
    }
    ACCEPT(ps, TK_NEWLINE);
}

void parse_stmts(ParserState *ps) {
    while (ps->ls->token.val != TK_END && ps->ls->token.val != TK_DEDENT) {
        parse_stmt(ps);
    }
}

void parse(ParserState *ps) {
    next(ps);
    //parse_expr(ps);
    parse_stmts(ps);
    kv_pushbc(ps->info->code->opcodes, BC_PRINT);
    kv_pushbc(ps->info->code->opcodes, BC_HALT);
}

void func_push(ParserState *ps) {
    ParserInfo *info;
    // 如果没有空槽
    if (!ps->info_used) {
        info = pylt_realloc(NULL, sizeof(ParserInfo));
    // 如果有用过的空槽
    } else {
        info = ps->info_used;
        ps->info_used = ps->info_used->prev;
    }

    // 初始化
    info->code = pylt_obj_code_snippet_new(ps->state);
    info->loop_depth = 0;
    info->prev = ps->info;
    ps->info = info;
}

ParserInfo* func_pop(ParserState *ps) {
    ParserInfo *info = ps->info;
    ps->info = ps->info->prev;

    info->prev = ps->info_used;
    ps->info_used = info;
    return info;
}

void pylt_parser_init(PyLiteState* state, ParserState *ps, LexState *ls) {
    ps->state = state;
    ps->ls = ls;

    ps->info = NULL;
    ps->info_used = NULL;
    func_push(ps);
}
