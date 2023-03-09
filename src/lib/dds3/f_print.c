static char rcsid[]="$Id: f_print.c 117 2009-09-29 19:26:22Z user $";
/*
RCS-t=DDS, create and control printing to a unique print file 

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
#define PAD_MAX 32 
#define FMT_NOM_LEN 2 * DDS_KB

/* macro to retrieve character (integer) argument */
#define VA_ARG_C(c_arg)  \
   if(hlL == 'h') { (c_arg) = *(va_arg(*ap, short*));  \
   } else if(hlL == 'l') { (c_arg) = *(va_arg(*ap, long*));  \
   } else if(hlL == 'L') { (c_arg) = *(va_arg(*ap, HOST_LLONG_T*));  \
   } else { (c_arg) = *(va_arg(*ap, int*)); }

/* macro to retrieve string pointer argument */
#define VA_ARG_S(s_arg, s_len)  \
   { (s_arg) = va_arg(*ap, char*);  \
     (s_len) = ap_len ? (va_arg(*ap_len, HOST_FTN_LEN)) : 0; }

/* macro to retrieve signed integer argument */
#define VA_ARG_I(i_arg)  \
   if(hlL == 'h') { (i_arg) = *(va_arg(*ap, short*));  \
   } else if(hlL == 'l') { (i_arg) = *(va_arg(*ap, long*));  \
   } else if(hlL == 'L') { (i_arg) = *(va_arg(*ap, HOST_LLONG_T*));  \
   } else { (i_arg) = *(va_arg(*ap, int*)); }

/* macro to retrieve unsigned integer argument */
#define VA_ARG_U(u_arg)  \
   if(hlL == 'h') { (u_arg) = *(va_arg(*ap, unsigned short*));  \
   } else if(hlL == 'l') { (u_arg) = *(va_arg(*ap, unsigned long*));  \
   } else if(hlL == 'L') { (u_arg) = *(va_arg(*ap, HOST_ULLONG_T*));  \
   } else { (u_arg) = *(va_arg(*ap, unsigned*)); }

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
         ddsStrCatN(&string, fill, pad_size); \
         pad_width -= pad_size; } }


static const char blanks[PAD_MAX+1]= "                                ";
static const char zeros[PAD_MAX+1] = "00000000000000000000000000000000";
static const char ldigits[17] = "0123456789abcdef";
static const char udigits[17] = "0123456789ABCDEF";


/***********************************************************************
 *
 *	ddsPrtmsgF
 *
 **********************************************************************/
static char* ddsPrtmsgF(
   va_list *ap,
   va_list *ap_len,
   const char *fmt)
{
   const char *fmt_start;
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
   DDS_STRING string = NULL_STRING;

   /* arguments, returned by va_arg */
   HOST_LLONG_T i_arg;
   HOST_ULLONG_T u_arg;
   double d_arg;
   HOST_LDOUBLE_T ld_arg;
   char *s_arg;
   int c_arg;
   size_t s_len;

   short *short_ptr;
   int *int_ptr;
   long *long_ptr;
   HOST_LLONG_T *llong_ptr;

   fmt_start = fmt;

   while(*fmt) {
      value = fmt;
      min_width = pre_size = pre_zeros = 0;
      left_justify = DDS_FALSE;
		if(*fmt == '%') {
         /* '%' conversions specifier */
         fmt++;
         if(*fmt == '%') {
            /* append literal '%' */
            /* if (ap_len) ddsStrCatN(&string, fmt, 1); */
            fmt++;
            value_size = 2;
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
               min_width = *(va_arg(*ap, int*));
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
                  min_prec = *(va_arg(*ap, int*));
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

               PRE_ZEROS(pre_zeros, pre_size, value_size, u_arg, 
                         min_prec);

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

               PRE_ZEROS(pre_zeros, pre_size, value_size, u_arg,
                         min_prec);

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

               PRE_ZEROS(pre_zeros, pre_size, value_size, u_arg, 
                         min_prec);

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

               PRE_ZEROS(pre_zeros, pre_size, value_size, u_arg, 
                         min_prec);

               fmt++;
               break;
            case 'u': /* unsigned */
               VA_ARG_U(u_arg);

               NUMERIC(value, value_size, u_arg, ldigits, 10);

               PRE_ZEROS(pre_zeros, pre_size, value_size, u_arg,
                         min_prec);

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
               } else if(hlL == 'l') {
                  /* non-standard extension */
                  float_fmt[fmt_size - 2] = float_fmt[fmt_size - 1];
                  float_fmt[fmt_size - 1] = '\0';
                  d_arg = *(va_arg(*ap, double*));
                  sprintf(numeric, float_fmt, d_arg);
               } else {
                  d_arg = *(va_arg(*ap, float*));
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
               if (numeric[0] == '%') {
                  numeric[1] = '%';
                  value_size++;
               }
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
               if (string.ptr) i_arg = strlen(string.ptr);
               else i_arg = 0;
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
               ddsDebug("ddsPrtmsgF: dangling '%'\n\tstring= \"%s\"\n",
                  fmt_start);
               if (string.ptr) ddsFree(string.ptr);
               return 0;
            }
         }
      } else {
         /* append literal value from format string */
         while(*fmt && *fmt != '%') fmt++;
         value_size = fmt - value;
      }
      if (ap_len) {
         if(left_justify) {
            if(pre_size) ddsStrCatN(&string, prefix, pre_size);
            PAD(pre_zeros, zeros);
            if(value_size) ddsStrCatN(&string, value, value_size);
            PAD(min_width - (pre_size + pre_zeros + value_size), 
               blanks);
         } else {
            PAD(min_width - (pre_size + pre_zeros + value_size), 
               blanks);
            if(pre_size) ddsStrCatN(&string, prefix, pre_size);
            PAD(pre_zeros, zeros);
            if(value_size) ddsStrCatN(&string, value, value_size);
         }
      }
   }

   return string.ptr;
}


