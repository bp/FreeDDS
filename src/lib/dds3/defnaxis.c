static char rcsid[]="$Id: defnaxis.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, test for and convert "xxx.axis(1)" to "xxx.???"
(deprecate this feature asap)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

const char * ddsDefnAxis(
   const char *alias,
   const DDS_DICT *dict)
{
   static DDS_STRING new_alias = NULL_STRING;
   static DDS_TOKEN token = NULL_TOKEN;
   const DDS_DEFN *newer_defn, *axis_defn;
   size_t save_size, save_bias;
   DDS_ALIAS *save_list;
   int need;
   const char *start, *test, *value;
   char value_save;

   ddsStrCpy(&new_alias, "");
   for(start = test = alias; *test; test++) {
      if(! strncmp(test, ".axis(", 6) &&
         (test[6] - '0') > 0 && (test[6] - '0') <= 9 && test[7] == ')') {
         /* axis substitution */
         ddsStrCatN(&new_alias, start, (test - start + 1));
         save_list = ddsAliasList(NULL, "axis");
         for( /* null */ ; dict && save_list; dict = dict->right) {
            axis_defn = ddsDefnFind(&save_size, &save_bias,
               &save_list, dict);
            if(axis_defn) {
               /* current axis definition */
               need = test[6] - '0';
               newer_defn = (DDS_DEFN*)axis_defn->newer;
               value_save = dict->buf.ptr[newer_defn->start];
               dict->buf.ptr[newer_defn->start] = '\0';
               token.next = strchr(dict->buf.ptr + axis_defn->start, '=') + 1;
               while(need--) value = TOKEN_NEXT(token);
               if(value) {
                  ddsStrCat(&new_alias, value);
               } else {
                  dds_error = EINVAL;
                  ddsDebug("defn alias, axis substitution error\n\talias= %s  (%s)\n",
                     alias, dict->buf.ptr + axis_defn->start);
               }
               dict->buf.ptr[newer_defn->start] = value_save;
               break;
            }
         }

         ddsAliasDone(save_list);

         if(! dict) {
            dds_error = EINVAL;
            ddsDebug("defn alias, axis substitution error\n\talias= %s  (axis not defined)\n",
               alias);
         }
         test += 7;
         start = test + 1;
      }
   }
   ddsStrCatN(&new_alias, start, (test - start + 1));
   return new_alias.ptr;
}
