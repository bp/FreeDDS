static char rcsid[]="$Id: history.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, append one dictionary to another dictionary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsHistory(
   DDS_DICT *out_dict,
   const DDS_DICT *in_dict)
{
   int status = DDS_OK;
   size_t add_size, new_size, out_line;
   HOST_LLONG_T bias;
   DDS_DEFN *eof_defn;
   const DDS_DEFN *in_defn, *in_defn_oldest, *in_defn_newest;
   DDS_DEFN *out_defn_newest;
   const char *name;

   /* preserve scan state ??? */
   ddsScanDone();
      
   if(in_dict->defn.oldest != in_dict->defn.newest) {
      /* non-empty input dictionary */
      out_defn_newest = (DDS_DEFN*)out_dict->defn.newest;
      in_defn_newest = (DDS_DEFN*)in_dict->defn.newest;
      in_defn_oldest = (DDS_DEFN*)in_dict->defn.oldest;
      add_size = in_defn_newest->start - in_defn_oldest->start;
      new_size = out_defn_newest->start + add_size;

      /* ">=" required for string-like null terminator */
      if(new_size >= out_dict->buf.max) {
         /* increase output dict buffer size */
         out_dict->buf.max = 2 * new_size;
         out_dict->buf.ptr = ddsRealloc(out_dict->buf.ptr, out_dict->buf.max);
      }
      /* append input buffer to output buffer */
      memcpy(out_dict->buf.ptr +  out_defn_newest->start,
         in_dict->buf.ptr + in_defn_oldest->start, add_size);

      /* string-like null terminator for debugging and */
      /* something for "scan" processing to save and restore. */
      out_dict->buf.ptr[out_defn_newest->start + add_size] = '\0';

      bias = out_defn_newest->start - in_defn_oldest->start;
      out_line = out_dict->line;

      out_defn_newest->start += add_size;

      for(in_defn = in_defn_oldest;
         in_defn->newer;
         in_defn = (DDS_DEFN*)in_defn->newer) {
         out_dict->black_line = out_line + in_defn->line;
         name = in_defn->name;
         ddsDefnNew(out_dict, in_defn->start + bias,
            in_defn->alias, name);
      }
      out_dict->line = out_line + in_dict->line;
      out_dict->black_line = out_dict->line;
      eof_defn = (DDS_DEFN*)out_dict->defn.newest;
      eof_defn->start = in_defn->start + bias;
   }
   return status;
}
