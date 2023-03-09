static char rcsid[]="$Id: dictstderr.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open and initialize "stderr:" dictionary.

*/

/**********************************************/
/***   Copyright (C) 1997                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 6 March 1997           ***/
/**********************************************/

#include "dds_sys.h"

DDS_DICT * ddsDictStderr(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
{
   DDS_DICT *dict;
   const DDS_DEFN *eof_defn;
   DDS_MEDIA *media_clash;
   GFILE *media_fp;
   int fd;

   if(dict_state & DDS_DICT_READ_ONLY) {
      ddsError("dict: errno= %i, %s\n\topening 'stderr:' dictionary, mode= %s\n",
         EACCES, strerror(EACCES), mode);
      return NULL;
   }

   fd = fileno(stderr);
   media_clash = ddsMediaFind("stderr:", fd);

   if(media_clash && media_clash->bin) {
      dds_error = ddsError("dictionary not opened, name clash\n\tdictionary= stderr:\n");
      return NULL;
   }

   if(dict_state & DDS_DICT_TRUNCATE) {
      media_fp = gdopen(fd, "w+");
      dict_state &= (DDS_DICT_STATE) ~ DDS_DICT_LOAD;
      if(! media_fp) {
         media_state &= (DDS_MEDIA_STATE) ~ DDS_MEDIA_OPENR;
         media_fp = gdopen(fd, "w");
      }
   } else {
      dict_state |= DDS_DICT_LOAD;
      media_fp = gdopen(fd, "r+");
   }

   if(! media_fp) {
      dds_error = ddsError("dict: errno= %i, %s\n\topening 'stderr:' dictionary, mode= %s\n",
      errno, strerror(errno), mode);
      return ddsDictTmp("stderr:", mode, dict_state);
   } else if(media_fp->genus == GIO_MEDIA_GENUS_DIR) {
      ddsError("dict: dictionary 'stderr:' ignored (directory detected)\n");
      return ddsDictTmp("stderr:", mode, dict_state);
   } else {
      media_state |= DDS_MEDIA_OPENW;
   }

   dict = ddsDictNew("stderr:", dict_state, media_state,
      "stderr:", media_fp, NULL, 0);

   if(dict_state & DDS_DICT_LOAD) {
      /* load dictionary contents into memory */
      ddsDictLoad(dict, &dict->load,
         media_fp, NULL, 0);
      eof_defn = (DDS_DEFN*)dict->defn.newest;
      dict->buf_flush = eof_defn->start;
   }

   return dict;
}
