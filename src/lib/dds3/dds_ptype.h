#ifndef __DDS_PTYPE_H
#define __DDS_PTYPE_H
 
/*
RCS-t=DDS, ptype "utility" declarations
 
$Id: dds_ptype.h 39 2009-01-13 23:00:15Z user $
*/
 
/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

/*********************************/
/* typedef and enumerated values */
/*********************************/

typedef enum DDS_PTYPE
{  /* DDS prime types (implicitly known by DDS) */
   DDS_PTYPE_ZERO           = 0,
   /**********************************************************************/
   /* "C" language types on the host computer                            */
   /**********************************************************************/
   DDS_CTYPE_MIN            = DDS_VOID, /* first CTYPE */
 
   DDS_CTYPE_VOID           = DDS_VOID, /* zero precision, one alignment */
   DDS_CTYPE_PTR            = DDS_PTR,  /* host pointer */
   DDS_CTYPE_CHAR           = DDS_CHAR,  /* host character */

   DDS_CTYPE_SCHAR          = DDS_SCHAR, /* signed character */
   DDS_CTYPE_SHORT          = DDS_SHORT, /* signed integers */
   DDS_CTYPE_INT            = DDS_INT,
   DDS_CTYPE_LONG           = DDS_LONG,
   DDS_CTYPE_LLONG          = DDS_LLONG,
 
   DDS_CTYPE_UCHAR          = DDS_UCHAR, /* unsigned character */
   DDS_CTYPE_USHORT         = DDS_USHORT, /* unsigned integers */
   DDS_CTYPE_UINT           = DDS_UINT,
   DDS_CTYPE_ULONG          = DDS_ULONG,
   DDS_CTYPE_ULLONG         = DDS_ULLONG,
 
   DDS_CTYPE_FLOAT          = DDS_FLOAT, /* floating point */
   DDS_CTYPE_DOUBLE         = DDS_DOUBLE,
   DDS_CTYPE_LDOUBLE        = DDS_LDOUBLE,
 
   DDS_CTYPE_CPX            = DDS_CPX, /* struct{float real, imag;}; */
   DDS_CTYPE_DBLCPX         = DDS_DBLCPX,
   DDS_CTYPE_LDBLCPX        = DDS_LDBLCPX,
 
   DDS_CTYPE_MAX            = DDS_LDBLCPX + 1,
 
   /**********************************************************************/
   /* "DDS" intrinsic types (host independent, immutable typedefs)       */
   /**********************************************************************/
   DDS_STYPE_MIN            = DDS_ASCII1,  /* first stream type */
 
   DDS_STYPE_ASCII1         = DDS_ASCII1,  /* unsigned ascii */
   DDS_STYPE_EBCDIC1        = DDS_EBCDIC1, /* unsigned ebcdic */

   DDS_STYPE_INTEGER1       = DDS_INTEGER1,
   DDS_STYPE_INTEGER2       = DDS_INTEGER2,
   DDS_STYPE_INTEGER2X      = DDS_INTEGER2X,
   DDS_STYPE_INTEGER4       = DDS_INTEGER4,
   DDS_STYPE_INTEGER4X      = DDS_INTEGER4X,
   DDS_STYPE_INTEGER8       = DDS_INTEGER8,
   DDS_STYPE_INTEGER8X      = DDS_INTEGER8X,
 
   DDS_STYPE_UNSIGNED1      = DDS_UNSIGNED1,
   DDS_STYPE_UNSIGNED2      = DDS_UNSIGNED2,
   DDS_STYPE_UNSIGNED2X     = DDS_UNSIGNED2X,
   DDS_STYPE_UNSIGNED4      = DDS_UNSIGNED4,
   DDS_STYPE_UNSIGNED4X     = DDS_UNSIGNED4X,
   DDS_STYPE_UNSIGNED8      = DDS_UNSIGNED8,
   DDS_STYPE_UNSIGNED8X     = DDS_UNSIGNED8X,
 
   DDS_STYPE_FLOAT4         = DDS_FLOAT4,
   DDS_STYPE_FLOAT4X        = DDS_FLOAT4X,
   DDS_STYPE_FLOAT4IBM      = DDS_FLOAT4IBM,
   DDS_STYPE_FLOAT8         = DDS_FLOAT8,
   DDS_STYPE_FLOAT8X        = DDS_FLOAT8X,
   DDS_STYPE_FLOAT8IBM      = DDS_FLOAT8IBM,
   DDS_STYPE_FLOAT16        = DDS_FLOAT16,
   DDS_STYPE_FLOAT16X       = DDS_FLOAT16X,

   DDS_STYPE_COMPLEX8       = DDS_COMPLEX8, /* struct{DDS_FLOAT4 real, imag;};*/
   DDS_STYPE_COMPLEX8X      = DDS_COMPLEX8X,
   DDS_STYPE_COMPLEX8IBM    = DDS_COMPLEX8IBM,
   DDS_STYPE_COMPLEX16      = DDS_COMPLEX16,
   DDS_STYPE_COMPLEX16X     = DDS_COMPLEX16X,
   DDS_STYPE_COMPLEX16IBM   = DDS_COMPLEX16IBM,
   DDS_STYPE_COMPLEX32      = DDS_COMPLEX32,
   DDS_STYPE_COMPLEX32X     = DDS_COMPLEX32X,

   /**********************************************************************/
   /* "psuedo" prime types, for control purposes                         */
   /**********************************************************************/
   DDS_STYPE_STRUCT,        /* aggregate, sequential members */
   DDS_STYPE_UNION,         /* aggregate, coalesced  members */
   DDS_STYPE_MAX
} DDS_PTYPE;


