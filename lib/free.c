#include "include.h"
#include "macros.h"
#include "structs.h"

//TODO: fstream object array to free everythangg


uint8_t gbool_initalized = 0;

void _free_stream_objects(void) {
    /* ... */
    _puts("EXIT\n");
    return;
}

errno_t _myio_setup_gcollector(void) {
    if (gbool_initalized) return ST_FUNC_OK;
    atexit(_free_stream_objects);
    gbool_initalized = 1;
}