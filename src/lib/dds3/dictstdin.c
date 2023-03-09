static char rcsid[]="$Id: dictstdin.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open and initialize "stdin:" dictionary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_DICT * ddsDictStdin(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
{
   DDS_DICT *dict;
   const DDS_DEFN *eof_defn;
   DDS_MEDIA *media_clash;
   GFILE *media_fp;
   int fd;

   fd = fileno(stdin);
   media_clash = ddsMediaFind("stdin:", fd);

   if(media_clash && media_clash->bin) {
      dds_error = ddsError("dictionary not opened, name clash\n\tdictionary= stdin:\n");
      return NULL;
   }

   if(isatty(fd)) {
      ddsError("dict: dictionary 'stdin:' ignored (tty detected)\n");
      return ddsDictTmp("stdin:", mode, dict_state);
   }

   media_fp = gdopen(fd, "r");

   if(! media_fp) {
      dds_error = ddsError("dict: errno= %i, %s\n\topening 'stdin:' dictionary, mode= %s\n",
      errno, strerror(errno), mode);
      return ddsDictTmp("stdin:", mode, dict_state);
   } else if(media_fp->genus == GIO_MEDIA_GENUS_DIR) {
      ddsError("dict: dictionary 'stdin:' ignored (directory detected)\n");
      return ddsDictTmp("stdin:", mode, dict_state);
   }

   media_state &= (DDS_MEDIA_STATE) ~ (DDS_MEDIA_OPENW);

   dict = ddsDictNew("stdin:", dict_state, media_state,
      "stdin:", media_fp, NULL, 0);

   /* ddsDictNew may have detected binary data on original GFILE */
   media_fp = dict->media->fp;

   if(dict_state & DDS_DICT_LOAD) {
      /* load dictionary contents into memory */
      ddsDictLoad(dict, &dict->load,
         media_fp, NULL, 0);
      eof_defn = (DDS_DEFN*)dict->defn.newest;
      dict->buf_flush = eof_defn->start;
   }

   return dict;
}
