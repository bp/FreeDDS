static char rcsid[]="$Id: ctest2.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS main, test parameter dictionary functionality.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/
 
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
 
#include "cdds.h"

void foobar(const char *name, const char *note)
{
   /* retrieve and validate definitions */
   int i, j, n0, n1, n2, n3, n4, n5, nzero, neof;
   float f;
   char s1[16], s2[16], s3[16], s4[16];
   const char *value;

   i = j = n0 = n1 = n2 = n3 = n4 = n5 = nzero = neof = -123;
   f = -123.456;
   strcpy(s1, "default");
   strcpy(s2, "default");
   strcpy(s3, "default");
   strcpy(s4, "default");
   value = "default";

   fprintf(stderr, "Testing dictionary= %s (%s)\n", name, note);
 
   j = cdds_scanf("foo", "%8s %i %f %8s %8s %8s",
      s1, &i, &f, s2, s3, s4);
   if(5 != j ||
      strcmp(s1, "aaa") ||
      11 != i ||
      22.21 > f ||
      22.23 < f ||
      strcmp(s2, "bbb") ||
      strcmp(s3, "ccc")) {
      fprintf(stderr, "\tFail:scanf combo, j= %i, s1= %s, i= %i, f= %f, s2= %s, s3= %s, s4= %s\n",
         j, s1, i, f, s2, s3, s4);
   }

   j = cdds_scanf("bar", "");
   if(0 != j)
      fprintf(stderr, "\tFail:scanf select, j= %i\n", j);
 
   neof = cdds_scanf("", "%i", &i);
   j = cdds_scanf("", "%s", s1);
   if(EOF != neof || 1 != j || strcmp(s1, "ddd"))
      fprintf(stderr, "\tFail:scanf string, neof= %i, j= %i, s1= %s\n",
      neof, j, s1);
 
   neof = cdds_scanf("", "  %[a-zA-Z]", s1);
   j = cdds_scanf("", "%f", &f);
   if(EOF != neof || 1 != j || 33.32 > f || 33.34 < f)
      fprintf(stderr, "\tFail:scanf float, neof= %i, j= %i, f= %f\n",
         neof, j, f);
 
   neof = cdds_scanf("", "  %[a-zA-Z]", s1);
   j = cdds_scanf("", "%i", &i);
   if(EOF != neof || 1 != j || 44 != i)
      fprintf(stderr, "\tFail:scanf int, neof= %i, j= %i, i= %i\n",
         neof, j, i);
 
   j = cdds_scanf("bar", "  %[a-z]  %[0-9.]  %[0-9]", s1, s2, s3);
   if(3 != j ||
      strcmp(s1, "ddd") ||
      strcmp(s2, "33.330000") ||
      strcmp(s3,"44"))
      fprintf(stderr, "\tFail:scanf sets, j= %i, s1= %s, s2= %s, s3= %s\n",
         j, s1, s2, s3);

   j = cdds_scanv("foo", &value);
   neof = cdds_scanv("", &value);
   if(26 != j || strcmp(value, " aaa 11 22.220000 bbb ccc\n") || EOF != neof)
      fprintf(stderr, "\tFail:scanf value, j= %i, value= %s\n, neof= %i",
         j, value, neof);

   i = cdds_scank("bar", "");
   j = cdds_scank("", "44");
   nzero = cdds_scank("", "ABSENT");
   if(4 != i || 3 != j || nzero)
      fprintf(stderr, "\tFail:scanf key count, i= %i, j= %i, nzero= %i\n",
         i, j, nzero);
 
   j = cdds_scanf("chain", "%8s %i %f %8s %8s %8s",
      s1, &i, &f, s2, s3, s4);
   if(5 != j ||
      strcmp(s1, "aaa") ||
      11 != i ||
      22.21 > f ||
      22.23 < f ||
      strcmp(s2, "bbb") ||
      strcmp(s3, "ccc")) {
      fprintf(stderr, "\tFail:scanf combo, j= %i, s1= %s, i= %i, f= %f, s2= %s, s3= %s, s4= %s\n",
         j, s1, i, f, s2, s3, s4);
   }

   j = cdds_scanf("$chain", "%8s", s1);
   if(1 != j || strcmp(s1, "foo")) {
      fprintf(stderr, "\tFail:$chain, j= %i, s1= %s\n", j, s1);
   }

   j = cdds_scanf("color", "");
   if(1 != (n0 = cdds_key("red")) ||
      3 != (n1 = cdds_key(NULL)) ||
      EOF != (neof = cdds_key("clear")) ||
      2 != (n2 = cdds_key("GREEN green Green")) ||
      5 != (n3 = cdds_scant("", &value)) ||
      strcmp(value, "green") ||
      3 != (n4 = cdds_key("blue"))) {
      fprintf(stderr, "\tFail: keys, j= %i, n0= %i, n1= %i, n2= %i, n3= %i, n4= %i, neof= %i, value= %s\n",
         j, n0, n1, n2, n3, n4, (int)neof, value);
   }

   j = cdds_scanf("UnDeFiNeD", "%c", s1);
   if(EOF != j) {
      fprintf(stderr, "\tFail: UnDeFiNeD, j= %i\n", j);
   }

   j = cdds_scanv("axis", &value);
   if(j != 11 || strcmp(value, "  t xx yyy\n"))
      fprintf(stderr, "\tFail: axis, j= %i, value= %s\n", j, value);
 
   j = cdds_scanf("size.axis(2)", "%i", &n1);
   if(j != 1 || 22 != n1)
      fprintf(stderr, "\tFail: axis, j= %i, n1= %i\n", j, n1);
 
   return;
}

