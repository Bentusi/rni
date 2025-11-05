/**************************************************************************************************
*Filename:     printk.c
*Purpose:      调试打印
*Log:          Date          Author    Modified
*              2021/08/31    hdq       create
**************************************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include "HL_sci.h"

/****************************************************************************
**                    EXTERNAL FUNCTION DECLARATIONS
****************************************************************************/
void bsp_output_char(char s)
{
    sciSendByte(sciREG3, s); /* Send out the character */
    if (s == '\n')
    {
        sciSendByte(sciREG3, '\r'); /* Send out the character */
    }
}

static void printNum(
        long num,
        unsigned base,
        bool sign,
        unsigned maxwidth,
        char lead
);

void vprintk(const char *fmt, va_list ap)
{
    for (; *fmt != '\0'; fmt++)
    {
        unsigned base = 0;
        unsigned width = 0;
        bool lflag = false;
        bool minus = false;
        bool sign = false;
        char lead = ' ';
        char c = ' ';

        if (*fmt != '%')
        {
            bsp_output_char(*fmt);
            continue;
        }
        fmt++;
        if (*fmt == '0' )
        {
            lead = '0';
            fmt++;
        }
        if (*fmt == '-' )
        {
            minus = true;
            fmt++;
        }
        while (*fmt >= '0' && *fmt <= '9' )
        {
            width *= 10;
            width += ((unsigned) *fmt - '0');
            fmt++;
        }

        if ((c = *fmt) == 'l')
        {
            lflag = true;
            c = *++fmt;
        }
        if ( c == 'c' )
        {
            /* need a cast here since va_arg() only takes fully promoted types */
            char chr = (char) va_arg(ap, int);
            bsp_output_char(chr);
            continue;
        }
        if ( c == 's' )
        {
            unsigned i, len;
            char *s, *str;

            str = va_arg(ap, char *);

            if ( str == NULL )
            {
                str = "";
            }

            /* calculate length of string */
            for ( len=0, s=str ; *s ; len++, s++ );

            /* leading spaces */
            if ( !minus )
            {
                for ( i=len ; i<width ; i++ )
                {
                    bsp_output_char(' ');
                }
            }

            /* no width option */
            if (width == 0)
            {
                width = len;
            }

            /* output the string */
            for ( i=0 ; i<width && *str ; str++ )
            {
                bsp_output_char(*str);
            }

            /* trailing spaces */
            if ( minus )
            {
                for ( i=len ; i<width ; i++ )
                {
                    bsp_output_char(' ');
                }
            }

            continue;
        }

        /* must be a numeric format or something unsupported */
        if ( c == 'o' || c == 'O' )
        {
            base = 8;
            sign = false;
        }
        else if ( c == 'i' || c == 'I' ||
                c == 'd' || c == 'D' )
        {
            base = 10; sign = true;
        }
        else if ( c == 'u' || c == 'U' )
        {
            base = 10; sign = false;
        }
        else if ( c == 'x' || c == 'X' )
        {
            base = 16; sign = false;
        }
        else if ( c == 'p' ) {
            base = 16; sign = false; lflag = true;
        }
        else
        {
            bsp_output_char(c);
            continue;
        }

        printNum(lflag ? va_arg(ap, long) : (long) va_arg(ap, int),
                        base,
                        sign,
                        width,
                        lead
        );
    }
}

/**
*   function:
*             printNum - print number in a given base.
*
*   Input:
*            num:  number to print
*            base: base used to print the number.
*
*/
static void printNum(
        long num,
        unsigned base,
        bool sign,
        unsigned maxwidth,
        char lead
    )
{
    unsigned long unsigned_num;
    unsigned long n;
    unsigned count;
    char toPrint[20];

    if ( sign && (num <  0) )
    {
        bsp_output_char('-');
        unsigned_num = (unsigned long) -num;
        if (maxwidth)
            maxwidth--;
    }
    else
    {
        unsigned_num = (unsigned long) num;
    }

    count = 0;
    while ((n = unsigned_num / base) > 0)
    {
        toPrint[count++] = (char) (unsigned_num - (n * base));
        unsigned_num = n;
    }
    toPrint[count++] = (char) unsigned_num;

    for (n=maxwidth ; n > count; n-- )
    {
        bsp_output_char(lead);
    }

    for (n = 0; n < count; n++)
    {
        bsp_output_char("0123456789ABCDEF"[(int)(toPrint[count-(n+1)])]);
    }
}

void printk(const char *fmt, ...)
{
    va_list  ap;       /* points to each unnamed argument in turn */

    va_start(ap, fmt); /* make ap point to 1st unnamed arg */
    vprintk(fmt, ap);
    va_end(ap);        /* clean up when done */
}

