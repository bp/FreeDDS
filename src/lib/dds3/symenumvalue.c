static char rcsid[]="$Id: symenumvalue.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create explicit value for enumerated DDS_OBJECT in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <limits.h>
#include "dds_sys.h"

DDS_OBJECT * ddsSymEnumValue(
   DDS_OBJECT *object,
   DDS_VALUE *value)
{
   void *value_ptr;
   DDS_TYPE *type;
   DDS_CON_STATUS status;
#if defined(_WIN32) || defined(MINGW)
   long long enum_value, min_value, max_value;
#else
   long enum_value, min_value, max_value;
#endif

   enum_value = ddsSymConstLong(value, 0, DDS_SYM_CONST_DECL);

   /* lex context, want IDENTIFIER_TOKEN (not existing object). */
   /* This is only needed because the parser has "enumerator_list_FIX" */
   /* which handles dangling commas, i.e. enum {a, b,} xxx; */
   /* The parser must look ahead one symbol and misses the */
   /* "dds_declare->flag |= DDS_SYM_IDENT" on "enumerator_list:" */
   dds_declare->flag |= DDS_SYM_IDENT;

   type = dds_declare->type;

   value_ptr = ddsTablePush(dds_symbol, type->prec, type->align);
   object->ptr = value_ptr;
   object->track = value;

#if defined(_WIN32) || defined(MINGW)
   max_value = ((~ (HOST_ULLONG_T)0) >>
      (1 + CHAR_BIT * (HOST_LLONG_P - type->prec)));
   min_value = - max_value - 1;

   status = CON_AASS(type->ptype, HOST_LLONG_DDS,
      value_ptr, &enum_value);
#else
   max_value = ((~ (HOST_ULONG_T)0) >>
      (1 + CHAR_BIT * (HOST_LONG_P - type->prec)));
   min_value = - max_value - 1;

   status = CON_AASS(type->ptype, HOST_LONG_DDS,
      value_ptr, &enum_value);
#endif

   if(enum_value > max_value)
      ddsYYWarn("integral constant overflow, enum type '%s'\n",
         dds_prime[type->ptype].name);
   else if(enum_value < min_value)
      ddsYYWarn("integral constant underflow, enum type '%s'\n",
         dds_prime[type->ptype].name);
   else if(status != DDS_CON_OK)
      ddsYYWarn("constant conversion error to enum type '%s'\n",
         dds_prime[type->ptype].name);

   return object;
}
