#include "../../include/qk/eval.h"
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

typedef enum {
    TOK_LP,
    TOK_RP,
    TOK_NUM,
    TOK_PLUS,
    TOK_MINUS,
    TOK_MUL,
    TOK_DIV,
    TOK_INVALID,
    TOK_END,
} token_type;

typedef enum {
    EVAL_INT,
    EVAL_FLOAT,
} eval_type;

typedef struct {
    token_type type;
    int len;
    union {
        int i;
        float f;
    };
} token;

static int eval(const char *str, eval_type type, void *r);
static token primary(const char **p, eval_type type);
static token unary(const char **p, eval_type type);
static token expr(const char **p, eval_type type);
static token add(const char **p, eval_type type);
static token mul(const char **p, eval_type type);

static token get_token(const char **p, eval_type type);
static void unget_token(token t, const char **p);

static token get_token(const char **p, eval_type type)
{
    while (isspace(**p)) (*p)++;

    char c = **p;

    switch (c) {
    case '*': return ((*p)++, (token){.len = 1, .type = TOK_MUL});
    case '/': return ((*p)++, (token){.len = 1, .type = TOK_DIV});
    case '(': return ((*p)++, (token){.len = 1, .type = TOK_LP});
    case ')': return ((*p)++, (token){.len = 1, .type = TOK_RP});
    case '+': return ((*p)++, (token){.len = 1, .type = TOK_PLUS});
    case '-': return ((*p)++, (token){.len = 1, .type = TOK_MINUS});
    case 0  : return (token){.len = 0, .type = TOK_END};
    }

    char *end;
    long l;
    token tok;

    switch (type) {
    case EVAL_INT:
        errno = 0;
        l = strtol(*p, &end, 0);
        if (end != *p) {
            tok.type = TOK_NUM;
            if (errno || (l > INT_MAX || l < INT_MIN))
                tok.type = TOK_INVALID;
            tok.i = l;
            tok.len = end - *p;
            *p = end;
            return tok;
        }
        break;
    case EVAL_FLOAT:
        errno = 0;
        tok.f = strtof(*p, &end);
        if (end != *p) {
            tok.type = TOK_NUM;
            if (errno)
                tok.type = TOK_INVALID;
            tok.len = end - *p;
            *p = end;
            return tok;
        }
        break;
    default:
        break;
    }

    (*p)++;
    return (token){.len = 1, .type = TOK_INVALID};
}

static void unget_token(token t, const char **p)
{
    (*p) -= t.len;
}

static token expr(const char **p, eval_type type)
{
    return add(p, type);
}

static token unary(const char **p, eval_type type)
{
    token left = get_token(p, type);
    token next;

    switch (left.type) {
    case TOK_MINUS:
        next = unary(p, type);
        switch (next.type) {
        case TOK_NUM:
            switch (type) {
            case EVAL_INT:
                next.i = -next.i;
                return next;
            case EVAL_FLOAT:
                next.f = -next.f;
                return next;
            }
            break;
        case TOK_INVALID:
            return next;
        default:
            unget_token(next, p);
            return unary(p, type);
        }
        break;
    case TOK_PLUS:
        return unary(p, type);
    case TOK_INVALID:
        return left;
    default:
        break;
    }
    unget_token(left, p);
    return primary(p, type);
}

static token primary(const char **p, eval_type type)
{
    token tok = get_token(p, type);
    token tmp;
    token next;
    switch (tok.type) {
    case TOK_NUM:
        return tok;
    case TOK_LP:
        tmp = expr(p, type);
        if (tmp.type == TOK_INVALID) return tmp;
        next = get_token(p, type);
        if (next.type != TOK_RP)
            return (token){.type = TOK_INVALID, .len = next.len};
        return tmp;
    default:
        tok.type = TOK_INVALID;
        return tok;
    }
}


static token mul(const char **p, eval_type type)
{
    token left = unary(p, type);
    if (left.type == TOK_INVALID) return left;

    token op;
    token right;

    for (;;) {
        op = get_token(p, type);
        switch (op.type) {
        case TOK_MUL:
            right = unary(p, type);
            if (right.type == TOK_INVALID) return right;
            switch (type) {
            case EVAL_INT:
                left.i *= right.i;
                break;
            case EVAL_FLOAT:
                left.f *= right.f;
                break;
            }
            break;
        case TOK_DIV:
            right = unary(p, type);
            if (right.type == TOK_INVALID) return right;
            switch (type) {
            case EVAL_INT:
                if (right.i == 0)
                    return (token){.type = TOK_INVALID, .len = right.len};
                left.i /= right.i;
                break;
            case EVAL_FLOAT:
                left.f /= right.f;
                break;
            }
            break;
        case TOK_INVALID:
            return op;
        default:
            unget_token(op, p);
            return left;
        }
    }
}

static token add(const char **p, eval_type type)
{
    token left = mul(p, type);
    if (left.type == TOK_INVALID) return left;

    token op;
    token right;

    for (;;) {
        op = get_token(p, type);
        switch (op.type) {
        case TOK_PLUS:
            right = mul(p, type);
            if (right.type == TOK_INVALID) return right;
            switch (type) {
            case EVAL_INT:
                left.i += right.i;
                break;
            case EVAL_FLOAT:
                left.f += right.f;
                break;
            }
            break;
        case TOK_MINUS:
            right = mul(p, type);
            if (right.type == TOK_INVALID) return right;
            switch (type) {
            case EVAL_INT:
                left.i -= right.i;
                break;
            case EVAL_FLOAT:
                left.f -= right.f;
                break;
            }
            break;
        case TOK_INVALID:
            return op;
        default:
            unget_token(op, p);
            return left;
        }
    }
}

static int eval(const char *str, eval_type type, void *r)
{
    const char **p = &str;
    token tok = expr(p, type);
    if (tok.type == TOK_INVALID)
        return errno ? QK_ERRNO : QK_INVALID;
    token next = get_token(p, type);
    if (next.type != TOK_END) return QK_INVALID;
    if (r) {
        switch (type) {
        case EVAL_INT:
            *(int*)r = tok.i;
            break;
        case EVAL_FLOAT:
            *(float*)r = tok.f;
            break;
        }
    }
    return QK_OK;
}

QKAPI int qk_eval_i(const char *str, int *r)
{
    return eval(str, EVAL_INT, r);
}

QKAPI int qk_eval_f(const char *str, float *r)
{
    return eval(str, EVAL_FLOAT, r);
}
