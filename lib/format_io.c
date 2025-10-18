#include "protos.h"
#include "macros.h"
#include "structs.h"
#include "include.h"

#define private

//Variadic functions!!
private errno_t _vfprintf(fstream *f, char *fmt, va_list args) {
    
    //TODO!!!
    (void)f;
    (void)fmt;
    (void)args;
    return ST_FUNC_OK;
}

errno_t _fprintf(fstream* f, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    errno_t e = _vfprintf(f, FMTID_NULL, va_list);
    va_end(args);
    return e;
}


errno_t _printf(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    errno_t e = _vfprintf(_stdout, fmt, va_list);
    va_end(args);
    return e;
}