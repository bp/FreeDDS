static char rcsid[]="$Id: printfc.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, append a formatted value, C interface.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <limits.h>
#include "dds_sys.h"

#define PREFIX_MAX 8
#define NUMERIC_MAX 512
#define FLOAT_FMT_MAX 64

                                   /*           1         2         3    */
#define PAD_MAX 32                 /*  12345678901234567890123456789012  */
static const char blanks[PAD_MAX+1] = "                                ";
static const char zeros[PAD_MAX+1]  = "00000000000000000000000000000000";
static const char ldigits[17] =       "0123456789abcdef";
static const char udigits[17] =       "0123456789ABCDEF";

/* macro to retrieve character (integer) argument */
#define VA_ARG_C(c_arg)  (c_arg) = va_arg(*ap, int);

/* macro to retrieve string pointer argument */
#define VA_ARG_S(s_arg)  (s_arg) = va_arg(*ap, char *);

/* macro to retrieve signed integer argument */
#define VA_ARG_I(i_arg)  \
   if(hlL == 'l') { (i_arg) = va_arg(*ap, long);  \
   } else if(hlL == 'L') { (i_arg) = va_arg(*ap, HOST_LLONG_T);  \
   } else { (i_arg) = va_arg(*ap, int); }

/* macro to retrieve unsigned integer argument */
#define VA_ARG_U(u_arg)  \
   if(hlL == 'l') { (u_arg) = va_arg(*ap, unsigned long);  \
   } else if(hlL == 'L') { (u_arg) = va_arg(*ap, HOST_ULLONG_T);  \
   } else { (u_arg) = va_arg(*ap, unsigned); }

/* macro to convert unsigned argument to character string */
#define NUMERIC(value, size, u_arg, digits, base)  \
   { num = numeric + NUMERIC_MAX; numerator = (u_arg);  \
      do {quotient = numerator / (base);  \
         *--num = (digits)[numerator - (base) * quotient];  \
         numerator = quotient;} while(numerator);  \
         (size) = numeric + NUMERIC_MAX - num; (value) = num; }

/* macro to compute the number of prefix zeros */
#define PRE_ZEROS(pre_zeros, pre_size, value_size, u_arg, min_prec)  \
   { precision = (min_prec < 0) ? - min_prec : min_prec;  \
      if((precision) > (value_size)) {  \
         (pre_zeros) = (precision) - (value_size);  \
      } else if(zero_pad && ! left_justify && (min_prec) < 0 &&  \
         min_width > ((pre_size) + (value_size))) {  \
         (pre_zeros) = min_width - ((pre_size) + (value_size));  \
      } else {  \
         (pre_zeros) = 0;  \
         if(! u_arg && ! (precision) && ! (pre_size)) (value_size) = 0;  \
      } }

/* macro to pad zeros or blanks */
#define PAD(width, fill)  \
   { pad_width = width;  \
      while(pad_width > 0) {  \
         pad_size = (PAD_MAX < pad_width) ? PAD_MAX : pad_width;  \
         ddsDictLoad(dict, &dict->load, NULL, fill, pad_size);  \
         pad_width -= pad_size; } }

