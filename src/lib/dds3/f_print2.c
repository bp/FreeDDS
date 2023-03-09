static char rcsid[]="$Id: f_print2.c 108 2009-08-17 20:36:53Z user $";
/*
RCS-t=DDS, create and control printing to a unique print file 

*/

/*
     Formatted DDS dictionary output with run-time type-checking
     August 2009 by Musa Maharramov, BP High Performance Computing
     To be linked with dds.F90 interface module

     Based on ddsPrtMsgF by Randy Selzler, xx May 1996
*/

#include <limits.h>
#include "dds_sys.h"

#define PREFIX_MAX 8
#define NUMERIC_MAX 512
#define FLOAT_FMT_MAX 64
#define PAD_MAX 32 
#define FMT_NOM_LEN 2 * DDS_KB

/* pointers to "DDS_ADDR" structures are passed to fdds_printf2 */
typedef struct DDS_ADDR {
  int t; /* character deining type of argument ('i','f','s') */
  int l; /* size in bytes or length in chars */
  void * p; /* argument address */
} dds_addr;

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
           ddsError("Format string: %s\nexpected Fortran string in argument %d to fdds_prt***2, passed %c%d",fmt_start, argcount, p->t,p->l); \
        if (p->l <= 0)  \
           ddsError("Format string: %s\nexpected +ive Fortran string length in argument %d to fdds_prt***2",fmt_start, argcount, fmt_start); \
        (s_arg) = (char*)_fcdtocp((_fcd)p); (s_len) = _fcdlen((_fcd)p); }
#else  /* HOST_FTN_CRAY_STR */ 
  #define VA_ARG_S(s_arg, s_len)  \
      { dds_addr * p=va_arg(*ap,dds_addr*); argcount++; \
        if (p->t != 's')  \
           ddsError("Format string: %s\nexpected Fortran string in argument %d to fdds_prt***2, passed %c%d",fmt_start, argcount, p->t,p->l); \
        if (p->l <= 0)  \
           ddsError("Format string: %s\nexpected +ive Fortran string length in argument %d to fdds_prt***2",fmt_start, argcount); \
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
         ddsStrCatN(&string, fill, pad_size); \
         pad_width -= pad_size; } }


static const char blanks[PAD_MAX+1]= "                                ";
static const char zeros[PAD_MAX+1] = "00000000000000000000000000000000";
static const char ldigits[17] = "0123456789abcdef";
static const char udigits[17] = "0123456789ABCDEF";


/***********************************************************************
 *
 *	ddsPrtmsgF2
 *
 **********************************************************************/
static char* ddsPrtmsgF2(
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
               ddsDebug("ddsPrtmsgF2: dangling '%'\n\tstring= \"%s\"\n",
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
 *	FDDS_SPRINTF2
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_sprintf2, FDDS_SPRINTF2)(
   #ifdef HOST_FTN_CRAY_STR
      _fcd str_fcd,
      _fcd fmt_fcd,
      ...   
   #else
      HOST_FTN_STR str_ftn,
      HOST_FTN_STR fmt_ftn,
      ...
   #endif
   )
{
   static DDS_STRING fmt_str = NULL_STRING;
   int size;
   char *msg;
   va_list ap, ap_len;

   #ifdef HOST_FTN_CRAY_STR
      HOST_FTN_STR str_ftn;
      HOST_FTN_STR fmt_ftn;
      HOST_FTN_LEN str_len;

      str_ftn = (char*)_fcdtocp(str_fcd);
      fmt_ftn = (char*)_fcdtocp(fmt_fcd);
      str_len = _fcdlen(alias_fcd);
      va_start(ap, fmt_fcd);

      DDS_API_INIT;

      FTN_FMT_TO_STR(fmt_str, fmt_ftn);
   #else 
      HOST_FTN_LEN str_len;
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

      DDS_API_INIT;

      FTN_FMT_TO_STR(fmt_str, fmt_ftn);
 
      /* Walk "ap_len" past the pointers to actual args */
      memcpy((void*)&ap_len, (void*)&ap, sizeof(va_list));
      ddsPrtmsgF2(&ap_len, NULL, fmt_str.ptr);

      /* length of "str" character argument */
      str_len = va_arg(ap_len, HOST_FTN_LEN);
 
      /* length of "fmt" character argument */
      va_arg(ap_len, HOST_FTN_LEN);   
   #endif /* HOST_FTN_CRAY_STR */
 
   msg = ddsPrtmsgF2(&ap, &ap_len, fmt_str.ptr);

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

   return(size);
}



/***********************************************************************
 *
 *	FDDS_PRTMSG2
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_prtmsg2, FDDS_PRTMSG2)(
   #ifdef HOST_FTN_CRAY_STR
      _fcd fmt_fcd,
      ...   
   #else
      HOST_FTN_STR fmt_ftn,
      ...
   #endif
   )
{

   static DDS_STRING fmt_str = NULL_STRING;
   int size;
   char *msg;
   va_list ap, ap_len;

   #ifdef HOST_FTN_CRAY_STR
      HOST_FTN_STR fmt_ftn;

      fmt_ftn = (char*)_fcdtocp(fmt_fcd);
      va_start(ap, fmt_fcd);

      DDS_API_INIT;

      FTN_FMT_TO_STR(fmt_str, fmt_ftn);
   #else 
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


      DDS_API_INIT;

      FTN_FMT_TO_STR(fmt_str, fmt_ftn);
 
      /* Walk "ap_len" past the pointers to actual args */
      memcpy((void*)&ap_len, (void*)&ap, sizeof(va_list));
      ddsPrtmsgF2(&ap_len, NULL, fmt_str.ptr);
 
      /* length of "fmt" character argument */
      va_arg(ap_len, HOST_FTN_LEN);   
   #endif /* HOST_FTN_CRAY_STR */
 
   msg = ddsPrtmsgF2(&ap, &ap_len, fmt_str.ptr);

   va_end(ap);

   if (msg) {
      size = strlen(msg);
      cdds_prtmsg(msg);
      ddsFree(msg);
      return size;
   }

   return 0;
}


