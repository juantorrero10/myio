#include "myio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> //NAN and Infinty
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
    printf("fopen -> %d\n", _fopen(&f, PATH, fp_write | fp_create, fs_ascii));
    printf("fseek -> %d\n", _fseek(f, seek_set, 10));
    printf("fseek -> %d\n", _fseek(f, seek_cur, -4));
    printf("fputs  -> %d\n", _fputs(f, "SAMPLE TEXT"));
    _fclose(f);
    //Test read
    fstream* f2;
    printf("fopen -> %d\n", _fopen(&f2, PATH, fp_read, fs_ascii));
    char *s;
    printf("fgets -> %d, %s\n", _fgets(f2, &s, 50), s);
    _fclose(f2);
    /*char *s2;
    printf("fgets -> %d\n", _fgets(_stdin, &s2, 10));
    printf("\n_puts -> %d\n", _puts(s2));
    */
    //relocation_test
    for (size_t i = 0; i < 16; i++)
    {
        fstream* f = NULL;
        _fopen(&f, PATH, fp_write, fs_ascii);
        _fclose(f);
    }
    fstream* f3;
    printf("fopen -> %d\n", _fopen(&f3, PATH, fp_read, fs_ascii));
    printf("fgets -> %d, %s\n", _fgets(f2, &s, 5), s);
    _fclose(f3);

    //PRINTF TESTING
    _printf("PRINTF TESTING: %010ld %d %hhd %2hhu %c %s\n", -2UL, 4UL, (uint8_t)255, (uint8_t)255, 'c', "string");
    _printf("Floats: %4.03f %llf %f %f\n", -93842.474849f, NAN, -INFINITY, 0.0f);

    _printf("MEMDUMP in test.txt: \n");
    fstream* memdump; _fopen(&memdump, PATH, fp_write, fs_ascii);
    _fseek(memdump, seek_set, 15); _fputs(memdump, "\n\n");
    //For memory dumping
    char* buff2 = malloc(64);
    for (int i = 0; i < 64; i++) {
        if (!(i%8) && i) _fprintf(memdump, "\n");
        _fprintf(memdump, "%2hhX ", buff2[i]);
    } free(buff2);

    return _puts("shut up\n");
}