static char rcsid[]="$Id: testptr.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS main, test pointer schemes

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <stdio.h>

#define MAX 9000

#define TEST(PTR, TYPE, BUF) \
   PTR = (TYPE*)(BUF); \
   lvalue = (long)PTR; \
   testptr2(#PTR "= (" #TYPE "*)(" #BUF ")", PTR, PTR, PTR, PTR, lvalue);

int main(
   const int argc,
   char **argv)
{
   char cbuf[MAX], *cptr;
   int ibuf[MAX], *iptr;
   float fbuf[MAX], *fptr;
   void *vptr;
   long lvalue;

   TEST(cptr, char, cbuf)
   TEST(cptr, char, cbuf + 1)
   TEST(vptr, void, cbuf)
   TEST(vptr, void, cbuf + 1)
   TEST(iptr, int, cbuf)
   TEST(iptr, int, cbuf + 1)

   return 0;
}
