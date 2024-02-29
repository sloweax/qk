#include "../../include/qk/eval.h"
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

typedef enum {
    EVAL_INT,
    EVAL_FLOAT,
} eval_type;

typedef enum {
    TOK_LP,
    TOK_RP,
    TOK_PLUS,
    TOK_MINUS,
    TOK_MUL,
    TOK_DIV,
    TOK_NUM,
    TOK_END,
} token_type;

typedef enum {
    NODE_NUM,
    NODE_MUL,
    NODE_DIV,
    NODE_ADD,
    NODE_SUB,
    NODE_NEG,
} node_type;

typedef struct token {
    token_type type;
    int len;
    const char *value;
    struct token *next;
} token;

typedef struct node {
    node_type type;
    struct node *left, *right;
    union {
        int i;
        float f;
    };
} node;

// primary = add | number
static node *primary(token **rest, token *tok, eval_type type);
// unary = (+ | -) primary
static node *unary(token **rest, token *tok, eval_type type);
// mul = unary (* | /) unary
static node *mul(token **rest, token *tok, eval_type type);
// add = mul (+ | -) mul
static node *add(token **rest, token *tok, eval_type type);

static int evalx(const char *str, void *r, int type);

static node *node_create();
static void node_free(node *n);
static void node_free_all(node *node);
static token *token_create();
static void token_free(token *t);
static void token_free_all(token *tokens);
static node *binary_create(node *l, node *r);
static int tokenize(const char *data, eval_type type, token *root);
static token *skip(token *token, token_type type);
static int solve_tree(node *root, int type);

static node *node_create()
{
    node *n = QK_MALLOC(sizeof(node));
    if (n == NULL) return n;
    n->left = n->right = NULL;
    return n;
}

static token *token_create()
{
    token *r = QK_MALLOC(sizeof(token));
    if (r == NULL) return r;
    r->next = NULL;
    return r;
}

static void token_free(token *t)
{
    if (t)
        QK_FREE(t);
}

static void node_free(node *n)
{
    if (n)
        QK_FREE(n);
}

static int tokenize(const char *data, eval_type type, token *root)
{
    switch (type) {
    case EVAL_INT:
    case EVAL_FLOAT:
        break;
    default:
        return QK_INVALID;
    }

    token *tok = root;
    token *tmp;
    size_t pos = 0;

    while (data[pos]) {
        switch (data[pos]) {
        case '(':
            tok->len = 1;
            tok->type = TOK_LP;
            goto next;
        case ')':
            tok->len = 1;
            tok->type = TOK_RP;
            goto next;
        case '+':
            tok->len = 1;
            tok->type = TOK_PLUS;
            goto next;
        case '-':
            tok->len = 1;
            tok->type = TOK_MINUS;
            goto next;
        case '*':
            tok->len = 1;
            tok->type = TOK_MUL;
            goto next;
        case '/':
            tok->len = 1;
            tok->type = TOK_DIV;
            goto next;
        default:
            if (isspace((unsigned char)data[pos])) {
                pos++;
                continue;
            }
        }

        int r;
        char *end;
        switch (type) {
        case EVAL_INT:
            errno = 0;
            r = strtol(&data[pos], &end, 0);
            if (errno) return QK_ERRNO;
            if (r > INT_MAX || r < INT_MIN) {
                errno = ERANGE;
            }
            if (errno) return QK_ERRNO;
            if (end != &data[pos]) {
                tok->type = TOK_NUM;
                tok->len = end - &data[pos];
                goto next;
            }
            break;
        case EVAL_FLOAT:
            errno = 0;
            strtof(&data[pos], &end);
            if (errno) return QK_ERRNO;
            if (end != &data[pos]) {
                tok->type = TOK_NUM;
                tok->len = end - &data[pos];
                goto next;
            }
            break;
        }

        return QK_INVALID;

    next:
        tok->value = &data[pos];
        pos += tok->len;
        tmp = token_create();
        tok->next = tmp;
        if (tmp == NULL) return QK_ERRNO;
        tok = tmp;
        continue;
    }

    tok->type = TOK_END;

    return QK_OK;
}

static token *skip(token *token, token_type type)
{
    if (token->type != type)
        return NULL;
    return token->next;
}

static node *binary_create(node *l, node *r)
{
    if (l == NULL || r == NULL) return NULL;
    node *n = node_create();
    if (n == NULL) {
        node_free_all(l);
        node_free_all(r);
        return NULL;
    }
    n->left = l;
    n->right = r;
    return n;
}

static node *unary(token **rest, token *tok, eval_type type)
{
    node *n;
    switch (tok->type) {
    case TOK_MINUS:
        n = node_create();
        if (n == NULL) return n;
        n->left = unary(rest, tok->next, type);
        if (n->left == NULL) {
            node_free_all(n);
            return NULL;
        }
        n->type = NODE_NEG;
        return n;
    case TOK_PLUS:
        return unary(rest, tok->next, type);
    default:
        return primary(rest, tok, type);
    }
}

