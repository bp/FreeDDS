static char rcsid[]="$Id: hint.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, Cleanup dirty text for printing.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define HINT_MAX  256

const char * ddsHint(
   const char *text)
{
   static char hint[HINT_MAX+1];
   int n, test_char;
 
   for(n = 0; text[n] && n < HINT_MAX; n++) {
      test_char = (unsigned char)text[n];
      if(isprint(0377 & test_char))
         hint[n] = test_char;
      else
         hint[n] = '\?';
   }
   hint[n] = '\0';
 
   return (const char*)hint;
}

const char * ddsHint2(
   const char *text)
{
   static char hint[HINT_MAX+1];
   int n, test_char;
 
   for(n = 0; text[n] && n < HINT_MAX; n++) {
      test_char = (unsigned char)text[n];
      if(isprint(0377 & test_char))
         hint[n] = test_char;
      else
         hint[n] = '\?';
   }
   hint[n] = '\0';
 
   return (const char*)hint;
}

const char * ddsHint3(
   const char *text)
{
   static char hint[HINT_MAX+1];
   int n, test_char;
 
   for(n = 0; text[n] && n < HINT_MAX; n++) {
      test_char = (unsigned char)text[n];
      if(isprint(0377 & test_char))
         hint[n] = test_char;
      else
         hint[n] = '\?';
   }
   hint[n] = '\0';
 
   return (const char*)hint;
}

const char * ddsHint4(
   const char *text)
{
   static char hint[HINT_MAX+1];
   int n, test_char;
 
   for(n = 0; text[n] && n < HINT_MAX; n++) {
      test_char = (unsigned char)text[n];
      if(isprint(0377 & test_char))
         hint[n] = test_char;
      else
         hint[n] = '\?';
   }
   hint[n] = '\0';
 
   return (const char*)hint;
}

const char * ddsHintN(
   const char *text,
   size_t size)
{
   static char hint[HINT_MAX+1];
   int n, test_char;
 
   for(n = 0; n < size && n < HINT_MAX; n++) {
      test_char = (unsigned char)text[n];
      if(isprint(0377 & test_char))
         hint[n] = test_char;
      else
         hint[n] = '\?';
   }
   hint[n] = '\0';
 
   return (const char*)hint;
}

const char * ddsHintN2(
   const char *text,
   size_t size)
{
   static char hint[HINT_MAX+1];
   int n, test_char;
 
   for(n = 0; n < size && n < HINT_MAX; n++) {
      test_char = (unsigned char)text[n];
      if(isprint(0377 & test_char))
         hint[n] = test_char;
      else
         hint[n] = '\?';
   }
   hint[n] = '\0';
 
   return (const char*)hint;
}
