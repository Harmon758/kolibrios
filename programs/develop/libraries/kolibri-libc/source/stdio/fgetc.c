#include <stdio.h>
#include <errno.h>
#include <sys/ksys.h>

int fgetc(FILE* stream)
{
    unsigned bytes_read;
    char c;

    unsigned status = _ksys_file_read_file(stream->name, stream->position, 1, &c, &bytes_read);

    if (status != KSYS_FS_ERR_SUCCESS) {
        switch (status) {
            case KSYS_FS_ERR_EOF:
                stream->eof = 1;
                break;
            case KSYS_FS_ERR_1:
            case KSYS_FS_ERR_2:
            case KSYS_FS_ERR_3:
            case KSYS_FS_ERR_4:
            case KSYS_FS_ERR_5:
            case KSYS_FS_ERR_7:
            case KSYS_FS_ERR_8:
            case KSYS_FS_ERR_9:
            case KSYS_FS_ERR_10:
            case KSYS_FS_ERR_11:
            default:
                // Just some IO error, who knows what exactly happened
                errno = EIO;
                stream->error = errno;
                break;
        }
        return EOF;
    }

    stream->position++;
    return c;
}
