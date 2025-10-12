#pragma once
#include "structs.h"
#include "include.h"
#define _out_

/*-------------BACKEND------------------------------------*/
errno_t _win32_write(_FSTREAM* stream, _STRING* buff, DWORD bytesToWrite);
errno_t _win32_open(_out_ _FSTREAM* stream, char * filePath, _FPERMIT fp);
errno_t _win32_seek(_FSTREAM* stream, _SEEK_REL_TYPE method, int64_t offset);
errno_t _chk_str_obj(_STRING* s);
errno_t _init_str_obj(_STRING* s, char* buff);
errno_t _chk_fstream_obj(_FSTREAM* fs);
errno_t _init_fstream_obj(_FSTREAM* fs);

/*-------------FRONT END------------------------------------*/
errno_t _fputs(fstream *f, char *s);
errno_t _puts(char * s);
errno_t _fflush(fstream* f);
errno_t _fclear(fstream* f);
errno_t _fseek(fstream* f, _SEEK_REL_TYPE s, int64_t offset);
errno_t _fopen(_out_ fstream **f, char *file_path , _FPERMIT fp, _FSTYPE type);
errno_t _fclose(fstream *f);