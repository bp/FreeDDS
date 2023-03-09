static char rcsid[]="$Id: scanfc.c 232 2011-07-18 23:08:48Z user $";
/*
RCS-t=DDS, retrieve formatted value, C interface.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <limits.h>
#include "dds_sys.h"

#define FIELD_MAX 511 /* numeric field maximum width */

#ifdef DDS_VALIDATE_SCANFC
   #define VALIDATE_VALUE(type, ptr, big)  \
      { ptr = va_arg(*ap, type *);  \
        *(type*)(&validate_union) = big;  \
         if(memcmp(ptr, &validate_union, sizeof(type)) {  \
            ddsVerbose("scanfc: validate %i + format= \"%s\"\n\tvalue=\"%s\"\n",  \
               (int)(fmt - fmt_start), fmt_start, *value);  \
      } };
VALIDATE_VALUE(short, short_ptr, ibig);
#else  /* DDS_VALIDATE_SCANFC */
   #define VALIDATE_VALUE(type, ptr, big)  \
      { ptr = va_arg(*ap, type *);  \
        *ptr = big; };
#endif /* DDS_VALIDATE_SCANFC */


int ddsScanfC(
   const char **value,
   va_list *ap,
   const char *fmt,
   const char *dict_name,
   const char *defn_name)
{
   int assigned = 0; /* count of arguments converted and assigned */
   DDS_BOOLEAN suppress; /* suppress assignment */
   DDS_BOOLEAN negate; /* scan set negation "[^..." */
   char scanset[UCHAR_MAX]; /* scan set */
   const char *setend; /* scan set end "...]" */
   size_t setsize; /* scan set size */
   int test, test_prev, test_next; /* scan set test character */
   char hlL; /* argument precision 'h', 'l', 'L', or '\0' */
   HOST_LLONG_T width; /* actual field width */
   int max_width; /* maximum field width */
   int base; /* numeric base */
   const char *fmt_start; /* format string start */
   const char *next, *start; /* temporary buffer and pointers */
   char field[FIELD_MAX+1], *stop;
   int n;

   HOST_LLONG_T ibig; /* numeric value (signed big) */
   HOST_ULLONG_T ubig; /* numeric value (unsigned big) */
   double dbig; /* numeric value (double precision float) */

   /* argument pointers, returned by va_arg */
   char *char_ptr;
   short *short_ptr;
   int *int_ptr;
   long *long_ptr;
   HOST_LLONG_T *llong_ptr;
   unsigned short *ushort_ptr;
   unsigned *uint_ptr;
   unsigned long *ulong_ptr;
   HOST_ULLONG_T *ullong_ptr;
   float *float_ptr;
   double *double_ptr;
   HOST_LDOUBLE_T *long_double_ptr;
   void *void_ptr;

   #ifdef DDS_VALIDATE_SCANFC
      union {
         long l;
         long_double ld;
         void *p;
      } validate_union;
      int validate_rc;
      va_list validate_ap;
 
      memcpy(validate_ap, ap, sizeof(va_list));
      validate_rc = vsscanf(*value, fmt, validate_ap);
   #endif /* DDS_VALIDATE_SCANFC */

   fmt_start = fmt;
   next = *value;

   while(*fmt) {
      if(isspace(*fmt)) {
         /* skip white space in format and value */
         while(isspace(*fmt)) fmt++;
         while(isspace(*next)) next++;
      }
      if(*fmt == '%') {
         /* '%' conversion specifier */
         fmt++;
         if(*fmt == '%') {
            /* match one '%' in value */
            if(*next != '%') break;
            fmt++;
            next++;
         } else {
            /* parse modifier for conversion specifier */
            if(*fmt == '*') {
               fmt++;
               suppress = DDS_TRUE;
            } else {
               suppress = DDS_FALSE;
            }
            if(isdigit(*fmt)) {
               for(max_width = 0; isdigit(*fmt); fmt++)
                  max_width = 10 * max_width + (*fmt - '0');
               if(! max_width) {
                  ddsDebug("format string syntax, zero max field width\n\tstring= \"%s\"\n\tdict= %s, defn= %s\n",
                     fmt_start, dict_name, defn_name);
                  break;
               }
            } else {
               max_width = -1;
            }
            if(*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
               hlL = *(fmt++);
            } else {
               hlL = '\0';
            }
            /* parse conversion specifier */
            switch(*fmt) {
            case 'd': /* decimal integer */
            case 'i': /* integer (octal, decimal, hexadecimal) */
               while(isspace(*next)) next++;
               if(*fmt == 'd')
                  base = 10;
               else
                  base = 0;
               if(max_width == -1) {
                  start = next;
               } else {
                  if(max_width > FIELD_MAX) {
                     ddsDebug("format string syntax, huge max field width\n\tstring= \"%s\"\n\tdict= %s, defn= %s\n",
                     fmt_start, dict_name, defn_name);
                     max_width = FIELD_MAX;
                  }
                  for(n = 0; n < max_width && next[n]; n++)
                     field[n] = next[n];
                  field[n] = '\0';
                  start = field;
               }
               ibig = DDS_STRTO_LLONG(start, &stop, base);
               if(! suppress && start != stop) {
                  if(hlL == 'h') {
                     short_ptr = va_arg(*ap, short *);
                     *short_ptr = ibig;
                  } else if(hlL == 'l') {
                     long_ptr = va_arg(*ap, long *);
                     *long_ptr = ibig;
                  } else if(hlL == 'L') { /* extension */
                     llong_ptr = va_arg(*ap, HOST_LLONG_T *);
                     *llong_ptr = ibig;
                  } else {
                     int_ptr = va_arg(*ap, int *);
                     *int_ptr = ibig;
                  }
                  assigned++;
               }
               break;
            case 'o': /* unsigned, octal */
            case 'x': /* unsigned, hexadecimal */
            case 'u': /* unsigned */
            case 'p': /* pointer (host independent via hexadecimal) */
               while(isspace(*next)) next++;
               if(*fmt == 'o')
                  base = 8;
               else if(*fmt == 'x' ||
                  *fmt == 'p') /* extension, host independent */
                  base = 16;
               else
                  base = 10;
               if(max_width == -1) {
                  start = next;
               } else {
                  if(max_width > FIELD_MAX) {
                     ddsDebug("format string syntax, huge max field width\n\tstring= \"%s\"\n\tdict= %s, defn= %s\n",
                     fmt_start, dict_name, defn_name);
                     max_width = FIELD_MAX;
                  }
                  for(n = 0; n < max_width && next[n]; n++)
                     field[n] = next[n];
                  field[n] = '\0';
                  start = field;
               }
               ubig = DDS_STRTO_ULLONG(start, &stop, base);
               if(! suppress && start != stop) {
                  if(hlL == 'h') {
                     ushort_ptr = va_arg(*ap, unsigned short *);
                     *ushort_ptr = ubig;
                  } else if(hlL == 'l') {
                     ulong_ptr = va_arg(*ap, unsigned long *);
                     *ulong_ptr = ubig;
                  } else if(*fmt == 'p') { /* extension, host independent */
                     void_ptr = va_arg(*ap, void *);
                     *ulong_ptr = (unsigned long)void_ptr;
                  } else if(hlL == 'L') { /* extension */
                     ullong_ptr = va_arg(*ap, HOST_ULLONG_T *);
                     *ullong_ptr = ubig;
                  } else {
                     uint_ptr = va_arg(*ap, unsigned *);
                     *uint_ptr = ubig;
                  }
                  assigned++;
               }
               break;
            case 'e': /* float */
            case 'f': /* float */
            case 'g': /* float */
               while(isspace(*next)) next++;
               if(max_width == -1) {
                  start = next;
               } else {
                  if(max_width > FIELD_MAX) {
                     ddsDebug("format string syntax, huge max field width\n\tstring= \"%s\"\n\tdict= %s, defn= %s\n",
                     fmt_start, dict_name, defn_name);
                     max_width = FIELD_MAX;
                  }
                  for(n = 0; n < max_width && next[n]; n++)
                     field[n] = next[n];
                  field[n] = '\0';
                  start = field;
               }
               dbig = strtod(start, &stop);
               if(! suppress && start != stop) {
                  if(hlL == 'l') {
                     double_ptr = va_arg(*ap, double *);
                     *double_ptr = dbig;
                  } else if(hlL == 'L') { /* extension */
                     long_double_ptr = va_arg(*ap, HOST_LDOUBLE_T *);
                     *long_double_ptr = dbig;
                  } else {
                     float_ptr = va_arg(*ap, float *);
                     *float_ptr = dbig;
                  }
                  assigned++;
               }
               break;
            case '[': /* scan set */
               fmt++;
               if(*fmt == '^') {
                  negate = DDS_TRUE;
                  fmt++;
               } else {
                  negate = DDS_FALSE;
               }
               if(! *fmt) {start = stop = NULL; break;}
               setend = strchr(fmt + 1, ']');
               if(! setend) {start = stop = NULL; break;}
               setsize = setend - fmt;
               memset(scanset, '\0', UCHAR_MAX);
               test_prev = '\0';
               for(n = 0; n < setsize; n++) {
                  test = ((unsigned char*)fmt)[n];
                  if(test == '-') {
                     test_next = ((unsigned char*)fmt)[n + 1];
                     if(isdigit(test_prev) && isdigit(test_next) &&
                        test_prev <= test_next) {
                        for(test = test_prev; test <= test_next; test++)
                           scanset[test] = 'x';
                        test = '\0';
                        n++;
                     } else if(islower(test_prev) && islower(test_next) &&
                        test_prev <= test_next) {
                        for(test = test_prev; test <= test_next; test++)
                           if(islower(test)) scanset[test] = 'x';
                        test = '\0';
                        n++;
                     } else if(isupper(test_prev) && isupper(test_next) &&
                        test_prev <= test_next) {
                        for(test = test_prev; test <= test_next; test++)
                           if(isupper(test)) scanset[test] = 'x';
                        test = '\0';
                        n++;
                     } else {
                        scanset[test] = 'x';
                     }
                  } else {
                     scanset[test] = 'x';
                  }
                  test_prev = test;
               }
               if(max_width == -1) max_width = INT_MAX;
	       if(!suppress) /* MRL */
                  char_ptr = (char*)va_arg(*ap, char *);
               if(negate) {
                  if(suppress) {
                     for(n = 0; n < max_width && next[n]; n++) {
                        test = ((unsigned char*)next)[n];
                        if(scanset[test]) break;
                     }
                  } else {
                     for(n = 0; n < max_width && next[n]; n++) {
                        test = ((unsigned char*)next)[n];
                        if(scanset[test]) break;
                        char_ptr[n] = test;
                     }
                     if(n) {
                        assigned++;
                        char_ptr[n] = '\0';
                     }
                  }
               } else {
                  if(suppress) {
                     for(n = 0; n < max_width && next[n]; n++) {
                        test = ((unsigned char*)next)[n];
                        if(! scanset[test]) break;
                     }
                  } else {
                     for(n = 0; n < max_width && next[n]; n++) {
                        test = ((unsigned char*)next)[n];
                        if(! scanset[test]) break;
                        char_ptr[n] = test;
                     }
                     if(n) {
                        assigned++;
                        char_ptr[n] = '\0';
                     }
                  }
               }
               fmt = setend;
               start = next;
               stop = (char*)next + n;
               break;
            case 'c': /* character array */
               if(max_width == -1) max_width = 1;
	       if(!suppress) /* MRL */
                  char_ptr = (char*)va_arg(*ap, char *);
               if(suppress) {
                  for(n = 0; n < max_width && next[n]; n++) /* null */ ;
               } else {
                  for(n = 0; n < max_width && next[n]; n++)
                     char_ptr[n] = next[n];
                  if(n) assigned++;
               }
               start = next;
               stop = (char*)next + n;
               break;
            case 's': /* character string */
               while(isspace(*next)) next++;
               if(max_width == -1) max_width = INT_MAX;
               if(suppress) {
                  for(n = 0; n < max_width && next[n] &&
                     ! isspace(next[n]); n++) /* null */ ;
               } else {
                  char_ptr = va_arg(*ap, char *); /* MRL */
                  for(n = 0; n < max_width && next[n] &&
                     ! isspace(next[n]); n++)
                     char_ptr[n] = next[n];
                  if(n) {
                     assigned++;
                     char_ptr[n] = '\0';
                  }
               }
               start = next;
               stop = (char*)next + n;
               break;
            case 'n': /* number of characters consumed */
               ibig = next - *value;
               if(! suppress) {
                  if(hlL == 'h') {
                     short_ptr = va_arg(*ap, short *);
                     *short_ptr = ibig;
                  } else if(hlL == 'l') {
                     long_ptr = va_arg(*ap, long *);
                     *long_ptr = ibig;
                  } else if(hlL == 'L') { /* extension */
                     llong_ptr = va_arg(*ap, HOST_LLONG_T *);
                     *llong_ptr = ibig;
                  } else {
                     int_ptr = va_arg(*ap, int *);
                     *int_ptr = ibig;
                  }
               }
               break;
            default: /* error, if none of the above */
               ddsDebug("format syntax, dangling '%'\n\tstring= \"%s\"\n\tdict= %s, defn= %s\n",
                  fmt_start, dict_name, defn_name);
               start = stop = NULL;
               break;
            }
            width = stop - start;
            if(! width) break; /* something is rotten */
            next += width;
            fmt++;
         }
      } else {
         /* match non-white space in format and value */
         while(*fmt && *fmt == *next &&
            *fmt != '%' && ! isspace(*fmt)) {fmt++; next++;}
         if(*fmt != '%' && ! isspace(*fmt)) break;
      }
   }

   /* th, th, tha, tha, tha, that's all folks...  8^) */
   *value = next;
   return assigned;
}
