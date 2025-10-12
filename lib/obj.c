#include "protos.h"
#include "structs.h"
#include "macros.h"
#include "include.h"

/*--------------STANDARD FILE DESCRIPTORS---------------*/
_FSTREAM gfs_stdout = {NULL, fp_write, fs_ascii, 0, ao_stdout, 0};
_FSTREAM gfs_stdin = {NULL, fp_read, fs_ascii, 0, ao_stdin, 0};
_FSTREAM gfs_stderr = {NULL, fp_write, fs_ascii, 0, ao_stderr, 0};

fstream* _stdout = (fstream*)&gfs_stdout;
fstream* _stdin = (fstream*)&gfs_stdin;
fstream* _stderr = (fstream*)&gfs_stderr;

errno_t _chk_str_obj(_STRING* s) {
    if (s->str == NULL)return ST_STR_NULLPTR;
    if (!s->b_init || s->sz == STR_NULL_LEN){
        s->sz = strlen(s->str);
        s->b_init = 1;
        return (s->sz == STR_NULL_LEN)? ST_STR_LENZERO : ST_STR_LENUPDATED;
    }
    return (s->sz > STR_MAX) ? ST_STR_TOOBIG : ST_STR_LENNOTUPDATED;
}

errno_t _init_str_obj(_STRING* s, char* buff) {
    if (buff == NULL) {
        return ST_STR_NULLPTR;   
    }
    s->str = buff;
    s->sz = strlen(buff);
    s->b_init = 1;
    return _chk_str_obj(s);
}

errno_t _chk_fstream_obj(_FSTREAM* fs) {
    if (fs->w32_handle == INVALID_HANDLE_VALUE || fs->w32_handle == NULL) {

        return ST_FS_INVALIDHANDLE;
    } return ST_FS_OK;
}

errno_t _init_fstream_obj(_FSTREAM* fs) {
    int i = 0;
    if (!fs) return ST_FS_NULL; 
    if (fs->b_init) {
        i = _chk_fstream_obj(fs);
        return (i == ST_FS_OK)? ST_FS_NOTUPDATED : i;
    }
    //If File stream is std
    if (fs->b_std != 0 && (fs->w32_handle == NULL || fs->w32_handle == INVALID_HANDLE_VALUE)) {
        switch (fs->b_std) {
            case ao_stdin:
                fs->w32_handle = GetStdHandle(W32_STDIN); break;
            case ao_stdout:
                fs->w32_handle = GetStdHandle(W32_STDOUT); break;
            case ao_stderr:
                fs->w32_handle = GetStdHandle(W32_STDERR); break;
            default:
                return ST_FS_INVALIDTYPE;
        }
        fs->b_init = 1;
    }
    else if (fs->b_std == ao_file && fs->w32_handle == NULL) {
        if (fs->path == NULL) return ST_FS_INVALIDPATH;
        i = _win32_open(fs, fs->path, fs->fp);
        if (i == ST_FUNC_ERROR) {
            switch (GetLastError()){
                case ERROR_FILE_NOT_FOUND:
                    return ST_FS_FILENOTFOUND;
                case ERROR_ACCESS_DENIED:
                    return ST_FS_ACCESSDENIED;
            }
        }
        fs->b_init = 1;

    }
    return ST_FS_OK;
}


    