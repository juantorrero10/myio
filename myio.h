#ifndef MYIO_H
#define MYIO_H

#include <stdint.h>
#include <stddef.h>

#define _out_ 

typedef struct fstream fstream;

typedef enum _FILE_PERMISSION_TYPE {
    fp_none = 0,
    fp_read = 1,
    fp_write = 2,
    fp_create = 4
}fpermit;

typedef enum _FILE_STREAM_TYPE {
    fs_ascii = 0,
    fs_bin = 1,
}fstype;

typedef enum _SEEK_REL_TYPE {
    seek_set,
    seek_cur,
    seek_end
}seek_rel_type;

extern fstream* _stdout;
extern fstream* _stdin;
extern fstream* _stderr;

errno_t _fputs(fstream *f, char *s);
errno_t _puts(char * s);
errno_t _fflush(fstream* f);
errno_t _fclear(fstream* f);
errno_t _fseek(fstream* f, seek_rel_type s, int64_t offset);
errno_t _fgets(fstream *f, _out_ char** out, size_t amount);
errno_t _gets(_out_ char** out, size_t amount);

errno_t _fopen(_out_ fstream **f, char *file_path , fpermit fp, fstype type);
errno_t _fclose(fstream *f);


#endif //!MYIO_H