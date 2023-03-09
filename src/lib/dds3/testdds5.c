static char rcsid[]="$Id: testdds5.c 16 2009-01-06 21:52:47Z user $";
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
   const char *in_dict, *buf_dict, *out_dict;
   BIN_TAG in_bin, buf_bin, out_bin;
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
/***
***/
 
   in_dict = cdds_in("in", "stdin:", "testdds5");
   in_bin = cdds_open(in_dict, "in_format", "in_data", "r");

   buf_dict = cdds_out("", "", in_dict);
   cdds_printf("alias:old_format", " format\n");
   cdds_printf("buf_format", " usp\n");
   buf_bin = cdds_open(buf_dict, "buf_format", "", "m");

   out_dict = cdds_out("out", "stdout:", buf_dict);
   out_bin = cdds_open(out_dict, "out_format", "out_data", "w");

   cdds_openm(buf_bin, 0, in_bin, 0);
   cdds_openm(out_bin, 0, buf_bin, 0);

   while(1 == cdds_readm(in_bin, 0, buf_bin, 0, buf, 1)) {
      rc = cdds_writem(out_bin, 0, buf_bin, 0, buf, 1);
   }

   cdds_close(in_bin);
   cdds_close(buf_bin);
   cdds_close(out_bin);

   return 0;
}
