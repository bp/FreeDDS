static char rcsid[]="$Id: f_scanf2.c 108 2009-08-17 20:36:53Z user $";
/*
RCS-t=DDS API, retrieve a formatted value, Fortran interface.
*/

/*
     Formatted DDS dictionary input with run-time type-checking
     July 2009 by Musa Maharramov, BP High Performance Computing
     To be linked with dds.F interface module

     Based on ddsScanfF by Randy Selzler, xx May 1996
*/


#include <limits.h>
#include "dds_sys.h"

#define FIELD_MAX 511 /* numeric field maximum width */

/* pointers to "DDS_ADDR" structures are passed to fdds_printf2/fdds_scanf2 */
typedef struct DDS_ADDR {
  int t; /* character deining type of argument ('i','f','s') */
  int l; /* size in bytes or length in chars */
  void * p; /* argument address */
} dds_addr;

/* macro to retrieve a pointer from DDS_ADDR parameter */
#define DP(c,type) \
  ((type *)(check_type(c,sizeof(type),ap,fmt_start,&argcount)))

/* character array */
#define DPAR(c,type) \
  ((type *)(check_type(c,0,ap,fmt_start,&argcount)))

/* check for type the next DDS_ADDR* points to and return its pointer member */
void * check_type(char type, int size,va_list* ap,const char * fmt,int * argcount) {
  dds_addr * pd=va_arg(*ap,dds_addr*);  
  (*argcount)++;
  if (!size && (pd->t!=type))
    ddsError("Format string: %s\nexpected  an array of %c in argument %d to fdds_scanf2, passed %c%d",fmt,type,*argcount, pd->t,pd->l);
  else if (size && ((pd->t!=type) || (pd->l != size)))
    ddsError("Format string: %s\nexpected %c%d in argument %d to fdds_scanf2, passed %c%d\n",fmt, type,size, *argcount, pd->t,pd->l);
  return pd->p;
}

/* macro to retrieve string pointer argument */
#ifdef HOST_FTN_CRAY_STR 
  #define VA_ARG_S(s_arg, s_len)  \
      { dds_addr * p=va_arg(*ap,dds_addr*); argcount++; \
        if (p->t != 's')  \
           ddsError("Format string: %s\nexpected Fortran string in argument %d to fdds_scanf2, passed %c%d",fmt_start, argcount, p->t,p->l); \
        if (p->l <= 0)  \
           ddsError("Format string: %s\nexpected +ive Fortran string length in argument %d to fdds_scanf2",fmt_start, argcount, fmt_start); \
        (s_arg) = (char*)_fcdtocp((_fcd)p); (s_len) = _fcdlen((_fcd)p); }
#else  /* HOST_FTN_CRAY_STR */ 
  #define VA_ARG_S(s_arg, s_len)  \
      { dds_addr * p=va_arg(*ap,dds_addr*); argcount++; \
        if (p->t != 's')  \
           ddsError("Format string: %s\nexpected Fortran string in argument %d to fdds_scanf2, passed %c%d",fmt_start, argcount, p->t,p->l); \
        if (p->l <= 0)  \
           ddsError("Format string: %s\nexpected +ive Fortran string length in argument %d to fdds_scanf2",fmt_start, argcount); \
        (s_arg) = (char*)(p->p); (s_len) = (HOST_FTN_LEN)(p->l); }
#endif /* HOST_FTN_CRAY_STR */


