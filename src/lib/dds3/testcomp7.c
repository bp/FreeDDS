static char rcsid[]="$Id: testcomp7.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS main, test map functionality.

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
   char r_buf[4000], m_buf[4000], w_buf[4000];
   int rc;

   setargcv(argc, argv);

   cdds_dict("par:", "print");
   cdds_printf("data", " classic.usp\n");
   cdds_printf("format", " usp\n");
   cdds_printf("out", " junk\n");
 
   in_dict = cdds_in("in", "stdin:", "testcomp3");
   in_bin = cdds_open(in_dict, "in_format", "in_data", "r");

   buf_dict = cdds_out("", "", in_dict);
   cdds_printf("alias:old_format", " format\n");
   cdds_printf("buf_format", "usp -green_prefix\n");
   buf_bin = cdds_open(buf_dict, "buf_format", "", "m");

   out_dict = cdds_out("out", "stdout:", buf_dict);
   out_bin = cdds_open(out_dict, "out_format", "out_data", "w");

ddsDumpBinary();

   cdds_openm(buf_bin, 0, in_bin, 0);
   cdds_openm(out_bin, 0, buf_bin, 0);
/***
   cdds_openm(out_bin, 0, in_bin, 0);
***/

   rc = cdds_read(in_bin, 0, r_buf, 1);

   cdds_map(buf_bin, 0, m_buf,  in_bin, 0, r_buf);
   cdds_map(out_bin, 0, w_buf, buf_bin, 0, m_buf);
/***
   cdds_map(out_bin, 0, w_buf,  in_bin, 0, r_buf);
***/

   rc = cdds_write(out_bin, 0, w_buf, 1);

   cdds_close(in_bin);
   cdds_close(out_bin);

   return 0;
}
