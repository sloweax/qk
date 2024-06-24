#include "../../include/qk/eval.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TOK_COMMA,
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
    ID_POW,
    ID_COS,
    ID_SIN,
    ID_TAN,
    ID_ACOS,
    ID_ASIN,
    ID_ATAN,
    ID_SQRT,
} identifier_type;

#define MAX_NARGS 2

typedef struct {
    token_type type;
    identifier_type id;
    int nargs;
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
// func = identifier "(" expr [, expr...] ")"
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
static token pow_token(token a, token b);
static token cos_token(token a);
static token sin_token(token a);
static token tan_token(token a);
static token acos_token(token a);
static token asin_token(token a);
static token atan_token(token a);

static token pow_token(token a, token b)
{
    double ad, bd;
    token r;
    switch (a.value.type) {
    case QK_EVAL_RESULT_LONG:
        ad = ((double)a.value.l);
        break;
    default:
        ad = a.value.d;
        break;
    }
    switch (b.value.type) {
    case QK_EVAL_RESULT_LONG:
        bd = ((double)b.value.l);
        break;
    default:
        bd = b.value.d;
        break;
    }
    r.type = a.type;
    r.value.type = QK_EVAL_RESULT_DOUBLE;
    r.value.d = pow(ad, bd);
    return r;
}

static token sqrt_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_LONG:
        a.value.type = QK_EVAL_RESULT_DOUBLE;
        a.value.d = sqrt((double)a.value.l);
        return a;
    case QK_EVAL_RESULT_DOUBLE:
        a.value.d = sqrt(a.value.d);
        return a;
    default:
        return a;
    }
}

static token atan_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_LONG:
        a.value.type = QK_EVAL_RESULT_DOUBLE;
        a.value.d = atan((double)a.value.l);
        return a;
    case QK_EVAL_RESULT_DOUBLE:
        a.value.d = atan(a.value.d);
        return a;
    default:
        return a;
    }
}

static token tan_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_LONG:
        a.value.type = QK_EVAL_RESULT_DOUBLE;
        a.value.d = tan((double)a.value.l);
        return a;
    case QK_EVAL_RESULT_DOUBLE:
        a.value.d = tan(a.value.d);
        return a;
    default:
        return a;
    }
}

static token sin_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_LONG:
        a.value.type = QK_EVAL_RESULT_DOUBLE;
        a.value.d = sin((double)a.value.l);
        return a;
    case QK_EVAL_RESULT_DOUBLE:
        a.value.d = sin(a.value.d);
        return a;
    default:
        return a;
    }
}

static token asin_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_LONG:
        a.value.type = QK_EVAL_RESULT_DOUBLE;
        a.value.d = asin((double)a.value.l);
        return a;
    case QK_EVAL_RESULT_DOUBLE:
        a.value.d = asin(a.value.d);
        return a;
    default:
        return a;
    }
}

static token cos_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_LONG:
        a.value.type = QK_EVAL_RESULT_DOUBLE;
        a.value.d = cos((double)a.value.l);
        return a;
    case QK_EVAL_RESULT_DOUBLE:
        a.value.d = cos(a.value.d);
        return a;
    default:
        return a;
    }
}

static token acos_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_LONG:
        a.value.type = QK_EVAL_RESULT_DOUBLE;
        a.value.d = acos((double)a.value.l);
        return a;
    case QK_EVAL_RESULT_DOUBLE:
        a.value.d = acos(a.value.d);
        return a;
    default:
        return a;
    }
}

static bool is_zero_token(token a)
{
    switch(a.value.type) {
    case QK_EVAL_RESULT_DOUBLE:
        return a.value.d == 0.0;
    case QK_EVAL_RESULT_LONG:
        return a.value.l == 0;
    default:
        return true;
    }
}

static token neg_token(token a)
{
    switch (a.value.type) {
    case QK_EVAL_RESULT_LONG:
        a.value.l = -a.value.l;
        break;
    case QK_EVAL_RESULT_DOUBLE:
        a.value.d = -a.value.d;
        break;
    }
    return a;
}

