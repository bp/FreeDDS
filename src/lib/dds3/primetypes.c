static char rcsid[]="$Id: primetypes.c 81 2009-05-19 13:59:35Z user $";
/*
RCS-t=DDS, initialize table of prime data types

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

/******************************************************************/
/* Note: this function is called by cdds_init *and* main makecon2 */
/******************************************************************/

/* kludge, needed by typedef DDS_PRIME and #include sequence */
typedef struct {int junk;} DDS_TYPE;

#include "cdds.h"
#include "dds_ptype.h"

extern DDS_PRIME *dds_prime;

#define PRIME_PTYPE(p_name, p_type, p_genus, p_prec, p_align, p_dump) \
   dds_prime[DDS_PTYPE_##p_type].name  = (p_name); \
   dds_prime[DDS_PTYPE_##p_type].macro  = #p_type; \
   dds_prime[DDS_PTYPE_##p_type].genus = DDS_TYPE_ ## p_genus; \
   /* dds_prime[DDS_PTYPE_##p_type].prec  = (p_prec); */ \
   /* dds_prime[DDS_PTYPE_##p_type].align = (p_align); */ \
   /* dds_prime[DDS_PTYPE_##p_type].dump = (p_dump); */ \
   dds_prime[DDS_PTYPE_##p_type].stype = DDS_PTYPE_##p_type; \
   dds_prime[DDS_PTYPE_##p_type].type  = NULL;

#define PRIME_STYPE(s_name, s_type, s_genus, s_prec, s_align, s_dump) \
   dds_prime[DDS_STYPE_##s_type].name  = (s_name); \
   dds_prime[DDS_STYPE_##s_type].macro  = #s_type; \
   dds_prime[DDS_STYPE_##s_type].genus = DDS_TYPE_ ## s_genus; \
   dds_prime[DDS_STYPE_##s_type].prec  = (s_prec); \
   /* Need allignment on precision so cdds_index will work: Ehlers May 2009 */ \
   /* dds_prime[DDS_STYPE_##s_type].align = (s_align); */ \
   dds_prime[DDS_STYPE_##s_type].align = ((s_prec)>(s_align)) ? (s_prec) : (s_align); \
   dds_prime[DDS_STYPE_##s_type].dump = (s_dump); \
   dds_prime[DDS_STYPE_##s_type].stype = DDS_STYPE_##s_type; \
   dds_prime[DDS_STYPE_##s_type].type  = NULL;

#define PRIME_CTYPE(c_name, c_type, c_genus, c_dump) \
   dds_prime[DDS_CTYPE_##c_type].name  = (c_name); \
   dds_prime[DDS_CTYPE_##c_type].macro  = #c_type; \
   dds_prime[DDS_CTYPE_##c_type].genus = DDS_TYPE_ ## c_genus; \
   dds_prime[DDS_CTYPE_##c_type].prec  = HOST_ ## c_type ## _P; \
   /* Need allignment on precision so cdds_index will work: Ehlers May 2009 */ \
   /* dds_prime[DDS_CTYPE_##c_type].align = HOST_ ## c_type ## _A; */ \
   dds_prime[DDS_CTYPE_##c_type].align = ((HOST_ ## c_type ## _P)>(HOST_ ## c_type ## _A)) ? \
     (HOST_ ## c_type ## _P) : (HOST_ ## c_type ## _A); \
   dds_prime[DDS_CTYPE_##c_type].dump = (c_dump); \
   dds_prime[DDS_CTYPE_##c_type].stype = (DDS_PTYPE)HOST_##c_type ## _DDS; \
   dds_prime[DDS_CTYPE_##c_type].type  = NULL;

void ddsPrimeTypes(void)
{
   /* initialize table of prime data types */

   /********************************************/
   /* "C" language types on the host computer  */
   /********************************************/

   PRIME_CTYPE("void",               VOID,    VOIDS,      1)
   PRIME_CTYPE("*",                  PTR,     PTRS,       8)

   PRIME_CTYPE("char",               CHAR,    CHARS,     80)
   PRIME_CTYPE("signed char",        SCHAR,   INTEGERS,  16)
   PRIME_CTYPE("unsigned char",      UCHAR,   UNSIGNEDS, 16)

   PRIME_CTYPE("short",              SHORT,   INTEGERS,   8)
   PRIME_CTYPE("int",                INT,     INTEGERS,   8)
   PRIME_CTYPE("long",               LONG,    INTEGERS,   8)
   PRIME_CTYPE("long long",          LLONG,   INTEGERS,   8)

   PRIME_CTYPE("unsigned short",     USHORT,  UNSIGNEDS,  8)
   PRIME_CTYPE("unsigned",           UINT,    UNSIGNEDS,  8)
   PRIME_CTYPE("unsigned long",      ULONG,   UNSIGNEDS,  8)
   PRIME_CTYPE("unsigned long long", ULLONG,  UNSIGNEDS,  8)

   PRIME_CTYPE("float",              FLOAT,   FLOATS,     8)
   PRIME_CTYPE("double",             DOUBLE,  FLOATS,     8)
   PRIME_CTYPE("long double",        LDOUBLE, FLOATS,     8)

   PRIME_CTYPE("complex",            CPX,     COMPLEXS,   4)
   PRIME_CTYPE("complex double",     DBLCPX,  COMPLEXS,   4)
   PRIME_CTYPE("complex long",       LDBLCPX, COMPLEXS,   4)

   /*************************************************************/
   /* binary stream types, that provide portability among hosts */
   /*************************************************************/
   PRIME_STYPE("ascii1",         ASCII1,        CHARS,      1,  1, 80)
   PRIME_STYPE("ebcdic1",        EBCDIC1,       CHARS,      1,  1, 80)

   PRIME_STYPE("integer1",       INTEGER1,      INTEGERS,   1,  1, 16)
   PRIME_STYPE("integer2",       INTEGER2,      INTEGERS,   2,  2,  8)
   PRIME_STYPE("integer2x",      INTEGER2X,     INTEGERS,   2,  2,  8)
   PRIME_STYPE("integer4",       INTEGER4,      INTEGERS,   4,  4,  8)
   PRIME_STYPE("integer4x",      INTEGER4X,     INTEGERS,   4,  4,  8)
   PRIME_STYPE("integer8",       INTEGER8,      INTEGERS,   8,  8,  8)
   PRIME_STYPE("integer8x",      INTEGER8X,     INTEGERS,   8,  8,  8)

   PRIME_STYPE("unsigned1",      UNSIGNED1,     UNSIGNEDS,  1,  1, 16)
   PRIME_STYPE("unsigned2",      UNSIGNED2,     UNSIGNEDS,  2,  2,  8)
   PRIME_STYPE("unsigned2x",     UNSIGNED2X,    UNSIGNEDS,  2,  2,  8)
   PRIME_STYPE("unsigned4",      UNSIGNED4,     UNSIGNEDS,  4,  4,  8)
   PRIME_STYPE("unsigned4x",     UNSIGNED4X,    UNSIGNEDS,  4,  4,  8)
   PRIME_STYPE("unsigned8",      UNSIGNED8,     UNSIGNEDS,  8,  8,  8)
   PRIME_STYPE("unsigned8x",     UNSIGNED8X,    UNSIGNEDS,  8,  8,  8)

   PRIME_STYPE("float4",         FLOAT4,        FLOATS,     4,  4,  8)
   PRIME_STYPE("float4x",        FLOAT4X,       FLOATS,     4,  4,  8)
   PRIME_STYPE("float4ibm",      FLOAT4IBM,     FLOATS,     4,  4,  8)
   PRIME_STYPE("float8",         FLOAT8,        FLOATS,     8,  8,  8)
   PRIME_STYPE("float8x",        FLOAT8X,       FLOATS,     8,  8,  8)
   PRIME_STYPE("float8ibm",      FLOAT8IBM,     FLOATS,     8,  8,  8)
   PRIME_STYPE("float16",        FLOAT16,       FLOATS,    16, 16,  8)
   PRIME_STYPE("float16x",       FLOAT16X,      FLOATS,    16, 16,  8)

   PRIME_STYPE("complex8",       COMPLEX8,      COMPLEXS,   8,  8,  4)
   PRIME_STYPE("complex8x",      COMPLEX8X,     COMPLEXS,   8,  8,  4)
   PRIME_STYPE("complex8ibm",    COMPLEX8IBM,   COMPLEXS,   8,  8,  4)
   PRIME_STYPE("complex16",      COMPLEX16,     COMPLEXS,  16, 16,  4)
   PRIME_STYPE("complex16x",     COMPLEX16X,    COMPLEXS,  16, 16,  4)
   PRIME_STYPE("complex16ibm",   COMPLEX16IBM,  COMPLEXS,  16, 16,  4)
   PRIME_STYPE("complex32",      COMPLEX32,     COMPLEXS,  32, 32,  4)
   PRIME_STYPE("complex32x",     COMPLEX32X,    COMPLEXS,  32, 32,  4)

   PRIME_STYPE("struct",         STRUCT,        STRUCTS,    0,  1,  0)
   PRIME_STYPE("union",          UNION,         UNIONS,     0,  1,  0)

   return;
}