static node *primary(token **rest, token *tok, eval_type type)
{
    node *n;
    switch (tok->type) {
    case TOK_NUM:
        n = node_create();
        if (n == NULL) return n;
        n->type = NODE_NUM;
        switch (type) {
        case EVAL_INT:
            n->i = strtol(tok->value, NULL, 0);
            break;
        case EVAL_FLOAT:
            n->f = strtof(tok->value, NULL);
            break;
        default:
            node_free(n);
            return NULL;
        }
        *rest = tok->next;
        return n;
    case TOK_LP:
        n = add(&tok, tok->next, type);
        if (n == NULL) return NULL;
        *rest = skip(tok, TOK_RP);
        if (*rest == NULL) {
            node_free_all(n);
            return NULL;
        }
        return n;
    default:
        return NULL;
    }
}

static node *add(token **rest, token *tok, eval_type type)
{
    node *n = mul(&tok, tok, type);
    if (n == NULL) return n;
    node *tmp;

    for (;;) {
        switch (tok->type) {
        case TOK_PLUS:
            tmp = binary_create(n, mul(&tok, tok->next, type));
            if (tmp == NULL) {
                node_free_all(n);
                return NULL;
            }
            n = tmp;
            n->type = NODE_ADD;
            continue;
        case TOK_MINUS:
            tmp = binary_create(n, mul(&tok, tok->next, type));
            if (tmp == NULL) {
                node_free_all(n);
                return NULL;
            }
            n = tmp;
            n->type = NODE_SUB;
            continue;
        default:
            *rest = tok;
            return n;
        }
    }
}

static node *mul(token **rest, token *tok, eval_type type)
{
    node *n = unary(&tok, tok, type);
    if (n == NULL) return n;
    node *tmp;
    for (;;) {
        switch (tok->type) {
        case TOK_MUL:
            tmp = binary_create(n, unary(&tok, tok->next, type));
            if (tmp == NULL) {
                node_free_all(n);
                return NULL;
            }
            n = tmp;
            n->type = NODE_MUL;
            continue;
        case TOK_DIV:
            tmp = binary_create(n, unary(&tok, tok->next, type));
            if (tmp == NULL) {
                node_free_all(n);
                return NULL;
            }
            n = tmp;
            n->type = NODE_DIV;
            continue;
        default:
            *rest = tok;
            return n;
        }
    }
}

static void token_free_all(token *tokens)
{
    token *tok, *next;
    for (tok = tokens; tok && (next = tok->next, 1); tok = next) {
        token_free(tok);
    }
}

static void node_free_all(node *node)
{
    if (node->left)
        node_free_all(node->left);
    if (node->right)
        node_free_all(node->right);
    node_free(node);
}

static int solve_tree(node *root, int type)
{
    int r;
    if (root->left) {
        r = solve_tree(root->left, type);
        if (r != QK_OK) return r;
    }
    if (root->right) {
        r = solve_tree(root->right, type);
        if (r != QK_OK) return r;
    }

    switch (type) {
    case EVAL_INT:
        switch (root->type) {
        case NODE_NEG:
            root->i = -root->left->i;
            break;
        case NODE_ADD:
            root->i = root->left->i + root->right->i;
            break;
        case NODE_SUB:
            root->i = root->left->i - root->right->i;
            break;
        case NODE_MUL:
            root->i = root->left->i * root->right->i;
            break;
        case NODE_DIV:
            if (root->right->i == 0) return QK_INVALID;
            root->i = root->left->i / root->right->i;
            break;
        default:
            break;
        }
        break;
    case EVAL_FLOAT:
        switch (root->type) {
        case NODE_NEG:
            root->f = -root->left->f;
            break;
        case NODE_ADD:
            root->f = root->left->f + root->right->f;
            break;
        case NODE_SUB:
            root->f = root->left->f - root->right->f;
            break;
        case NODE_MUL:
            root->f = root->left->f * root->right->f;
            break;
        case NODE_DIV:
            root->f = root->left->f / root->right->f;
            break;
        default:
            break;
        }
        break;
    }
    return QK_OK;
}

static int evalx(const char *str, void *r, int type)
{
    token tokens;
    tokens.next = NULL;
    tokens.type = TOK_END;

    errno = 0;
    int s = tokenize(str, type, &tokens);
    if (s != QK_OK) goto error;

    token *tok = &tokens;
    errno = 0;
    node *tree = add(&tok, &tokens, type);
    if (tree == NULL) goto error;

    if (tok->type != TOK_END) goto error_node;

    s = solve_tree(tree, type);
    if (s != QK_OK) goto error_node;

    if (r) {
        switch (type) {
        case EVAL_INT:
            *(int*)r = tree->i;
            break;
        case EVAL_FLOAT:
            *(float*)r = tree->f;
            break;
        }
    }

    node_free_all(tree);
    token_free_all(tokens.next);

    return QK_OK;

error_node:
    node_free_all(tree);
error:
    token_free_all(tokens.next);
    return errno ? QK_ERRNO : QK_INVALID;
}

QKAPI int qk_eval_i(const char *str, int *r)
{
    return evalx(str, (void*)r, EVAL_INT);
}

QKAPI int qk_eval_f(const char *str, float *r)
{
    return evalx(str, (void*)r, EVAL_FLOAT);
}
