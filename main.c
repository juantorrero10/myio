#include "myio.h"
#include <stdio.h>

#define PATH "C:\\Users\\juan\\Desktop\\myio\\test.txt"

int main(void) {
    //Test 0 length
    char a = 0;
    char* b = &a;
    printf("fputs -> %d\n", _fputs(_stdout, b));

    //Test fputs()
    printf("fputs -> %d\n", _fputs(_stdout, "aaaaaaa\n"));

    //Stderr
    _fputs(_stderr, "some error!!!!\n");

    //Try to write to _stdin
    printf("fputs(_stdin) -> %d\n", _fputs(_stdin, "Not working!!"));

    //Writing to file, cursor
    fstream* f;
    printf("fopen -> %d\n", _fopen(&f, PATH, fp_write, fs_ascii));
    printf("fseek -> %d\n", _fseek(f, seek_set, 10));
    printf("fseek -> %d\n", _fseek(f, seek_cur, -4));
    printf("fputs  -> %d\n", _fputs(f, "hhhhhhhh"));
    _fclose(f);

    //Test read
    fstream* f2;
    printf("fopen -> %d\n", _fopen(&f2, PATH, fp_read, fs_ascii));
    char *s;
    printf("fgets -> %d, %s\n", _fgets(f2, &s, 10), s);

    return _puts("shut up\n");
}