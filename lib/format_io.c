#include "protos.h"
#include "macros.h"
#include "structs.h"
#include "include.h"



#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define BUFF_SIZE 32
#define private static

private uint64_t fetch_arg(va_list* args, uint8_t byteLen, bool bFloat) {
    //Note that all arguments get "promoted" to 32bit 
    uint64_t ret = 0; double t = 0;
    if (bFloat) {
        t = va_arg(*args, double);
        return *(uint64_t*)&t;
    }
    switch(byteLen) {
        case 8: ret = va_arg(*args, uint64_t); break;
        case 2: ret = (uint16_t)va_arg(*args, uint32_t); break;
        case 1: ret = (uint8_t)va_arg(*args, uint32_t); break;

        case 4:
        default: ret = va_arg(*args, uint32_t); break;
    } return ret;
}

private size_t __itoa(
    uint64_t value, uint8_t radix, uint8_t byteLen, int32_t intCap,
    bool bSigned, bool bUppercase, char* buff
    )
{
    if (radix < 2 || (radix > 10 && radix != 16)) return 0; //Only 2-10 + 16 radix supported
    if (!buff) return 0;
    memset(buff, 0, BUFF_SIZE);
    if (intCap == -1 && value == 0) {buff[0] = '0'; return 1;} //0 is a special case

    char lowc[16] = "0123456789abcdef";
    char uppc[16] = "0123456789ABCDEF";
    char* lookup = bUppercase? uppc : lowc;
    bool finalSign = 0; uint64_t cvalue = value;

    //get absolute value and flip "finalSign" to add a - at the end
    if (bSigned) {
        switch (byteLen)
        {
        case 8:
            if (value > INT64_MAX) {
                cvalue = (uint64_t)(-(int64_t)value);
                finalSign = 1;
            }break;
        case 2:
            if (value > INT16_MAX) {
                cvalue = (uint64_t)(-(int16_t)value);
                finalSign = 1;
            }break;
        case 1:
            if (value > INT8_MAX) {
                cvalue = (uint64_t)(-(int8_t)value);
                finalSign = 1;
            }break;
        case 4:
        default:
            if (value > INT32_MAX) {
                cvalue = (uint64_t)(-(int32_t)value);
                finalSign = 1;
            }break;
        }
    }

    
    //Main loop
    uint8_t rem; int i = 0;
    bool nolimit = intCap == -1;
    if (nolimit) intCap = INT32_MAX;
    int32_t cIntCap = intCap;
    while (cvalue && cIntCap--) {
        rem = cvalue % radix;
        cvalue /= radix;
        buff[i++] = lookup[rem];
    }

    //FIll rest with zeros
    const int32_t ci = i;
    if (!nolimit && ((intCap - ci) > 0)) {
        for (int x = 0; x < (intCap - ci); x++) {
            buff[i++] = '0';
        }
    }

    if (finalSign) buff[i++] = '-';
    strrev(buff);
    return (size_t)i;
}


private void __ftoa(double value, uint8_t precision, int32_t intCap, char *buff) {
    if (!buff) return;
    memset(buff, 0, BUFF_SIZE);

    // === Handle special floating states first ===
    if (isnan(value)) {
        strcpy(buff, "nan");
        return;
    }
    if (isinf(value)) {
        if (value < 0) strcpy(buff, "-inf");
        else strcpy(buff, "inf");
        return;
    }
    if (value == 0.0) {
        // Signed zero handling
        if (signbit(value))
            strcpy(buff, "-0");
        else
            strcpy(buff, "0");

        if (precision > 0) {
            size_t len = strlen(buff);
            buff[len++] = '.';
            memset(buff + len, '0', precision);
            buff[len + precision] = '\0';
        }
        return;
    }

    bool neg = value < 0.0;
    if (neg) value = -value;

    // Extract integer part
    double int_d = floor(value);
    int64_t intPart = (int64_t)int_d;

    // Convert integer part
    size_t len = __itoa(intPart, 10, 8, intCap, 0, 0, buff);

    if (neg) {
        // shift right to insert negative sign
        memmove(buff + 1, buff, len);
        buff[0] = '-';
        len++;
    }

    // No decimals requested → done
    if (precision == 0) {
        buff[len] = '\0';
        return;
    }

    buff[len++] = '.';

    // Fractional part
    double frac = value - int_d;

    for (uint8_t i = 0; i < precision; i++) {
        frac *= 10.0;
        int digit = (int)frac;
        frac -= digit;
        buff[len++] = (char)('0' + digit);
    }

    buff[len] = '\0';
}

