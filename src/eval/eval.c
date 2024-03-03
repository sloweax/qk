#include "../../include/qk/eval.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TOK_LP,
    TOK_RP,
    TOK_NUM,
    TOK_PLUS,
    TOK_MINUS,
    TOK_MUL,
    TOK_DIV,
    TOK_INVALID,
    TOK_IDENTIFIER,
    TOK_END,
} token_type;

typedef enum {
    ID_UNKNOWN,
    ID_COS,
    ID_SIN,
    ID_TAN,
    ID_ACOS,
    ID_ASIN,
    ID_ATAN,
    ID_SQRT,
} identifier_type;

typedef struct {
    token_type type;
    identifier_type id;
    int len;
    qk_eval_result value;
} token;

// primary = number
//         | "(" expr ")"
//         | func
static token primary(const char **p);
// unary = primary
//       | ("+"|"-") unary
static token unary(const char **p);
// func = identifier "(" unary ")"
static token func(const char **p);
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
static token sqrt_token(token a);
static token cos_token(token a);
static token sin_token(token a);
static token tan_token(token a);
static token acos_token(token a);
static token asin_token(token a);
static token atan_token(token a);

static token sqrt_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_INT:
        a.value.type = QK_EVAL_RESULT_FLOAT;
        a.value.f = sqrtf((float)a.value.i);
        return a;
    case QK_EVAL_RESULT_FLOAT:
        a.value.f = sqrtf(a.value.f);
        return a;
    default:
        return a;
    }
}

static token atan_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_INT:
        a.value.type = QK_EVAL_RESULT_FLOAT;
        a.value.f = atanf((float)a.value.i);
        return a;
    case QK_EVAL_RESULT_FLOAT:
        a.value.f = atanf(a.value.f);
        return a;
    default:
        return a;
    }
}

static token tan_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_INT:
        a.value.type = QK_EVAL_RESULT_FLOAT;
        a.value.f = tanf((float)a.value.i);
        return a;
    case QK_EVAL_RESULT_FLOAT:
        a.value.f = tanf(a.value.f);
        return a;
    default:
        return a;
    }
}

static token sin_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_INT:
        a.value.type = QK_EVAL_RESULT_FLOAT;
        a.value.f = sinf((float)a.value.i);
        return a;
    case QK_EVAL_RESULT_FLOAT:
        a.value.f = sinf(a.value.f);
        return a;
    default:
        return a;
    }
}

static token asin_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_INT:
        a.value.type = QK_EVAL_RESULT_FLOAT;
        a.value.f = asinf((float)a.value.i);
        return a;
    case QK_EVAL_RESULT_FLOAT:
        a.value.f = asinf(a.value.f);
        return a;
    default:
        return a;
    }
}

static token cos_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_INT:
        a.value.type = QK_EVAL_RESULT_FLOAT;
        a.value.f = cosf((float)a.value.i);
        return a;
    case QK_EVAL_RESULT_FLOAT:
        a.value.f = cosf(a.value.f);
        return a;
    default:
        return a;
    }
}

static token acos_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_INT:
        a.value.type = QK_EVAL_RESULT_FLOAT;
        a.value.f = acosf((float)a.value.i);
        return a;
    case QK_EVAL_RESULT_FLOAT:
        a.value.f = acosf(a.value.f);
        return a;
    default:
        return a;
    }
}

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

    const char *i = *p;

    while (isalnum(*i)) i++;

    if (i != *p) {
        tok.len = i - *p;
        tok.type = TOK_IDENTIFIER;
        switch (tok.len) {
        case 4:
            if (strncmp(*p, "acos", 4) == 0)
                tok.id = ID_ACOS;
            else if (strncmp(*p, "atan", 4) == 0)
                tok.id = ID_ATAN;
            else if (strncmp(*p, "asin", 4) == 0)
                tok.id = ID_ASIN;
            else if (strncmp(*p, "sqrt", 4) == 0)
                tok.id = ID_SQRT;
            break;
        case 3:
            if (strncmp(*p, "cos", 3) == 0)
                tok.id = ID_COS;
            else if (strncmp(*p, "tan", 3) == 0)
                tok.id = ID_TAN;
            else if (strncmp(*p, "sin", 3) == 0)
                tok.id = ID_SIN;
            break;
        default:
            tok.id = ID_UNKNOWN;
            break;
        }
        *p = i;
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

static token func(const char **p)
{
    token identifier = get_token(p);
    if (identifier.type != TOK_IDENTIFIER) return (token){.type = TOK_INVALID};
    if (identifier.id == ID_UNKNOWN) return (token){.type = TOK_INVALID};
    token tmp = get_token(p);
    if (tmp.type != TOK_LP) return (token){.type = TOK_INVALID};
    unget_token(tmp, p);
    token arg = unary(p);
    if (arg.type == TOK_INVALID) return arg;
    switch (identifier.id) {
    case ID_SQRT:
        return sqrt_token(arg);
    case ID_COS:
        return cos_token(arg);
    case ID_SIN:
        return sin_token(arg);
    case ID_TAN:
        return tan_token(arg);
    case ID_ACOS:
        return acos_token(arg);
    case ID_ASIN:
        return asin_token(arg);
    case ID_ATAN:
        return atan_token(arg);
    default:
        return (token){.type = TOK_INVALID};
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
            return (token){.type = TOK_INVALID};
        return tmp;
    case TOK_IDENTIFIER:
        unget_token(tok, p);
        return func(p);
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
