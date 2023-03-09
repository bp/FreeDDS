/***************************************/
/***   Copyright (C) 2004            ***/
/***   BP America Inc, Houston, TX   ***/
/***   Mike O'brien, March 2004      ***/
/***************************************/

#include "dds_sys.h"

DDS_STRING * ddsStrCpyF(
   DDS_STRING *str,
   const char *ftn_str)
{
   int ftn_size=0;
   char *ftn_tmp;
   char *ftn_str_new;
   int iloc;

   ftn_tmp = (char*)ftn_str;
   while ( *ftn_tmp ) {
      if ( *ftn_tmp == '\0' ) break;
      if ( *ftn_tmp == '\\' && *(ftn_tmp+1) == '0') break;
      if ( *ftn_tmp != '\\' ) ftn_size++;
      ftn_tmp++;
   }

   ftn_str_new=(char*)malloc(sizeof(char)*(ftn_size+1));

   ftn_tmp = (char*)ftn_str;
   for (iloc=0;iloc<ftn_size;iloc++) {
      if ( *ftn_tmp == '\\' ) {
         ftn_tmp++;
         /* will this gag on f77 '\\' strings ? */
         /* note that \ooo and \xhh have been ignored for now. */
         if ( *ftn_tmp == 'n'  ) ftn_str_new[iloc] = '\n';
         else if ( *ftn_tmp == 't'  ) ftn_str_new[iloc] = '\t';
         else if ( *ftn_tmp == 'v'  ) ftn_str_new[iloc] = '\v';
         else if ( *ftn_tmp == 'b'  ) ftn_str_new[iloc] = '\b';
         else if ( *ftn_tmp == 'r'  ) ftn_str_new[iloc] = '\r';
         else if ( *ftn_tmp == 'f'  ) ftn_str_new[iloc] = '\f';
         else if ( *ftn_tmp == 'a'  ) ftn_str_new[iloc] = '\a';
         else if ( *ftn_tmp == '\\' ) ftn_str_new[iloc] = '\\';
         else if ( *ftn_tmp == '?'  ) ftn_str_new[iloc] = '\?';
         else if ( *ftn_tmp == '\'' ) ftn_str_new[iloc] = '\'';
         else if ( *ftn_tmp == '\"' ) ftn_str_new[iloc] = '\"';
         else if ( *ftn_tmp == '0'  ) ftn_str_new[iloc] = '\0';
         else {
            ftn_str_new[iloc] = '\\';
            iloc++;
            if (iloc < ftn_size) ftn_str_new[iloc] = *ftn_tmp;
         }
         ftn_tmp++;
      } else {
         memcpy(&ftn_str_new[iloc],ftn_tmp,1);
         ftn_tmp++;
      }
   }
   ftn_str_new[ftn_size]='\0';

   ddsStrCpy(str,ftn_str_new);
   free(ftn_str_new);

   return str;
}
