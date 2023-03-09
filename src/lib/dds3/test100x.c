static char rcsid[]="$Id: test100x.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS main, create test usp data

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
   BIN_TAG out_bin;
   int *i4, buf[4000];
   short *i2;
   float *f4;
   int j, k, m, rc;

   setargcv(argc, argv);

   in_dict = cdds_in("", "", "test100");

   out_dict = cdds_out("out", "test100", in_dict);

   cdds_printf("out_format", " usp\n");
   cdds_printf("axis", " t x y\n");
   cdds_printf("size.t", " 1000\n");
   cdds_printf("size.x", " 50\n");
   cdds_printf("size.y", " 2\n");
   cdds_printf("units.t", " msec\n");
   cdds_printf("units.x", " feet\n");
   cdds_printf("units.y", " feet\n");

   out_bin = cdds_open(out_dict, "out_format", "test100.usp", "w");

   i2 = (short*)(buf + 1);
   i4 = (int*)(buf + 1);
   f4 = (float*)(buf + 65);
   buf[0] = 0;
   for(j = 0; j < 2; j++) {
      for(k = 0; k < 50; k++) {
         for(m = 0; m < 128; m++) {
            i2[m] = 1000 * k + 2 * m + 4;
         }
         for(m = 0; m < 1000; m++) {
            f4[m] = 1000 * k + m + 4;
         }

         i4[1] = 1000 * k + 4 + 4;

         i4[11] = 1000 * k + 44 + 4;
         i4[12] = 1000 * k + 48 + 4;
         i4[13] = 1000 * k + 52 + 4;
         i4[14] = 1000 * k + 56 + 4;
         i4[15] = 1000 * k + 60 + 4;
         i4[16] = 1000 * k + 64 + 4;
         i4[17] = 1000 * k + 68 + 4;
         i4[18] = 1000 * k + 72 + 4;
         i4[19] = 1000 * k + 76 + 4;

         i4[50] = 1000 * k + 200 + 4;
         i4[51] = 1000 * k + 204 + 4;

         i4[56] = 1000 * k + 224 + 4;
         i4[57] = 1000 * k + 228 + 4;

         rc = cdds_write(out_bin, 0, buf, 1);
         if(rc != 1) break;
      }
   }

   cdds_close(out_bin);

   return 0;
}
