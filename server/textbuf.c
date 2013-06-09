int
fcopy(FILE *src, FILE *dest, off_t len)
{
    char buf[4096];
    /* TODO error checking */

}

int
tbopen(const char *path, textbuf *buf)
{
    struct stat s;
    buf->path = path;
    if (!(buf->f = fopen(path, "r")))
        return -1;
    if (fstat(fileno(buf->f), &s) < 0)
        return -1;
    buf->size = s.st_size;
    return 0;
}

int
tbread(textbuf *buf, char *dest, off_t pos, off_t len)
{
    if (fseek(buf->f, pos, SEEK_SET) < 0)
        return -1;

    /* TODO error checking */
    return fread(dest, len, 1, buf->f);
}

int
tboverwrite(textbuf *buf, off_t pos, off_t origlen, const char *data, off_t newlen)
{
    char tempfilename[] = "tmpXXXXXX";
    textbuf newbuf = *buf;
    
    /* TODO error checking */
    newbuf->f = fdopen(mkstemp(tempfilename), "r+");
}