int main(
   int argc,
   char **argv)
{
/*** ctest2H contents
   foo= xyz
   bar= pqr
   axis  =  t xx yyy
   foo= aaa 11 22.220000 bbb ccc
   size.xx=  22
   bar= ddd 33.330000 44
   eee
   $chain= foo
   color= red green blue
***/

   char s[9];
   const char *name, *in_dict, *out_dict;
 
   setargcv(argc, argv);
   cdds_init();
 
/***
   cdds_printf("error_abort=", " false\n");
***/

   cdds_dict("", "print");
   cdds_dict("", "scan");

   assert(EOF == cdds_printf("junk", "closed\n"));   /* put dict closed*/
   assert(EOF == cdds_scanf("junk", "%8s", s));   /* get dict closed*/

   in_dict = cdds_in("in", "stdin:", "ctest2");
   out_dict = cdds_out("out", "stdout:", in_dict);
 
   /* open dict, and append definitions */
   remove("ctest2H");
   remove("ctest2aH");
   name = "ctest2H";
   cdds_dict(name, "print");
 
   cdds_printf("foo", " xyz\n");
   cdds_printf("bar", " pqr\n");

   cdds_printf("axis  ", "  t xx yyy\n");
 
   cdds_printf("foo", " aaa %i %f %s ccc\n", 11, 22.22, "bbb");
   cdds_printv("size.axis(2)", "22\n");
   cdds_printf("bar", "");
   cdds_printf("", " %s %f %i\n", "ddd", 33.33, 44);
   cdds_printf("", "eee\n");
   cdds_printf("$chain", " foo\n");
   cdds_printf("color", " red green blue\n");
 
   cdds_dict(name, "scan");
   foobar(name, "original dict");
 
   name = "ctest2aH";
   cdds_dict(name, "print");
   cdds_history();
   cdds_dict(name, "scan");
   foobar(name, "copy history");
   
   cdds_dict(name, "print");
   cdds_dict("par:", "scan");
   cdds_history();
   cdds_dict("env: ctest2H cmd:", "scan");
   cdds_copy("USER");
   cdds_copy("GaRbAgE");
   cdds_dict(name, "scan");
   foobar(name, "copy history + cmd: + misc");
   
/***
   name = "stdout:";
   cdds_dict(name, "print");
   cdds_dict("ctest2H cmd:", "scan");
   cdds_history();
   cdds_dict(name, "scan");
   foobar(name, "copy history + cmd:");
***/

   cdds_dict("cmd: ctest2H ctest2aH env: stdout:", "reset");

   name = "ctest2H";
   cdds_dict(name, "scan");
   foobar(name, "close & reopen");
 
   return 0;
}
