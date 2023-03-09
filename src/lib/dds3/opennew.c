static char rcsid[]="$Id: opennew.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open binary data, create new files

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <fcntl.h>
#include "dds_sys.h"
#include "dds_dump.h"

#ifdef SGI_IA64
#include <xfs/libxfs.h>
unsigned long len_kb;
struct xfs_flock64 xfsf;
#else
#ifdef F_RESVSP64
struct flock64 lckf;
#endif
#endif

int ddsOpenNew(
   DDS_BIN *bin,
   const char *data_name,
   DDS_CUBE *old_cube)
{
   DDS_MEDIA_STATE media_state;
   DDS_MEDIA *media;
   DDS_DICT *dict;
   GFILE *fp;
   DDS_TYPE *type;
   double bytes;
   int i;
   int reserve = 0;
   int fd = -1;
   char colon, ch, *prealloc;
   char cfd_mode[8];

   if(*bin->mode == 'm') {
      media_state = DDS_MEDIA_BUFFER;
   } else {
      if(*bin->mode == 'w') {
         media_state = DDS_MEDIA_OPENW;
      } else if(*bin->mode == 'a') {
         media_state = (DDS_MEDIA_STATE)(DDS_MEDIA_OPENW | DDS_MEDIA_OPENA);
      }

      if(bin->mode[1] == '+')
         media_state |= (DDS_MEDIA_STATE)(DDS_MEDIA_OPENR);
   }

   /* Look for existing media with this name. */
   media = ddsMediaFind(bin->name, -1);

   cdds_dict(bin->dict_open, "scan");

   if(media && ! media->dict && ! media->bin) {
      /* Media was saved for binary access (it wasn't a dict) */
      bin->media = media;
      media->bin = bin;
      media->state = (DDS_MEDIA_STATE)(
         (media->state & ~ DDS_MEDIA_OPENMODE) |
         (media->state &   DDS_MEDIA_OPENMODE & media_state));
   } else if(*bin->mode == 'm') {
      /* Instant media for memory buffers. */
      bin->media = ddsMediaNew(NULL, bin, media_state, data_name, NULL);
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
      for(dict = dds_scan_first; dict; dict = dict->right)
         if(dict->state & DDS_DICT_DATA) break;

      if(! dict && dds_scan_first->media) {
         /* Default, attach binary data to first dictionary. */
         bin->data_dict = dds_scan_first;
         bin->media = dds_scan_first->media;
         dds_scan_first->media->bin = bin;
      } else {
         ddsError("open, can't attach binary data to this dict\n\tdict= %s\n\tname= %s\n\tfile= %s\n",
            bin->dict_open, bin->name, data_name);
         return DDS_BAD;
      }
   } else {
      /* Binary data file is explicitly named. */
      if(bin->state & DDS_BIN_TAPE) {
         /* Tape media, initialize media descriptor. */
         bin->media = ddsMediaNew(NULL, bin, media_state, data_name, NULL);
      } else {
         /* Non-tape media, initialize media descriptor. */
         if(bin->state & DDS_BIN_TMP) {
            /* temporary file (disable cache file descriptor) */
            strncpy(cfd_mode, bin->mode, 6);
         } else {
#if defined(_WIN32) || defined(MINGW)
            cfd_mode[0] = 'c'; /* Windows have to add binary flag */
            cfd_mode[1] = 'b'; /* Windows have to add binary flag */
            strncpy(cfd_mode + 2, bin->mode, 6);
#else
            cfd_mode[0] = 'c';
            strncpy(cfd_mode + 1, bin->mode, 6);
#endif
         }

         /* open a new file */
         fp = gopen(data_name, cfd_mode);
         if(! fp) {
            dds_error = ddsError("data not opened, %s\n\tdict= %s\n\tname= %s\n\tfile= %s\n",
               strerror(errno), bin->dict_open, bin->name, data_name);
            return DDS_BAD;
         }

         /* flag to reserve space for more efficiency */
         /* For now, default off because of system problems - JWE May 2005 */
         /* reserve = 1; */
         prealloc = getenv("DDS_PREALLOC");
         if (prealloc) {
            if (*prealloc == 'y') reserve = 1;
            if (*prealloc == 'n') reserve = 0;
         }

         if(bin->state & DDS_BIN_TMP) unlink(data_name);

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

   /* create new data and associated format */
   bin->state |= DDS_BIN_EOF;

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
      if(0 < cdds_scank("format", "+charisma charisma"))
         bin->std.segy.flags |= DDS_SEGY_charisma;

      if(0 < cdds_scank("format", "-charisma"))
         bin->std.segy.flags &= (DDS_SEGY_FLAGS) ~ DDS_SEGY_charisma;
   }

   switch(bin->fmt) {
   case DDS_FMT_USP:
      ddsOpenNewUsp(bin, old_cube);
      break;
   case DDS_FMT_SEGY:
   case DDS_FMT_SEGY1:
   case DDS_FMT_SU:
      ddsOpenNewSegy(bin, old_cube);
      break;
   case DDS_FMT_GENERIC:
      ddsOpenNewGen(bin, old_cube);
      break;
   default:
      ddsPanic("ddsOpenNew: bad bin->fmt\n");
   }

   /* reserve space for more efficiency */

   if (reserve) {
      SYM_TAG_TO_TYPE(type, bin, 0, DDS_API_EXIT(DDS_BAD));
      bytes = type->stroke;
      for(i=2;i<=bin->cube.rank;i++) {
         bytes = bytes * bin->cube.axis[i].size;
      }
#ifdef SGI_IA64
      len_kb = bytes/1024;
      xfsf.l_whence = SEEK_SET;
      xfsf.l_start = 0;
      xfsf.l_len =len_kb << 10;
      xfsctl(data_name, fp->fd, XFS_IOC_RESVSP, &xfsf);
      ddsVerbose("ddsOpenNew: XFS_IOC_RESVSP, fd=%d bytes(kb)=%ld\tdata= %s\n",
         fp->fd, (unsigned long)len_kb, data_name);
/* turn off for now: makes I/O worse on SGI.  They are investigating... July 04
#else
#ifdef F_RESVSP64
      lckf.l_whence = SEEK_SET;
      lckf.l_start = 0;
      lckf.l_len = bytes;
      fcntl(fp->fd, F_RESVSP, &lckf);
      ddsVerbose("ddsOpenNew: F_RESVSP, fd=%d bytes=%ld\tdata= %s\n",
         fp->fd, (unsigned long)lckf.l_len, data_name);
#endif
*/
#endif
   }

   ddsDumpDataOpen(bin);

   if(dds_debug_api & DDS_DBG_SYMBOL) {
      ddsVerbose("ddsOpenNew: symbol table dump, bin tag= %i\n\tdata= %s\n",
         (int)bin->tag, data_name);
      ddsDumpBinSym(0, bin->tag);
   }

   return DDS_OK;
}
