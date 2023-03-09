static char rcsid[]="$Id";
/*
RCS-t=GIO, hello world test

*/

#include <stdio.h>

int main(int argc, const char** argv)
{
   GIO_BIG base, align, bias, skip, buf;

   printf("hello world\n");
   for(base = 1024; base < 1088; base++) {
      for(align = 1; align <= 8192; align *= 2) {
         for(bias = 0; bias <= 16; bias += 1) {
            if(! ((base + bias) % align)) continue;
            skip = align - ((base + bias) & (align - 1));
            buf = base + skip;
            printf("base= %6i, align= %6i, bias= %6i, skip= %6i, buf= %6i\n",
               (int)base, (int)align, (int)bias, (int)skip, (int)buf);
/***
***/
            if((buf + bias) % align)
               printf(" *** bad ***\n");
         }
      }
   }
}
