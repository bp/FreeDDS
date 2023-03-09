static char rcsid[]="$Id: c_index.c 81 2009-05-19 13:59:35Z user $";
/*
RCS-t=DDS API, return buffer index (zero based) to member

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 15 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_index(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   int prime)
{
   DDS_BIN *bin;
   DDS_MEMBER *member;
   int  prec, ptype;
#if defined(_WIN32) || defined(MINGW)
   size_t *bias;
#else
   long	*bias;
#endif

   DDS_API_ENTRY("index");

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD))

   SYM_TAG_TO_MEMBER(member, bin, sym_tag, DDS_API_EXIT(DDS_BAD))

   SYM_TAG_TO_BIAS(bias, bin, sym_tag, DDS_API_EXIT(DDS_BAD))

   VALID_PRIME(prime, DDS_API_EXIT(DDS_BAD))

   if (*bias < 0) {
      ddsDebug("member offset unset, tag= %i (%s), type= %s, prime= %s\n\tbinary= %s\n",
         (int)sym_tag, member->name ? member->name : "<anonymous>",
         ddsHintType(bin, member->type), dds_prime[prime].name,
         bin->name);
      DDS_API_EXIT(DDS_BAD);
   }

   ptype = member->type->ptype;
   if(dds_prime[ptype].stype != dds_prime[prime].stype) {
      ddsDebug("incompatible member tag= %i (%s), type= %s, prime= %s ptype= %s\n\tbinary= %s\n",
         (int)sym_tag, member->name ? member->name : "<anonymous>",
         ddsHintType(bin, member->type), dds_prime[prime].name,
         dds_prime[ptype].name, bin->name);
      DDS_API_EXIT(DDS_BAD);
   }

   prec = member->type->prec;
   if (! prec || *bias % prec) {
      ddsDebug("alignment of member tag= %i (%s), type= %s, prime= %s\n\tbinary= %s\n",
         (int)sym_tag, member->name ? member->name : "<anonymous>",
         ddsHintType(bin, member->type), dds_prime[prime].name,
         bin->name);
      DDS_API_EXIT(DDS_BAD);
   }

   DDS_API_EXIT( *bias / prec );
}