/***********************************************************************
 *
 *	FDDS_OPENPR
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_openpr, FDDS_OPENPR)(
   HOST_FTN_STR pname_ftn,
   HOST_FTN_STR id_ftn,
   HOST_FTN_LEN pname_len,
   HOST_FTN_LEN id_len)
{
   static DDS_STRING pname = NULL_STRING;
   static DDS_STRING id = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(pname, pname_ftn, pname_len);
   FTN_STR_TO_STR(id, id_ftn, id_len);

   return cdds_openpr(pname.ptr, id.ptr);
}


/***********************************************************************
 *
 *	FDDS_SAVEPR
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_savepr, FDDS_SAVEPR)()
{
   DDS_API_INIT;

   return cdds_savepr();
}


/***********************************************************************
 *
 *	FDDS_PRTHDR
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_prthdr, FDDS_PRTHDR)(
   HOST_FTN_STR prg_ftn,
   HOST_FTN_STR id_ftn,
   HOST_FTN_LEN prg_len,
   HOST_FTN_LEN id_len)
{
   static DDS_STRING prg = NULL_STRING;
   static DDS_STRING id = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(prg, prg_ftn, prg_len);
   FTN_STR_TO_STR(id, id_ftn, id_len);

   return cdds_prthdr(prg.ptr, id.ptr);
}


/***********************************************************************
 *
 *	FDDS_SPRINTF
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_sprintf, FDDS_SPRINTF)(
   HOST_FTN_STR str_ftn,
   HOST_FTN_STR fmt_ftn,
   ...)
{
   static DDS_STRING fmt_str = NULL_STRING;
   int size;
   char *msg;
   va_list ap, ap_len;

   HOST_FTN_LEN str_len, fmt_len;
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
   */  

   DDS_API_ENTRY("sprintf")

   FTN_FMT_TO_STR(fmt_str, fmt_ftn);

   /* Walk "ap_len" past the pointers to actual args */
   memcpy((void*)&ap_len, (void*)&ap, sizeof(va_list));
   ddsPrtmsgF(&ap_len, NULL, fmt_str.ptr);

   /* length of "str" character argument */
   str_len = va_arg(ap_len, HOST_FTN_LEN);

   /* length of "fmt" character argument */
   fmt_len = va_arg(ap_len, HOST_FTN_LEN);   
   if (fmt_len < strlen(fmt_str.ptr)) {
      ddsError("fdds_sprintf: definition format (%s)\n\tnull termination length=%d > length=%d\n",
               fmt_str.ptr, strlen(fmt_str.ptr), fmt_len);
      va_end(ap);
      DDS_API_EXIT(EOF);
   }

   msg = ddsPrtmsgF(&ap, &ap_len, fmt_str.ptr);

   va_end(ap);

   if (msg) {
      size = strlen(msg);
      if (size > str_len) size = str_len;
      STR_TO_FTN_STR(str_ftn, str_len, msg);
      ddsFree(msg);
   } else {
      size = 0;
      STR_TO_FTN_STR(str_ftn, str_len, NULL);
   }

   DDS_API_EXIT(size);
}



