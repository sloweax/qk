#include "../../include/qk/eval.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

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

typedef struct {
    token_type type;
    int len;
    qk_eval_result value;
} token;

// primary = number
//         | "(" expr ")"
static token primary(const char **p);
// unary = primary
//       | ("+"|"-") unary
static token unary(const char **p);
// mul = unary
//     | unary ("*"|"/") unary
static token mul(const char **p);
// add = mul
//     | mul ("+"|"-") mul
static token add(const char **p);
// expr = add
static token expr(const char **p);

static token get_token(const char **p);
static void unget_token(token t, const char **p);
static token add_token(token a, token b);
static token sub_token(token a, token b);
static token mul_token(token a, token b);
static token div_token(token a, token b);
static token neg_token(token a);
static bool is_zero_token(token a);

static bool is_zero_token(token a)
{
    switch(a.value.type) {
    case QK_EVAL_RESULT_FLOAT:
        return a.value.f == 0.0;
    case QK_EVAL_RESULT_INT:
        return a.value.i == 0;
    default:
        return true;
    }
}

static token neg_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_INT:
        a.value.i = -a.value.i;
        break;
    case QK_EVAL_RESULT_FLOAT:
        a.value.f = -a.value.f;
        break;
    }
    return a;
}

static token mul_token(token a, token b)
{
    token r;
    switch (a.value.type) {
    case QK_EVAL_RESULT_INT:
        switch (b.value.type) {
        case QK_EVAL_RESULT_INT:
            r.value.type = QK_EVAL_RESULT_INT;
            r.value.i = a.value.i * b.value.i;
            break;
        case QK_EVAL_RESULT_FLOAT:
            r.value.type = QK_EVAL_RESULT_FLOAT;
            r.value.f = (float)a.value.i * b.value.f;
            break;
        }
        break;
    case QK_EVAL_RESULT_FLOAT:
        r.value.type = QK_EVAL_RESULT_FLOAT;
        switch (b.value.type) {
        case QK_EVAL_RESULT_INT:
            r.value.f = a.value.f * (float)b.value.i;
            break;
        case QK_EVAL_RESULT_FLOAT:
            r.value.f = a.value.f * b.value.f;
            break;
        }
        break;
    }
    return r;
}

static token div_token(token a, token b)
{
    token r;
    r.type = a.type;
    if (is_zero_token(b)) return (token){.type = TOK_INVALID};

    switch (a.value.type) {
    case QK_EVAL_RESULT_INT:
        switch (b.value.type) {
        case QK_EVAL_RESULT_INT:
            r.value.type = QK_EVAL_RESULT_INT;
            r.value.i = a.value.i / b.value.i;
            break;
        case QK_EVAL_RESULT_FLOAT:
            r.value.type = QK_EVAL_RESULT_FLOAT;
            r.value.f = (float)a.value.i / b.value.f;
            break;
        }
        break;
    case QK_EVAL_RESULT_FLOAT:
        r.value.type = QK_EVAL_RESULT_FLOAT;
        switch (b.value.type) {
        case QK_EVAL_RESULT_INT:
            r.value.f = a.value.f / (float)b.value.i;
            break;
        case QK_EVAL_RESULT_FLOAT:
            r.value.f = a.value.f / b.value.f;
            break;
        }
        break;
    }
    return r;
}

static token add_token(token a, token b)
{
    token r;
    switch (a.value.type) {
    case QK_EVAL_RESULT_INT:
        switch (b.value.type) {
        case QK_EVAL_RESULT_INT:
            r.value.type = QK_EVAL_RESULT_INT;
            r.value.i = a.value.i + b.value.i;
            break;
        case QK_EVAL_RESULT_FLOAT:
            r.value.type = QK_EVAL_RESULT_FLOAT;
            r.value.f = (float)a.value.i + b.value.f;
            break;
        }
        break;
    case QK_EVAL_RESULT_FLOAT:
        r.value.type = QK_EVAL_RESULT_FLOAT;
        switch (b.value.type) {
        case QK_EVAL_RESULT_INT:
            r.value.f = a.value.f + (float)b.value.i;
            break;
        case QK_EVAL_RESULT_FLOAT:
            r.value.f = a.value.f + b.value.f;
            break;
        }
        break;
    }
    return r;
}

static token sub_token(token a, token b)
{
    token r;
    switch (a.value.type) {
    case QK_EVAL_RESULT_INT:
        switch (b.value.type) {
        case QK_EVAL_RESULT_INT:
            r.value.type = QK_EVAL_RESULT_INT;
            r.value.i = a.value.i - b.value.i;
            break;
        case QK_EVAL_RESULT_FLOAT:
            r.value.type = QK_EVAL_RESULT_FLOAT;
            r.value.f = (float)a.value.i - b.value.f;
            break;
        }
        break;
    case QK_EVAL_RESULT_FLOAT:
        r.value.type = QK_EVAL_RESULT_FLOAT;
        switch (b.value.type) {
        case QK_EVAL_RESULT_INT:
            r.value.f = a.value.f - (float)b.value.i;
            break;
        case QK_EVAL_RESULT_FLOAT:
            r.value.f = a.value.f - b.value.f;
            break;
        }
        break;
    }
    return r;
}

