static char rcsid[]="$Id: tstsetup.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstSetup: print setup, initialize statistics, validate parameters.

*/

#include "tstsys.h"

int tstSetup(          /* status (non-zero == error). */
   TST_SETUP *setup,   /* setup for one particular trial. */
   long *size_limit,
   int *align_limit,
   int *bias_limit)
{
   time_t now_time;
   int i;
   char tmp_title[20], title[2000];
   char tmp_file[GIO_FILENAME_MAX+1];

   now_time = time(NULL);

   /************************************/
   /* report trial specific parameters */
   /************************************/

   strcpy(title, "   title= ");
   strcat(title, form_name[setup->form - TC_verify]);
   strcat(title, " ");
   strcat(title, pack_name[setup->pack - TC_stdio]);
   strcat(title, " ");
   strcat(title, mode_name[setup->mode- TC_nop]);
   strcat(title, " size");

   for(i = 0; i < setup->size_max2; i++) {
      sprintf(tmp_title, " %6i", setup->size_list2[i]);
      strcat(title, " ");
      strcat(title, tmp_title);
   }

   strcat(title, ", align");

   for(i = 0; i < setup->align_max2; i++) {
      sprintf(tmp_title, " %2i", setup->align_list2[i]);
      strcat(title, " ");
      strcat(title, tmp_title);
   }

   strcat(title, ", bias");

   for(i = 0; i < setup->bias_max2; i++) {
      sprintf(tmp_title, " %2i", setup->bias_list2[i]);
      strcat(title, " ");
      strcat(title, tmp_title);
   }

   strcat(title, ", block");
   sprintf(tmp_title, " %6i", setup->block);
   strcat(title, " ");
   strcat(title, tmp_title);
   strcat(title, "\n");

   fprintf(stderr, "tstSetup: trial count= %i, %s, %s",
      ++setup->trial_count, setup->hostname, ctime(&now_time));
   setup->title = title;
   fprintf(stderr, setup->title);

   /*******************************/
   /* initialize vital statistics */
   /*******************************/
   setup->loop = 0;

   setup->rc = 0;
   setup->error = 0;
   setup->short_read = 0;
   setup->elapsed= 1.0e+4;
   setup->cpu = 1.0e+4;

   setup->read_mb = 1.0e-6;
   setup->write_mb = 1.0e-6;

   setup->read = 0;
   setup->write = 0;
   setup->seek = 0;

   setup->read_bytes = 0;
   setup->write_bytes = 0;

/***
   setup->user_read = 0;
   setup->user_write = 0;
   setup->user_seek = 0;

   setup->system_read = 0;
   setup->system_write = 0;
   setup->system_seek = 0;

   setup->user_read_mb = 0.0;
   setup->user_write_mb = 0.0;
   setup->system_read_mb = 0.0;
   setup->system_write_mb = 0.0;
***/

   /*******************************/
   /* validate control parameters */
   /*******************************/

   /* cpu clock wrap around occurs near 2147 sec on sun */
   if(setup->time_limit > 2000) setup->rc = 133;

   *size_limit = 0;

   for(i = 0; i < setup->size_max2; i++) {
      if(setup->size_list2[i] <= 0) {
         setup->rc = 1;
      }

      if(*size_limit < setup->size_list2[i])
         *size_limit = setup->size_list2[i];
   }

   if(! *size_limit) setup->rc = 4;

   *align_limit = 0;

   for(i = 0; i < setup->align_max2; i++) {
      if(setup->align_list2[i] < 0) setup->rc = 2;
      if(*align_limit < setup->align_list2[i])
         *align_limit = setup->align_list2[i];
   }

   *bias_limit = 0;

   for(i = 0; i < setup->bias_max2; i++) {
      if(setup->bias_list2[i] < 0) setup->rc = 3;
      if(*bias_limit < setup->bias_list2[i])
         *bias_limit = setup->bias_list2[i];
   }

   if(setup->mode < TC_nop ||
      setup->mode > TC_create_forward) setup->rc = 7;

   if(setup->pack < TC_stdio ||
      setup->pack > TC_raw_cpy) setup->rc = 8;

   if(setup->form < TC_verify ||
      setup->form > TC_speed) setup->rc = 9;

   if(setup->file_limit < *size_limit + *align_limit + *bias_limit)
      setup->rc = 10;

   #ifdef TST_USP
      if(setup->pack == TC_usp && *size_limit < SZDTHD) {
         setup->rc = 11;
      }
   #endif /* TST_USP */

   if(setup->rc) {
      fprintf(stderr, "tstSetup: rc= %i, fatal setup errors\n\t%s\n",
         setup->rc, setup->title);
      return setup->rc;
   }

   srand(setup->seed);

   return 0;
}
