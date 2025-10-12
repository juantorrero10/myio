#include "include.h"
#include "protos.h"
#include "structs.h"
#include "macros.h"

errno_t _win32_open(_Out_ _FSTREAM* stream, char * filePath, _FPERMIT fp) {
    DWORD access = 0;
    DWORD cd = 0;
    if (fp & fp_read) access |= GENERIC_READ;
    if (fp & fp_write) access |= GENERIC_WRITE;
    cd = (fp & fp_create)? CREATE_NEW : OPEN_EXISTING;
    stream->w32_handle = CreateFileA(filePath, access, 0, NULL, cd, FILE_ATTRIBUTE_NORMAL, NULL);
    return (stream->w32_handle == INVALID_HANDLE_VALUE)? ST_FUNC_ERROR : ST_FUNC_OK;
}


errno_t _win32_write(_FSTREAM* stream, _STRING* buff, DWORD bytesToWrite) {
    //Check objects
    int r = _chk_str_obj(buff);
    if ((r & (ST_STR_LENZERO | ST_STR_NULLPTR | ST_STR_TOOBIG)) != 0) {
        return ST_FUNC_STROBJ_INVALID;
    }
    r = _chk_fstream_obj(stream);
    if ((r & (ST_FS_INVALIDHANDLE)) != 0) {
        return ST_FUNC_FSOBJ_INVALID;
    }
    //UNSAFE!! only for testing
    if (bytesToWrite == UINT32_MAX) {
        bytesToWrite = buff->sz;
    }

    DWORD dwWritten;
    r = (int)WriteFile(stream->w32_handle, buff->str, bytesToWrite, &dwWritten, NULL);
    return r ^ 1;

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