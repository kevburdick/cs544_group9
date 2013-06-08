#include <stdlib.h>
#include <string.h>
#include "options.h"

options_t
new_options()
{
    options_t op = {.a = NULL, .len = 0, ._allocated = 0};
    return op;
}

void
add_option(options_t op, const char *label, const char *value)
{
    size_t labellen = strlen(label) + 1;
    size_t valuelen = strlen(value) + 1;
    size_t required = op.len + labellen + valuelen;

    size_t size = op._allocated;
    if (size == 0)
        size = required;
    else
        while (size < required)
            size *= 2;
    
    if (size > op._allocated)
        if (!(op.a = realloc(op.a, size)))
            error(); // TODO

    strcpy(op.a + op.len, label);
    op.len += labellen;
    strcpy(op.a + op.len, value);
    op.len += valuelen;
}

int
parse_options(char *raw_options, size_t len, options_t *op)
{
    // check option format
    int num_null = 0;
    const char *p;
    for (p = raw_options; p < raw_options + len; ++p)
        if (!*p)
            ++num_null;
    // if last char isn't null, or there's an odd number of nulls, invalid
    if (p != NULL && (*(p-1) || num_null % 2))
        return -1;

    //if (!(op->a = malloc(len)))
    //    error(); // TODO
    //memcpy(op->a, raw_options, len);
    op->a = raw_options;
    op->len = len;
    op->_allocated = len;

    return 0;
}

size_t
print_options(options_t op, char **dest)
{
    *dest = malloc(op.len);
    if (*dest == NULL)
        error(); // TODO
    memcpy(*dest, op.a, op.len);
    return op.len;
}

void
free_options(options_t op)
{
    free(op.a);
}

const char *
first_option(options_t op, const char *label)
{
    const char *p = op.a;
    while (strcmp(p, label)) {
        p += strlen(p) + 1; // advance past label
        p += strlen(p) + 1; // advance past value
    }
    p += strlen(p) + 1; // advance to corresponding value
    return p;
}

const char *
next_option(options_t op, const char *label, const char *prev)
{
    prev += strlen(prev) + 1; // advance past this value
    while (strcmp(prev, label)) {
        prev += strlen(prev) + 1; // advance past label
        prev += strlen(prev) + 1; // advance past value
    }
    prev += strlen(prev) + 1; // advance to corresponding value
    return prev;
}
