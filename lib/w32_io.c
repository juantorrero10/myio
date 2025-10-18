#include "include.h"
#include "protos.h"
#include "structs.h"
#include "macros.h"

errno_t _win32_open(_Out_ _FSTREAM* stream, char * filePath, _FPERMIT fp) {
    DWORD access = 0;
    DWORD cd = 0;

    //OPEN STANDARD FILE DESCRIPTORS
    if (stream->b_std != 0) {
        switch (stream->b_std) {
            case ao_stdin:
                stream->w32_handle = GetStdHandle(STD_INPUT_HANDLE); break;
            case ao_stdout:
                stream->w32_handle = GetStdHandle(STD_OUTPUT_HANDLE); break;
            case ao_stderr:
                stream->w32_handle = GetStdHandle(STD_ERROR_HANDLE); break;
            default:
                return ST_FS_INVALIDTYPE;
        }
        return (stream->w32_handle == INVALID_HANDLE_VALUE || !stream->w32_handle)? ST_FUNC_ERROR : ST_FUNC_OK;
    }

    //OPEN FILE
    else {
        if (fp & fp_read) access |= GENERIC_READ;
        if (fp & fp_write) access |= GENERIC_WRITE;
        cd = (fp & fp_create)? OPEN_ALWAYS : OPEN_EXISTING;
        stream->w32_handle = CreateFileA(filePath, access, 0, NULL, cd, FILE_ATTRIBUTE_NORMAL, NULL);
        return (stream->w32_handle == INVALID_HANDLE_VALUE)? ST_FUNC_WINAPI_ERROR : ST_FUNC_OK;
    } return ST_FUNC_FSOBJ_INVALID;
}


errno_t _win32_write(_FSTREAM* stream, _STRING* buff, uint32_t bytes_to_write) {
    //Check objects
    int r = _chk_str_obj(buff);
    if ((r & (ST_STR_LENZERO | ST_STR_NULLPTR | ST_STR_TOOBIG)) != 0) {
        return ST_FUNC_STROBJ_INVALID;
    }
    r = _chk_fstream_obj(stream);
    if ((r & (ST_FS_INVALIDHANDLE)) != 0) {
        return ST_FUNC_FSOBJ_INVALID;
    }

    if (!(stream->fp & fp_write)) {
        return ST_FUNC_ACCESS_DENIED;
    }
    //UNSAFE!! only for testing
    if (bytes_to_write == UINT32_MAX) {
        bytes_to_write = (uint32_t)buff->sz;
    }

    DWORD dwWritten;
    r = (int)WriteFile(stream->w32_handle, buff->str, (DWORD)bytes_to_write, &dwWritten, NULL);
    if (!r) return ST_FUNC_WINAPI_ERROR;
    return ST_FUNC_OK;

}

errno_t _win32_seek(_FSTREAM* stream, _SEEK_REL_TYPE method, int64_t offset) {

    int r = _chk_fstream_obj(stream);
    if ((r & (ST_FS_INVALIDHANDLE)) != 0) {
        return ST_FUNC_FSOBJ_INVALID;
    }
    LARGE_INTEGER li;
    li.QuadPart = offset;
    LARGE_INTEGER new_pos;
    DWORD moveMethod;
    switch (method) {
        case seek_set: moveMethod = FILE_BEGIN; break;
        case seek_cur: moveMethod = FILE_CURRENT; break;
        case seek_end: moveMethod = FILE_END; break;
        default: return ST_FS_INVALIDTYPE;
    }

    r = SetFilePointerEx(stream->w32_handle, li, &new_pos, moveMethod);
    stream->seek = new_pos.QuadPart;
    return r ^ 1;
}

errno_t _win32_read(_FSTREAM* stream, uint32_t bytes_to_read,_out_ _STRING* out_buff, _out_ uint32_t* bytes_read) {
    //Check and reset string obj
    int i = _reset_str_obj(out_buff);
    if (i == ST_STR_NULL) return ST_FUNC_STROBJ_INVALID;
    i = _chk_str_obj(out_buff);
    if (!(i & (ST_STR_NULLPTR | ST_STR_LENZERO)))return ST_FUNC_STROBJ_INVALID;

    out_buff->str = malloc(bytes_to_read + 1);
    if (!out_buff) return ST_FUNC_MEMORY_ERROR;
    memset(out_buff->str, 0, bytes_to_read + 1);
    //Check fstream object
    i = _chk_fstream_obj(stream);
    if ((i & (ST_FS_INVALIDHANDLE | ST_FS_NULL)))return ST_FUNC_FSOBJ_INVALID;
    if (!(stream->fp & fp_read)) return ST_FS_ACCESSDENIED;

    
    i = (int)ReadFile(stream->w32_handle, out_buff->str, (DWORD)bytes_to_read, bytes_read, NULL);
    if (!i) return ST_FUNC_WINAPI_ERROR;

    i = _chk_str_obj(out_buff);
    if (i & (ST_STR_LENZERO | ST_STR_LENNOTUPDATED)) return ST_FUNC_STROBJ_INVALID;


    return ST_FUNC_OK;
}

errno_t _win32_translate_error(DWORD e) {
    switch (e){
        case ERROR_FILE_NOT_FOUND:
            return ST_FUNC_FILE_NOT_FOUND;
        case ERROR_ACCESS_DENIED:
            return ST_FUNC_ACCESS_DENIED;
        case ERROR_SHARING_VIOLATION:
            return ST_FUNC_ACCESS_DENIED;
        case ERROR_INVALID_NAME:
            return ST_FUNC_FILE_NOT_FOUND;
        case ERROR_PATH_NOT_FOUND:
            return ST_FUNC_FILE_NOT_FOUND;
        default:
            if (e != ERROR_SUCCESS) {
                printf("undocumented winapi error: %lX\n", GetLastError());
                return ST_FUNC_WINAPI_ERROR;
            }
    }
    return ST_FUNC_OK;
}

errno_t _win32_get_file_size(_FSTREAM *f, size_t* out_sz) {
    int i = _chk_fstream_obj(f);
    if (i & (ST_FS_INVALIDHANDLE | ST_FS_NULL)) return ST_FUNC_FSOBJ_INVALID;
    LARGE_INTEGER li;
    if (!GetFileSizeEx(f->w32_handle, &li)) {
        return _win32_translate_error(GetLastError());
    }
    *out_sz = (size_t)li.QuadPart; return ST_FS_OK;
}