/***********************************************************************
 *
 *	FDDS_PRTMSG
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_prtmsg, FDDS_PRTMSG)(
   HOST_FTN_STR fmt_ftn,
   ...)
{
   HOST_FTN_LEN fmt_len;
   static DDS_STRING fmt_str = NULL_STRING;
   int size;
   char *msg;
   va_list ap, ap_len;

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
   */  

   DDS_API_ENTRY("prtmsg")

   FTN_FMT_TO_STR(fmt_str, fmt_ftn);

   /* Walk "ap_len" past the pointers to actual args */
   memcpy((void*)&ap_len, (void*)&ap, sizeof(va_list));
   ddsPrtmsgF(&ap_len, NULL, fmt_str.ptr);

   /* length of "fmt" character argument */
   fmt_len = va_arg(ap_len, HOST_FTN_LEN);   
   if (fmt_len < strlen(fmt_str.ptr)) {
      ddsError("fdds_prtmsg: definition format (%s)\n\tnull termination length=%d > length=%d\n",
               fmt_str.ptr, strlen(fmt_str.ptr), fmt_len);
      va_end(ap);
      DDS_API_EXIT(EOF);
   }

   msg = ddsPrtmsgF(&ap, &ap_len, fmt_str.ptr);

   va_end(ap);

   if (msg) {
      size = strlen(msg);
      cdds_prtmsg(msg);
      ddsFree(msg);
      DDS_API_EXIT(size);
   }

   DDS_API_EXIT(0);
}


/***********************************************************************
 *
 *	FDDS_PRTCON
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_prtcon, FDDS_PRTCON)(
   HOST_FTN_STR fmt_ftn,
   ...)
{
   HOST_FTN_LEN fmt_len;
   static DDS_STRING fmt_str = NULL_STRING;
   int size;
   char *msg;
   va_list ap, ap_len;

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
   */  

   DDS_API_ENTRY("prtcon")

   FTN_FMT_TO_STR(fmt_str, fmt_ftn);

   /* Walk "ap_len" past the pointers to actual args */
   memcpy((void*)&ap_len, (void*)&ap, sizeof(va_list));
   ddsPrtmsgF(&ap_len, NULL, fmt_str.ptr);

   /* length of "fmt" character argument */
   fmt_len = va_arg(ap_len, HOST_FTN_LEN);   
   if (fmt_len < strlen(fmt_str.ptr)) {
      ddsError("fdds_prtcon: definition format (%s)\n\tnull termination length=%d > length=%d\n",
               fmt_str.ptr, strlen(fmt_str.ptr), fmt_len);
      va_end(ap);
      DDS_API_EXIT(EOF);
   }

   msg = ddsPrtmsgF(&ap, &ap_len, fmt_str.ptr);

   va_end(ap);

   if (msg) {
      size = strlen(msg);
      cdds_prtcon(msg);
      ddsFree(msg);
      DDS_API_EXIT(size);
   }

   DDS_API_EXIT(0);
}


/***********************************************************************
 *
 *	FDDS_PRTERR
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_prterr, FDDS_PRTERR)(
   HOST_FTN_STR fmt_ftn,
   ...)
{
   HOST_FTN_LEN fmt_len;
   static DDS_STRING fmt_str = NULL_STRING;
   int size;
   char *msg;
   va_list ap, ap_len;

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
   */  

   DDS_API_ENTRY("prterr")

   FTN_FMT_TO_STR(fmt_str, fmt_ftn);

   /* Walk "ap_len" past the pointers to actual args */
   memcpy((void*)&ap_len, (void*)&ap, sizeof(va_list));
   ddsPrtmsgF(&ap_len, NULL, fmt_str.ptr);

   /* length of "fmt" character argument */
   fmt_len = va_arg(ap_len, HOST_FTN_LEN);   
   if (fmt_len < strlen(fmt_str.ptr)) {
      ddsError("fdds_prerrn: definition format (%s)\n\tnull termination length=%d > length=%d\n",
               fmt_str.ptr, strlen(fmt_str.ptr), fmt_len);
      va_end(ap);
      DDS_API_EXIT(EOF);
   }

   FTN_FMT_TO_STR(fmt_str, fmt_ftn);
 
   msg = ddsPrtmsgF(&ap, &ap_len, fmt_str.ptr);

   va_end(ap);

   if (msg) {
      size = strlen(msg);
      cdds_prterr(msg);
      ddsFree(msg);
      DDS_API_EXIT(size);
   }

   DDS_API_EXIT(0);
}


/***********************************************************************
 *
 *	FDDS_INFO
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_info, FDDS_INFO)(
   HOST_FTN_INT *enable
   )
{
   return cdds_info(*enable);
}


/***********************************************************************
 *
 *	FDDS_ERRORS
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_errors, FDDS_ERRORS)()
{
   return cdds_errors();
}


/***********************************************************************
 *
 *	FDDS_CLOSEPR
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_closepr, FDDS_CLOSEPR)()
{
   return cdds_closepr();
}


/***********************************************************************
 *
 *	FDDS_CLOSEPR2
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_closepr2, FDDS_CLOSEPR2)(
   HOST_FTN_INT *status
   )
{
   return cdds_closepr2(*status);
}
