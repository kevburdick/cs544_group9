#ifndef __TEXTBUF_H__
#define __TEXTBUF_H__

#include <stdio.h>

typedef struct textbuf {
    const char *path;
    FILE *f;
    off_t size;
} textbuf;

int tbopen(const char *path, textbuf *buf);

int tbread(textbuf *buf, char *dest, off_t pos, off_t len)

int tboverwrite(textbuf *buf, off_t pos, off_t origlen, const char *data, off_t newlen);

int tbclose(textbuf *buf);

#endif
