static char rcsid[]="$Id: tstclose.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstClose: close test file and cleanup after trial.

*/

#include "tstsys.h"

int tstClose(
   TST_SETUP *setup,
   char *tmp_file,
   FILE *fp_clear,
   FILE *fp_stdio,
   GFILE *fp_gio,
   int lu_usp,
   char *cbuf,
   unsigned *ubuf)
{
   time_t now_time;

   if(setup->verbose) {
      now_time = time(NULL);
      fprintf(stderr, "   cleanup after trial %s",
         ctime(&now_time));
   }

   free(cbuf);
   free(ubuf);

   /********************/
   /* close tuner file */
   /********************/

   switch(setup->pack) {
   case TC_map_ptr:
   case TC_map_cpy:
   case TC_buf_ptr:
   case TC_buf_cpy:
   case TC_raw_ptr:
   case TC_raw_cpy:
      fprintf(stderr, "   cache_r=%6li, cache_w=%6li, size_p=%6li, size_m=%6li, size_s=%6li, size_u=%6li\n",
         (long)fp_gio->cache_r, (long)fp_gio->cache_w,
         (long)fp_gio->size_p, (long)fp_gio->size_m,
         (long)fp_gio->size_s, (long)fp_gio->size_u);
      if(setup->pack == TC_raw_ptr || setup->pack == TC_raw_cpy) {
         fprintf(stderr, "   blkalign= %6li, minsize=%6li, maxsize=%6li\n",
            (long)fp_gio->blkalign,
            (long)fp_gio->minsize, (long)fp_gio->maxsize);
      }
      setup->error = fp_gio->error;
      gclose(fp_gio);
      break;
   case TC_usp:
      #ifdef TST_USP
         C_LBCLOS(lu_usp);
      #endif /* TST_USP */
      break;
   case TC_stdio:
      fclose(fp_stdio);
      break;
   }

   if(setup->mode != TC_create_forward) {
      /*** fclose(fp_clear); ***/
      remove(tmp_file);
   }

   return setup->rc;
}
