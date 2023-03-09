static char rcsid[]="$Id: tstinit.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstInit: remove file or initialize its contents.

*/

#include "tstsys.h"
#ifdef TST_USP
#include "sisio.h"
#endif

int tstInit(
   TST_SETUP *setup,
   FILE **fp_clear,
   char *tmp_file)
{
   time_t now_time;
   FILE *fp_stdio;
   HOST_LLONG_T lloffset_now, lloffset_next;
   int scale = sizeof(unsigned);
   unsigned *ubuf, *ubuf_clear;
   unsigned init_first;
   int init_bytes, init_words;
   int j, usp_fd, ibytes, sis_bytes;
   GIO_BIG n_write;
#ifdef TST_USP
   int lu_usp;
   /* defined in sisio.h */
   fitentry fit;
   lu_usp = -1;
#endif

   fp_stdio = NULL;
   *fp_clear = NULL;

   if(setup->verbose) {
      now_time = time(NULL);
      fprintf(stderr, "   remove file or initialize its contents %s",
         ctime(&now_time));
   }

   if(setup->pack == TC_usp) {
      init_words = *setup->size_list2 / sizeof(unsigned);
   } else {
      init_words = (setup->cache_limit * (GIO_MB / INIT_RATIO)
         + sizeof(unsigned) - 1) / sizeof(unsigned);
   }

   init_bytes = init_words * sizeof(unsigned);

   if(setup->mode == TC_create_forward) {
      /* remove any existing test file */
      remove(setup->filename);
   } else {
      /*****************************************************************/
      /* open test file, and initialize contents (good or bad values) */
      /*****************************************************************/
      switch(setup->pack) {
      case TC_usp:
         #ifdef TST_USP
            C_LBOPEN(&lu_usp, setup->filename, "w");
            if(lu_usp < 0) {
               fprintf(stderr, "tstInit: %s,LBOPEN initialize %s\n\t%s\n",
                  strerror(errno), setup->filename, setup->title);
               return setup->rc = 12;
            }
            /* initialize usp line header */
            ibytes = HLHOFF + 2 * HLHINT;
            ubuf = (unsigned*)safemalloc(ibytes);
            memset((void*)ubuf, '\0', ibytes);
            put_hw_val(ubuf, "Format", LINEHEADER, (short)3);
            put_hw_val(ubuf, "SmpInt", LINEHEADER, (int)1);
            put_hw_val(ubuf, "NumSmp", LINEHEADER, (int)setup->usp_samples);
            put_hw_val(ubuf, "NumTrc", LINEHEADER, (int)1);
            put_hw_val(ubuf, "NumRec", LINEHEADER, (int)setup->usp_traces);
            sis_bytes = ibytes;
            C_WRTAPE(lu_usp, ubuf, sis_bytes);
            free(ubuf);
            /*** 7/15/98 RLS, diagnose ieeeio parameters
            C_SISPRT(lu_usp);
            ***/
         #endif /* TST_USP */
         break;
      default:
         fp_stdio = fopen(setup->filename, "w");
         if(! fp_stdio) {
            fprintf(stderr, "tstInit: %s,fopen initialize %s\n\t%s\n",
               strerror(errno), setup->filename, setup->title);
            return setup->rc = 12;
         }

         /********************************************************/
         /* The read speed is very dependent upon the block size */
         /* used to read AND write (initialize) the test file.   */
         /* Initialize the test file using the same block size   */
         /* that will be used to access it during speed tests.   */
         /********************************************************/

         if(! setup->block || setup->block > 1) {
            setvbuf(fp_stdio, NULL, _IOFBF, setup->block);
         } else {
            setvbuf(fp_stdio, NULL, _IOFBF, 512 * GIO_KB);
         }
         break;
      }

      ubuf = (unsigned*)safemalloc(init_bytes);

      /**********************************************************/
      /* initialize verify buffer with recognizable 'bad' value */
      /**********************************************************/
      for(j = 0; j < init_words; j++) ubuf[j] = 0x12345678;

      for(lloffset_now = 0;
         lloffset_now < setup->file_limit;
         lloffset_now = lloffset_next) {

         ibytes = init_bytes;
         lloffset_next = lloffset_now + init_bytes;
         if(lloffset_next > setup->file_limit) {
            if(setup->pack == TC_usp) break;
            /* retrict size to bytes remaining within file */
            ibytes = setup->file_limit - lloffset_now;
            lloffset_next = setup->file_limit;
         }
         if(setup->mode <= TC_read_random ||
            setup->mode == TC_write_random) {
            /***********************************************/
            /* initialize file contents with 'good' values */
            /***********************************************/
            init_first = lloffset_now & ~(scale - 1);
            for(j = 0; j < init_words; j++)
               ubuf[j] = init_first + j * scale;
         }
         switch(setup->pack) {
         case TC_usp:
            #ifdef TST_USP
               C_WRTAPE(lu_usp, ubuf, init_bytes);
            #endif /* TST_USP */
            break;
         default:
            n_write = fwrite(ubuf, 1, ibytes, fp_stdio);
            if(n_write != ibytes) {setup->rc = 13; break;}
            break;
         }
      }

      free(ubuf);

      /********************/
      /* close test file */
      /********************/
      switch(setup->pack) {
      case TC_usp:
         #ifdef TST_USP
            /*** C_SISSEEK fails on write with large buffers
            C_SISSEEK(lu_usp, (int)1);
            ***/
/***
            C_SISINFO(lu_usp, &fit); usp_fd = fit.filedes;
***/
            usp_fd = USP_C_FUNC(lucfd)(lu_usp);
            fsync(usp_fd);
            C_LBCLOS(lu_usp);
         #endif /* TST_USP */
         break;
      default:
         fflush(fp_stdio);
         fsync(fileno(fp_stdio));
         fclose(fp_stdio);
         break;
      }

      /* Let the file system stabilize ? */
      sleep(2);

      /***********************************************/
      /* flush system buffers by writing a huge file */
      /* for both verify and speed comparisons.      */
      /***********************************************/
      ibytes = setup->cache_limit * (GIO_MB / INIT_RATIO);
      if(setup->verbose) {
         now_time = time(NULL);
         fprintf(stderr, "   flush system buffers by writing %li bytes %s",
            (long)(ibytes * INIT_RATIO), ctime(&now_time));
      }

      strcpy(tmp_file, setup->filename);
      strcat(tmp_file, "TMP");
      *fp_clear = fopen(tmp_file, "w");

      if(! *fp_clear) {
         fprintf(stderr, "tstInit: %s,fopen clear %s\n\t%s\n",
            strerror(errno), tmp_file, setup->title);
         return setup->rc = 14;
      }

      /* Initialize using a large buffer size for efficiency. */
      /*** setvbuf(*fp_clear, NULL, _IOFBF, 512 * GIO_KB); ***/

      ubuf_clear = (unsigned*)safemalloc(ibytes);

      for(j = 0; j < ibytes / sizeof(unsigned); j++)
         ubuf_clear[j] = 0x87654321;

      for(j = 0; j < INIT_RATIO; j++) {
         n_write = fwrite(ubuf_clear, 1, ibytes, *fp_clear);
         if(n_write != ibytes) {setup->rc = 15; break;}
      }
      free(ubuf_clear);

      /* close the file used to flush the cache, otherwise */
      /* strange things happen to the speed of usp and gio. */
      fflush(*fp_clear);
      fsync(fileno(*fp_clear));
      fclose(*fp_clear);
      /*** remove(tmp_file); ***/

      /* Let the file system stabilize ? */
      sleep(2);
   }

   if(setup->rc) {
      fprintf(stderr, "tstInit: %s, rc= %i after initialize\n",
         strerror(errno), setup->rc);
      return setup->rc;
   }

   return 0;
}
