#include <libgen.h>
#include <string.h>

char* basename(char* s)
{
    size_t i;
    if (!s || !*s)
        return ".";
    i = strlen(s) - 1;
    for (; i && s[i] == '/'; i--)
        s[i] = 0;
    for (; i && s[i - 1] != '/'; i--)
        ;
    return s + i;
}
