static char rcsid[]="$Id: parload.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, recursively load parameter files, reverse token sequence.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsParLoad(
   DDS_TOKEN *list,      /* token, list of par files */
   DDS_DICT *dict,       /* load dictionary */
   DDS_LOAD *load)       /* load state */
{
   int status = DDS_OK;
   const char *par_file;
   DDS_LOAD level;
   DDS_DEFN *eof_defn;
   GFILE *par_fp;

   par_file = TOKEN_NEXT(*list);

   if(! par_file) return DDS_OK; /* recursion done, reverse processing */

   if(load->depth > 4) {
      ddsError("parameter file recursion exceeds limit\n\t%s\n\t-> %s\n\t--> %s\n\t---> %s\n\t----> %s\n\t-----> ... ignored\n",
         load->prev->prev->prev->prev->name,
         load->prev->prev->prev->name,
         load->prev->prev->name,
         load->prev->name,
         load->name);
      return DDS_BAD;
   }

   level.name = ddsString(par_file);
   level.prev = load;
   level.depth = load->depth + 1;
   level.escape = DDS_FALSE;
   level.quote = DDS_FALSE;
   level.black_size = 0;
   level.white_size = 0;

   if(ddsParLoad(list, dict, &level)) status = DDS_BAD;

   par_fp = gopen(level.name, "cr");
   if(! par_fp) {
      dds_error = ddsError("dict: errno= %i, %s\n\topening parameter file= %s\n\trecursively loading '%s' dictionary\n",
         errno, strerror(errno), level.name, dict->name);
      return DDS_BAD;
   }

   if(ddsDictLoad(dict, &level, par_fp, NULL, 0)) status = DDS_BAD;

   gclose(par_fp);

   /* quietly repair syntax errors */
   eof_defn = (DDS_DEFN*)dict->defn.newest;
   if(level.escape) {
      dict->buf.ptr[eof_defn->start] = '\n';
   } else if(dict->defn.oldest != dict->defn.newest) {
      if(! isspace(dict->buf.ptr[eof_defn->start - 1])) {
         if(ddsDictLoad(dict, &level, NULL, "\n", 1)) status = DDS_BAD;
      }
   } else {
      eof_defn->start = 0;
   }
 
   ddsFree((void*)level.name);

   return status;
}
