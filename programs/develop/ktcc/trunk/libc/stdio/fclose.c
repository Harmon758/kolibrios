#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int fclose(FILE* file)
{
    if(!file)
    {
        errno = E_INVALIDPTR;
        return EOF;
    }

    if(file->buffer)
        free(file->buffer);
	free(file);

	return 0;
}
