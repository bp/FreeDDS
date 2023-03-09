static char rcsid[]="$Id: openold.c 82 2009-05-19 15:40:08Z user $";
/*
RCS-t=DDS, open binary data, access existing files

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

int ddsOpenOld(
   DDS_BIN *bin,
   const char *data_name,
   DDS_CUBE *old_cube)
{
   DDS_MEDIA_STATE media_state;
   DDS_MEDIA *media;
   DDS_DICT *dict;
   DDS_AUTO_FMT detect;
   GFILE *fp;
   int fd;
   char colon, ch;
   char cfd_mode[8];

   if(*bin->mode == 'r') {
      media_state = DDS_MEDIA_OPENR;
   } else if(*bin->mode == 'a') {
      media_state = (DDS_MEDIA_STATE)(DDS_MEDIA_OPENW | DDS_MEDIA_OPENA);
   }

   if(bin->mode[1] == '+')
      media_state |= (DDS_MEDIA_STATE)(DDS_MEDIA_OPENR | DDS_MEDIA_OPENW);

   /* Look for existing media with this name. */
   media = ddsMediaFind(bin->name, -1);

   if(media && ! media->dict && ! media->bin) {
      /* Media was saved for binary access (it wasn't a dict) */
      bin->media = media;
      media->bin = bin;
      media->state = (DDS_MEDIA_STATE)(
         (media->state & ~ DDS_MEDIA_OPENMODE) |
         (media->state &   DDS_MEDIA_OPENMODE & media_state));
   } else if(! strcmp(data_name, "stdin:")) {
      fp = gdopen(fileno(stdin), bin->mode);
      media_state |= DDS_MEDIA_NOLOAD;
      bin->media = ddsMediaNew(NULL, bin, media_state, data_name, fp);
   } else if(! strcmp(data_name, "stdout:")) {
      fp = gdopen(fileno(stdout), bin->mode);
      media_state |= DDS_MEDIA_NOLOAD;
      bin->media = ddsMediaNew(NULL, bin, media_state, data_name, fp);
   } else if(2 == sscanf(data_name, "fd%d%c%c", &fd, &colon, &ch) &&
      colon == ':') {
      fp = gdopen(fd, bin->mode);
      media_state |= DDS_MEDIA_NOLOAD;
      bin->media = ddsMediaNew(NULL, bin, media_state, data_name, fp);
   } else if(! strcmp(data_name, "dict:")) {
      /* Search for a dictionary with data attached to it. */
      cdds_dict(bin->dict_open, "scan");

      for(dict = dds_scan_first; dict; dict = dict->right)
         if(dict->state & DDS_DICT_DATA) break;

      if(dict) {
         bin->data_dict = dict;
         bin->media = dict->media;
         dict->media->bin = bin;
      } else {
         ddsError("open, binary data not attached to dictionary\n\tdict= %s\n",
            bin->dict_open);
         return DDS_BAD;
      }
   } else {
      /* Binary data file is explicitly named. */
      if(bin->state & DDS_BIN_TAPE) {
         /* Tape media, initialize media descriptor. */
         bin->media = ddsMediaNew(NULL, bin, media_state, data_name, NULL);
      } else {
         /* Non-tape media, initialize media descriptor. */
#if defined(_WIN32) || defined(MINGW)
         cfd_mode[0] = 'c'; /* Windows have to add binary flag */
         cfd_mode[1] = 'b'; /* Windows have to add binary flag */
         strncpy(cfd_mode + 2, bin->mode, 6);
#else
         cfd_mode[0] = 'c';
         strncpy(cfd_mode + 1, bin->mode, 6);
#endif
         fp = gopen(data_name, cfd_mode);
         if(! fp && !strcmp(bin->mode,"r+")) {
           /* if cannot open with "r+", try just "r" */
#if defined(_WIN32) || defined(MINGW)
            cfd_mode[3] = '\0';
#else
            cfd_mode[2] = '\0';
#endif
            fp = gopen(data_name, cfd_mode);
         }

         if(! fp) {
            dds_error = ddsError(
               "data not opened, %s\n\tdict= %s\n\tname= %s\n\tfile= %s\n",
               strerror(errno), bin->dict_open, bin->name, data_name);
            return DDS_BAD;
         }

         bin->media = ddsMediaNew(NULL, bin, media_state, data_name, fp);
      }
   }

   if(media != bin->media) {
      if(media) {
         ddsWarn("open, multiple media with the same name\n\tdict= %s\n\tname= %s\n\tfile= %s\n",
         bin->dict_open, bin->name, data_name);
      }
      media = bin->media;
   }

   if(media->state & DDS_MEDIA_SWAP) {
      if(ddsMediaFirst(media, bin->mode, bin->dict_open)) {
         dds_error = ddsError("tape data not opened, %s\n\tdict= %s\n\tname= %s\n\tfile= %s\n",
            strerror(errno), bin->dict_open, bin->name, data_name);
         return DDS_BAD;
      }
   }

   if(! (media->state & DDS_MEDIA_SWAP)) {
      ddsAutoFmt(&detect, media->fp);
   }

   if(! *bin->fmt_name) {
      /* Automatic format selection. */
      if(detect.fmt == DDS_FMT_UNKNOWN) {
         dds_error = ddsError("input data not opened, format unknown\n\tdict= %s\n\tname= %s\n\tfile= %s\n",
         bin->dict_open, bin->name, data_name);
         return DDS_BAD;
      } else {
         bin->fmt = detect.fmt;
         bin->fmt_name = ddsString(dds_fmt_name[detect.fmt]);
         ddsVerbose("auto detect data format= %s\n\tdict= %s\n\tname= %s\n\tfile= %s\n",
            bin->fmt_name,
            bin->dict_open, bin->name, data_name);
         cdds_dict(bin->dict_open, "print");
         cdds_printf("format", " %s\n", bin->fmt_name);
      }
   }

   ddsCubeScanInit(bin);

   if(!(media->state & DDS_MEDIA_SWAP) && bin->fmt != detect.fmt &&
      !(bin->fmt == DDS_FMT_GENERIC && detect.fmt == DDS_FMT_UNKNOWN) &&
      detect.fmt != DDS_FMT_SU) {
      /* SU is a weak autodection */
      ddsWarn("data format= %s, auto detect is %s\n\tdict= %s\n\tname= %s\n\tfile= %s\n",
         bin->fmt_name, dds_fmt_name[detect.fmt],
         bin->dict_open, bin->name, data_name);
   }

   /***************************/
   /* Initialize symbol table */
   /***************************/
   ddsTableInit(&bin->symbol, SYM_HASH_SIZE,
      (SYM_HASH_ALPHA * SYM_HASH_SIZE * sizeof(DDS_MEMBER)),
      SYM_HASH_ALPHA, SYM_HASH_BETA);

   ddsStackInit(&bin->relocate, DDS_RELOC_MIN * sizeof(DDS_RELOC));

   /* "old" block in symbol table */
   dds_compile = bin;
   dds_symbol = &bin->symbol;
   dds_block = NULL;
   bin->block_old = ddsSymBlockBegin("<old>", (DDS_SYM_FLAG)0, NULL);

   /* "new" block in symbol table */
   dds_compile = bin;
   dds_symbol = &bin->symbol;
   bin->block_new = ddsSymBlockBegin("<new>", (DDS_SYM_FLAG)0, NULL);

   bin->sym_tag = dds_prime_tag;
   bin->sym_tag.ptr = ddsMalloc(bin->sym_tag.max);
   memcpy(bin->sym_tag.ptr, dds_prime_tag.ptr, dds_prime_tag.top);

   /*******************************************************/
   /* Format independent open processing is now complete. */
   /*******************************************************/

   /* Retrieve format modifier flags. */
   cdds_dict(bin->dict_open, "scan");

   if(media->state & DDS_MEDIA_SWAP) {
      /* Swap media (tape), default format modifier flags. */
      switch(bin->fmt) {
      case DDS_FMT_USP:
         bin->state |= (DDS_BIN_STATE)(DDS_BIN_CRAM);
         break;
      case DDS_FMT_SEGY:
      case DDS_FMT_SEGY1:
         bin->state |= (DDS_BIN_STATE)(DDS_BIN_UNBLOCK | DDS_BIN_FILES);
         break;
      }

      if(0 < cdds_scank("format", "cram"))
         bin->state |= DDS_BIN_CRAM;

      if(0 < cdds_scank("format", "no_cram"))
         bin->state &= (DDS_BIN_STATE) ~ DDS_BIN_CRAM;

      if(0 < cdds_scank("format", "no_block"))
         bin->state |= DDS_BIN_UNBLOCK;

      if(0 < cdds_scank("format", "block"))
         bin->state &= (DDS_BIN_STATE) ~ DDS_BIN_UNBLOCK;

      if(0 < cdds_scank("format", "merge"))
         bin->state |= DDS_BIN_FILES;

      if(0 < cdds_scank("format", "no_merge"))
         bin->state &= (DDS_BIN_STATE) ~ DDS_BIN_FILES;

      /* Impose manditory constraints upon modifier flags. */

      switch(bin->fmt) {
      case DDS_FMT_GENERIC:
         bin->state &= (DDS_BIN_STATE) ~ (DDS_BIN_CRAM | DDS_BIN_UNBLOCK);
         bin->block_size = media->opt_block;
         break;
      case DDS_FMT_USP:
         bin->block_size = media->opt_block;
         break;
      case DDS_FMT_SEGY:
      case DDS_FMT_SEGY1:
         bin->block_size = media->opt_block;
         break;
      }

      ddsOpenBlock(bin);

   } else {
      /* non-swap media (not tape), default format modifier flags. */
      switch(bin->fmt) {
      case DDS_FMT_USP:
         if(dds_sislgbuf) {
            /* usp emulation */
            if(*dds_sislgbuf) {
               bin->state &= (DDS_BIN_STATE) ~ DDS_BIN_UNBLOCK;
            } else {
               bin->state |= DDS_BIN_UNBLOCK;
               /* application explicitly turned off large buffers */
               ddsOpenBlock(bin);
            }
         }
         break;
      }
   }

   if(bin->fmt == DDS_FMT_SEGY || bin->fmt == DDS_FMT_SEGY1) {
      if(0 < cdds_scank("format", "charisma"))
         bin->std.segy.flags |= DDS_SEGY_charisma;

      if(0 < cdds_scank("format", "no_charisma"))
         bin->std.segy.flags &= (DDS_SEGY_FLAGS) ~ DDS_SEGY_charisma;

      if(0 < cdds_scank("format", "TrcNumSmp"))
         bin->std.segy.flags |= DDS_SEGY_NumSmp;
   }

   switch(bin->fmt) {
   case DDS_FMT_USP:
      ddsOpenOldUsp(bin, old_cube);
      break;
   case DDS_FMT_SEGY:
   case DDS_FMT_SEGY1:
   case DDS_FMT_SU:
      ddsOpenOldSegy(bin, old_cube);
      break;
   case DDS_FMT_GENERIC:
      ddsOpenOldGen(bin, old_cube);
      break;
   default:
      ddsPanic("ddsOpenOld: bad bin->fmt\n");
   }

   ddsDumpDataOpen(bin);

   if(dds_debug_api & DDS_DBG_SYMBOL) {
      ddsVerbose("ddsOpenBin: symbol table dump, bin tag= %i\n\tdata= %s\n",
         (int)bin->tag, data_name);
      ddsDumpBinSym(0, bin->tag);
   }

   return DDS_OK;
}
