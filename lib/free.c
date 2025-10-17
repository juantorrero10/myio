#include "include.h"
#include "macros.h"
#include "structs.h"

#define ARRAY_CHUNK 16

//TODO: fstream object array to free everythangg

// This type is defined to separate from normal _FSTREAM* uses
typedef _FSTREAM* _FS_ARRAY;

//Main data structure
_FS_ARRAY array = NULL;        //Head ptr
size_t array_size = 0;         //Size (amount of objects)


//Standard file descriptors
fstream* _stdout = NULL;
fstream* _stdin = NULL;
fstream* _stderr = NULL;

uint8_t gbool_initalized = 0;  //Bool

//Relocate and resize the array
errno_t _array_relocate() {
    
    //Sanity check
    if (((array_size + gbool_initalized) % ARRAY_CHUNK) != 0)
        return ST_FUNC_NO_ACTION;

    //Allocation attempt -> Linear allocation
    size_t new_byte_size = (sizeof(_FSTREAM) * (array_size + 1)) + ARRAY_CHUNK;
    _FS_ARRAY temp = malloc(new_byte_size);
    if (!temp) return ST_FUNC_MEMORY_ERROR;

    //Set to 0
    memset(temp, 0, new_byte_size);

    //Copy everything
    if (array) memcpy(temp, array, (array_size + 1) * sizeof(_FSTREAM));

    //Free old array
    free(array);

    //Set the array pointer to the new one
    array = temp;
    return ST_FUNC_OK;
}

_FSTREAM* _alloc_fstream() {
    int i = 0;
    _FSTREAM *f = FS_INVALID_PTR;

    //If array needs relocation
    if (((array_size + 1) % ARRAY_CHUNK) == 0)
        i = _array_relocate();
        if (i != ST_FUNC_OK) {
            return f;
        }

    f = array + array_size++;
    return f; 
}

void _free_stream_objects(void) {
    /* ... */
    for (size_t i = 0; i < array_size; i++)
    {
        _FSTREAM* f = array + i;
        if (f->w32_handle) CloseHandle(f->w32_handle);
    }
    
    free(array);
    return;
}

/*
*   This funcion is essentialy a initializer of the whole library.
*   Its called from another function which is essential to all functionality
*   so its garanteed to be called before doing anything.
*   
*   Among other things, it setups the main array of fstream objects
*   that has the sole purpose of being a garbage collector for when
*   the program its terminated.
*/
errno_t _myio_setup_gcollector(void) {
    if (gbool_initalized) return ST_FUNC_NO_ACTION;

    //Initialize array
    _array_relocate();
    if (!array) return ST_FUNC_MEMORY_ERROR;

    gbool_initalized = 1;

    //Initialize standard file descriptors
    _stdout = _alloc_fstream();
    _stdin = _alloc_fstream();
    _stderr = _alloc_fstream();

    _stdout->b_std=ao_stdout;_stdout->w32_handle = NULL; _stdout->b_init = 0;
    _stdout->fp=fp_write;_stdout->fs=fs_ascii;_stdout->path=NULL,_stdout->seek=0;

    _stdin->b_std=ao_stdin;_stdin->w32_handle = NULL; _stdin->b_init = 0;
    _stdin->fp=fp_read;_stdin->fs=fs_ascii;_stdin->path=NULL,_stdin->seek=0;

    _stderr->b_std=ao_stderr;_stderr->w32_handle = NULL; _stderr->b_init = 0;
    _stderr->fp=fp_write;_stderr->fs=fs_ascii;_stderr->path=NULL,_stderr->seek=0;

    //Add the free routine to a list of routines that executes after main()
    atexit(_free_stream_objects);
}