#ifndef __OPTIONS_H__
#define __OPTIONS_H__

typedef struct options {
    char *a;
    size_t len; // includes null terminators
    size_t _allocated;
} options_t;

options_t new_options();

void add_option(options_t op, const char *label, const char *value);

/* -1 for error (malformed), 0 for success */
int parse_options(char *raw_options, size_t len, options_t *op);

/* Result must be free()d by user */
size_t print_options(options_t op, char **dest);

void free_options(options_t op);

/* Return the value of the first option for label in op. */
const char *first_option(options_t op, const char *label);

/* Return the value of the nex option for label in op after prev. */
const char *next_option(options_t op, const char *label, const char *prev);

#endif
