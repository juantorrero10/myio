#pragma once
#include "include.h"

typedef enum _FILE_PERMISSION_TYPE {
    fp_none = 0,
    fp_read = 1,
    fp_write = 2,
    fp_create = 4
}_FPERMIT;

typedef enum _FILE_STREAM_TYPE {
    fs_ascii = 0,
    fs_bin = 1,
}_FSTYPE;

typedef enum _ASK_OPEN_TYPE {
    ao_file = 0,
    ao_stdin = 1,
    ao_stdout = 2,
    ao_stderr = 3
}_AO_TYPE;

typedef enum _SEEK_REL_TYPE {
    seek_set,
    seek_cur,
    seek_end
}_SEEK_REL_TYPE;

typedef struct __STRING {
    char* str;
    size_t sz;
    uint8_t b_init;
}_STRING;

typedef struct _FILE_STREAM {
    HANDLE w32_handle;
    _FPERMIT fp;
    _FSTYPE fs;
    uint8_t b_init;
    _AO_TYPE b_std;
    uint64_t seek;
    char* path;
}_FSTREAM;

typedef _FSTREAM fstream;

extern fstream* _stdout;
extern fstream* _stdin;
extern fstream* _stderr;