typedef enum DDS_TYPE_GENUS
{  /* type genus (functional summary of data types) */
   DDS_TYPE_VOIDS,          /* void type */
   DDS_TYPE_CHARS,          /* printable characters */
   DDS_TYPE_INTEGERS,       /* signed integers */
   DDS_TYPE_UNSIGNEDS,      /* unsigned integers */
   DDS_TYPE_FLOATS,         /* floating point numbers */
   DDS_TYPE_COMPLEXS,       /* float pairs (real and imaginary) */
   DDS_TYPE_STRUCTS,        /* sequential member composition */
   DDS_TYPE_UNIONS,         /* coalesced  member composition */
   DDS_TYPE_PTRS,          /* executeable function */
   DDS_TYPE_GENUS_MAX       /* keyword count */
} DDS_TYPE_GENUS;


typedef struct DDS_PRIME
{  /* "prime" attributes (one for each DDS_PTYPE) */
   const char *name;        /* "int", "unsigned", "integer1", ... */
   const char *macro;       /* "INT", "UINT",     "INTEGER1", ... */
   DDS_TYPE_GENUS genus;    /* functional summary of data types. */
   int dump;                /* maximum number of datums per line */
   int align;               /* minimum byte alignment per datum */
   long prec;               /* precision, bytes per datum */
   DDS_PTYPE stype;         /* corresponding stream independent type */
   struct DDS_TYPE *type;   /* type entry in prime symbol table */
} DDS_PRIME;

typedef struct DDS_PRIME_GENUS
{  /* type genus "prime" attributes (one for each DDS_TYPE_GENUS) */
   const char *note;        /* type genus description */
} DDS_PRIME_GENUS;

/*****************************************/
/* extern global variables and constants */
/*****************************************/

extern DDS_PTYPE dds_mutate[DDS_STYPE_MAX];
extern DDS_PTYPE dds_concur[DDS_STYPE_MAX][DDS_STYPE_MAX];

extern DDS_PRIME *dds_prime;
 
extern const DDS_PRIME_GENUS dds_prime_genus[DDS_TYPE_GENUS_MAX];

/***********************/
/* function prototypes */
/***********************/

void ddsPrimeTypes(void)
;
 
#endif /* __DDS_PTYPE_H */