/*s
 * Implementation of printf through a state machine...
 *
 * Format examples:
 * %ld:
 * --------> A 32-bit signed integer shown in decimal.
 *
 * %02hhX
 * --------> An byte shown in uppercase hexadecimal with a 2 integer cap
 *          (Fills left digit with '0' if its value is zero)
 *
 * %10.2llf.
 * --------> An 8-byte float (double) number capped to 10 digits in the integer part
 *           and capped to 2 digits in the decimal part.
 */
private errno_t _vsfprintf(fstream* f, char* fmt, va_list* args) {
    
//Init
    char* ptr = fmt;
    char buff[BUFF_SIZE];
Init: //Set default values


    uint8_t byteLen = 4;        //Byte length specified by the user (def: 4  -> 32-bits)
    uint8_t radix = 10;         //Number Base for ASCII conversion  (def: 10 -> decimal)
    uint8_t decPrecision = 2;   //Number of decimal of precision for floats (def: 2)
    int32_t intCap = -1;        //Max number of integer digits to display and fill with zeros
                                //                                  (def: -1 -> Show all and don't fill)
    //Flags
    bool bSigned = 0, bUppercase = 0, bFloat = 0, bString = 0;
    uint8_t Initctrl1 = 0, Initctrl2 = 0;
//State machine
Normal:
    switch(*ptr) {
        case 0:goto exit;
        case '%':
            ptr++; goto LenStage1;
        default:
            _fputc(f, *(ptr++)); goto Normal;
    }

LenStage1:
    switch(*ptr) {
        case 0:goto exit;
        case '%': _fputc(f, '%'); ptr++; goto Normal;
        case 'h': byteLen /= 2; ptr++; goto LenStage2;
        //first l does nothing
        case 'l': ptr++; goto LenStage2;
        case '.': ptr++; Initctrl2=2; goto FPrecStage;
        case '0':
            ptr++;
            if (Initctrl2) intCap *= 10;
            goto LenStage1;
        default:
            char d = *ptr;
            if (d >= '0' && d <= '9' && Initctrl2 < 2) {
                if (!Initctrl2) {intCap = 0; Initctrl2 = 1;}
                intCap = intCap * 10 + d - '0';
                ptr++;
                goto LenStage1;
            }
            goto LenStage2;
    }

FPrecStage:
    switch(*ptr) {
        case 0:goto exit;
        case '0':
            ptr++;
            if (Initctrl1) intCap *= 10;
            goto FPrecStage;
        default:
            char d = *ptr;
            if (d >= '0' && d <= '9') {
                if (!Initctrl1) {decPrecision = 0; Initctrl1 = 1;}
                decPrecision *= 10;
                decPrecision += (d - '0');
                ptr++;
                goto FPrecStage;
            } goto LenStage1;
    }

LenStage2:
    switch(*ptr) {
        case 0:goto exit;
        case 'h': byteLen /= 2; ptr++; goto ValStage;
        case 'l': byteLen *= 2; ptr++; goto ValStage;
        default: goto ValStage;
    }

ValStage:
    switch(*ptr) {
        case 0: goto exit;
        //Signed decimal
        case 'i':
        case 'd':
            bSigned = 1; goto PrintValue;
        //Unsigned decimal
        case 'u': goto PrintValue;
        //Unsigned binary
        case 'b': radix = 2; goto PrintValue;
        //Unsigned octal
        case 'o': radix = 8; goto PrintValue;
        //Unsigned lowercase hexadecimal
        case 'x': radix = 16; goto PrintValue;
        //Unsigned uppercase hexadecimal
        case 'X': radix = 16; bUppercase = 1; goto PrintValue;
        //Character
        case 'c': goto PrintText;
        //String
        case 's': bString = 1; goto PrintText;
        //Float
        case 'f': bFloat = 1; goto PrintValue;
        default: goto Init;
            
    }


PrintValue:
    uint64_t val = fetch_arg(args, byteLen, bFloat);
    if (!bFloat)__itoa(val, radix, byteLen, intCap, bSigned, bUppercase, buff);
    else __ftoa(*(double*)&val, decPrecision, intCap, buff);
    _fputs(f, buff);
    ptr++;
    goto Init;

PrintText:
    if (bString) printf("%s", va_arg(*args, const char*));
    else {
        char c = (char)va_arg(*args, uint32_t);
        _fputc(f, c);
    }
    ptr++; goto Init;

exit:
    va_end(args);
    return ST_FUNC_OK;
}

errno_t _fprintf(fstream* f, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    errno_t e = _vsfprintf(f, fmt, &args);
    va_end(args);
    return e;
}


errno_t _printf(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    errno_t e = _vsfprintf(_stdout, fmt, &args);
    va_end(args);
    return e;
}