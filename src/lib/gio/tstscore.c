static char rcsid[]="$Id: tstscore.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstScore: print the raw score for one trial.

*/

#include "tstsys.h"

void tstScore(
   TST_SETUP *setup)
{
   GIO_BIG i;

   /* print trial report */
   if(setup->rc || setup->error) fprintf(stderr, "\trc= %i, errno= %i, %s\n",
      setup->rc, setup->error, strerror(setup->error));

   fprintf(stderr, "   elapsed= %6.1f  %5.2f mb/s, cpu= %6.1f  %5.2f mb/s,  %6.1f mb, %li\n",
      setup->elapsed,
      (setup->read_mb + setup->write_mb) / setup->elapsed,
      setup->cpu,
      (setup->read_mb + setup->write_mb) / setup->cpu,
      setup->read_mb + setup->write_mb,
      (long)setup->loop);

   if(setup->short_read) fprintf(stderr, "      ERROR ? SHORT_READ= %li\n",
      (long)setup->short_read);

   if(! setup->verbose) return;

   fprintf(stderr, "\tsize=");
   for(i = 0; i < setup->size_max2; i++)
      fprintf(stderr, " %i", setup->size_list2[i]);
   fprintf(stderr, "\n");

   fprintf(stderr, "\talign=");
   for(i = 0; i < setup->align_max2; i++)
      fprintf(stderr, " %i", setup->align_list2[i]);
   fprintf(stderr, "\n");

   fprintf(stderr, "\tbias=");
   for(i = 0; i < setup->bias_max2; i++)
      fprintf(stderr, " %i", setup->bias_list2[i]);
   fprintf(stderr, "\n");

   fprintf(stderr, "\ttrial= %s\n", form_name[setup->form - TC_verify]);

   fprintf(stderr, "\tpackage= %s\n", pack_name[setup->pack - TC_stdio]);

   fprintf(stderr, "\tmode= %s\n", mode_name[setup->mode - TC_nop]);

   fprintf(stderr, "\tseed= 0x%x\n", (int)setup->seed);

   fprintf(stderr, "\tloop count= %li, short read= %li\n",
      (long)setup->loop, (long)setup->short_read);
   fprintf(stderr, "\tcount   read=  %8li,     write= %8li, seek= %8li\n",
      (long)setup->read, (long)setup->write, (long)setup->seek);
   fprintf(stderr, "\ttotal   read=  %9.3f mb, write= %9.3f mb\n",
      setup->read_mb, setup->write_mb);

   #ifdef xxxGIO_DEBUG
      if(setup->pack == TC_buf_ptr) {
         fprintf(stderr, "\tuser    read=  %8li,     write= %8li, seek= %8li\n",
            (long)setup->user_read, (long)setup->user_write, (long)setup->user_seek);
         fprintf(stderr, "\tsystem  read=  %8li,     write= %8li, seek= %8li\n",
            (long)setup->system_read, (long)setup->system_write, (long)setup->system_seek);
         fprintf(stderr, "\tuser    read=  %9.3f mb, write= %9.3f mb\n",
            setup->user_read_mb, setup->user_write_mb);
         fprintf(stderr, "\tsystem  read=  %9.3f mb, write= %9.3f mb\n",
            setup->system_read_mb, setup->system_write_mb);
         fprintf(stderr, "\tcached  read= %5.1f percent, write= %5.1f percent\n",
            100.0 * setup->user_read_mb / setup->system_read_mb,
            100.0 * setup->user_write_mb / setup->system_write_mb);

         fprintf(stderr, "\tuser   elapsed= %8.3f mb/s, cpu= %8.3f mb/s\n",
            (setup->user_read_mb + setup->user_write_mb) /
               setup->elapsed,
            (setup->user_read_mb + setup->user_write_mb) /
               setup->cpu);
         fprintf(stderr, "\tsystem elapsed= %8.3f mb/s, cpu= %8.3f mb/s\n",
            (setup->system_read_mb + setup->system_write_mb) /
               setup->elapsed,
            (setup->system_read_mb + setup->system_write_mb) /
               setup->cpu);
      }
   #endif /* GIO_DEBUG */

   return;
}
