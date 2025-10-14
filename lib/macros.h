#define _myio_out_ 

/*------------w32----------------*/
#define W32_STDIN -10
#define W32_STDOUT -11
#define W32_STDERR -12

/*------------STRING STRUCT-------------*/
#define STR_NULL_LEN 0
#define STR_MAX 2048
#define ST_STR_LENUPDATED 0
#define ST_STR_LENNOTUPDATED 1
#define ST_STR_LENZERO 2
#define ST_STR_TOOBIG 4
#define ST_STR_NULL 8
#define ST_STR_NULLPTR 16
#define ST_STR_INVALIDBUFFER 32

/*-------------FSTREAM STUCT----------*/
#define FS_INVALID_PTR (void*)-1
#define ST_FS_OK 0
#define ST_FS_NOTUPDATED 1
#define ST_FS_INVALIDHANDLE 2
#define ST_FS_INVALIDTYPE 4
#define ST_FS_NULL 8
#define ST_FS_INVALIDPATH 16
#define ST_FS_FILENOTFOUND 32
#define ST_FS_ACCESSDENIED 64

/*--------FUNCTION RET------------*/
#define ST_FUNC_OK 0
#define ST_FUNC_ERROR 1
#define ST_FUNC_STROBJ_INVALID 4
#define ST_FUNC_FSOBJ_INVALID 8
#define ST_FUNC_PTR_INVALID 16
#define ST_FUNC_MEMORY_ERROR 32
#define ST_FUNC_WINAPI_ERROR 64
#define ST_FUNC_ACCESS_DENIED 128
#define ST_FUNC_FILE_NOT_FOUND 256
#define ST_FUNC_FILE_ALREADY_OPENED 512
