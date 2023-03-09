static char rcsid[]="$Id: testasp.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=testasp main, create test dataset
 
*/
 
/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/
 
#include <stdlib.h>
#include <stdio.h>

#include "cdds.h"

int main(argc, argv)
   int argc;
   char **argv;
{
   const char *in, *out;
   BIN_TAG out_bin;
   SYM_TAG samples_tag;
   int n, i, size, size1, total;
   const char *recipe;
   char *buf;
   float *samples;

   /* help synopsis */
   setargcv(argc, argv);
   cdds_dict("cmd:", "scan");
   if(0 < cdds_scank("cmd_args", DDS_HELP_KEY)) {
      fprintf(stderr, "bridge: help, command synopsis\n"
"\tCreate test data\n"
"\t{help}                       <this> command synopsis\n"
"\t{in= in_dict}                input dictionary (stdin:)\n"
"\t{asp= fmt.dds}               format recipe (testasp.fmt.dds)\n"
"\t{out= out_dict}              output dictionary (stdout:)\n"
"\t{out_format= out_fmt}        output format (same as input)\n"
"\t{out_data= out_data}         output data ($DATA_PATH/out_dict-)\n"
"\t{par= dict}                  input parameter dictionary\n"
"\n");
      exit(1);
   }

   /* standard initialization */
   in = cdds_in("in", "stdin:", "testasp, create test data");

   recipe = "testasp.fmt.dds";
   cdds_scant("asp", &recipe);
   cdds_dict(recipe, "scan");
   cdds_dict("override:", "print");
   cdds_history();

   out = cdds_out("out", "stdout:", in);
   cdds_printf("old_format", " asp\n");
/***
   cdds_printf("write:usp.TrcNum", " 1 + ordinal(2)\n");
   cdds_printf("write:usp.RecNum", " 1 + ordinal(3)\n");
   cdds_printf("write:segy.CdpTrcNum", " 1 + ordinal(2)\n");
   cdds_printf("write:segy.Spare.Hdr.RecNum", " 1 + ordinal(3)\n");
***/

   /* open binary output */
   out_bin = cdds_open(out, "out_format", "out_data", "w");

   size = cdds_prec(out_bin, 0);
   buf = (char*)cdds_malloc(size);
   samples_tag = cdds_member(out_bin, 0, "Samples");
   samples = (float*)buf + cdds_index(out_bin, samples_tag, DDS_FLOAT);
   total = cdds_axis_prod(2);
   size1 = 0;
   cdds_scanf("size.axis(1)", "%i", &size1);

   /* process data (create, write) */
   for(n = 0; n < total; n++) {
      /* fill entire buffer with short int pattern */
      for(i = 0; i < size; i += sizeof(short)) {
         *(short*)(buf + i) = n * 100 + i;
      }
      /* fill Samples with float pattern */
      for(i = 0; i < size1; i++) {
         samples[i] = n * 100 + i;
      }
      cdds_write(out_bin, 0, buf, 1);
   }

   /* close dictionaries and binary streams */
   cdds_close(out_bin);
   cdds_dict(out, "close");
   cdds_dict(in, "close");

   return 0;
}
