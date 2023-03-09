static char rcsid[]="$Id: dictfile.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open and initialize "filename" dictionary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_DICT * ddsDictFile(
   const char *name,
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
{
   DDS_DICT *dict;
   const DDS_DEFN *eof_defn;
   DDS_MEDIA *media_clash;
   GFILE *media_fp;
   const char *data_file;

   /* change network path to fastest path */
   data_file = ddsPathFast(name);

   if(dict_state & DDS_DICT_TRUNCATE) {
      media_fp = gopen(data_file, "cw+");
      dict_state &= (DDS_DICT_STATE) ~ DDS_DICT_LOAD;
      if(! media_fp) {
         media_state &= (DDS_MEDIA_STATE) ~ DDS_MEDIA_OPENR;
         media_fp = gopen(data_file, "cw");
      }
   } else if(dict_state & DDS_DICT_READ_ONLY) {
      media_fp = gopen(data_file, "cr");
   } else if(DDS_MEDIA_FIFO == ddsStatGenus(data_file)) {
      if(dict_state & DDS_DICT_LOAD) {
         dict_state |= DDS_DICT_READ_ONLY;
         media_fp = gopen(data_file, "cr");
      } else {
         media_fp = gopen(data_file, "cw");
      }
   } else {
      media_fp = gopen(data_file, "cr+");
      if(! media_fp) {
         dict_state |= DDS_DICT_READ_ONLY;
         media_state &= (DDS_MEDIA_STATE) ~ DDS_MEDIA_OPENW;
         media_fp = gopen(data_file, "cr");
      }
   }

   if(! media_fp) {
      dds_error = ddsError("dict: errno= %i, %s\n\topening '%s' dictionary, mode= %s\n",
      errno, strerror(errno), name, mode);
      return ddsDictTmp(name, mode, dict_state);
   } else if(media_fp->genus == GIO_MEDIA_GENUS_TTY) {
      ddsError("dict: dictionary '%s' ignored (tty detected)\n",
         name);
      return ddsDictTmp(name, mode, dict_state);
   } else if(media_fp->genus == GIO_MEDIA_GENUS_DIR) {
      ddsError("dict: dictionary '%s' ignored (directory detected)\n",
         name);
      return ddsDictTmp(name, mode, dict_state);
   }

   /* Just check name; otherwise too much cfd checking     */
   /* J. Ehlers Nov 2002                                   */
   /* media_clash = ddsMediaFind(name, gfileno(media_fp)); */
   media_clash = ddsMediaFind(name, -1);

   if(media_clash && media_clash->bin) {
      dds_error = ddsError("dictionary not opened, name clash\n\tdict= %s\n\tdict= %s\n",
         name, data_file);
      gclose(media_fp);
      return NULL;
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