int ddsPrintfC(
   DDS_DICT *dict,
   va_list *ap,
   const char *fmt)
{
   const char *fmt_start;
   size_t eof_start;
   DDS_BOOLEAN left_justify;
   DDS_BOOLEAN show_sign;
   DDS_BOOLEAN show_blank; /* force blank prefix */
   DDS_BOOLEAN alt_form;
   DDS_BOOLEAN zero_pad;
   char hlL; /* argument qualifier 'h', 'l', 'L', or '\0' */
   int min_width;
   int min_prec;
   size_t pre_size, pre_zeros, value_size;
   char *num;
   const char *value;
   size_t fmt_size;
   char prefix[PREFIX_MAX+1];        /* "+", '-', '0', '0x', '0X' */
   char numeric[NUMERIC_MAX+1];      /* unsigned, signed, and float */
   char float_fmt[FLOAT_FMT_MAX+1];  /* punt... float format */
   int pad_width;
   size_t pad_size;
   int n;
   size_t precision;
   HOST_ULLONG_T numerator, quotient;
   DDS_DEFN *eof_defn;

   /* arguments, returned by va_arg */
   HOST_LLONG_T i_arg;
   HOST_ULLONG_T u_arg;
   double d_arg;
   HOST_LDOUBLE_T ld_arg;
   char *s_arg;
   int c_arg;

   short *short_ptr;
   int *int_ptr;
   long *long_ptr;
   HOST_LLONG_T *llong_ptr;

   #ifdef DDS_VALIDATE_PRINTFC
      #define VALIDATE_BUF_MAX (10 * DDS_KB)
      size_t slen;
      char validate_buf[VALIDATE_BUF_MAX+1];
      va_list validate_ap;

      memcpy((void*)&validate_ap, (void*)ap, sizeof(va_list));
      validate_buf[0] = '\0';
      slen = VALIDATE_BUF_MAX;
      slen = vsnprintf(validate_buf, slen, fmt, validate_ap);
      if (slen >= VALIDATE_BUF_MAX) {
         ddsWarn("VALIDATE_PRINTFC: buffer length maxed out at %d\n%s\n", 
            (int)slen, validate_buf);
      }
   #endif /* DDS_VALIDATE_PRINTFC */

   fmt_start = fmt;
   eof_defn = (DDS_DEFN*)dict->defn.newest;
   eof_start = eof_defn->start;

   while(*fmt) {
      value = fmt;
      min_width = pre_size = pre_zeros = 0;
      left_justify = DDS_FALSE;
      if(*fmt == '%') {
         /* '%' conversions specifier */
         fmt++;
         if(*fmt == '%') {
            /* append literal '%' */
            ddsDictLoad(dict, &dict->load,
               NULL, fmt, 1);
            fmt++;
         } else {
            /* parse modifier for conversion specifier */
            show_sign = show_blank = alt_form = zero_pad = DDS_FALSE;
            while(*fmt) {
               if(*fmt == '-')
                  left_justify = DDS_TRUE;
               else if(*fmt == '+')
                  show_sign = DDS_TRUE;
               else if(*fmt == ' ')
                  show_blank = DDS_TRUE;
               else if(*fmt == '#')
                  alt_form = DDS_TRUE;
               else if(*fmt == '0')
                  zero_pad = DDS_TRUE;
               else
                  break;
               fmt++;
            }
            if(*fmt == '*') {
               fmt++;
               min_width = va_arg(*ap, int);
               if(min_width < 0) {
                  left_justify = DDS_TRUE;
                  min_width = - min_width;
               }
            } else if(isdigit(*fmt)) {
               for(min_width = 0; isdigit(*fmt); fmt++)
                  min_width = 10 * min_width + (*fmt - '0');
            } else {
               min_width = 0; /* default */
            }
            if(*fmt == '.') {
               fmt++;
               if(*fmt == '*') {
                  fmt++;
                  min_prec = va_arg(*ap, int);
                  if(min_prec < 0) min_prec = 0;
               } else if(isdigit(*fmt)) {
                  for(min_prec = 0; isdigit(*fmt); fmt++)
                     min_prec = 10 * min_prec + (*fmt - '0');
               } else {
                  min_prec = 0;
               }
            } else {
               min_prec = -1; /* default */
            }
            if(*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
               hlL = *(fmt++);
               if(hlL == 'l' && *fmt == 'l') {
                  fmt++;
                  hlL = 'L'; /* SGI */
               }
            } else {
               hlL = '\0';
            }
            /* parse conversion specifier */
            switch(*fmt) {
            case 'd': /* signed decimal */
            case 'i': /* signed integer */
               VA_ARG_I(i_arg);
               u_arg = (i_arg < 0) ? - i_arg : i_arg;

               NUMERIC(value, value_size, u_arg, ldigits, 10);

               if(i_arg < 0) {
                  prefix[0] = '-';
                  pre_size = 1;
               } else if(show_sign) {
                  prefix[0] = '+';
                  pre_size = 1;
               } else if(show_blank) {
                  prefix[0] = ' ';
                  pre_size = 1;
               } else {
                  pre_size = 0;
               }

               PRE_ZEROS(pre_zeros, pre_size, value_size, u_arg, min_prec);

               fmt++;
               break;
            case 'p': /* pointer (host independent via hexadecimal) */
            case 'X': /* unsigned, HEXADECIMAL */
               VA_ARG_U(u_arg);

               NUMERIC(value, value_size, u_arg, udigits, 16);

               if(alt_form && u_arg) {
                  prefix[0] = '0';
                  prefix[1] = 'X';
                  pre_size = 2;
               } else {
                  pre_size = 0;
               }

               PRE_ZEROS(pre_zeros, pre_size, value_size, u_arg, min_prec);

               fmt++;
               break;
            case 'x': /* unsigned, hexadecimal */
               VA_ARG_U(u_arg);

               NUMERIC(value, value_size, u_arg, ldigits, 16);

               if(alt_form && u_arg) {
                  prefix[0] = '0';
                  prefix[1] = 'x';
                  pre_size = 2;
               } else {
                  pre_size = 0;
               }

               PRE_ZEROS(pre_zeros, pre_size, value_size, u_arg, min_prec);

               fmt++;
               break;
            case 'o': /* unsigned, octal */
               VA_ARG_U(u_arg);

               NUMERIC(value, value_size, u_arg, ldigits, 8);

               if(alt_form && u_arg) {
                  prefix[0] = '0';
                  pre_size = 1;
               } else {
                  pre_size = 0;
               }

               PRE_ZEROS(pre_zeros, pre_size, value_size, u_arg, min_prec);

               fmt++;
               break;
            case 'u': /* unsigned */
               VA_ARG_U(u_arg);

               NUMERIC(value, value_size, u_arg, ldigits, 10);

               PRE_ZEROS(pre_zeros, pre_size, value_size, u_arg, min_prec);

               fmt++;
               break;
            case 'e': /* floats */
            case 'E': /* floats */
            case 'f': /* floats */
            case 'g': /* floats */
            case 'G': /* floats */
               /* punt... use sprintf to build numeric values */
               fmt_size = fmt - value + 1;
               assert(fmt_size <= FLOAT_FMT_MAX);
               memcpy(float_fmt, value, fmt_size);
               float_fmt[fmt_size] = '\0';
               numeric[0] = '\0';
               if(hlL == 'L') {
                  ld_arg = *(va_arg(*ap, HOST_LDOUBLE_T *));
                  sprintf(numeric, float_fmt, ld_arg);
               } else {
                  d_arg = va_arg(*ap, double);
                  sprintf(numeric, float_fmt, d_arg);
               }
               value = numeric;
               value_size = strlen(numeric);
               assert(value_size <= NUMERIC_MAX);
               fmt++;
               break;
            case 'c': /* character (via integer arg) */
               VA_ARG_C(c_arg);
               numeric[0] = c_arg;
               value = numeric;
               value_size = 1;
               fmt++;
               break;
            case 's': /* character string */
               if(min_prec < 0) min_prec = INT_MAX;
               VA_ARG_S(s_arg);
               for(n = 0; n < min_prec && s_arg[n]; n++) /* null */ ;
               value = s_arg;
               value_size = n;
               fmt++;
               break;
            case 'n': /* number of characters appended */
               i_arg = eof_defn->start - eof_start;
               if(hlL == 'h') {
                  short_ptr = va_arg(*ap, short *);
                  *short_ptr = i_arg;
               } else if(hlL == 'l') {
                  long_ptr = va_arg(*ap, long *);
                  *long_ptr = i_arg;
               } else if(hlL == 'L') { /* extension */
                  llong_ptr = va_arg(*ap, HOST_LLONG_T *);
                  *llong_ptr = i_arg;
               } else {
                  int_ptr = va_arg(*ap, int *);
                  *int_ptr = i_arg;
               }
               value_size = 0;
               fmt++;
               break;
            default: /* error, if none of the above */
               ddsDebug("format syntax, dangling '%'\n\tstring= \"%s\"\n\tdict= %s, defn= %s\n",
                  fmt_start, dict->name, dict->print_name.ptr);
               return eof_defn->start - eof_start;
            }
         }
      } else {
         /* append literal value from format string */
         while(*fmt && *fmt != '%') fmt++;
         value_size = fmt - value;
      }
      if(left_justify) {
         if(pre_size) ddsDictLoad(dict, &dict->load,
            NULL, prefix, pre_size);
         PAD(pre_zeros, zeros);
         if(value_size) ddsDictLoad(dict, &dict->load,
            NULL, value, value_size);
         PAD(min_width - (pre_size + pre_zeros + value_size),
            blanks);
      } else {
         PAD(min_width - (pre_size + pre_zeros + value_size),
            blanks);
         if(pre_size) ddsDictLoad(dict, &dict->load,
            NULL, prefix, pre_size);
         PAD(pre_zeros, zeros);
         if(value_size) ddsDictLoad(dict, &dict->load,
            NULL, value, value_size);
      }
   }

   #ifdef DDS_VALIDATE_PRINTFC
      if(strcmp(validate_buf, dict->buf.ptr + eof_start)) {
         ddsVerbose("printfc: validate format= \"%s\"\n\tprintfc=\"%s\"\n\tsprintf=\"%s\"\n",
            fmt_start, dict->buf.ptr + eof_start, validate_buf);
      }
   #endif /* DDS_VALIDATE_PRINTFC */

   return eof_defn->start - eof_start;
}
