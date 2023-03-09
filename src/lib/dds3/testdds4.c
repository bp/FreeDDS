static char rcsid[]="$Id: testdds4.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS main, test binary functionality.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int main(
   int argc,
   char **argv)
{
   const char *in_dict, *out_dict;
   BIN_TAG in_bin, out_bin;
   char buf[40000];
   int i, rc;

   setargcv(argc, argv);

   cdds_dict("par:", "print");
   cdds_printf("axis", " t x y\n");
   cdds_printf("axis.t", " +sparse\n");
   cdds_printf("axis.x", " +sparse\n");
   cdds_printf("axis.y", " +sparse\n");
   cdds_printf("modify.t", " -warn -force\n");
   cdds_printf("modify.x", " -warn -force\n");
   cdds_printf("modify.y", " -warn -force\n");
   cdds_printf("read_usp", " -pad_dead -drop_dead -drop_live\n");
   cdds_printf("write_usp", " -pad_dead -drop_dead -drop_live\n");
   cdds_printf("data", " classic.usp\n");
   cdds_printf("format", " usp\n");
   cdds_printf("out", " junk\n");
 
   in_dict = cdds_in("in", "stdin:", "testcomp4");
   in_bin = cdds_open(in_dict, "in_format", "in_data", "r");

   out_dict = cdds_out("out", "stdout:", in_dict);
   cdds_printf("alias:old_format", " format\n");
   out_bin = cdds_open(out_dict, "out_format", "out_data", "w");

/***
   while(1 == cdds_read(in_bin, 0, buf, 1))  ;
   if(1 == cdds_read(in_bin, 0, buf, 1)) {
      for(i = 0; i < 51200; i++) {
         rc = cdds_write(out_bin, 0, buf, 1);
      }
   }
***/
   while(1 == cdds_read(in_bin, 0, buf, 1)) {
      rc = cdds_write(out_bin, 0, buf, 1);
   }

   cdds_close(in_bin);
   cdds_close(out_bin);

   return 0;
}
