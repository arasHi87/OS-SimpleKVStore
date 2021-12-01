#include <string.h>

int ischar(char ch)
{
    return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'));
}

void trim(char *str)
{
    char *p = str;
    int l = strlen(p);
    while (!ischar(p[l - 1]))
        p[--l] = 0;
    while (*p && !ischar(*p))
        ++p, --l;
    memmove(str, p, l + 1);
}