int ddsScanfF2(
   const char **value,
   va_list *ap,
   va_list *ap_len,
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
   #ifdef HOST_FTN_CRAY_STR
      _fcd char_fcd;
   #endif /* HOST_FTN_CRAY_STR */
   size_t char_len;
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
   /* the argument being currently processed */
   int argcount;

   fmt_start = fmt;
   next = *value;

   argcount = 0;

   while(*fmt) {
      if(isspace(*fmt)) {
         /* skip white space in format and value */
         while(isspace(*fmt)) fmt++;
         if(ap_len) while(isspace(*next)) next++;
      }
      if(*fmt == '%') {
         /* '%' conversion specifier */
         fmt++;
         if(*fmt == '%') {
            /* match one '%' in value */
            if(ap_len) {
               if(*next != '%') break;
               next++;
            }
            fmt++;
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
               if(ap_len) while(isspace(*next)) next++;
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
                  if(ap_len) {
                     for(n = 0; n < max_width && next[n]; n++)
                        field[n] = next[n];
                  } else {
                     n = 0;
                  }
                  field[n] = '\0';
                  start = field;
               }
               if(ap_len) {
                  ibig = DDS_STRTO_LLONG(start, &stop, base);
                  if(! suppress && start != stop) {
                     if(hlL == 'h') {
                        short_ptr = DP('i', short);
                        *short_ptr = ibig;
                     } else if(hlL == 'l') {
                        long_ptr = DP('i',long);
                        *long_ptr = ibig;
                     } else if(hlL == 'L') { /* extension */
                        llong_ptr = DP('i',HOST_LLONG_T);
                        *llong_ptr = ibig;
                     } else {
                        int_ptr = DP('i', int);
                        *int_ptr = ibig;
                     }
                     assigned++;
                  }
               } else if(! suppress) {
                  if(hlL == 'h') {
                     DP('i', short);
                  } else if(hlL == 'l') {
                     DP('i', long);
                  } else if(hlL == 'L') { /* extension */
                     DP('i', HOST_LLONG_T);
                  } else {
                     DP('i', int);
                  }
               }
               break;
            case 'o': /* unsigned, octal */
            case 'x': /* unsigned, hexadecimal */
            case 'u': /* unsigned */
            case 'p': /* pointer (host independent via hexadecimal) */
               if(ap_len) while(isspace(*next)) next++;
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
                  if(ap_len) {
                     for(n = 0; n < max_width && next[n]; n++)
                        field[n] = next[n];
                  } else {
                     n = 0;
                  }
                  field[n] = '\0';
                  start = field;
               }
               if(ap_len) {
                  ubig = DDS_STRTO_ULLONG(start, &stop, base);
                  if(! suppress && start != stop) {
                     if(hlL == 'h') {
                        ushort_ptr = DP('i', unsigned short);
                        *ushort_ptr = ubig;
                     } else if(hlL == 'l') {
                        ulong_ptr = DP('i', unsigned long);
                        *ulong_ptr = ubig;
                     } else if(*fmt == 'p') { /* extension, host independent */
/* MAM note we use void* even thou the var address is void -- done to call sizeof() */
                        void_ptr = DP('i', void*);
                        *ulong_ptr = (unsigned long)void_ptr;
                     } else if(hlL == 'L') { /* extension */
                        ullong_ptr = DP('i', HOST_ULLONG_T);
                        *ullong_ptr = ubig;
                     } else {
                        uint_ptr = DP('i', unsigned);
                        *uint_ptr = ubig;
                     }
                     assigned++;
                  }
               } else if(! suppress) {
                  if(hlL == 'h') {
                     DP('i', unsigned short);
                  } else if(hlL == 'l') {
                     DP('i', unsigned long);
                  } else if(*fmt == 'p') { /* extension, host independent */
                     DP('i', void *);
                  } else if(hlL == 'L') { /* extension */
                     DP('i', HOST_ULLONG_T);
                  } else {
                     DP('i', unsigned);
                  }
               }
               break;
            case 'e': /* float */
            case 'f': /* float */
            case 'g': /* float */
               if(ap_len) while(isspace(*next)) next++;
               if(max_width == -1) {
                  start = next;
               } else {
                  if(max_width > FIELD_MAX) {
                     ddsDebug("format string syntax, huge max field width\n\tstring= \"%s\"\n\tdict= %s, defn= %s\n",
                     fmt_start, dict_name, defn_name);
                     max_width = FIELD_MAX;
                  }
                  if(ap_len) {
                     for(n = 0; n < max_width && next[n]; n++)
                        field[n] = next[n];
                  } else {
                     n = 0;
                  }
                  field[n] = '\0';
                  start = field;
               }
               if(ap_len) {
                  dbig = strtod(start, &stop);
                  if(! suppress && start != stop) {
                     if(hlL == 'l') {
                        double_ptr = DP('f', double);
                        *double_ptr = dbig;
                     } else if(hlL == 'L') { /* extension */
                        long_double_ptr = DP('f', HOST_LDOUBLE_T);
                        *long_double_ptr = dbig;
                     } else {
                        float_ptr = DP('f', float);
                        *float_ptr = dbig;
                     }
                     assigned++;
                  }
               } else if(! suppress) {
                  if(hlL == 'l') {
                     DP('f', double);
                  } else if(hlL == 'L') { /* extension */
                     DP('f', HOST_LDOUBLE_T);
                  } else {
                     DP('f', float);
                  }
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
               if(ap_len) {
			if (!suppress) {
                        VA_ARG_S(char_ptr, char_len);
                  }
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
                           if(n < char_len) char_ptr[n] = test;
                        }
                        if(n) {
                           assigned++;
                           if(n < char_len) memset(char_ptr + n, ' ',
                              (char_len - n));
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
                           if(n < char_len) char_ptr[n] = test;
                        }
                        if(n) {
                           assigned++;
                           if(n < char_len) memset(char_ptr + n, ' ',
                              (char_len - n));
                        }
                     }
                  }
               } else {
                  if (!suppress) DPAR('s',char);
                  n = 0;
               }
               fmt = setend;
               start = next;
               stop = (char*)next + n;
               break;
            case 'c': /* character array */
               if(max_width == -1) max_width = 1;
               if(ap_len) {
                  if(suppress) {
                     for(n = 0; n < max_width && next[n]; n++) /* null */ ;
                  } else {
                     VA_ARG_S(char_ptr, char_len);
                     for(n = 0; n < max_width && next[n]; n++)
                        if(n < char_len) char_ptr[n] = next[n];
                     if(n) {
                        assigned++;
                        if(n < char_len) memset(char_ptr + n, ' ',
                           (char_len - n));
                     }
                  }
               } else {
                  if (!suppress) DPAR('s',char);
                  n = 0;
               }
               start = next;
               stop = (char*)next + n;
               break;
            case 's': /* character string */
               if(ap_len) {
                  while(isspace(*next)) next++;
                  if(max_width == -1) max_width = INT_MAX;
                  if(suppress) {
                     for(n = 0; n < max_width && next[n] &&
                        ! isspace(next[n]); n++) /* null */ ;
                  } else {
                     VA_ARG_S(char_ptr, char_len);
                     for(n = 0; n < max_width && next[n] &&
                        ! isspace(next[n]); n++)
                        if(n < char_len) char_ptr[n] = next[n];
                     if(n) {
                        assigned++;
                        if(n < char_len) memset(char_ptr + n, ' ',
                           (char_len - n));
                     }
                  }
               } else {
                  if (!suppress) DPAR('s', char);
                  n = 0;
               }
               start = next;
               stop = (char*)next + n;
               break;
            case 'n': /* number of characters consumed */
               if(ap_len) {
                  ibig = next - *value;
                  if(! suppress) {
                     if(hlL == 'h') {
                        short_ptr = DP('i', short);
                        *short_ptr = ibig;
                     } else if(hlL == 'l') {
                        long_ptr = DP('i', long);
                        *long_ptr = ibig;
                     } else if(hlL == 'L') { /* extension */
                        llong_ptr = DP('i', HOST_LLONG_T);
                        *llong_ptr = ibig;
                     } else {
                        int_ptr = DP('i', int);
                        *int_ptr = ibig;
                     }
                  }
               } else if(! suppress) {
                  if(hlL == 'h') {
                     DP('i', short);
                  } else if(hlL == 'l') {
                     DP('i', long);
                  } else if(hlL == 'L') { /* extension */
                     DP('i', HOST_LLONG_T *);
                  } else {
                     DP('i', int);
                  }
               }
               break;
            default: /* error, if none of the above */
               ddsDebug("format syntax, dangling '%'\n\tstring= \"%s\"\n\tdict= %s, defn= %s\n",
                  fmt_start, dict_name, defn_name);
               start = stop = NULL;
               break;
            }
            if(ap_len) {
               width = stop - start;
               if(! width) break; /* something is rotten */
               next += width;
            }
            fmt++;
         }
      } else {
         /* match non-white space in format and value */
         if(ap_len) {
            while(*fmt && *fmt == *next &&
               *fmt != '%' && ! isspace(*fmt)) {fmt++; next++;}
         } else {
            while(*fmt &&
               *fmt != '%' && ! isspace(*fmt)) {fmt++;}
         }
         if(*fmt != '%' && ! isspace(*fmt)) break;
      }
   }

   /* th, th, tha, tha, tha, that's all folks...  8^) */
   if(ap_len) {
      *value = next;
      if(*fmt && ! assigned) assigned = EOF;
   }
   return assigned;
}

