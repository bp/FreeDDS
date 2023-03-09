static char rcsid[]="$Id: ieeetest.c 44 2009-01-20 17:36:39Z user $";
/*
RCS-t=DDS main, test ieeeibm function.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <cdds.h>
#include <dds_sys.h>


/* convert 4 byte single precision float in ibm format
  to 4 byte single precision ieee floating point */

void ibmieee(char *a, char *b)
{
   int exp, sign, frac;
   int ibmexp, ibmfrac;

   sign = a[0] & 0200;
   ibmexp = a[0] & 0177;
   ibmfrac = ((a[1]&0377)<<16) + ((a[2]&0377)<<8) + (a[3]&0377);
   exp = (ibmexp<<2) - 130;
   while(ibmfrac<8388608 && ibmfrac != 0) {
      ibmfrac+=ibmfrac;
      exp--;
   }
   if (ibmfrac == 0) {
      frac = 0;
      exp = 0;
   } else {
      frac = ibmfrac - (01<<23);
   }
   b[0] = ((char)sign & 0200) + (((char)exp&0376)>>1);
   b[1] = (((char)exp&01)<<7) + ((char)(frac>>16)&0177);
   b[2] = ((char)(frac>>8)&0377);
   b[3] = ((char)frac     &0377);
}


/* convert 4 byte ieee single precision float in a
   to a 4 byte ibm single precision into b

   IEEE

   SEEEEEEE EFFFFFFF FFFFFFFF FFFFFFFF

	fpn = (-1)**s 2^(E-127) (1 + F/2^23)

   IBM

   SEEEEEEE FFFFFFFF FFFFFFFF FFFFFFFF

	fpn = (-1)**s 16^(E-64) F/16^6

  */
void ieeeibm(char *a, char *b)	
{
   int exp, sign;
   int frac;
   int ibmexp,ibmfrac;

   frac = ((a[1] & 0177)<<16) +  ((a[2] & 0377)<<8) + (a[3]&0377);
   sign = a[0] & 0200;
   exp = (((int)a[0]&0177)<<1) + (((int)a[1]&0200)>>7);
   if (exp == 0) {
      ibmexp = 64;
      ibmfrac = 0;
   } else {
      ibmexp = (exp+130)/4 + 1;
      ibmfrac = ((01<<23) + frac)/(01 << (4 - (exp+130)%4));
   }
   b[0] = (((char)sign & 0200 ) + ((char)ibmexp & 0177));
   b[1] = (ibmfrac>>16)&0377;
   b[2] = (ibmfrac>> 8)&0377;
   b[3] = (ibmfrac    )&0377;
}


void F_SISR4(float *from, float *to)	/* return the next four bytes as a float */
{
   ibmieee((char*)from, (char*)to);
}


void F_R4SIS(float *from, float *to)
{
   ieeeibm((char*)from, (char*)to);
}


int main(int argc, char **argv)
{
   int i;
   float x, y, z, zibm, z1, z2;

   for(i = 0; i < 50; i++) {
      x = rand(); y = rand();
      if(0. == y) continue;
      if(0 == i)
         z = 0;
      else if(1 == i)
         z = 1;
      else
         z = x / y;

/*** /home/gpsc/usp/src/lib/ut/fcsub.c ***/
      F_R4SIS(&z, &zibm);
      F_SISR4(&zibm, &z1);

/*** my version ***/
      ddsIEEEIBM((DDS_IEEE4_OUT*)&zibm, (DDS_IBM4_IN*)&z, 1);
      ddsIBMIEEE((DDS_IBM4_OUT*)&z2, (DDS_IEEE4_IN*)&zibm, 1);

      printf(" z= %f (%x), z1= %f (%x), z2= %f (%x)\n", 
         z, *(int*)(&z), z1, *(int*)(&z1), z2, *(int*)(&z2));
   }
   return 0;
}

