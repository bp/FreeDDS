static char rcsid[]="$Id: f_printf2.c 108 2009-08-17 20:36:53Z user $";
/*
RCS-t=DDS API, append a formatted value, Fortran interface.
*/

/*
     Formatted DDS dictionary output with run-time type-checking
     July 2009 by Musa Maharramov, BP High Performance Computing
     To be linked with dds.F90 interface module

     Based on ddsPrintfF by Randy Selzler, xx May 1996
*/

#include <limits.h>
#include "dds_sys.h"

#define PREFIX_MAX 8
#define NUMERIC_MAX 512
#define FLOAT_FMT_MAX 64

/* pointers to "DDS_ADDR" structures are passed to fdds_printf2 */
typedef struct DDS_ADDR {
  int t; /* character deining type of argument ('i','f','s') */
  int l; /* size in bytes or length in chars */
  void * p; /* argument address */
} dds_addr;

                                   /*           1         2         3    */
#define PAD_MAX 32                 /*  12345678901234567890123456789012  */
static const char blanks[PAD_MAX+1] = "                                ";
static const char zeros[PAD_MAX+1]  = "00000000000000000000000000000000";
static const char ldigits[17] =       "0123456789abcdef";
static const char udigits[17] =       "0123456789ABCDEF";

/* macro to retrieve a pointer from DDS_ADDR parameter */
#define DP(c,type) \
  ((type *)(check_type(c,sizeof(type),ap,fmt_start,&argcount)))

/* check for type the next DDS_ADDR* points to and return its pointer member */
void * check_type(char type, int size,va_list* ap,const char * fmt,int * argcount); 

/* macro to retrieve character (integer) argument */
#define VA_ARG_C(c_arg)  \
   if(hlL == 'h') { (c_arg) = *(DP('i',short));  \
   } else if(hlL == 'l') { (c_arg) = *(DP('i',long));  \
   } else if(hlL == 'L') { (c_arg) = *(DP('i',HOST_LLONG_T));  \
   } else { (c_arg) = *(DP('i',int)); }

/* macro to retrieve string pointer argument */
#ifdef HOST_FTN_CRAY_STR 
  #define VA_ARG_S(s_arg, s_len)  \
      { dds_addr * p=va_arg(*ap,dds_addr*); argcount++; \
        if (p->t != 's')  \
           ddsError("Format string: %s\nexpected Fortran string in argument %d to fdds_printf2, passed %c%d",fmt_start, argcount, p->t,p->l); \
        if (p->l <= 0)  \
           ddsError("Format string: %s\nexpected +ive Fortran string length in argument %d to fdds_printf2",fmt_start, argcount, fmt_start); \
        (s_arg) = (char*)_fcdtocp((_fcd)p); (s_len) = _fcdlen((_fcd)p); }
#else  /* HOST_FTN_CRAY_STR */ 
  #define VA_ARG_S(s_arg, s_len)  \
      { dds_addr * p=va_arg(*ap,dds_addr*); argcount++; \
        if (p->t != 's')  \
           ddsError("Format string: %s\nexpected Fortran string in argument %d to fdds_printf2, passed %c%d",fmt_start, argcount, p->t,p->l); \
        if (p->l <= 0)  \
           ddsError("Format string: %s\nexpected +ive Fortran string length in argument %d to fdds_printf2",fmt_start, argcount); \
        (s_arg) = (char*)(p->p); (s_len) = (HOST_FTN_LEN)(p->l); }
#endif /* HOST_FTN_CRAY_STR */


/* macro to retrieve signed integer argument */
#define VA_ARG_I(i_arg)  \
   if(hlL == 'h') { (i_arg) = *(DP('i',short));  \
   } else if(hlL == 'l') { (i_arg) = *(DP('i',long)); \
   } else if(hlL == 'L') { (i_arg) = *(DP('i',HOST_LLONG_T)); \
   } else { (i_arg) = *(DP('i',int)); }

