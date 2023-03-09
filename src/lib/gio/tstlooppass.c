static char rcsid[]="$Id: tstlooppass.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstLoopPass: 1 through pass_max (repeatability test)
   tstLoopGoal: verify, speed
      tstLoopPack: stdio, usp, gio (buf/map/raw and ptr/cpy)
         tstLoopMode: read/write/create and foward/backward/random
            tstLoopSize: fixed, random
               tstLoopAlign: fixed, random
                  tstLoopBias: fixed, random
                     tstLoopBlock: system io block sizes
                        tstTrial: conduction one test/tuneup trial.
   tstSummary: print summary of test results.
*/

#include "tstsys.h"

void tstLoopPass(
   TST_SETUP *setup)
{
   time_t start_time, stop_time;
   double file_mb;
   int pass;

   /************************************************/
   /* loop over all passes and trial combinations, */
   /* where pass equals                            */
   /*    -1 == count trial combinations            */ 
   /*     0, 1, 2... = repeat pass                 */
   /*     pass_max == summarize results            */
   /************************************************/

   for(pass = -1;
      pass <= setup->pass_max;
      pass++) {
      if(pass == -1) {
         start_time = time(NULL);
         fprintf(stderr, "\n\n***  tstLoopPass, preview trials  ***\n\n");
      } else if(pass == setup->pass_max && setup->pass_max != 0) {
         stop_time = time(NULL);
         fprintf(stderr, "\n\n***  tstLoopPass, summarize results  ***\n\n");
         fprintf(stderr, "\tstart= %s", ctime(&start_time));
         fprintf(stderr, "\tstop=  %s", ctime(&stop_time));
      }
      if(pass == -1 ||
         (pass == setup->pass_max && setup->pass_max != 0)) {
         /* report constant parameter for all trials */
         fprintf(stderr, "\ttitle= %s\n",
            setup->title);
         fprintf(stderr, "\tpass_max= %i best trial\n",
            setup->pass_max);
         fprintf(stderr, "\tfilename= %s\n",
            setup->filename);
         file_mb = setup->file_limit / GIO_MB;
         fprintf(stderr, "\tfile_limit= %6.2f MB\n",
            file_mb);
         fprintf(stderr, "\ttime_limit= %4.1f seconds\n",
            setup->time_limit);
         fprintf(stderr, "\tcache_limit= %li mb\n",
            (long)setup->cache_limit);
         fprintf(stderr, "\tpage_size= %li bytes\n",
            (long)gio.page_size);
         fprintf(stderr, "\tblk_size= %li bytes\n",
            (long)setup->blk_size);
         fprintf(stderr, "\tgsetvbuf special mode flag= 0x%x\n",
            setup->cache_flags);
         fprintf(stderr, "\tseed= 0x%x\n\n",
            (int)setup->seed);
      }
      if(pass >= 0 && pass < setup->pass_max) {
         fprintf(stderr, "\n******************************\n");
         fprintf(stderr, "******  pass = %i  ******\n",
            pass);
         fprintf(stderr, "********************************\n\n");
      }

      tstLoopGoal(setup, pass);

      if(pass < 0) {
         fprintf(stderr, "tstLoopPass: %i passes * %i trials = elapsed time = %6.2f hours\n\n",
            (int)setup->pass_max, (int)setup->loop,
            (double)(setup->pass_max * setup->elapsed / 3600.));
      }
   }

   tstSummary(setup);

   return;
}
