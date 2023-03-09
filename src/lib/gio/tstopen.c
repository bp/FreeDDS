static char rcsid[]="$Id: tstopen.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstOpen: open test file, in preparation for trial.

*/

#include "tstsys.h"

int tstOpen(
   TST_SETUP *setup,
   FILE **fp_stdio,
   GFILE **fp_gio,
   int *lu_usp,
   char **cbuf,
   unsigned **ubuf,
   long size_limit)
{
   time_t now_time;
   const char *fopen_mode, *gopen_mode;
   int scale = sizeof(unsigned);
   int ibytes, sis_bytes;

   if(setup->verbose) {
      now_time = time(NULL);
      fprintf(stderr, "   open test file, in preparation for trial %s",
         ctime(&now_time));
   }

   *lu_usp = -1;
   *fp_stdio = NULL;
   *fp_gio = NULL;

   if(setup->mode == TC_create_forward) {
      fopen_mode = "w";
      gopen_mode = "cw";
   } else {
      fopen_mode = "r+";
      gopen_mode = "cr+";
   }

   switch(setup->pack) {
   case TC_map_ptr:
   case TC_map_cpy:
   case TC_buf_ptr:
   case TC_buf_cpy:
   case TC_raw_ptr:
   case TC_raw_cpy:
      *fp_gio = gopen(setup->filename, gopen_mode);
      if(! *fp_gio) {
         fprintf(stderr, "tune3:gopen: %s %s\n\t%s\n",
            strerror(errno), setup->filename, setup->title);
         return setup->rc = 16;
      }

      if(setup->block < 0) {
         /****************/
         /* non-buffered */
         /****************/
         gsetvbuf(*fp_gio, NULL, (int)_IONBF, 0);
      } else {
         /*********************/
         /* explicit blocking */
         /*********************/
         if(setup->pack == TC_map_ptr || setup->pack == TC_map_cpy) {
            gsetvbuf(*fp_gio, NULL,
               (int)(_IOFBF | _IOMMAP_READ | _IOMMAP_WRITE |
                  setup->cache_flags),
               setup->block);
         } else if(setup->pack == TC_raw_ptr || setup->pack == TC_raw_cpy) {
            gsetvbuf(*fp_gio, NULL,
               (int)(_IOFBF | _IORAW_READ | _IORAW_WRITE |
                  setup->cache_flags),
               setup->block);
         } else {
            gsetvbuf(*fp_gio, NULL, 
               (int)(_IOFBF | _IOCOPY_READ | _IOCOPY_WRITE |
                  setup->cache_flags),
               setup->block);
         }
      }
      break;
   case TC_usp:
      #ifdef TST_USP
         C_LBOPEN(lu_usp, setup->filename, fopen_mode);
         if(setup->block < 0) {
            /****************/
            /* non-buffered */
            /****************/
            C_SISLGBUF(*lu_usp, "off");
         } else {
            /*******************/
            /* large buffering */
            /*******************/
            /*** C_SISLGBUF(*lu_usp, "on"); it should be on by default ***/
         }
         /***********************************/
         /* initialize/skip usp line header */
         /***********************************/
         ibytes = HLHOFF + 2 * HLHINT;
         *ubuf = (unsigned*)safemalloc(ibytes);
         memset((void*)*ubuf, '\0', ibytes);
         if(setup->mode == TC_create_forward) {
            put_hw_val(*ubuf, "Format", LINEHEADER, (short)3);
            put_hw_val(*ubuf, "SmpInt", LINEHEADER, (int)1);
            put_hw_val(*ubuf, "NumSmp", LINEHEADER, (int)setup->usp_samples);
            put_hw_val(*ubuf, "NumTrc", LINEHEADER, (int)1);
            put_hw_val(*ubuf, "NumRec", LINEHEADER, (int)setup->usp_traces);
            sis_bytes = ibytes;
            C_WRTAPE(*lu_usp, *ubuf, sis_bytes);
         } else {
            sis_bytes = ibytes;
            C_RTAPE(*lu_usp, *ubuf, &sis_bytes);
         }
         free(*ubuf);
/***
         C_SISPRT(*lu_usp);
***/
      #endif /* TST_USP */
      break;
   case TC_stdio:
      *fp_stdio = fopen(setup->filename, fopen_mode);
      if(! *fp_stdio) {
         fprintf(stderr, "tune3:fopen: %s %s\n\t%s\n",
            strerror(errno), setup->filename, setup->title);
         return setup->rc = 17;
      }
      if(setup->block < 0) {
         /****************/
         /* non-buffered */
         /****************/
         setvbuf(*fp_stdio, NULL, _IONBF, 0);
      } else if(setup->block > 0) {
         /*********************/
         /* explicit blocking */
         /*********************/
         if(setup->block > 1) {
            setvbuf(*fp_stdio, NULL, _IOFBF, setup->block);
         } else {
            setvbuf(*fp_stdio, NULL, _IOFBF, 512 * GIO_KB);
         }
      }
      break;
   }

   /*******************************************/
   /* allocate buffer required during testing */
   /*******************************************/

   *ubuf = (unsigned*)safemalloc(size_limit + 2 * scale);
   *cbuf = (char*)safemalloc(size_limit + 2 * scale);

   return setup->rc;
}
