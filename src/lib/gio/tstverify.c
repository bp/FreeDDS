static char rcsid[]="$Id: tstverify.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstVerify: verify contents of file, if trial was writing.

*/

#include "tstsys.h"

int tstVerify(
   TST_SETUP *setup)
{
   time_t now_time;
   FILE *fp_stdio;
   HOST_LLONG_T lloffset_now, lloffset_next;
   HOST_LLONG_T verify_start, verify_stop;
   int scale = sizeof(unsigned);
   off_t offset;
   unsigned *ubuf, *ubuf2;
   unsigned init_first;
   int lu_usp;
   int init_bytes, init_words, n_trace_next;
   int i, j, ibytes, sis_bytes;
   GIO_BIG n_read, n_shift;

   if(setup->pack == TC_usp) {
      init_words = *setup->size_list2 / sizeof(unsigned);
   } else {
      init_words = (setup->cache_limit * (GIO_MB / INIT_RATIO)
         + sizeof(unsigned) - 1) / sizeof(unsigned);
   }

   init_bytes = init_words * sizeof(unsigned);

   if(setup->form == TC_verify &&
      setup->mode >= TC_write_forward &&
      setup->mode <= TC_create_forward) {
      /****************************************/
      /* open test file, and verify contents */
      /****************************************/
      if(setup->verbose) {
         now_time = time(NULL);
         fprintf(stderr, "   open test file, and verify contents %s",
            ctime(&now_time));
      }
      if(setup->mode != TC_write_random &&
         setup->file_limit > setup->write_bytes) {
         if(setup->mode == TC_write_backward) {
            if(setup->pack == TC_usp) {
               verify_stop = *setup->size_list2 * setup->usp_traces;
               verify_start = verify_stop - setup->write_bytes;
            } else {
               verify_start = setup->file_limit - setup->write_bytes;
               verify_stop = setup->file_limit;
            }
         } else {
            verify_start = 0;
            verify_stop = setup->write_bytes;
         }
      } else {
         verify_start = 0;
         if(setup->pack == TC_usp) {
            verify_stop = *setup->size_list2 * setup->usp_traces;
         } else {
            verify_stop = setup->file_limit;
         }
      }

      switch(setup->pack) {
      case TC_usp:
         #ifdef TST_USP
            C_LBOPEN(&lu_usp, setup->filename, "r");
            /* skip usp line header */
            ibytes = HLHOFF + 2 * HLHINT;
            ubuf = (unsigned*)safemalloc(ibytes);
            sis_bytes = ibytes;
            C_RTAPE(lu_usp, ubuf, &sis_bytes);
            free(ubuf);
            if(verify_start) {
               offset = verify_start;
               n_trace_next = offset / *setup->size_list2;
               C_SISSEEK(lu_usp, n_trace_next + 1);
               if(setup->block >= 0 &&
                  (setup->mode == TC_write_backward ||
                   setup->mode == TC_write_random)) {
                   fprintf(stderr, "tstVerify: EXPECT VERIFY ERRORS (usp bug)\n");
               }
            }
         #endif /* TST_USP */
         break;
      default:
         fp_stdio = fopen(setup->filename, "r");
         if(! fp_stdio) {
            fprintf(stderr, "tstVerify: %s, fopen verify %s\n\t%s\n",
               strerror(errno), setup->filename, setup->title);
            return setup->rc = 18;
         }

         setvbuf(fp_stdio, NULL, _IOFBF, 512 * GIO_KB);

         if(verify_start) {
            offset = verify_start;
            fseek(fp_stdio, offset, SEEK_SET);
         }
         break;
      }

      ubuf = (unsigned*)safemalloc(init_bytes + 2 * scale);
      ubuf2 = (unsigned*)safemalloc(init_bytes + 2 * scale);

      /**********************************************************/
      /* initialize verify buffer with recognizable 'bad' value */
      /**********************************************************/
      for(j = 0; j < init_words; j++) ubuf[j] = 0x78563412;

      for(lloffset_now = verify_start;
         lloffset_now >= 0;
         lloffset_now = lloffset_next) {

         ibytes = init_bytes;
         lloffset_next = lloffset_now + init_bytes;
         if(lloffset_next >= verify_stop) {
            if(setup->pack == TC_usp) break;
            /*******************************************/
            /* retrict size to bytes written into file */
            /*******************************************/
            ibytes = verify_stop - lloffset_now;
            lloffset_next = -1;
         }
         switch(setup->pack) {
         case TC_usp:
            #ifdef TST_USP
               sis_bytes = ibytes;
               C_RTAPE(lu_usp, ubuf, &sis_bytes);
            #endif /* TST_USP */
            n_read = sis_bytes;
            break;
         default:
            n_read = fread(ubuf, 1, ibytes, fp_stdio);
            break;
         }
         if(n_read != ibytes) {setup->rc = 19; break;}
         init_first = lloffset_now & ~(scale - 1);
         init_words = 2 + ibytes / scale;
         n_shift = lloffset_now & (scale - 1);
         for(j = 0; j < init_words; j++)
            ubuf2[j] = init_first + j * scale;
         if(memcmp((const void*)ubuf, (char*)ubuf2 + n_shift, ibytes)) {
            HOST_FTN_FUNC(verify, VERIFY)
               ((int*)ubuf, (int*)ubuf2, &lloffset_now, &ibytes);
            break;
         }
      }

      switch(setup->pack) {
      case TC_usp:
         #ifdef TST_USP
            C_LBCLOS(lu_usp);
         #endif /* TST_USP */
         break;
      default:
         fclose(fp_stdio);
         break;
      }

      free(ubuf);
      free(ubuf2);

      if(setup->rc) {
         fprintf(stderr, "tstVerify: %s, rc= %i after verify %s\n\t%s\n",
            strerror(errno), setup->rc, setup->filename, setup->title);
      }
   }

   return setup->rc;
}