static token mul_token(token a, token b)
{
    token r;
    switch (a.value.type) {
    case QK_EVAL_RESULT_LONG:
        switch (b.value.type) {
        case QK_EVAL_RESULT_LONG:
            r.value.type = QK_EVAL_RESULT_LONG;
            r.value.l = a.value.l * b.value.l;
            break;
        case QK_EVAL_RESULT_DOUBLE:
            r.value.type = QK_EVAL_RESULT_DOUBLE;
            r.value.d = (double)a.value.l * b.value.d;
            break;
        }
        break;
    case QK_EVAL_RESULT_DOUBLE:
        r.value.type = QK_EVAL_RESULT_DOUBLE;
        switch (b.value.type) {
        case QK_EVAL_RESULT_LONG:
            r.value.d = a.value.d * (double)b.value.l;
            break;
        case QK_EVAL_RESULT_DOUBLE:
            r.value.d = a.value.d * b.value.d;
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
    case QK_EVAL_RESULT_LONG:
        switch (b.value.type) {
        case QK_EVAL_RESULT_LONG:
            r.value.type = QK_EVAL_RESULT_LONG;
            r.value.l = a.value.l / b.value.l;
            break;
        case QK_EVAL_RESULT_DOUBLE:
            r.value.type = QK_EVAL_RESULT_DOUBLE;
            r.value.d = (double)a.value.l / b.value.d;
            break;
        }
        break;
    case QK_EVAL_RESULT_DOUBLE:
        r.value.type = QK_EVAL_RESULT_DOUBLE;
        switch (b.value.type) {
        case QK_EVAL_RESULT_LONG:
            r.value.d = a.value.d / (double)b.value.l;
            break;
        case QK_EVAL_RESULT_DOUBLE:
            r.value.d = a.value.d / b.value.d;
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
    case QK_EVAL_RESULT_LONG:
        switch (b.value.type) {
        case QK_EVAL_RESULT_LONG:
            r.value.type = QK_EVAL_RESULT_LONG;
            r.value.l = a.value.l + b.value.l;
            break;
        case QK_EVAL_RESULT_DOUBLE:
            r.value.type = QK_EVAL_RESULT_DOUBLE;
            r.value.d = (double)a.value.l + b.value.d;
            break;
        }
        break;
    case QK_EVAL_RESULT_DOUBLE:
        r.value.type = QK_EVAL_RESULT_DOUBLE;
        switch (b.value.type) {
        case QK_EVAL_RESULT_LONG:
            r.value.d = a.value.d + (double)b.value.l;
            break;
        case QK_EVAL_RESULT_DOUBLE:
            r.value.d = a.value.d + b.value.d;
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
    case QK_EVAL_RESULT_LONG:
        switch (b.value.type) {
        case QK_EVAL_RESULT_LONG:
            r.value.type = QK_EVAL_RESULT_LONG;
            r.value.l = a.value.l - b.value.l;
            break;
        case QK_EVAL_RESULT_DOUBLE:
            r.value.type = QK_EVAL_RESULT_DOUBLE;
            r.value.d = (double)a.value.l - b.value.d;
            break;
        }
        break;
    case QK_EVAL_RESULT_DOUBLE:
        r.value.type = QK_EVAL_RESULT_DOUBLE;
        switch (b.value.type) {
        case QK_EVAL_RESULT_LONG:
            r.value.d = a.value.d - (double)b.value.l;
            break;
        case QK_EVAL_RESULT_DOUBLE:
            r.value.d = a.value.d - b.value.d;
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
    case ',': return ((*p)++, (token){.len = 1, .type = TOK_COMMA});
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
        tok.value.type = QK_EVAL_RESULT_LONG;
        tok.type = TOK_NUM;
        if (errno)
            tok.type = TOK_INVALID;
        tok.value.l = l;
        tok.len = end - *p;
        *p = end;
        return tok;
    }

    if (*end == '.') {
        errno = 0;
        tok.value.d = strtod(*p, &end);
        if (end != *p) {
            tok.value.type = QK_EVAL_RESULT_DOUBLE;
            tok.type = TOK_NUM;
            if (errno)
                tok.type = TOK_INVALID;
            tok.len = end - *p;
            *p = end;
            return tok;
        }
    }

    const char *i = *p;

    while (isalnum(*i)) i++;

    if (i != *p) {
        tok.len = i - *p;
        tok.type = TOK_IDENTIFIER;
        switch (tok.len) {
        case 4:
            if (strncmp(*p, "acos", 4) == 0) {
                tok.nargs = 1;
                tok.id = ID_ACOS;
            }
            else if (strncmp(*p, "atan", 4) == 0) {
                tok.nargs = 1;
                tok.id = ID_ATAN;
            }
            else if (strncmp(*p, "asin", 4) == 0) {
                tok.nargs = 1;
                tok.id = ID_ASIN;
            }
            else if (strncmp(*p, "sqrt", 4) == 0) {
                tok.nargs = 1;
                tok.id = ID_SQRT;
            }
            break;
        case 3:
            if (strncmp(*p, "cos", 3) == 0) {
                tok.nargs = 1;
                tok.id = ID_COS;
            }
            else if (strncmp(*p, "tan", 3) == 0) {
                tok.nargs = 1;
                tok.id = ID_TAN;
            }
            else if (strncmp(*p, "sin", 3) == 0) {
                tok.nargs = 1;
                tok.id = ID_SIN;
            }
            else if (strncmp(*p, "pow", 3) == 0) {
                tok.nargs = 2;
                tok.id = ID_POW;
            }
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

    switch (left.type) {
    case TOK_MINUS:
        return neg_token(unary(p));
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

    token args[MAX_NARGS];
    for (int i = 0; i < identifier.nargs; i++) {
        token arg = expr(p);
        if (arg.type == TOK_INVALID) return arg;
        args[i] = arg;
        if (i == identifier.nargs - 1)
            break;
        tmp = get_token(p);
        if (tmp.type != TOK_COMMA) return (token){.type = TOK_INVALID};
    }

    tmp = get_token(p);
    if (tmp.type != TOK_RP)
        return (token){.type = TOK_INVALID};

    switch (identifier.id) {
    case ID_POW:
        return pow_token(args[0], args[1]);
    case ID_SQRT:
        return sqrt_token(args[0]);
    case ID_COS:
        return cos_token(args[0]);
    case ID_SIN:
        return sin_token(args[0]);
    case ID_TAN:
        return tan_token(args[0]);
    case ID_ACOS:
        return acos_token(args[0]);
    case ID_ASIN:
        return asin_token(args[0]);
    case ID_ATAN:
        return atan_token(args[0]);
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

QKAPI int qk_eval_l(const char *str, long *r)
{
    qk_eval_result result;
    int s = qk_eval(str, &result);
    if (s != QK_OK) return s;
    if (result.type != QK_EVAL_RESULT_LONG) return QK_INVALID;
    if (r) *r = result.l;
    return QK_OK;
}

QKAPI int qk_eval_d(const char *str, double *r)
{
    qk_eval_result result;
    int s = qk_eval(str, &result);
    if (s != QK_OK) return s;
    switch (result.type) {
    case QK_EVAL_RESULT_DOUBLE:
        if (r) *r = result.d;
        break;
    case QK_EVAL_RESULT_LONG:
        if (r) *r = (double)result.l;
        break;
    }
    return QK_OK;
}
