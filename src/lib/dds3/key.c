static char rcsid[]="$Id: key.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, search remaining value of scan definition for keywords

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define NOM_TOKEN_MAX (2 * DDS_KB)

int ddsKey(
   const char *alias)
{
   int status = DDS_OK;
   const char *scan_next, *scan_value, *alias_value;
   static DDS_TOKEN scan_token = {{0, NULL}, NULL};
   static DDS_TOKEN alias_token = {{0, NULL}, NULL};

   /* START FROM THE CURRENT POSITION IN THE VALUE */

   /* skip white space in alias key */
   if(alias) while(isspace(*alias)) alias++;

   if(alias && *alias) {
      /* search for alias within remaining value */
      if(! scan_token.str.max)
         ddsStrSetMin(&scan_token.str, "", NOM_TOKEN_MAX);

      if(! alias_token.str.max)
         ddsStrSetMin(&alias_token.str, "", DDS_DEFN_NAME_MAX);

      scan_token = dds_scan_token;
      scan_next = dds_scan_token.next;

      /* check each token */
      for(scan_value = TOKEN_NEXT(scan_token);
         scan_value;
         scan_value = TOKEN_NEXT(scan_token)) {
         status++;

         /* check each alias */
         for(alias_value = TOKEN_INIT(alias_token, alias);
            alias_value;
            alias_value = TOKEN_NEXT(alias_token)) {
            if(! strcmp(scan_value, alias_value)) {
               /* KEYWORD FOUND */
               dds_scan_token.next = scan_next;
               return status;
            }
         }
         scan_next = scan_token.next;
      }
      /* KEYWORD NOT FOUND */
      status = 0;
   } else {
      /* count tokens remaining within value */
      status = TOKEN_COUNT(dds_scan_token.next);
   }
   return status;
}
