static char rcsid[]="$Id: tstexecute.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=GIO, tstExecute: execute one test trial.

*/

#include "tstsys.h"
#ifdef TST_USP
#include "sisio.h"
#endif

#undef FTN_VERIFY

/***#define CLOCKS_PER_SEC 60***/

int page_peek; /* global variable to defeat optimization */

int tstExecute(
   TST_SETUP *setup,     /* tuner descriptor */
   long size_limit,      /* maximum size */
   int align_limit,      /* maximum align */
   int bias_limit,       /* maximum bias */
   FILE *fp_stdio,       /* stdio file pointer */
   GFILE *fp_gio,        /* GIO file pointer */
   int lu_usp,           /* logical unit usp */
   char *cbuf,           /* copy i/o buffer */
   unsigned *ubuf)       /* verify buffer */
{
   time_t now_time;
   unsigned long loop_check, short_read = 0, short_write = 0;
   clock_t start_cpu, check_cpu;
   time_t start_time, check_time;
   double elapsed_time, percent_limit;
   int n_trace_now;
   HOST_LLONG_T lloffset_now, lloffset_ftn;
   gpos_t gio_pos_t;
   GIO_BIG size;
   size_t align, bias;
   int ibytes, sis_bytes, ishift;
   GIO_BIG n_expect, n_read, n_write;
   int j, usp_fd, init_words, scale = sizeof(unsigned);
   unsigned init_first;
   char *buf;
   void *fbuf;
#ifdef TST_USP
   /* defined in sisio.h */
   fitentry fit;
#endif

   if(setup->verbose) {
      now_time = time(NULL);
      fprintf(stderr, "   setup done.  starting trial, %4.1f second limit %s",
         setup->time_limit, ctime(&now_time));
   }

   switch(setup->mode) {
   case TC_read_forward:
   case TC_write_forward:
   case TC_create_forward:
      lloffset_now = 0;
      n_read = n_write = 0;
      n_expect = 1;
      break;
   case TC_read_backward:
   case TC_write_backward:
      lloffset_now = 0;
      break;
   }

   if(setup->pack != TC_map_ptr && setup->pack != TC_buf_ptr)
      buf = cbuf;

   loop_check = 1;

   if(setup->watch) fprintf(stdout, "begin %s %s watch cache size\n",
      mode_name[setup->mode - TC_nop], pack_name[setup->pack - TC_stdio]);

   /************************/
   /*** trial initiation ***/
   /************************/

   start_cpu = clock();
   start_time = time(NULL);

   /******************************************************/
   /******** main processing loop for tuner trial ********/
   /******************************************************/

   for(setup->loop = 0; GIO_TRUE; setup->loop++) {
      if(setup->watch && setup->loop > setup->watch) break;
      if(setup->loop >= loop_check) {
         /* check time limit, and estimate trial termination */
         check_time = time(NULL);
         elapsed_time = difftime(check_time, start_time);
         if(elapsed_time >= setup->time_limit) break;
         percent_limit = 100.0 * (elapsed_time / setup->time_limit);

         /***********************************************************/
         /* refine termination estimate, based upon actual progress */
         /***********************************************************/
         if(percent_limit < 1.0) {
            /* insufficient data for accurate loop estimate */
            if(setup->loop > setup->loop_max) {
               /* eternity ??? */
               setup->rc = 20;
               break;
            }
            loop_check *= 8.0;
         } else if(percent_limit < 2.0) {
            loop_check *= 8.0;
         } else if(percent_limit < 5.0) {
            loop_check *= 4.0;
         } else if(percent_limit < 10.0) {
            loop_check *= 2.0;
         } else if(percent_limit < 20.0) {
            /* estimate loop count at 45.0 percent of elapsed time */
            loop_check *= 45.00 / percent_limit;
         } else if(percent_limit < 40.0) {
            /* estimate loop count at 90.0 percent of elapsed time */
            loop_check *= 90.00 / percent_limit;
         } else if(percent_limit < 80.0) {
            /* estimate loop count at 100.1 percent of elapsed time */
            loop_check *= 100.1 / percent_limit;
         } else {
            /* estimate loop count at 100.5 percent of elapsed time */
            loop_check *= 100.5 / percent_limit;
         }
      }

      /****************************************************/
      /* determine size, align, and bias for current loop */
      /****************************************************/

      if(setup->size_max2 == 1) {
         size = size_limit;
      } else {
         /* select a random size for the transfer */
         size = setup->size_list2[((unsigned)rand() >> 1) %
            setup->size_max2];
      }

      if(setup->align_max2 == 1) {
         align = align_limit;
      } else {
         /* select a random alignment for the transfer */
         align = setup->align_list2[((unsigned)rand() >> 1) %
            setup->align_max2];
      }

      if(setup->bias_max2 == 1) {
         bias = bias_limit;
      } else {
         /* select a random bias for the transfer */
         bias = setup->bias_list2[((unsigned)rand() >> 1) %
            setup->bias_max2];
      }

      /**********************************************/
      /* seek based upon mode, prior to i/o request */
      /**********************************************/

      switch(setup->mode) {
      case TC_read_forward:
      case TC_write_forward:
      case TC_create_forward:
         /**********************/
         /* forward processing */
         /**********************/
         if(n_expect) {
            /**************************************/
            /* data was expected by previous loop */
            /**************************************/
            lloffset_now += n_read + n_write;
         } else {
            /******************************/
            /* EOF was expected last time */
            /******************************/
            if(setup->mode == TC_create_forward) {
               /* trial terminated, file has been created */
               goto terminate;
            } else if(setup->mode == TC_read_forward) {
               /* trial terminated, file has been consumed */
               goto terminate;
            }
            /*******************************/
            /* rewind to beginning of file */
            /*******************************/
            lloffset_now = 0;
            switch(setup->pack) {
            case TC_map_ptr:
            case TC_map_cpy:
            case TC_buf_ptr:
            case TC_buf_cpy:
            case TC_raw_ptr:
            case TC_raw_cpy:
               if(gseek(fp_gio, 0, SEEK_SET)) {
                  setup->rc = 23;
                  goto terminate;
               }
               break;
            case TC_usp:
               #ifdef TST_USP
                  C_SISSEEK(lu_usp, (int)1);
               #endif /* TST_USP */
               break;
            case TC_stdio:
               if(fseek(fp_stdio, 0L, SEEK_SET)) {
                  setup->rc = 24;
                  goto terminate;
               }
               break;
            }
            setup->seek++;
         }
         if(lloffset_now + size > setup->file_limit) {
            /* retrict n_expect to bytes remaining within file */
            if(setup->pack == TC_usp) {
               n_expect = 0;
            } else {
               n_expect = setup->file_limit - lloffset_now;
            }
         } else {
            n_expect = size;
         }
         break;
      case TC_read_backward:
      case TC_write_backward:
         /***********************/
         /* backward processing */
         /***********************/
         if(lloffset_now > 0) {
            lloffset_now -= size;
            if(setup->pack == TC_usp) --n_trace_now;
            if(lloffset_now < 0) {
               size += lloffset_now;
               lloffset_now = 0;
            }
         } else {
            /* seek to end of file, minus io size */
            if(setup->pack == TC_usp) {
               n_trace_now = setup->usp_traces - 1;
               lloffset_now = n_trace_now * size;
            } else {
               lloffset_now = setup->file_limit - size;
            }
         }

         n_expect = size;

         switch(setup->pack) {
         case TC_map_ptr:
         case TC_map_cpy:
         case TC_buf_ptr:
         case TC_buf_cpy:
         case TC_raw_ptr:
         case TC_raw_cpy:
            gio_pos_t = lloffset_now;
            if(gsetpos(fp_gio, &gio_pos_t)) {
               setup->rc = 25;
               goto terminate;
            }
            break;
         case TC_usp:
            #ifdef TST_USP
               C_SISSEEK(lu_usp, n_trace_now + 1);
            #endif /* TST_USP */
            break;
         case TC_stdio:
            if(fseek(fp_stdio, lloffset_now, SEEK_SET)) {
               setup->rc = 26;
               goto terminate;
            }
            break;
         }
         setup->seek++;
         break;
      case TC_read_random:
      case TC_write_random:
         /*********************/
         /* random processing */
         /*********************/
         lloffset_now = (HOST_LLONG_T)(rand() << 30) + (HOST_LLONG_T)rand();
         lloffset_now *= align;
         lloffset_now += bias;
         lloffset_now &= (~((HOST_ULONG_T)0)) >> 1;
         lloffset_now %= setup->file_limit;

         switch(setup->pack) {
         case TC_map_ptr:
         case TC_map_cpy:
         case TC_buf_ptr:
         case TC_buf_cpy:
         case TC_raw_ptr:
         case TC_raw_cpy:
            gio_pos_t = lloffset_now;
            if(gsetpos(fp_gio, &gio_pos_t)) {
               setup->rc = 28;
               goto terminate;
            }
            break;
         case TC_usp:
            #ifdef TST_USP
               n_trace_now = (((0x7fff & rand()) << 16) + (0x7fff & rand())) %
                  setup->usp_traces;
               lloffset_now = n_trace_now * size;
               C_SISSEEK(lu_usp, n_trace_now + 1);
            #endif /* TST_USP */
            break;
         case TC_stdio:
            if(fseek(fp_stdio, lloffset_now, SEEK_SET)) {
               setup->rc = 29;
               goto terminate;
            }
            break;
         }
         setup->seek++;
         if(lloffset_now + size > setup->file_limit) {
            /* retrict n_expect to bytes remaining within file */
            n_expect = setup->file_limit - lloffset_now;
         } else {
            n_expect = size;
         }
         break;
      }

      /**********************************/
      /* execute the trial i/o function */
      /**********************************/

      switch(setup->mode) {
      case TC_read_forward:
      case TC_read_backward:
      case TC_read_random:
         switch(setup->pack) {
         case TC_map_cpy:
         case TC_buf_cpy:
         case TC_raw_cpy:
            n_read = greadc(fp_gio, (void*)buf, size);

            if(setup->watch) fprintf(stdout, "size= %7i cache=%8li, %+3i * %li, min= %li, max= %li, big= %li\n",
               (int)size,
               (long)fp_gio->cache_r, (int)fp_gio->delta, (long)fp_gio->mod_r,
               (long)fp_gio->min_r, (long)fp_gio->max_r, (long)fp_gio->big_r);
            break;
         case TC_map_ptr:
         case TC_buf_ptr:
         case TC_raw_ptr:
            n_read = greada(fp_gio, (const void**)&buf, size, GIO_CHAR);

            if(setup->watch) fprintf(stdout, "size= %7i cache=%8li, %+3i * %li, min= %li, max= %li, big= %li\n",
               (int)size,
               (long)fp_gio->cache_r, (int)fp_gio->delta, (long)fp_gio->mod_r,
               (long)fp_gio->min_r, (long)fp_gio->max_r, (long)fp_gio->big_r);
            break;
         case TC_usp:
            #ifdef TST_USP
               sis_bytes = size;
               C_RTAPE(lu_usp, buf, &sis_bytes);
               n_read = sis_bytes;
            #endif /* TST_USP */
            break;
         case TC_stdio:
            n_read = fread((void*)buf, 1, size, fp_stdio);
            break;
         }
         if(n_expect) {
            if(n_read != n_expect) {
               setup->rc= 30;
               fprintf(stderr, "   short read, loop= %li, n_read= %i, n_expect= %i\n      errno= %i, %s\n",
                  (long)setup->loop, (int)n_read, (int)n_expect,
                  (int)errno, strerror(errno));
               assert(n_read < n_expect && "read overrun !");
               if(short_read++ > 5) goto terminate;
            }
         } else if(n_read) {
            assert(size && "zero length read error");
            assert(! size && "eof not detected");
         } else {
            switch(setup->pack) {
            case TC_map_ptr:
            case TC_map_cpy:
            case TC_buf_ptr:
            case TC_buf_cpy:
            case TC_raw_ptr:
            case TC_raw_cpy:
               if(! geof(fp_gio))
                  fprintf(stderr, "geof not set, loop= %li\n",
                     (long)setup->loop);
               break;
            case TC_stdio:
               if(! feof(fp_stdio))
                  fprintf(stderr, "feof not set, loop= %li\n",
                     (long)setup->loop);
               break;
            }
         }
         setup->read++;
         setup->read_bytes += n_read;
         if(n_read && setup->form == TC_verify) {
            init_first = lloffset_now & ~(scale - 1);
            init_words = 2 + n_read / scale;
            ibytes = n_read;
            ishift =  lloffset_now & (scale - 1);
            #ifdef FTN_VERIFY
               /*******************************************/
               /* verify every "whole word" that was read */
               /*******************************************/
               if(ishift) {
                  init_first++;
                  init_words--;
                  ibytes -= ishift;
                  fbuf = buf + (scale - ishift);
               } else {
                  fbuf = buf;
               }
               lloffset_ftn = lloffset_now + ishift;
               if(HOST_FTN_FUNC(frverify, FRVERIFY)
                  ((int*)fbuf, (int*)ubuf, &lloffset_ftn,
                     &ibytes, (int*)&init_first, (int*)&init_words,
                     (int*)&ishift, &scale)) {
                  setup->rc = 27;
               }
            #else /* FTN_VERIFY */
               /***********************************/
               /* verify every byte that was read */
               /***********************************/
               for(j = 0; j < init_words; j++)
                  ubuf[j] = init_first + j * scale;
               if(memcmp(buf, (char*)ubuf + ishift, ibytes)) {
                  HOST_FTN_FUNC(verify, VERIFY)
                     ((int*)buf, (int*)ubuf, &lloffset_now, &ibytes);
                  setup->rc = 27;
               }
            #endif /* FTN_VERIFY */
         } else if(buf && (setup->pack == TC_map_ptr)) {
            /* peek at each page, to actually transfer data */
            for(j = 0; j < n_read; j += gio.page_size)
               page_peek += ((char*)buf)[j];
         }
         break;
      case TC_write_forward:
      case TC_write_backward:
      case TC_write_random:
      case TC_create_forward:
         switch(setup->pack) {
         case TC_map_cpy:
         case TC_buf_cpy:
         case TC_raw_cpy:
            if(setup->form == TC_verify) {
               /****************************************/
               /* intialize every byte that is written */
               /****************************************/
               init_first = lloffset_now & ~(scale - 1);
               init_words = 2 + size / scale;
               ishift = lloffset_now & (scale - 1);
               #ifdef FTN_VERIFY
                  HOST_FTN_FUNC(fwverify, FWVERIFY)
                     ((int*)buf, (int*)&init_first, (int*)&init_words,
                        (int*)&ishift, &scale);
               #else /* FTN_VERIFY */
                  for(j = 0; j < init_words; j++)
                     ubuf[j] = init_first + j * scale;
                  memcpy(buf, (char*)ubuf + ishift, size);
               #endif /* FTN_VERIFY */
            }
            n_write = gwritec(fp_gio, (const void*)buf, size);

            if(setup->watch) fprintf(stdout, "size= %7i cache=%8li, %+3i * %li, min= %li, max= %li, big= %li\n",
               (int)size,
               (long)fp_gio->cache_w, (int)fp_gio->delta, (long)fp_gio->mod_w,
               (long)fp_gio->min_w, (long)fp_gio->max_w, (long)fp_gio->big_w);
            break;
         case TC_map_ptr:
         case TC_buf_ptr:
         case TC_raw_ptr:
            n_write = gwritea(fp_gio, (void**)&buf, size, GIO_CHAR);

            if(setup->watch) fprintf(stdout, "size= %7i cache=%8li, %+3i * %li, min= %li, max= %li, big= %li\n",
               (int)size,
               (long)fp_gio->cache_w, (int)fp_gio->delta, (long)fp_gio->mod_w,
               (long)fp_gio->min_w, (long)fp_gio->max_w, (long)fp_gio->big_w);
            if(setup->form == TC_verify && n_write > 0) {
               init_first = lloffset_now & ~(scale - 1);
               init_words = 2 + n_write / scale;
               ishift = lloffset_now & (scale - 1);
               #ifdef FTN_VERIFY
                  HOST_FTN_FUNC(fwverify, FWVERIFY)
                     ((int*)buf, (int*)&init_first, (int*)&init_words,
                        (int*)&ishift, &scale);
               #else /* FTN_VERIFY */
                  /****************************************/
                  /* intialize every byte that is written */
                  /****************************************/
                  for(j = 0; j < init_words; j++)
                     ubuf[j] = init_first + j * scale;
                  ibytes = n_write;
                  memcpy(buf, (char*)ubuf + ishift, ibytes);
               #endif /* FTN_VERIFY */
            }
            break;
         case TC_usp:
            if(setup->form == TC_verify) {
               /****************************************/
               /* intialize every byte that is written */
               /****************************************/
               init_first = lloffset_now & ~(scale - 1);
               init_words = 2 + size / scale;
               ishift = lloffset_now & (scale - 1);
               #ifdef FTN_VERIFY
                  HOST_FTN_FUNC(fwverify, FWVERIFY)
                     ((int*)buf, (int*)&init_first, (int*)&init_words,
                        (int*)&ishift, &scale);
               #else /* FTN_VERIFY */
                  for(j = 0; j < init_words; j++)
                     ubuf[j] = init_first + j * scale;
                  memcpy(buf, (char*)ubuf + ishift, size);
               #endif /* FTN_VERIFY */
            }
            size = size_limit;
            #ifdef TST_USP
               sis_bytes = size;
               C_WRTAPE(lu_usp, buf, sis_bytes);
               n_write = sis_bytes;
            #endif /* TST_USP */
            break;
         case TC_stdio:
            if(setup->form == TC_verify) {
               /****************************************/
               /* intialize every byte that is written */
               /****************************************/
               init_first = lloffset_now & ~(scale - 1);
               init_words = 2 + size / scale;
               ishift = lloffset_now & (scale - 1);
               #ifdef FTN_VERIFY
                  HOST_FTN_FUNC(fwverify, FWVERIFY)
                     ((int*)buf, (int*)&init_first, (int*)&init_words,
                        (int*)&ishift, &scale);
               #else /* FTN_VERIFY */
                  for(j = 0; j < init_words; j++)
                     ubuf[j] = init_first + j * scale;
                  memcpy(buf, (char*)ubuf + ishift, size);
               #endif /* FTN_VERIFY */
            }
            n_write = fwrite((void*)buf, 1, size, fp_stdio);
            break;
         }
         if(n_write != size) {
            setup->rc= 30;
            fprintf(stderr, "   short write, loop= %li, n_write= %i, expect= %i\n      errno= %i, %s\n",
               (long)setup->loop, (int)n_write, (int)size,
               (int)errno, strerror(errno));
            assert(n_write < size && "write overrun !");
            if(short_write++ > 5) goto terminate;
         }

         setup->write_bytes += n_write;
         setup->write++;
         break;
      case TC_nop:
         n_write = size;
         break;
      }

      if(setup->rc) break;
   }

   /*****************************/
   /* flush buffers, if writing */
   /*****************************/

   terminate:
   switch(setup->mode) {
   case TC_write_forward:
   case TC_write_backward:
   case TC_write_random:
   case TC_create_forward:
      switch(setup->pack) {
      case TC_map_ptr:
      case TC_map_cpy:
      case TC_buf_ptr:
      case TC_buf_cpy:
      case TC_raw_ptr:
      case TC_raw_cpy:
         gflush(fp_gio);
         fsync(gfileno(fp_gio));
         break;
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
         #endif /* TST_USP */
         break;
      case TC_stdio:
         fflush(fp_stdio);
         fsync(fileno(fp_stdio));
         break;
      }
      /*******************************/
      /* synchronize the file system */
      /*******************************/
      /*** system("sync"); not effective, use fsync instead. */
      break;
   }

   /*************************/
   /*** trial termination ***/
   /*************************/

   check_time = time(NULL);
   check_cpu = clock();

   /*************************/
   /* save vital statistics */
   /*************************/
   setup->short_read = short_read;
   setup->elapsed = difftime(check_time, start_time);
   if(setup->elapsed <= 0.0) setup->elapsed = 1.0;
   setup->cpu = ((double)(check_cpu - start_cpu)) / CLOCKS_PER_SEC;
   if(setup->cpu <= 0.0) setup->cpu = 0.1 / CLOCKS_PER_SEC;
   setup->read_mb = setup->read_bytes / GIO_MB;
   if(setup->read_mb <= 0.0) setup->read_mb = 1.0e-6;
   setup->write_mb = setup->write_bytes / GIO_MB;
   if(setup->write_mb <= 0.0) setup->write_mb = 1.0e-6;

   return setup->rc;
}
