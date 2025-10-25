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

#ifdef _WIN32
  #ifdef MYIO_EXPORTS
    #define MYIO_API __declspec(dllexport)
  #else
    #define MYIO_API __declspec(dllimport)
  #endif

MYIO_API fstream* _stdout;
MYIO_API fstream* _stdin;
MYIO_API fstream* _stderr;

MYIO_API errno_t _fputs(fstream *f, char *s);
MYIO_API errno_t _puts(char * s);
MYIO_API errno_t _fflush(fstream* f);
MYIO_API errno_t _fclear(fstream* f);
MYIO_API errno_t _fseek(fstream* f, seek_rel_type s, int64_t offset);
MYIO_API errno_t _fgets(fstream *f, _out_ char** out, size_t amount);
MYIO_API errno_t _gets(_out_ char** out, size_t amount);

MYIO_API errno_t _fopen(_out_ fstream **f, char *file_path , fpermit fp, fstype type);
MYIO_API errno_t _fclose(fstream *f);

MYIO_API errno_t _printf(char* fmt, ...);
MYIO_API errno_t _fprintf(fstream* f, char *fmt, ...);

#else
  #define MYIO_API
#endif

#endif //!MYIO_H