#define FMT_NOM_LEN 2 * DDS_KB

#define ALIAS_NOM_LEN 4 * DEFNNAME_MAX

HOST_FTN_INT
HOST_FTN_FUNC(fdds_scanf2, FDDS_SCANF)(
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

   const char *alias;
   int status = DDS_OK;
   size_t save_size;
   const DDS_DEFN *defn;
   DDS_DICT *dict;
   DDS_ALIAS *list;
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

   DDS_API_ENTRY("scanf")

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

MAM -- this "kludge" is no longer needed -- fdds_scanf2 is passed
      pointers to DDS_ADDR structures describing the actual args;
      however, ap_len is kept here to minimise changes of old code  

      */

      /* Walk "ap_len" past the pointers to actual args */
      memcpy((void*)&ap_len, (void*)&ap, sizeof(va_list));
      status = ddsScanfF2(&dds_scan_token.next, &ap_len, NULL, fmt_str.ptr,
         "<fake>", "<fake>");
      if(status == EOF) {
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
      /* de-select current scan definition */
      ddsScanDone();

      /* initialize alias list (definition names) */
      list = ddsAliasList(NULL, ddsDefnAxis(alias, dds_scan_first));

      /* search for a defn, while dict and alias exist */
      for(dict = dds_scan_first; dict && list; dict = dict->right) {
         defn = ddsDefnFind(&save_size, &dds_scan_value_bias,
            &list, dict);
         if(defn) {
            /* DEFINITION FOUND */
            dds_scan_dict = dict;
            dds_scan_defn = defn;
            dds_scan_name = defn->name + defn->alias;
            dds_scan_newer = (DDS_DEFN*)defn->newer;
            dds_scan_value_save = dict->buf.ptr[dds_scan_newer->start];
            dict->buf.ptr[dds_scan_newer->start] = '\0';
            dds_scan_token.next = dict->buf.ptr + dds_scan_value_bias;
            break;
         }
      }
      ddsAliasDone(list);
   }
   if(dds_scan_dict) {
      ddsParTag((DDS_DEFN*)dds_scan_defn, dds_scan_dict);
      status = ddsScanfF2(&dds_scan_token.next, &ap, &ap_len, fmt_str.ptr,
         dds_scan_dict->name, dds_scan_name);
   } else {
      status = EOF;
   }

   va_end(ap);

   DDS_API_EXIT(status);
}
