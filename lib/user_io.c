#include "include.h"
#include "protos.h"
#include "structs.h"
#include "macros.h"
 
errno_t _fopen(_myio_out_ fstream **f, char *file_path ,_FPERMIT fp, _FSTYPE type) {
    _FSTREAM *fs = _alloc_fstream();
    if (!fs) return ST_FUNC_MEMORY_ERROR;

    memset(fs, 0, sizeof(_FSTREAM));
    fs->w32_handle = NULL;
    fs->fp = fp;
    fs->fs = type;
    fs->b_init = 0; fs->b_std = 0; fs->path = file_path;

    errno_t err = _init_fstream_obj(fs);
    if (err != ST_FS_OK) {
        free(fs);
        *f = FS_INVALID_PTR;
        return err;
    }

    *f = (fstream*)fs;
    return ST_FUNC_OK;
}

errno_t _fclose(fstream* f) {
    if (!f || f == FS_INVALID_PTR) return ST_FS_NULL;
    _FSTREAM* fs = (_FSTREAM*)f;

    if (fs->w32_handle)
        CloseHandle(fs->w32_handle);
        f->w32_handle = NULL;
    return ST_FS_OK;
}

errno_t _fputs(fstream *f, char *s) {
    _FSTREAM* fs = (_FSTREAM*)f;
    int i = 0;
    i = _init_fstream_obj(fs);
    if (!f || f == FS_INVALID_PTR) return ST_FS_NULL;
    if (i & (ST_FS_INVALIDHANDLE)) 
        return ST_FUNC_FSOBJ_INVALID;

    _STRING* str = malloc(sizeof(_STRING));
    str->str = NULL;str->b_init = 0;str->sz = 0;
    i = _init_str_obj(str, s);
    
    if (i == ST_STR_NULLPTR) return ST_FUNC_STROBJ_INVALID;
    i = _win32_write((_FSTREAM*)f, str, (DWORD)str->sz);
    //Flush stdout if ends in newline
    if (i == ST_FUNC_OK) {
        if (str->str[str->sz - 1] == '\n')_fflush(_stdout);
    } if (str) free(str); 
    return i;
}

errno_t _puts(char * s) {
    return _fputs(_stdout, s);
}


errno_t _fflush(fstream* f) {
    _FSTREAM* fs = (_FSTREAM*)f;
    if (!fs->w32_handle) return ST_FS_INVALIDHANDLE;
    if (!FlushFileBuffers(fs->w32_handle))
        return ST_FS_INVALIDHANDLE;
    return ST_FUNC_OK;
}

errno_t _fclear(fstream* f) {
    _FSTREAM* fs = (_FSTREAM*)f;
    if (fs->b_std != ao_stdin) return ST_FS_INVALIDTYPE;

    // Use ReadFile to clear available data
    char temp[256];
    DWORD read;
    while (ReadFile(fs->w32_handle, temp, sizeof(temp), &read, NULL) && read > 0);
    return ST_FUNC_OK;
}

errno_t _fseek(fstream* f, _SEEK_REL_TYPE s, int64_t offset) {
    if (!f || f == FS_INVALID_PTR) return ST_FS_NULL;
    _FSTREAM* fs = (_FSTREAM*)f;
    return _win32_seek(fs, s, offset);
}


errno_t _fgets(fstream *f, _out_ char** out, size_t amount) {
    int i = 0;
    i = _init_fstream_obj((_FSTREAM*)f);
    if (!f) return ST_FUNC_FSOBJ_INVALID;
    if (!out) return ST_FUNC_PTR_INVALID;
    if (i & (ST_FS_INVALIDHANDLE)) ST_FUNC_FSOBJ_INVALID;
    _STRING* s = (_STRING*)malloc(sizeof(_STRING));
    s->str = NULL;
    s->b_init = 0;
    s->sz = 0;

    uint32_t read;
    i = _win32_read((_FSTREAM*)f, (uint32_t)amount, s, &read);
    if (((_FSTREAM*)f)->b_std != 0){
        _fflush(_stdin);
        if (read < amount)s->str[s->sz - 2] = 0;
    }
    if (i != ST_FUNC_OK) return i;
    *out = s->str;
    if (s) free(s);
    return ST_FUNC_OK;
}

errno_t _gets(_out_ char** out, size_t amount) {return _fgets(_stdin, out, amount);}