/* macro to retrieve unsigned integer argument */
#define VA_ARG_U(u_arg)  \
   if(hlL == 'h') { (u_arg) = *(DP('i', unsigned short)); \
   } else if(hlL == 'l') { (u_arg) = *(DP('i', unsigned long)); \
   } else if(hlL == 'L') { (u_arg) = *(DP('i', HOST_ULLONG_T)); \
   } else { (u_arg) = *(DP('i', unsigned)); }

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

int ddsPrintfF2(
   DDS_DICT *dict,
   va_list *ap,
   va_list *ap_len,
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
   size_t s_len;
   #ifdef HOST_FTN_CRAY_STR
      _fcd s_fcd;
   #endif /* HOST_FTN_CRAY_STR */

   short *short_ptr;
   int *int_ptr;
   long *long_ptr;
   HOST_LLONG_T *llong_ptr;

   /* the argument being currently processed */
   int argcount;

   argcount = 0;

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
            if(ap_len) ddsDictLoad(dict, &dict->load,
               NULL, fmt, 1);
            fmt++;
         } else {
            /* parse modifier for conversion specifier */
            left_justify = show_sign = show_blank =
               alt_form = zero_pad = DDS_FALSE;
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
               min_width = *(DP('i',int));
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
                  min_prec = *(DP('i',int));
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
                  ld_arg = *(DP('f',HOST_LDOUBLE_T));
                  /* non-standard extension */
                  float_fmt[fmt_size - 2] = float_fmt[fmt_size - 1];
                  float_fmt[fmt_size - 1] = '\0';
                  sprintf(numeric, float_fmt, ld_arg);
               } else if(hlL == 'l') {
                  /* non-standard extension */
                  float_fmt[fmt_size - 2] = float_fmt[fmt_size - 1];
                  float_fmt[fmt_size - 1] = '\0';
                  d_arg = *(DP('f',double));
                  sprintf(numeric, float_fmt, d_arg);
               } else {
                  d_arg = *(DP('f',float));
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
               VA_ARG_S(s_arg, s_len);
               while(s_len && s_arg[s_len - 1] == ' ') s_len--;
               if(s_len > min_prec) s_len = min_prec;
               for(n = 0; n < s_len && s_arg[n]; n++) /* null */ ;
               value = s_arg;
               value_size = n;
               fmt++;
               break;
            case 'n': /* number of characters appended */
               i_arg = eof_defn->start - eof_start;
               if(hlL == 'h') {
                  short_ptr = DP('i',short);
                  *short_ptr = i_arg;
               } else if(hlL == 'l') {
                  long_ptr = DP('i',long);
                  *long_ptr = i_arg;
               } else if(hlL == 'L') { /* extension */
                  llong_ptr = DP('i',HOST_LLONG_T);
                  *llong_ptr = i_arg;
               } else {
                  int_ptr = DP('i', int);
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
      if(ap_len) {
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
   }

   return eof_defn->start - eof_start;
}


#define ALIAS_NOM_LEN 4 * DEFNNAME_MAX

#define FMT_NOM_LEN 2 * DDS_KB


HOST_FTN_INT
HOST_FTN_FUNC(fdds_printf2, FDDS_PRINTF2)(
   #ifdef HOST_FTN_CRAY_STR
      _fcd alias_fcd,
      _fcd fmt_fcd,
      ...
   #else  /* HOST_FTN_CRAY_STR */
      HOST_FTN_STR alias_ftn,
      HOST_FTN_STR fmt_ftn,
      ...
   #endif /* HOST_FTN_CRAY_STR */
   )
{
   static DDS_STRING alias_str = NULL_STRING;
   static DDS_STRING fmt_str = NULL_STRING;

   const char *alias, *test;
   int value_size;
   DDS_DEFN *newest_defn;
   va_list ap, ap_len;

   #ifdef HOST_FTN_CRAY_STR
      HOST_FTN_STR alias_ftn;
      HOST_FTN_STR fmt_ftn;
      HOST_FTN_LEN alias_len;
      HOST_FTN_LEN fmt_len;

      alias_ftn = (char*)_fcdtocp(alias_fcd);
      fmt_ftn = (char*)_fcdtocp(fmt_fcd);
      alias_len = _fcdlen(alias_fcd);
      fmt_len = _fcdlen(fmt_fcd);
   #else  /* HOST_FTN_CRAY_STR */
      HOST_FTN_LEN alias_len;
   #endif /* HOST_FTN_CRAY_STR */

   DDS_API_ENTRY("printf")

   if(! dds_print_first) {
      DDS_API_EXIT(EOF);
   }

   FTN_FMT_TO_STR(fmt_str, fmt_ftn);

   #ifdef HOST_FTN_CRAY_STR
      va_start(ap, fmt_fcd);
   #else  /* HOST_FTN_CRAY_STR */
      va_start(ap, fmt_ftn);
      /* NOTE: explaination of the following insane kludge...
         The length information for Fortran character arguments
         appears in the argument list following the normal pointers.
         "ap_len" contains an independent copy of va_list info.
         It must be walked pasted the normal pointers,
         using the format '%' conversion specifications.
         The format and argument conversion can then proceed,
         using the original "ap" (pointers to actual args) and
         "ap_len" (length of Fortran character args).

MAM -- this "kludge" is no longer needed -- fdds_printf2 is passed
      pointers to DDS_ADDR structures describing the actual args;
      however, ap_len is kept here to minimise changes of old code  

      */

      /* Walk "ap_len" past the pointers to actual args */
      memcpy((void*)&ap_len, (void*)&ap, sizeof(va_list));
      value_size = ddsPrintfF2(dds_print_first, &ap_len, NULL, fmt_str.ptr);
      if(value_size == EOF) {
         va_end(ap);
         DDS_API_EXIT(EOF);
      }

      /* length of "alias" character argument */
      alias_len = va_arg(ap_len, HOST_FTN_LEN);

      /* length of "fmt" character argument */
      va_arg(ap_len, HOST_FTN_LEN);
   #endif /* HOST_FTN_CRAY_STR */

   FTN_STR_TO_STR(alias_str, alias_ftn, alias_len);

   alias = alias_str.ptr;

   while(isspace(*alias)) alias++;

   if(*alias) {
      /* validate syntax of definition name */
      alias = ddsDefnAxis(alias, dds_print_first);
      for(test=alias; DEFN_SYNTAX(*test); test++) /* null */ ;

      while(isspace(*test)) test++;

      if(*test == '=' && test[1] == '\0') {
         /* control defn (deprecate) */
         assert(! "cdds_print control defn (deprecate)");
      } else if(*test) {
         dds_error = EINVAL;
         ddsError("print: definition name syntax (%s)\n\tdictionary= %s\n",
            ddsHint(alias), dds_print_first->name);
         va_end(ap);
         DDS_API_EXIT(EOF);
      }

      /* append new definition name */
      newest_defn = (DDS_DEFN*)dds_print_first->defn.newest;
      if(newest_defn->start &&
         ! isspace(dds_print_first->buf.ptr
            [newest_defn->start - 1])) {
         /* insert '\n' (white space) name prefix */
         ddsDictLoad(dds_print_first, &dds_print_first->load,
            NULL, "\n", 1);
      }
      ddsDictLoad(dds_print_first, &dds_print_first->load,
            NULL, alias, (test - alias));
      ddsDictLoad(dds_print_first, &dds_print_first->load,
            NULL, "=", 1);
   }

   /* append definition value */
   value_size = ddsPrintfF2(dds_print_first, &ap, &ap_len, fmt_str.ptr);
   va_end(ap);

   DDS_API_EXIT(value_size);
}

