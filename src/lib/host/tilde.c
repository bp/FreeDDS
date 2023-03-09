static char rcsid[]="$Id: tilde.c 50 2009-02-10 21:29:01Z user $";
/*
RCS-t=HOST, main, create "..._tilde" conversion tables for ASCII and EBCDIC

*/

#include <stdio.h>

#include "ascii_ebcdic.h"

/*! MAIN Program: generate tilde conversion tables for ASCII and EBCIDC */
int main(int argc, char **argv)
{
   /* create "..._tilde" conversion tables for ASCII and EBCDIC */
   int i;

   printf("\n/* ASCII table (garbage in, '~' out, rules) */\n");
   printf("char ASCII_tilde[256] = {\n");
   for(i = 0; i < 256; i++) {
      if(isprint(i))
         printf(" '\\%-.3o'", i);
      else
         printf("    '~'");
      if(i < 255) printf(",");
      if(0 == (i + 1) % 8) printf("\n");
   }
   printf("};\n");

   printf("\n/* EBCDIC table (garbage in, '~' out, rules) */\n");
   printf("char EBCDIC_tilde[256] = {\n");
   for(i = 0; i < 256; i++) {
      if(isprint((unsigned char)toASCII_table[i]))
         printf(" '\\%-.3o'", i);
      else
         printf("    '~'");
      if(i < 255) printf(",");
      if(0 == (i + 1) % 8) printf("\n");
   }
   printf("};\n");

   printf("\n/* ASCII to EBCDIC table (garbage in, '~' out, rules) */\n");
   printf("char toEBCDIC_tilde[256] = {\n");
   for(i = 0; i < 256; i++) {
      if(isprint(i))
         printf(" '\\%-.3o'", (unsigned char)toEBCDIC_table[i]);
      else
         printf("    '~'");
      if(i < 255) printf(",");
      if(0 == (i + 1) % 8) printf("\n");
   }
   printf("};\n");

   printf("\n/* EBCDIC to ASCII table (garbage in, '~' out, rules) */\n");
   printf("char toASCII_tilde[256] = {\n");
   for(i = 0; i < 256; i++) {
      if(isprint((unsigned char)toASCII_table[i]))
         printf(" '\\%-.3o'", (unsigned char)toASCII_table[i]);
      else
         printf("    '~'");
      if(i < 255) printf(",");
      if(0 == (i + 1) % 8) printf("\n");
   }
   printf("};\n");

   return 0;
}
