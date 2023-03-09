static char rcsid[]="$Id: tstsummary.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstSummary: print summary of test results.

*/

#include "tstsys.h"

void tstSummary(
   TST_SETUP *setup)
{
   TST_SCORE *score2;
   int pick_size, pick_align, pick_bias, pick_block;
   int max_pack, max_align, max_bias, max_block;
   double max_mbs;
   double max_speed_percent, max_verify_percent;
   double max_read_speed_mbs, max_create_speed_mbs;
   double max_read_verify_mbs, max_create_verify_mbs;
   double max_verify_mbs, max_speed_mbs;

   fprintf(stderr, "\n");

   /******************************************/
   /* find the maximum gio buf_cpy rate for  */
   /*    read_speed, read_verify,            */
   /*    create_speed, create_verify.        */
   /******************************************/

   setup->pack = TC_buf_cpy;
   max_read_speed_mbs = 0.0;
   max_read_verify_mbs = 0.0;
   max_create_speed_mbs = 0.0;
   max_create_verify_mbs = 0.0;

   for(pick_size = 0; pick_size < TST_SIZE_MAX + 1; pick_size++) {

      for(pick_align = 0; pick_align < TST_ALIGN_MAX + 1; pick_align++) {

         for(pick_bias = 0; pick_bias < TST_BIAS_MAX + 1; pick_bias++) {

            for(pick_block = 0; pick_block < TST_BLOCK_MAX; pick_block++) {
               score2 = &(*setup->board)[TC_speed - TC_verify]
                  [setup->pack - TC_stdio][TC_read_forward - TC_nop]
                  [pick_size][pick_align][pick_bias][pick_block];

               if(max_read_speed_mbs < score2->elapsed_mbs)
                  max_read_speed_mbs = score2->elapsed_mbs;

               score2 = &(*setup->board)[0]
                  [setup->pack - TC_stdio][TC_read_forward - TC_nop]
                  [pick_size][pick_align][pick_bias][pick_block];

               if(max_read_verify_mbs < score2->elapsed_mbs)
                  max_read_verify_mbs = score2->elapsed_mbs;

               score2 = &(*setup->board)[TC_speed - TC_verify]
                  [setup->pack - TC_stdio][TC_create_forward - TC_nop]
                  [pick_size][pick_align][pick_bias][pick_block];

               if(max_create_speed_mbs < score2->elapsed_mbs)
                  max_create_speed_mbs = score2->elapsed_mbs;

               score2 = &(*setup->board)[0]
                  [setup->pack - TC_stdio][TC_create_forward - TC_nop]
                  [pick_size][pick_align][pick_bias][pick_block];

               if(max_create_verify_mbs < score2->elapsed_mbs)
                  max_create_verify_mbs = score2->elapsed_mbs;
            }
         }
      }
   }

   /**********************************************/
   /* report maximum gio read/write speed/verify */
   /**********************************************/

   fprintf(stderr, "tstSummary: gio buf_cpy max read   forward speed= %5.2f mb/s, verify= %5.2f mb/s\n",
      max_read_speed_mbs, max_read_verify_mbs);

   fprintf(stderr, "tstSummary: gio buf_cpy max create forward speed= %5.2f mb/s, verify= %5.2f mb/s\n",
      max_create_speed_mbs, max_create_verify_mbs);

   fprintf(stderr, "\n");

   /***************************/
   /* report size_list values */
   /***************************/

   fprintf(stderr, "size_list=");

   for(pick_size = 0; pick_size < setup->size_max; pick_size++)
      fprintf(stderr, "  %i", setup->size_list[pick_size]);

   fprintf(stderr, "\n");

   fprintf(stderr, "block_list=");

   for(pick_block = 0; pick_block < setup->block_max; pick_block++)
      fprintf(stderr, "  %i", setup->block_list[pick_block]);

   fprintf(stderr, "\ndefault file block size= %i\n", setup->blk_size);
   fprintf(stderr,   "host memory  page  size= %i", gio.page_size);

   #ifdef GIO_MMAP_IS_SUPPORTED
      fprintf(stderr, "  mmap IS supported\n");
   #else /* GIO_MMAP_IS_SUPPORTED */
      fprintf(stderr, "  mmap NOT supported\n\n");
   #endif /* GIO_MMAP_IS_SUPPORTED */

   #ifdef GIO_RAWIO_IS_SUPPORTED
      fprintf(stderr, "raw (direct) i/o IS supported\n\n");
   #else /* GIO_RAWIO_IS_SUPPORTED */
      fprintf(stderr, "raw (direct) i/o NOT supported\n\n");
   #endif /* GIO_RAWIO_IS_SUPPORTED */

   /*************************************/
   /* report comparison to maximum rate */
   /*************************************/

   for(setup->pack = TC_stdio;
      setup->pack <= TC_raw_cpy;
      setup->pack++) {
      #ifndef TST_USP
         if(setup->pack == TC_usp) continue;
      #endif /* TST_USP */
      #ifndef TST_VDS
         if(setup->pack == TC_vds) continue;
      #endif /* TST_VDS */

      fprintf(stderr, "%s\n", pack_name[setup->pack - TC_stdio]);

      for(setup->mode = TC_read_forward;
         setup->mode <= TC_create_forward;
         setup->mode++) {
         if(strstr(mode_name[setup->mode - TC_nop], "read")) {
            max_speed_percent = max_read_speed_mbs;
            max_verify_percent = max_read_verify_mbs;

            if(max_speed_percent <= 0. && max_verify_percent <= 0.) continue;

            fprintf(stderr, "  %-14s max gio read   mb/s speed %5.2f, verify %5.2f\n",
               mode_name[setup->mode - TC_nop], max_speed_percent, max_verify_percent);
         } else {
            max_speed_percent = max_create_speed_mbs;
            max_verify_percent = max_create_verify_mbs;

            if(max_speed_percent <= 0. && max_verify_percent <= 0.) continue;

            fprintf(stderr, "  %-14s max gio create mb/s speed %5.2f, verify %5.2f\n",
               mode_name[setup->mode - TC_nop], max_speed_percent, max_verify_percent);
         }

         if(max_speed_percent == 0.0) max_speed_percent = 1e-6;

         if(max_verify_percent == 0.0) max_verify_percent = 1e-6;

         max_speed_percent /= 100.;
         max_verify_percent /= 100.;

         for(pick_size = 0; pick_size < TST_SIZE_MAX; pick_size++) {
            max_verify_mbs = 0.;
            max_speed_mbs = 0.;

            for(pick_align = 0;
               pick_align < TST_ALIGN_MAX;
               pick_align++) {

               for(pick_bias = 0;
                  pick_bias < TST_BIAS_MAX;
                  pick_bias++) {
                  setup->form = TC_verify;

                  for(pick_block = 0;
                     pick_block < TST_BLOCK_MAX;
                     pick_block++) {
                     score2 = &(*setup->board)[setup->form - TC_verify]
                        [setup->pack - TC_stdio][setup->mode - TC_nop]
                        [pick_size][pick_align][pick_bias][pick_block];

                     if(! score2->loop || max_verify_mbs > score2->elapsed_mbs)
                        continue;

                     max_verify_mbs = score2->elapsed_mbs;
                  }

                  setup->form = TC_speed;

                  for(pick_block = 0;
                     pick_block < TST_BLOCK_MAX;
                     pick_block++) {
                     score2 = &(*setup->board)[setup->form  - TC_verify]
                        [setup->pack - TC_stdio][setup->mode - TC_nop]
                        [pick_size][pick_align][pick_bias][pick_block];

                     if(! score2->loop || max_speed_mbs > score2->elapsed_mbs)
                        continue;

                     max_speed_mbs = score2->elapsed_mbs;
                  }
               }
            }

            if(max_verify_mbs == 0. && max_speed_mbs == 0.) continue;

            if(pick_size < setup->size_max) {
               fprintf(stderr, "              size= %8i bytes,",
                  setup->size_list[pick_size]);
            } else {
               fprintf(stderr, "                        random sizes,");
            }
            fprintf(stderr, "  speed %4.0f%%, verify %4.0f%%\n",
               max_speed_mbs / max_speed_percent,
               max_verify_mbs / max_verify_percent);
         }
      }
   }

   fprintf(stderr, "\n\n");

   for(setup->form = TC_verify;
      setup->form <= TC_speed;
      setup->form++) {
      fprintf(stderr, "%s\n", form_name[setup->form - TC_verify]);

      for(setup->mode = TC_read_forward;
         setup->mode <= TC_create_forward;
         setup->mode++) {
         fprintf(stderr, "  %s\n", mode_name[setup->mode - TC_nop]);

         for(pick_size = 0; pick_size < TST_SIZE_MAX; pick_size++) {
            max_mbs = 0.;

            for(setup->pack = TC_stdio;
               setup->pack <= TC_raw_cpy;
               setup->pack++) {

               for(pick_align = 0;
                  pick_align < TST_ALIGN_MAX;
                  pick_align++) {

                  for(pick_bias = 0;
                     pick_bias < TST_BIAS_MAX;
                     pick_bias++) {

                     for(pick_block = 0;
                        pick_block < TST_BLOCK_MAX;
                        pick_block++) {
                        score2 = &(*setup->board)[setup->form - TC_verify]
                           [setup->pack - TC_stdio][setup->mode - TC_nop]
                           [pick_size][pick_align][pick_bias][pick_block];

                        if(! score2->loop || max_mbs > score2->elapsed_mbs)
                           continue;

                        max_mbs = score2->elapsed_mbs;
                        max_pack = setup->pack;
                        max_align = setup->align_list[pick_align];
                        max_bias = setup->bias_list[pick_bias];
                        max_block = setup->block_list[pick_block];
                     }
                  }
               }
            }

            if(max_mbs == 0.) continue;

            if(pick_size < setup->size_max2) {
               fprintf(stderr, "    %8i bytes,", setup->size_list[pick_size]);
            } else {
               fprintf(stderr, "      random sizes,");
            }
            fprintf(stderr, "  %6s wins,", pack_name[max_pack - TC_stdio]);
            fprintf(stderr, "  %5.2f mb/s,", max_mbs);

            if(max_block < 0) {
               fprintf(stderr, "    non-buffered,");
            } else if(max_block == 0) {
               fprintf(stderr, "  %6i default,", setup->blk_size);
            } else {
               fprintf(stderr, "  %6i buffer, ", max_block);
            }
            fprintf(stderr, "  %i align,  %i bias\n", max_align, max_bias);
         }
      }
   }

   fprintf(stderr, "\n");

   return;
}
