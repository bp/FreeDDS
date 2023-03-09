static char rcsid[]="$Id: dictfd.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open and initialize "fd#:" dictionary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_DICT * ddsDictFd(
   const char *name,
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
{
   DDS_DICT *dict;
   const DDS_DEFN *eof_defn;
   DDS_MEDIA *media_clash;
   GFILE *media_fp;
   char colon[2];
   int fd;

   if(2 != sscanf(name, "fd%d%c", &fd, colon) ||
      0 > fd || ':' != colon[0]) {
      ddsDebug("dict: dictionary name syntax (%s)\n", name);
      return NULL;
   }

   media_clash = ddsMediaFind(name, fd);

   if(media_clash && media_clash->bin) {
      dds_error = ddsError("dictionary not opened, name clash\n\tdictionary= %s\n",
         name);
      return NULL;
   }

   if(isatty(fd)) {
      ddsError("dict: dictionary '%s' ignored (tty detected)\n",
         name);
      return ddsDictTmp(name, mode, dict_state);
   }
   if(dict_state & DDS_DICT_TRUNCATE) {
      dict_state &= (DDS_DICT_STATE) ~ DDS_DICT_LOAD;
      media_fp = gdopen(fd, "w+");
      if(! media_fp && ! (dict_state & DDS_DICT_READ_ONLY)) {
         media_state &= (DDS_MEDIA_STATE) ~ DDS_MEDIA_OPENR;
         media_fp = gdopen(fd, "w");
      }
   } else {
      dict_state |= DDS_DICT_LOAD;
      if(dict_state & DDS_DICT_READ_ONLY) {
         media_state &= (DDS_MEDIA_STATE) ~ DDS_MEDIA_OPENW;
         media_fp = gdopen(fd, "r");
      } else {
         media_fp = gdopen(fd, "r+");
         if(! media_fp) {
            media_state &= (DDS_MEDIA_STATE) ~ DDS_MEDIA_OPENW;
            media_fp = gdopen(fd, "r");
         }
      }
   }

   if(! media_fp) {
      dds_error = ddsError("dict: errno= %i, %s\n\topening '%s' dictionary, mode= %s\n",
      errno, strerror(errno), name, mode);
      return NULL;
   } else if(media_fp->genus == GIO_MEDIA_GENUS_DIR) {
      ddsError("dict: dictionary '%s' ignored (directory detected)\n",
         name);
      return ddsDictTmp(name, mode, dict_state);
   }

   dict = ddsDictNew(name, dict_state, media_state,
      name, media_fp, NULL, 0);

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
