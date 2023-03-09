#ifndef __DDS_DUMP_H
#define __DDS_DUMP_H

/*
RCS-t=DDS, diagnostic dump "utility" declarations

$Id: dds_dump.h 16 2009-01-06 21:52:47Z user $
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

/*********************************/
/* typedef and enumerated values */
/*********************************/

typedef struct DDS_DUMP_DIR
{  /* one structure per entry in dump list */
   const char *name;    /* field name */
   int width;           /* column width */
} DDS_DUMP_DIR;
 
/*****************************************/
/* extern global variables and constants */
/*****************************************/

extern DDS_DUMP_DIR *dds_dump_dir; /* column directory for DDS_DUMP_ITEM. */
                                   /*    Required iff DDS_DUMP_LIST is set. */
                                   /*    Directory shared by all binaries. */
                                   /*    Binaries print data in relevant */
                                   /*    columns or pads them with blanks. */

extern int dds_dump_max;           /* maximum entry in list or vector. */
                                   /*    If DDS_DUMP_LIST is set, this */
                                   /*       is the number of entries in */
                                   /*       dds_dump_dir and dump_item. */
                                   /*    If DDS_DUMP_DIFF is set, this */
                                   /*       is the maximum number of */
                                   /*       printable vector elements. */

extern DDS_STACK dds_dump_que;     /* dump "diff", print preamble queue */

/***********************/
/* function prototypes */
/***********************/

/* Called for binary i/o and DDS_DUMP options */
void ddsDumpDataBuf(
   DDS_BIN *bin,
   const char *mode,
   DDS_SYM *sym,
   const char *buf)
;

/* Dump global variables */
void ddsDumpGlobal(void)
;

/* Dump one simple symbol */
void ddsDumpS(
   DDS_SYM *sym)
;

/* Dump the current symbol table */
void ddsDumpSymbol(void)
;

/* Dump one binary and symbol table */
void ddsDumpBinSym(
   int indent,
   BIN_TAG bin_tag)
;

/* Dump all binary symbol tables */
void ddsDumpBinary(void)
;

/* Dump input map */
void ddsDumpMapIn(
   int indent,
   BIN_TAG in_bin_tag)
;

/* Dump output map */
void ddsDumpMapOut(
   int indent,
   BIN_TAG out_bin_tag)
;

/* Dump stack descriptor */
void ddsDumpStack(
   int indent,
   DDS_STACK *stack)
;

/* Dump one specific symbol */
void ddsDumpSym(
   int indent,
   DDS_SYM *sym)
;



void ddsDumpDataType(
   int indent,
   DDS_TYPE *type,
   size_t offset,
   const char *buf,
   const char *buf1,
   const char *buf2)
;
void ddsDumpDataBlock(
   int indent,
   DDS_BLOCK *block,
   size_t offset,
   const char *buf,
   const char *buf1,
   const char *buf2)
;
void ddsDumpDataArray(
   int indent,
   DDS_TYPE *type,
   size_t offset,
   const char *buf,
   const char *buf1,
   const char *buf2)
;
int ddsDumpDataDiff(
   int indent,
   DDS_TYPE *type,
   size_t offset,
   const char *buf,
   const char *buf1,
   const char *buf2)
;
void ddsDumpDataValue(
   int min,
   int max,
   DDS_TYPE *type,
   const char *buf)
;
void ddsDumpDataOpen(
   DDS_BIN *bin)
;
void ddsDumpDataList(
   DDS_BIN *bin,
   const char *mode,
   DDS_TYPE *type,
   const char *buf)
;
 
 
void ddsDumpBlock(
   int indent,
   DDS_BLOCK *block)
;
void ddsDumpBlockR(
   DDS_BOOLEAN init,
   int indent,
   DDS_BLOCK *block)
;
void ddsDumpBlockRS(
   int indent,
   DDS_SYM *sym)
;
void ddsDumpExpr(
   int indent,
   DDS_EXPR4 *expr4)
;
void ddsDumpMember(
   int indent,
   DDS_MEMBER *member)
;
void ddsDumpItem(
   int indent,
   DDS_ITEM *item)
;
void ddsDumpObject(
   int indent,
   DDS_OBJECT *object)
;
void ddsDumpSymFlag(
   int indent,
   DDS_SYM *sym)
;
void ddsDumpTable(
   int indent,
   DDS_TABLE *table)
;
void ddsDumpReloc(
   int indent,
   DDS_VALUE *value)
;
void ddsDumpPage(
   int indent,
   DDS_PAGE *page)
;
void ddsDumpDeclare(
   int indent,
   DDS_DECLARE *declare)
;
void ddsDumpScanner(
   void *handle)
;
void ddsDumpType(
   int indent,
   DDS_TYPE *type)
;
 

#endif /* __DDS_DUMP_H */