/***********************************************************************
 *
 *	FDDS_PRTCON2
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_prtcon2, FDDS_PRTCON2)(
   #ifdef HOST_FTN_CRAY_STR
      _fcd fmt_fcd,
      ...   
   #else
      HOST_FTN_STR fmt_ftn,
      ...
   #endif
   )
{

   static DDS_STRING fmt_str = NULL_STRING;
   int size;
   char *msg;
   va_list ap, ap_len;

   #ifdef HOST_FTN_CRAY_STR
      HOST_FTN_STR fmt_ftn;

      fmt_ftn = (char*)_fcdtocp(fmt_fcd);
      va_start(ap, fmt_fcd);

      DDS_API_INIT;

      FTN_FMT_TO_STR(fmt_str, fmt_ftn);
   #else 
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


      DDS_API_INIT;

      FTN_FMT_TO_STR(fmt_str, fmt_ftn);
 
      /* Walk "ap_len" past the pointers to actual args */
      memcpy((void*)&ap_len, (void*)&ap, sizeof(va_list));
      ddsPrtmsgF2(&ap_len, NULL, fmt_str.ptr);
 
      /* length of "fmt" character argument */
      va_arg(ap_len, HOST_FTN_LEN);   
   #endif /* HOST_FTN_CRAY_STR */
 
   msg = ddsPrtmsgF2(&ap, &ap_len, fmt_str.ptr);

   va_end(ap);

   if (msg) {
      size = strlen(msg);
      cdds_prtcon(msg);
      ddsFree(msg);
      return size;
   }

   return 0;
}


/***********************************************************************
 *
 *	FDDS_PRTERR2
 *
 **********************************************************************/
HOST_FTN_INT
HOST_FTN_FUNC(fdds_prterr2, FDDS_PRTERR2)(
   #ifdef HOST_FTN_CRAY_STR
      _fcd fmt_fcd,
      ...   
   #else
      HOST_FTN_STR fmt_ftn,
      ...
   #endif
   )
{

   static DDS_STRING fmt_str = NULL_STRING;
   int size;
   char *msg;
   va_list ap, ap_len;

   #ifdef HOST_FTN_CRAY_STR
      HOST_FTN_STR fmt_ftn;

      fmt_ftn = (char*)_fcdtocp(fmt_fcd);
      va_start(ap, fmt_fcd);

      DDS_API_INIT;

      FTN_FMT_TO_STR(fmt_str, fmt_ftn);
   #else 
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


      DDS_API_INIT;

      FTN_FMT_TO_STR(fmt_str, fmt_ftn);
 
      /* Walk "ap_len" past the pointers to actual args */
      memcpy((void*)&ap_len, (void*)&ap, sizeof(va_list));
      ddsPrtmsgF2(&ap_len, NULL, fmt_str.ptr);
 
      /* length of "fmt" character argument */
      va_arg(ap_len, HOST_FTN_LEN);   
   #endif /* HOST_FTN_CRAY_STR */

   FTN_FMT_TO_STR(fmt_str, fmt_ftn);
 
   msg = ddsPrtmsgF2(&ap, &ap_len, fmt_str.ptr);

   va_end(ap);

   if (msg) {
      size = strlen(msg);
      cdds_prterr(msg);
      ddsFree(msg);
      return size;
   }

   return 0;
}

