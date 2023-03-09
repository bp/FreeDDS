static char rcsid[]="$Id: dictstdout.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open and initialize "stdout:" dictionary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_DICT * ddsDictStdout(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
{
   DDS_DICT *dict;
   DDS_STATE save_state;
   const DDS_DEFN *eof_defn;
   DDS_MEDIA *media_clash;
   GFILE *media_fp;
   int fd;

   if(dict_state & DDS_DICT_READ_ONLY) {
      ddsError("dict: errno= %i, %s\n\topening 'stdout:' dictionary, mode= %s\n",
         EACCES, strerror(EACCES), mode);
      return NULL;
   }

   fd = fileno(stdout);
   media_clash = ddsMediaFind("stdout:", fd);

   if(media_clash && media_clash->bin) {
      dds_error = ddsError("dictionary not opened, name clash\n\tdictionary= stdout:\n");
      return NULL;
   }

   if(isatty(fd)) {
      ddsError("dict: dictionary 'stdout:' ignored (tty detected)\n");
      save_state = dds_state;
      dds_state |= DDS_SAVE_TMP;
      dict = ddsDictTmp("stdout:", mode, dict_state);
      dds_state = save_state;
      return dict;
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
      dds_error = ddsError("dict: errno= %i, %s\n\topening 'stdout:' dictionary, mode= %s\n",
      errno, strerror(errno), mode);
      return ddsDictTmp("stdout:", mode, dict_state);
   } else if(media_fp->genus == GIO_MEDIA_GENUS_DIR) {
      ddsError("dict: dictionary 'stdout:' ignored (directory detected)\n");
      return ddsDictTmp("stdout:", mode, dict_state);
   } else {
      media_state |= DDS_MEDIA_OPENW;
   }

   dict = ddsDictNew("stdout:", dict_state, media_state,
      "stdout:", media_fp, NULL, 0);

   if(dict_state & DDS_DICT_LOAD) {
      /* load dictionary contents into memory */
      ddsDictLoad(dict, &dict->load,
         media_fp, NULL, 0);
      eof_defn = (DDS_DEFN*)dict->defn.newest;
      dict->buf_flush = eof_defn->start;
   }

   return dict;
}
