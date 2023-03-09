
/*
RCS-t=DDS, parse or count tokens, within token list

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define NOM_STRING_MAX 2 * DDS_KB

int ddsToken(
   DDS_TOKEN *save,
   const char *list)
{
   /* Tokens are a character sequence, delimited by white space,
      and double quotes.  Within quoted tokens, special characters
      ('"', '=', '\') must be escaped with '\'.

      if(save && list) then...
         FIND THE FIRST TOKEN IN "list"
         1) initialize processing via "list"
         2) search for "next" (first) token
         3) if("next" token found) then...
            a) build "save" string from "next" token
            b) "save" "next" (for continuation)
            c) return length of "save" string.
         4) if(not found) then...
            a) "save" "list" as "next" (for continuation)
            b) return EOF
      if(save && ! list) then...
         FIND THE NEXT TOKEN VIA "save"
         1) continue processing via "save" "next"
         2) search for "next" token
         3) if("next" token found) then...
            a) build "save" string from "next" token
            b) "save" "next" (for continuation)
            c) return length of "save" string.
         4) if(not found) then...
            a) don't change "save" "next" (for continuation)
            b) return EOF
      if(! save && list) then...
         COUNT REMAINING TOKENS IN "list"
         1) initialize processing via "list"
         2) search for "next" token
         3) if("next" token found) then...
            a) increment token "count"
            b) goto (2)
         4) if(not found) then...
            a) return token "count"
      if(! save && ! list) then...
         return EOF
   */
   int count = 0;
   const char *msg, *start, *next;
   DDS_BOOLEAN escape = DDS_FALSE;

   if(save && list) {
      /* FIND THE FIRST TOKEN IN "list" */
      next = list;
      save->next = list;
   } else if(save && ! list) {
      /* FIND THE NEXT TOKEN VIA "save" */
      next = save->next;
      if (!next) return EOF;
   } else if(! save && list) {
      /* COUNT REMAINING TOKENS IN "list" */
      next = list;
   } else {
      return EOF;
   }

   msg = next; /* verbose message */

   if(save) {
      /* initialize DDS_STRING */
      if(! save->str.max) ddsStrSetMin(&save->str, "", NOM_STRING_MAX);
      save->str.ptr[0] = '\0';
   }

   count_loop: /* top of loop, when counting tokens */

   /* skip leading white space */
   while(*next && isspace(*next)) next++;

   if(! *next) {
      /* can't find another token */
      if(! save) return count;
      save->next = next;
      return EOF;
   }

   /* token found */
   count++;

   if(*next == '"') {
      /* quoted token, initial quote */
      start = ++next;
      while(*next) {
         /* process next character within quoted token */
         if(escape) {
            /* one character escaped */
            escape = DDS_FALSE;
            start = next++;
         } else if(*next == '"') {
            /* quoted token, final quote */
            if(! save) {
               /* count only, don't save state */
               next++;
               if(*next && ! isspace(*next)) {
                  ddsVerbose("token: count syntax error (%s)[%i]\n",
                     ddsHint(msg), (int)(next - msg));
                  return EOF;
               }
               /* token count loop */
               goto count_loop;
            }
            /* save string, state, and return status */
            if(next > start)
               ddsStrCatN(&save->str, start, (next - start));
            save->next = ++next;
            if(*next && ! isspace(*next)) {
               ddsVerbose("token: parse syntax error (%s)[%i]\n",
                  ddsHint(msg), (int)(next - msg));
               count = EOF;
            }
            return strlen(save->str.ptr);
         } else if(*next == '\\') {
            /* escape sequence */
            escape = DDS_TRUE;
            if(save) {
               /* save string */
               if(next > start)
                  ddsStrCatN(&save->str, start, (next - start));
            }
            ++next;
         } else {
            ++next;
         }
      }
      if(save) {
         save->next = next;
         ddsVerbose("token: parse syntax error (%s)[%i]\n",
            ddsHint(msg), (int)(next - msg));
      } else {
         ddsVerbose("token: count syntax error (%s)[%i]\n",
            ddsHint(msg), (int)(next - msg));
      }
      return EOF;
   } else {
      /* non-quoted token */
      start = next;
      while(*next && ! isspace(*next)) next++;
      if(save) {
         ddsStrCpyN(&save->str, start, (next - start));
         save->next = next;
         return next - start;
      }
      /*** bug ? next++; ***/
   }

   /* token count loop */
   goto count_loop;
}
