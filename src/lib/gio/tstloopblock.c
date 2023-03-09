static char rcsid[]="$Id: tstloopblock.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstLoopBlock: system io block sizes
   tstTrial: conduction one test/tuneup trial.

*/

#include "tstsys.h"

void tstLoopBlock(
   TST_SETUP *setup,
   int pass,
   long pick_size,
   int pick_align,
   int pick_bias)
{
   int pick_block;
   TST_SCORE *score2;

   double elapsed_mbs, cpu_mbs;

   for(pick_block = 0;
      pick_block < setup->block_max;
      pick_block++) {

      switch(setup->pack) {
      case TC_stdio:

         if(setup->block_list[pick_block] == 1) continue;

         if(setup->smart) {
            if(setup->block_list[pick_block] < 0) {
               if(setup->mode == TC_read_forward ||
                  setup->mode == TC_write_forward ||
                  setup->mode == TC_create_forward) continue;
            } else {
               if(setup->mode != TC_read_forward &&
                  setup->mode != TC_write_forward &&
                  setup->mode != TC_create_forward) continue;
            }
         }
         break;
      case TC_usp:
         if(setup->block_list[pick_block] > 0) continue;

         if(setup->smart) {
            if(setup->block_list[pick_block] < 0) {
               if(setup->mode == TC_read_forward ||
                  setup->mode == TC_write_forward ||
                  setup->mode == TC_create_forward) continue;
            } else {
               if(setup->mode != TC_read_forward &&
                  setup->mode != TC_write_forward &&
                  setup->mode != TC_create_forward) continue;
            }
         }
         break;
      case TC_vds:
         if(setup->block_list[pick_block] != 0) continue;
         break;
      case TC_map_cpy:
      case TC_map_ptr:
         if(setup->block_list[pick_block] < 0) continue;
         break;
      case TC_raw_cpy:
      case TC_raw_ptr:
         if(setup->block_list[pick_block] < 0) continue;
         break;
      case TC_buf_cpy:
      case TC_buf_ptr:
         break;
      }

      setup->block = setup->block_list[pick_block];

      score2 = &(*setup->board)[setup->form - TC_verify]
         [setup->pack - TC_stdio]
         [setup->mode - TC_nop]
         [pick_size]
         [pick_align]
         [pick_bias]
         [pick_block];

      if(pass < 0) {
         setup->loop++;

         if(setup->mode != TC_create_forward) {
            /* initialize and cache flush guestimate */
            setup->elapsed += setup->file_limit / (GIO_MB * setup->write_mbs);
            setup->elapsed += setup->cache_limit / setup->write_mbs;
            setup->elapsed += 2.0;
         }

         if(setup->form == TC_verify &&
            setup->mode >= TC_write_forward &&
            setup->mode <= TC_create_forward) {
            /* verify guestimate */
            setup->elapsed += setup->file_limit / (GIO_MB * setup->read_mbs);
         }

         if(setup->mode == TC_create_forward) {
            /* write once guestimate */
            setup->elapsed += setup->file_limit / (GIO_MB * setup->write_mbs);
         } else if(setup->mode == TC_read_forward) {
            /* read once guestimate */
            setup->elapsed += setup->file_limit / (GIO_MB * setup->read_mbs);
         } else {
            /* read and write guestimate, determined by time limit. */
            setup->elapsed += setup->time_limit;
         }
      } else if(pass < setup->pass_max) {
         /***********************/
         /***********************/
         /***                 ***/
         /***  EXECUTE TRIAL  ***/
         /***                 ***/
         /***********************/
         /***********************/
         tstTrial(setup);

         elapsed_mbs = (setup->read_mb + setup->write_mb) / setup->elapsed;
         cpu_mbs = (setup->read_mb + setup->write_mb) / setup->cpu;

         if(elapsed_mbs > score2->elapsed_mbs) {
            /* save the best score */
            score2->elapsed = setup->elapsed;
            score2->elapsed_mbs = elapsed_mbs;
            score2->cpu = setup->cpu;
            score2->cpu_mbs = cpu_mbs;
            score2->mb = setup->read_mb + setup->write_mb;
            score2->loop = setup->loop;
         }
      }

      if(pass == setup->pass_max) {
         fprintf(stderr, "        blk= %6i, ",
               setup->block_list[pick_block]);
         fprintf(stderr, "sec= %6.1f  %5.2f mb/s, cpu= %6.1f %5.2f mb/s, %5.1f mb, %li\n",
            score2->elapsed, score2->elapsed_mbs,
            score2->cpu, score2->cpu_mbs,
            score2->mb, (long)score2->loop);
      }
   }
   return;
}
