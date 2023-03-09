
/*
RCS-t=DDS, load dictionary from string or stream.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define LOAD (str ? ((start < size) ? str[start] : EOF) : (ggetc(fp))) 

#define APPEND(test)  \
   (((bias + start < dict->buf.max) ? dict->buf.ptr :  \
      (dict->buf.max *= 2,  \
       dict->buf.ptr = (char*)ddsRealloc(dict->buf.ptr, dict->buf.max)))  \
       [bias + start++] = (test))

int ddsDictLoad(
   DDS_DICT *dict,       /* load dictionary */
   DDS_LOAD *load,       /* load state */
   GFILE *fp,            /* file pointer, or NULL */
   const char *str,      /* string, or NULL */
   size_t size)          /* string length, if not NULL */
{
   int status = DDS_OK;
   DDS_TOKEN par_save = {{0, NULL}, NULL};
   DDS_DEFN *eof_defn, *test_defn, *par_defn;
   const char *recover;
   size_t bias, start, index, reload, par_value;
   size_t par_size, alias_size, name_size, bw_size;
   int bad_char = 0, bad_defn = 0;
   int i, test;

   /*
      Load dictionary from "str", unless its NULL.
      If NULL, then load dictionary from dict->media->fp.

      definition syntax, nominal and pathological...
            DEFINITION               NAME    VALUE
        1)  foo=caca...              foo       "caca..."
        2)  foo =caca...             foo       "caca..."
        3)  foo= caca...             foo       " caca..."
        4)  foo = caca...            foo       " caca..."
        5)  foo= caca bar=kaka...    foo       " caca "
                                     bar       "kaka..."
        6)  foo=bar=caca...          foo       ""
                                     bar       "caca..."
        7)  foo=  =caca...           foo       "  "       (anonymous)
                                     <void>    "caca..."
        8)  foo==caca...             foo       ""         (anonymous)
                                     <void>    "caca..."
        9)  foo=caca\=kaka...        foo       "caca\=kaka..."
       10)  foo=caca\=bar=kaka...    foo       "caca\="   (white escape)
                                     bar       "kaka..."
       11)  alias:foo=caca...        foo       "caca..."  (alias)
       12)  alias:=caca...           <void>    "caca..."  (anonymous)
       13)  $foo=caca...             foo       "caca..."  (deprecated)
       14)  $=caca...                <void>    "caca..."  (anonymous)

     '\' (escape) is NOT allowed in definition names.

   */

   start = 0;
   eof_defn = (DDS_DEFN*)dict->defn.newest;
   bias = eof_defn->start;
   test = LOAD;

   /* we need these kludge fixes on ia64       */
   /* because casting uchar to int not working */
   /* Ehlers June 2002                         */
   if (test != EOF) test = test & 255;

   /* load characters into dictionary, while not EOF/EOD */
   dict->line = 1;
   while(test != EOF && test != DDS_EOD_CHAR) {

      if(! load->white_size) {
         /* scan black space (defn name or non-white value) */
         while(test != EOF && (load->escape || !isspace(test) || load->quote)) {
            if(load->escape) {
               /* escape this character */
               load->escape = DDS_FALSE; /* ANY character (escaped) */
               load->black_size = -1; /* IMPLICIT white space */
               if(test == '\n') dict->line++;
            } else if(test == '\\') {
               /* escape next character */
               load->escape = DDS_TRUE; 
            } else if(test == '=') {
               /* definition delimiter (non-escaped "=") */
               if (load->quote) {
                  /* if inside quote, limit name back to any previous white space 
                     (unless the previous definition was "previous_history") */
                  if (strcmp(dict->print_name.ptr,"previous_history")) {
                     load->quote = DDS_FALSE;                  
                     bw_size = load->white_size + load->black_size + 1;
                     index = bias + start - bw_size;
                     for(i=bw_size-1;i>=0;i--) {
                        if (isspace(dict->buf.ptr[index+i])) {
                           load->black_size -= i;
                           break;
                        }
                     }
                     break;
                  }
               } else {
                  break;
               }
            } else if(test == DDS_EOD_CHAR) {
               /* end-of-dictionary (binary data separator) */
               break;
            } else if(test == '"') { 
               /* toggle quote flag */  
               if (load->quote) {
                  load->quote = DDS_FALSE;
               } else {
                  load->quote = DDS_TRUE;
               }
            }

            load->black_size++;
            dict->black_line = dict->line;

            if(isprint(test) || test=='\n' || test=='\t') {
               APPEND(test);
            } else {
               /* non-printable character */
               bad_char++;
               APPEND((int)'~');
            }
            test = LOAD;
            if (test != EOF) test = test & 255;
         }
         if(test == DDS_EOD_CHAR) break;
      }

      /* scan white space (between name and "=", or within value) */
      while(isspace(test)) {
         if(test == '\n') dict->line++;
         APPEND(test);
         load->white_size++;
         test = LOAD;
         if (test != EOF) test = test & 255;
      }

      if(test == EOF) break;

      /* check for "=" (definition delimiter) */
      if(test == '=') {
         /* create a new definition */
         APPEND(test);
         bw_size = load->white_size + load->black_size + 1;
         index = bias + start - bw_size;
         if(DDS_FALSE && /* disabled 31 aug 97 rls... bugs */
            ! strcmp(dict->name, "par:") &&
            ! strcmp(dict->print_name.ptr, "par")) {
            /* Insert contents of dictionaries named by "par= files"   */
            /* "SEP" feature requested by John Etgen and Joe Dellinger */

            /* SUMMARY: a "new" definition name has been loaded.       */
            /* The "par:" dictionary is currently being loaded.        */
            /* The previous definition within "par:" is "par= files".  */
            /* The contents of "files" must be inserted *after* the    */
            /* "par= files" and *before* this "new" definition.        */
            /* The "new" name must be unloaded, the contents inserted, */
            /* the "new" name reloaded, and processing resumed.        */
            /* This scheme works when "par= files" is the last defn    */
            /* in "par:" because "dds_par}= ..." flushes it out.    */

            dict->print_name.ptr[0] = '\0'; /* prevent recursion */

            /* save the "new" defn name to reload later. */
            reload = ddsStackPush(&dds_tmp, (bw_size + 1), 1);
            memcpy(dds_tmp.ptr + reload, dict->buf.ptr + index, bw_size);
            *(dds_tmp.ptr + reload + bw_size) = '\0';

            /* truncate dict buffer after the "par= files" defn. */
            dict->buf.ptr[index] = '\0';
            dict->line = dict->black_line;

            /* isolate the value (files) defined by "par= files". */
            eof_defn = (DDS_DEFN*)dict->defn.newest;
            par_defn = (DDS_DEFN*)eof_defn->older;
            recover = strchr(dict->buf.ptr + par_defn->start, '=') + 1;
            par_size = dict->buf.ptr + index - recover;

            if(0 < par_size) {
               /* process the value (files) defined by "par= files". */
               par_value = ddsStackPush(&dds_tmp, (par_size + 1),
                  1);
               memcpy(dds_tmp.ptr + par_value, recover, par_size);
               *(dds_tmp.ptr + par_value + par_size) = '\0';
               par_save.next = dds_tmp.ptr + par_value;
               ddsStrSetMin(&par_save.str, "", par_size);
               ddsParLoad(&par_save, dict, load);
               ddsStrFree(&par_save.str);
               ddsStackSet(&dds_tmp, par_value);
            }

            /* reload the "new" defn name that was saved earlier. */
            eof_defn = (DDS_DEFN*)dict->defn.newest;
            bias = eof_defn->start - start;
            for(recover = dds_tmp.ptr + reload; *recover; recover++) {
              if(*recover == '\n') dict->line++;
              APPEND(*recover);
            }
            ddsStackSet(&dds_tmp, reload);
            bias += bw_size;
            start -= bw_size;
            index = bias + start - bw_size;
         }

         if(dict->buf.ptr[index] == '$') {
            /* alias definition ($name= alias_1  alias_2  ...) */
            alias_size = 1; /* deprecated */
         } else if(load->black_size >= 6 &&
            ! memcmp(dict->buf.ptr + index, "alias:", 6)) {
            /* alias definition (alias:name= alias_1  alias_2  ...) */
            alias_size = 6;
         } else {
            /* normal definition (name= value ...) */
            alias_size = 0;
         }

         if(load->white_size) bad_defn++;

         name_size = load->black_size - alias_size;
         ddsStrCpyN(&dict->print_name, dict->buf.ptr + index + alias_size,
            name_size);
         ddsDefnNew(dict, index, alias_size, dict->print_name.ptr);
         test = LOAD;
         if (test != EOF) test = test & 255;
      }
      load->white_size = load->black_size = 0;
   }

   /* cleanup after loading dictionary */
   eof_defn = (DDS_DEFN*)dict->defn.newest;
   if(! str && test == DDS_EOD_CHAR) {
      /* binary data attached to dictionary (not a *real* EOF) */
      dict->state |= DDS_DICT_DATA;
      if(! (dict->state & DDS_DICT_READ_ONLY))
         dict->state |= DDS_DICT_FROZEN;
      ggetpos(fp, &dict->data_position);

      test_defn = (DDS_DEFN*)eof_defn->older;
      if(test_defn &&
         ! memcmp(dict->buf.ptr + test_defn->start, EOD_DEFN_NAME "=",
         (sizeof(EOD_DEFN_NAME) + 1))) {
         /* Unload this defn (EOD_DEFN_NAME). */
         /* It was automatically appended to round up the dict size. */
         /* Rounding is used to align the start of binary data, */
         /* when it is attached to a dictionary. */
         /* Rounding is *not* required (SEP doesn't), but it */
         /* "probably" increases efficiency if it is aligned. */
         dict->defn.newest = (DDS_ITEM*)test_defn;
         eof_defn = test_defn;
         eof_defn->name = 0;
         eof_defn->newer = 0;
         start = eof_defn->start - bias;
      }
   }

   /* update the EOF definition */
   eof_defn->start = bias + start;

   if(eof_defn->start >= dict->buf.max) {
      /* dictionary buffer needs space for trailing '\0' */
      dict->buf.max *= 2;
      dict->buf.ptr = (char*)ddsRealloc(dict->buf.ptr, dict->buf.max);
   }
   dict->buf.ptr[eof_defn->start] = '\0';

   if(bad_char) {
      ddsWarn("replaced %i non-printable characters with '~'\n\tdictionary= %s\n",
         bad_char, dict->name);
      status = DDS_BAD;
   }

   if(bad_defn) {
      ddsWarn("%i bad definitions (white space before '=')\n\tdictionary= %s\n",
         bad_defn, dict->name);
      status = DDS_BAD;
   }

   return status;
}