static token get_token(const char **p)
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

    errno = 0;
    l = strtol(*p, &end, 0);
    if (end != *p && *end != '.') {
        tok.value.type = QK_EVAL_RESULT_INT;
        tok.type = TOK_NUM;
        if (errno || (l > INT_MAX || l < INT_MIN))
            tok.type = TOK_INVALID;
        tok.value.i = l;
        tok.len = end - *p;
        *p = end;
        return tok;
    }

    errno = 0;
    tok.value.f = strtof(*p, &end);
    if (end != *p) {
        tok.value.type = QK_EVAL_RESULT_FLOAT;
        tok.type = TOK_NUM;
        if (errno)
            tok.type = TOK_INVALID;
        tok.len = end - *p;
        *p = end;
        return tok;
    }

    (*p)++;
    return (token){.len = 1, .type = TOK_INVALID};
}

static void unget_token(token t, const char **p)
{
    (*p) -= t.len;
}

static token expr(const char **p)
{
    return add(p);
}

static token unary(const char **p)
{
    token left = get_token(p);
    token next;

    switch (left.type) {
    case TOK_MINUS:
        next = unary(p);
        switch (next.type) {
        case TOK_NUM:
            return neg_token(next);
        case TOK_INVALID:
            return next;
        default:
            return unary(p);
        }
    case TOK_PLUS:
        return unary(p);
    case TOK_INVALID:
        return left;
    default:
        unget_token(left, p);
        return primary(p);
    }
}

static token primary(const char **p)
{
    token tok = get_token(p);
    token tmp;
    token next;
    switch (tok.type) {
    case TOK_NUM:
        return tok;
    case TOK_LP:
        tmp = expr(p);
        if (tmp.type == TOK_INVALID) return tmp;
        next = get_token(p);
        if (next.type != TOK_RP)
            return (token){.type = TOK_INVALID, .len = next.len};
        return tmp;
    default:
        tok.type = TOK_INVALID;
        return tok;
    }
}

static token mul(const char **p)
{
    token left = unary(p);
    if (left.type == TOK_INVALID) return left;

    token op;
    token right;

    for (;;) {
        op = get_token(p);
        switch (op.type) {
        case TOK_MUL:
            right = unary(p);
            if (right.type == TOK_INVALID) return right;
            left = mul_token(left, right);
            break;
        case TOK_DIV:
            right = unary(p);
            if (right.type == TOK_INVALID) return right;
            left = div_token(left, right);
            if (left.type == TOK_INVALID) return left;
            break;
        case TOK_INVALID:
            return op;
        default:
            unget_token(op, p);
            return left;
        }
    }
}

static token add(const char **p)
{
    token left = mul(p);
    if (left.type == TOK_INVALID) return left;

    token op;
    token right;

    for (;;) {
        op = get_token(p);
        switch (op.type) {
        case TOK_PLUS:
            right = mul(p);
            if (right.type == TOK_INVALID) return right;
            left = add_token(left, right);
            break;
        case TOK_MINUS:
            right = mul(p);
            if (right.type == TOK_INVALID) return right;
            left = sub_token(left, right);
            break;
        case TOK_INVALID:
            return op;
        default:
            unget_token(op, p);
            return left;
        }
    }
}

QKAPI int qk_eval(const char *str, qk_eval_result *r)
{
    const char **p = &str;
    token tok = expr(p);

    if (tok.type == TOK_INVALID)
        return errno ? QK_ERRNO : QK_INVALID;

    token next = get_token(p);
    if (next.type != TOK_END) return QK_INVALID;

    if (r)
        *r = tok.value;

    return QK_OK;
}

QKAPI int qk_eval_i(const char *str, int *r)
{
    qk_eval_result result;
    int s = qk_eval(str, &result);
    if (s != QK_OK) return s;
    if (result.type != QK_EVAL_RESULT_INT) return QK_INVALID;
    if (r) *r = result.i;
    return QK_OK;
}

QKAPI int qk_eval_f(const char *str, float *r)
{
    qk_eval_result result;
    int s = qk_eval(str, &result);
    if (s != QK_OK) return s;
    switch (result.type) {
    case QK_EVAL_RESULT_FLOAT:
        if (r) *r = result.f;
        break;
    case QK_EVAL_RESULT_INT:
        if (r) *r = (float)result.i;
        break;
    }
    return QK_OK;
}
