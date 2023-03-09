#ifndef __DDS_SYMBOL_H
#define __DDS_SYMBOL_H

/*
RCS-t=DDS, symbol table "utilities" (lex and yacc compiler stuff)
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

/********************************************/
/* macros to push symbols onto a hash table */
/********************************************/

#define PUSH_TYPE(TABLE, NAME, FLAG, BBLOCK, TTYPE)  \
   PUSH_SYM(TYPE, TABLE, NAME, (DDS_SYM_TYPE | FLAG), BBLOCK, TTYPE)

#define PUSH_BLOCK(TABLE, NAME, FLAG, BBLOCK, TTYPE)  \
   PUSH_SYM(BLOCK, TABLE, NAME, (DDS_SYM_BLOCK | FLAG), BBLOCK, TTYPE)

#define PUSH_MEMBER(TABLE, NAME, FLAG, BBLOCK, TTYPE)  \
   PUSH_SYM(MEMBER, TABLE, NAME, (DDS_SYM_MEMBER | FLAG), BBLOCK, TTYPE)

#define PUSH_SYM(SSYM, TTABLE, NNAME, FFLAG, BBLOCK, TTYPE)  \
   (dds_push_sym = (DDS_SYM*)ddsTablePushItem((TTABLE),  \
         (NNAME), sizeof(DDS_##SSYM)),  \
      dds_push_sym->tag = -1, \
      dds_push_sym->flag = (DDS_SYM_FLAG)(FFLAG), \
      dds_push_sym->block = (BBLOCK),  \
      dds_push_sym->type = (TTYPE),  \
      ((DDS_##SSYM*)dds_push_sym))

#define PUSH_OBJECT(TABLE, NAME, FLAG, BBLOCK, TTYPE)  \
   PUSH_VALUE(OBJECT, TABLE, NAME, (DDS_SYM_OBJECT | FLAG), BBLOCK, TTYPE, 0)

#define PUSH_EXPR(TABLE, NAME, FLAG, BBLOCK, TTYPE)  \
   PUSH_VALUE(EXPR, TABLE, NAME, (DDS_SYM_EXPR | FLAG), BBLOCK, TTYPE, 0)

#define PUSH_EXPR1(TABLE, NAME, FLAG, BBLOCK, TTYPE)  \
   PUSH_VALUE(EXPR1, TABLE, NAME, (DDS_SYM_EXPR | FLAG), BBLOCK, TTYPE, 0)

#define PUSH_EXPR2(TABLE, NAME, FLAG, BBLOCK, TTYPE)  \
   PUSH_VALUE(EXPR2, TABLE, NAME, (DDS_SYM_EXPR | FLAG), BBLOCK, TTYPE, 0)

#define PUSH_EXPR3(TABLE, NAME, FLAG, BBLOCK, TTYPE)  \
   PUSH_VALUE(EXPR3, TABLE, NAME, (DDS_SYM_EXPR | FLAG), BBLOCK, TTYPE, 0)

#define PUSH_EXPR4(TABLE, NAME, FLAG, BBLOCK, TTYPE)  \
   PUSH_VALUE(EXPR4, TABLE, NAME, (DDS_SYM_EXPR | FLAG), BBLOCK, TTYPE, 0)

#define PUSH_EXPRA(TABLE, NAME, FLAG, BBLOCK, TTYPE)  \
   PUSH_VALUE(EXPRA, TABLE, NAME, (DDS_SYM_EXPR | FLAG), BBLOCK, TTYPE, 0)

#define PUSH_EXPRC(TABLE, NAME, FLAG, BBLOCK, TTYPE)  \
   PUSH_VALUE(EXPRC, TABLE, NAME, (DDS_SYM_EXPR | FLAG), BBLOCK, TTYPE, 0)

#define PUSH_VALUE(SSYM, TTABLE, NNAME, FFLAG, BBLOCK, TTYPE, NPLUS)  \
   (dds_push_sym = (DDS_SYM*)ddsTablePushItem((TTABLE),  \
         (NNAME), sizeof(DDS_##SSYM) + NPLUS),  \
      dds_push_sym->tag = -1,  \
      dds_push_sym->flag = (DDS_SYM_FLAG)  \
         ((FFLAG) | ((BBLOCK)->flag & DDS_SYM_STORE)),  \
      dds_push_sym->block = (BBLOCK),  \
      dds_push_sym->type = (TTYPE),  \
      ((DDS_##SSYM*)dds_push_sym))

/* masking macro for initializing arguments */
#define EXPR_ARG(EARG, VALUE)  \
   (EARG)->track = (VALUE); \
   ddsSymValuePtr(&(EARG)->value.ptr, (VALUE));

#define RELOC_VALUE(VALUE)  \
   if((VALUE)->reloc && \
      (VALUE)->ptr != (void*)((DDS_RELOC*)(VALUE)->reloc->ptr)->ptrptr)  \
      ddsSymRelocValue((DDS_VALUE*)(VALUE)); /* update dependencies */

/****************************************************************/
/* Deprecate the old macros CONVAA, CONVAN, CONVNA, CONVNN.     */
/* The new versions are CON_AAVV, CON_ANVV, CON_NAVV, CON_NNVV. */
/* "SS" (scalar) and "VS" (vector, scalar) versions also exist. */
/****************************************************************/

#define deprecate_CONVAA(OUT_BUF, OUT_PTYPE, IN_BUF, IN_PTYPE, COUNT) \
   CON_AAVV(OUT_PTYPE, IN_PTYPE, OUT_BUF, IN_BUF, COUNT)

#define deprecate_CONVAN(OUT_BUF, OUT_PTYPE, IN_BUF, IN_PTYPE, COUNT) \
   CON_ANVV(OUT_PTYPE, IN_PTYPE, OUT_BUF, IN_BUF, COUNT)

#define deprecate_CONVNA(OUT_BUF, OUT_PTYPE, IN_BUF, IN_PTYPE, COUNT) \
   CON_NAVV(OUT_PTYPE, IN_PTYPE, OUT_BUF, IN_BUF, COUNT)

#define deprecate_CONVNN(OUT_BUF, OUT_PTYPE, IN_BUF, IN_PTYPE, COUNT) \
   CON_NNVV(OUT_PTYPE, IN_PTYPE, OUT_BUF, IN_BUF, COUNT)

/*****************************************/
/* convert between stream data types,    */
/* to aligned scalar from aligned scalar */
/*****************************************/

#define CON_AASS(LTYPE, RTYPE, LPTR, RPTR)  \
   (DDS_CON_EXPRA.opcode = (DDS_OPCODE)0 /* DDS_ASSIGN_OP */ ,  \
   DDS_CON_EXPRA.ptr = (void*)(LPTR),  \
   DDS_CON_EXPRA.arg2.value.ptr = (void*)(RPTR),  \
   DDS_CON_EXPRA.stype1 = dds_prime[LTYPE].stype,  \
   DDS_CON_EXPRA.stype2 = dds_prime[RTYPE].stype,  \
   DDS_CON_EXPRA.count = 1,  \
   ddsEvaluate((DDS_EXPR*)&DDS_CON_EXPRA), DDS_CON_OK)

/*****************************************/
/* convert between stream data types,    */
/* to aligned vector from aligned vector */
/*****************************************/

#define CON_AAVV(LTYPE, RTYPE, LPTR, RPTR, MAX)  \
   (DDS_CON_EXPRA.opcode = (DDS_OPCODE)0 /* DDS_ASSIGN_OP */ ,  \
   DDS_CON_EXPRA.ptr = (void*)(LPTR),  \
   DDS_CON_EXPRA.arg2.value.ptr = (void*)(RPTR),  \
   DDS_CON_EXPRA.stype1 = dds_prime[LTYPE].stype,  \
   DDS_CON_EXPRA.stype2 = dds_prime[RTYPE].stype,  \
   DDS_CON_EXPRA.count = (MAX),  \
   ddsEvaluate((DDS_EXPR*)&DDS_CON_EXPRA), DDS_CON_OK)

/*****************************************/
/* convert between stream data types,    */
/* to aligned vector from aligned scalar */
/*****************************************/

#define CON_AAVS(LTYPE, RTYPE, LPTR, RPTR, MAX)  \
   (DDS_CON_EXPRA.opcode = (DDS_OPCODE)0 /* DDS_ASSIGN_OP */ ,  \
   DDS_CON_EXPRA.ptr = (void*)(LPTR),  \
   DDS_CON_EXPRA.arg2.value.ptr = (void*)(RPTR),  \
   DDS_CON_EXPRA.stype1 = dds_prime[LTYPE].stype,  \
   DDS_CON_EXPRA.stype2 = dds_prime[RTYPE].stype,  \
   DDS_CON_EXPRA.count = (MAX),  \
   ddsEvaluate((DDS_EXPR*)&DDS_CON_EXPRA), DDS_CON_OK)


/*********************************************/
/* convert between stream data types,        */
/* to aligned scalar from non-aligned scalar */
/*********************************************/

#define CON_ANSS(LTYPE, RTYPE, LPTR, RPTR)  \
   (DDS_CON_EXPRA.opcode = (DDS_OPCODE)0 /* DDS_ASSIGN_OP */ ,  \
   DDS_CON_EXPRA.ptr = (void*)(LPTR),  \
   DDS_CON_EXPRA.arg2.value.ptr = (void*)(RPTR),  \
   DDS_CON_EXPRA.stype1 = dds_prime[LTYPE].stype,  \
   DDS_CON_EXPRA.stype2 = dds_prime[RTYPE].stype,  \
   DDS_CON_EXPRA.count = 1,  \
   ddsEvaluate((DDS_EXPR*)&DDS_CON_EXPRA), DDS_CON_OK)

/*********************************************/
/* convert between stream data types,        */
/* to aligned vector from non-aligned vector */
/*********************************************/

#define CON_ANVV(LTYPE, RTYPE, LPTR, RPTR, MAX)  \
   (DDS_CON_EXPRA.opcode = (DDS_OPCODE)0 /* DDS_ASSIGN_OP */ ,  \
   DDS_CON_EXPRA.ptr = (void*)(LPTR),  \
   DDS_CON_EXPRA.arg2.value.ptr = (void*)(RPTR),  \
   DDS_CON_EXPRA.stype1 = dds_prime[LTYPE].stype,  \
   DDS_CON_EXPRA.stype2 = dds_prime[RTYPE].stype,  \
   DDS_CON_EXPRA.count = (MAX),  \
   ddsEvaluate((DDS_EXPR*)&DDS_CON_EXPRA), DDS_CON_OK)

/*********************************************/
/* convert between stream data types,        */
/* to aligned vector from non-aligned scalar */
/*********************************************/

#define CON_ANVS(LTYPE, RTYPE, LPTR, RPTR, MAX)  \
   (DDS_CON_EXPRA.opcode = (DDS_OPCODE)0 /* DDS_ASSIGN_OP */ ,  \
   DDS_CON_EXPRA.ptr = (void*)(LPTR),  \
   DDS_CON_EXPRA.arg2.value.ptr = (void*)(RPTR),  \
   DDS_CON_EXPRA.stype1 = dds_prime[LTYPE].stype,  \
   DDS_CON_EXPRA.stype2 = dds_prime[RTYPE].stype,  \
   DDS_CON_EXPRA.count = (MAX),  \
   ddsEvaluate((DDS_EXPR*)&DDS_CON_EXPRA), DDS_CON_OK)


/*********************************************/
/* convert between stream data types,        */
/* to non-aligned scalar from aligned scalar */
/*********************************************/

#define CON_NASS(LTYPE, RTYPE, LPTR, RPTR)  \
   (DDS_CON_EXPRA.opcode = (DDS_OPCODE)0 /* DDS_ASSIGN_OP */ ,  \
   DDS_CON_EXPRA.ptr = (void*)(LPTR),  \
   DDS_CON_EXPRA.arg2.value.ptr = (void*)(RPTR),  \
   DDS_CON_EXPRA.stype1 = dds_prime[LTYPE].stype,  \
   DDS_CON_EXPRA.stype2 = dds_prime[RTYPE].stype,  \
   DDS_CON_EXPRA.count = 1,  \
   ddsEvaluate((DDS_EXPR*)&DDS_CON_EXPRA), DDS_CON_OK)

/*********************************************/
/* convert between stream data types,        */
/* to non-aligned vector from aligned vector */
/*********************************************/

#define CON_NAVV(LTYPE, RTYPE, LPTR, RPTR, MAX)  \
   (DDS_CON_EXPRA.opcode = (DDS_OPCODE)0 /* DDS_ASSIGN_OP */ ,  \
   DDS_CON_EXPRA.ptr = (void*)(LPTR),  \
   DDS_CON_EXPRA.arg2.value.ptr = (void*)(RPTR),  \
   DDS_CON_EXPRA.stype1 = dds_prime[LTYPE].stype,  \
   DDS_CON_EXPRA.stype2 = dds_prime[RTYPE].stype,  \
   DDS_CON_EXPRA.count = (MAX),  \
   ddsEvaluate((DDS_EXPR*)&DDS_CON_EXPRA), DDS_CON_OK)

/*********************************************/
/* convert between stream data types,        */
/* to non-aligned vector from aligned scalar */
/*********************************************/

#define CON_NAVS(LTYPE, RTYPE, LPTR, RPTR, MAX)  \
   (DDS_CON_EXPRA.opcode = (DDS_OPCODE)0 /* DDS_ASSIGN_OP */ ,  \
   DDS_CON_EXPRA.ptr = (void*)(LPTR),  \
   DDS_CON_EXPRA.arg2.value.ptr = (void*)(RPTR),  \
   DDS_CON_EXPRA.stype1 = dds_prime[LTYPE].stype,  \
   DDS_CON_EXPRA.stype2 = dds_prime[RTYPE].stype,  \
   DDS_CON_EXPRA.count = (MAX),  \
   ddsEvaluate((DDS_EXPR*)&DDS_CON_EXPRA), DDS_CON_OK)


/*************************************************/
/* convert between stream data types,            */
/* to non-aligned scalar from non-aligned scalar */
/*************************************************/

#define CON_NNSS(LTYPE, RTYPE, LPTR, RPTR)  \
   (DDS_CON_EXPRA.opcode = (DDS_OPCODE)0 /* DDS_ASSIGN_OP */ ,  \
   DDS_CON_EXPRA.ptr = (void*)(LPTR),  \
   DDS_CON_EXPRA.arg2.value.ptr = (void*)(RPTR),  \
   DDS_CON_EXPRA.stype1 = dds_prime[LTYPE].stype,  \
   DDS_CON_EXPRA.stype2 = dds_prime[RTYPE].stype,  \
   DDS_CON_EXPRA.count = 1,  \
   ddsEvaluate((DDS_EXPR*)&DDS_CON_EXPRA), DDS_CON_OK)

/*************************************************/
/* convert between stream data types,            */
/* to non-aligned vector from non-aligned vector */
/*************************************************/

#define CON_NNVV(LTYPE, RTYPE, LPTR, RPTR, MAX)  \
   (DDS_CON_EXPRA.opcode = (DDS_OPCODE)0 /* DDS_ASSIGN_OP */ ,  \
   DDS_CON_EXPRA.ptr = (void*)(LPTR),  \
   DDS_CON_EXPRA.arg2.value.ptr = (void*)(RPTR),  \
   DDS_CON_EXPRA.stype1 = dds_prime[LTYPE].stype,  \
   DDS_CON_EXPRA.stype2 = dds_prime[RTYPE].stype,  \
   DDS_CON_EXPRA.count = (MAX),  \
   ddsEvaluate((DDS_EXPR*)&DDS_CON_EXPRA), DDS_CON_OK)

/*************************************************/
/* convert between stream data types,            */
/* to non-aligned vector from non-aligned scalar */
/*************************************************/

#define CON_NNVS(LTYPE, RTYPE, LPTR, RPTR, MAX)  \
   (DDS_CON_EXPRA.opcode = (DDS_OPCODE)0 /* DDS_ASSIGN_OP */ ,  \
   DDS_CON_EXPRA.ptr = (void*)(LPTR),  \
   DDS_CON_EXPRA.arg2.value.ptr = (void*)(RPTR),  \
   DDS_CON_EXPRA.stype1 = dds_prime[LTYPE].stype,  \
   DDS_CON_EXPRA.stype2 = dds_prime[RTYPE].stype,  \
   DDS_CON_EXPRA.count = (MAX),  \
   ddsEvaluate((DDS_EXPR*)&DDS_CON_EXPRA), DDS_CON_OK)

/*********************************************/
/* macros for parser declaration temporaries */
/*********************************************/

/* Push declaration stack (nested declaration) */
/* Example: struct {int inner;} outer; */
#define DECLARE_INNER  \
   if(dds_declare->inner) {  \
      dds_declare = dds_declare->inner;  \
   } else {  \
      dds_declare->inner = ddsZalloc(sizeof(DDS_DECLARE));  \
      dds_declare->inner->outer = dds_declare;  \
      dds_declare = dds_declare->inner;  \
   } DECLARE_CLEAR

/* Pop declaration stack (compliment of INNER) */
#define DECLARE_OUTER  \
   assert(dds_declare->outer && "DECLARE_OUTER overrun"); \
   dds_declare = dds_declare->outer;

/* Comma seperated list of declarators */
/* Example: float x, y, z; */
#define DECLARE_MORE  \
   dds_declare->type = dds_declare->more;

/* Prepare for a new declaration (same level) */
/* Example: int x; float y; */
#define DECLARE_CLEAR  \
   dds_declare->flag = (DDS_SYM_FLAG)0;  \
   dds_declare->type = NULL;  \
   dds_declare->more = NULL;

/* Initial entry into parser (not recursive entries) */
#define DECLARE_RESET  \
   while(dds_declare->outer) {dds_declare = dds_declare->outer;}  \
   DECLARE_CLEAR;

/*************************************/
/* macro front end to ddsSymLookup() */
/*************************************/
 
#define UNIQUE_OBJECT(TABLE, NAME, SCOPE) \
   (DDS_OBJECT*)ddsSymLookup((TABLE), (NAME), DDS_SYM_OBJECT, (SCOPE), (SCOPE))
 
#define UNIQUE_BLOCK(TABLE, NAME, SCOPE) \
   (DDS_BLOCK*)ddsSymLookup((TABLE), (NAME), DDS_SYM_BLOCK, (SCOPE), (SCOPE))
 
#define UNIQUE_EXPR(TABLE, NAME, SCOPE) \
   (DDS_EXPR*)ddsSymLookup((TABLE), (NAME), DDS_SYM_EXPR, (SCOPE), (SCOPE))
 
#define UNIQUE_MEMBER(TABLE, NAME, SCOPE) \
   (DDS_MEMBER*)ddsSymLookup((TABLE), (NAME), DDS_SYM_MEMBER, (SCOPE), (SCOPE))
 
#define LOOKUP_OBJECT_NEW(TABLE, NAME, SCOPE) \
   (DDS_OBJECT*)ddsSymLookup((TABLE), (NAME), DDS_SYM_OBJECT,  \
      (SCOPE), dds_compile->block_new)
 
#define LOOKUP_OBJECT(TABLE, NAME, SCOPE) \
   (DDS_OBJECT*)ddsSymLookup((TABLE), (NAME), DDS_SYM_OBJECT,  \
      (SCOPE), dds_compile->block_old)
 
#define LOOKUP_BLOCK(TABLE, NAME, SCOPE) \
   (DDS_BLOCK*)ddsSymLookup((TABLE), (NAME), DDS_SYM_BLOCK,  \
      (SCOPE), dds_compile->block_old)
 
#define LOOKUP_EXPR(TABLE, NAME, SCOPE) \
   (DDS_EXPR*)ddsSymLookup((TABLE), (NAME), DDS_SYM_EXPR,  \
      (SCOPE), dds_compile->block_old)
 
#define LOOKUP_TYPE(TABLE, NAME, SCOPE) \
   (DDS_TYPE*)ddsSymLookup((TABLE), (NAME), DDS_SYM_TYPE,  \
      (SCOPE), dds_compile->block_old)

/* LOOKUP_MEMBER is ambiguous, use UNIQUE_MEMBER or ddsSymLookupMem. */


/**************************************************/
/* Macros to fake scanner stack for map messages. */
/**************************************************/

/* Push scanner stack (nested input) */
#define SCAMMER_PUSH(DICT, DEFN)  \
   dds_lexer->yy_buf = NULL;  \
   dds_lexer->leof = DDS_TRUE;  \
   dds_lexer->dict = (DICT);  \
   dds_lexer->defn = (DEFN);  \
   if(dds_lexer->inner) {  \
      dds_lexer = dds_lexer->inner;  \
   } else {  \
      dds_lexer->inner = ddsZalloc(sizeof(DDS_SCANNER));  \
      dds_lexer->inner->outer = dds_lexer;  \
      dds_lexer = dds_lexer->inner;  \
   }  \
   dds_lexer->leof = DDS_FALSE;  \
   dds_lexer->dict = NULL;  \
   dds_lexer->defn = NULL;  \
   dds_lexer->start = 0;  \
   dds_lexer->next = 0;  \
   dds_lexer->stop = 0;  \
   dds_lexer->line = 0;
 
/* Pop scanner stack (compliment of PUSH) */
#define SCAMMER_POP  \
   dds_lexer = dds_lexer->outer;
 

/*********************************/
/* typedef and enumerated values */
/*********************************/

/************************************************************************/
/*  Standard lex and yacc do not support recursive compilation.         */
/*  Gnu Flex and Bison are used because the compiler is recursive,      */
/*  i.e. the parser calls the scanner which may return to the parser    */
/*  OR the scanner may *call* the parser (recursively).                 */
/*                                                                      */
/*  ddsCompileFmt   -->--+                                              */
/*  (DDS_LEX_TRANSLATE)  |                                              */
/*                       |                                              */
/*  ddsMapStruct    -->--+                                              */
/*  (DDS_LEX_MAP)        |                                              */
/*                       |                                              */
/*           +-------->--+--->  parser.y  <--->  scanner.l              */
/*           |                  ddsYYparse       ddsYYlex   ----+       */
/*           |                                                  |       */
/*           |                                                  |       */
/*           |                                                  |       */
/*           +-----  defn_value("dict","defn")  <---------------+       */
/*           |         (DDS_LEX_DEFN)                           |       */
/*           |                                                  |       */
/*           +-----  ident("funky-name")  <---------------------+       */
/*           |         (DDS_LEX_IDENT)                          |       */
/*           |                                                  |       */
/*           +-----  undefined ? ddsYYdepend  <-----------------+       */
/*                     (DDS_LEX_TRANSLATE)                              */
/*                                                                      */
/************************************************************************/

typedef struct DDS_KEYWORD
{  /* keyword hash bucket (hash table entry) */
   DDS_ITEM_CLASS      /* hash table item */
                       /*    (hash, name, chain, older, newer) */
   int token;          /* token value for keyword. */
                       /*    value defined by GNU bison. */
} DDS_KEYWORD;


/*****************************************************************/
/* The parser and scanner append symbols to the "active" table.  */
/* The active symbol table and associated binary are defined by  */
/*    extern DDS_TABLE *dds_symbol;                              */
/*    extern DDS_BIN   *dds_compile;                             */
/*                                                               */
/* The symbol table for DDS prime types is shared by binaries.   */
/* The initial table entry is identified by dds_prime_block.     */
/* Nested within it are prime types and error recovery symbols.  */
/* The prime symbol table and special entries are defined by     */
/*    extern DDS_TABLE   dds_prime_table;                        */
/*    extern DDS_BLOCK  *dds_prime_block;                        */
/*    extern DDS_BLOCK  *dds_error_block;                        */
/*    extern DDS_TYPE   *dds_error_type;                         */
/*    extern DDS_OBJECT *dds_error_object;                       */
/*    extern DDS_MEMBER *dds_error_member;                       */
/*    extern DDS_EXPR   *dds_error_expr;                         */
/*    extern DDS_EXPR   *dds_error_thread;                       */
/*                                                               */
/* "dds_prime" is an array of DDS_PRIME structs (attributes).    */
/* The attributes include pointers into dds_prime_table.         */
/* "dds_prime_tag" STACK provides shared API symbol tags.        */
/*                                                               */
/* Each binary stream also has a private "symbol" table.         */
/* The initial table entry is identified by "block_old".         */
/* Nested within it are implicit symbols from the old format.    */
/* "block_new" is nested within block_old, which provides        */
/* the default symbols and outer scope for the new format.       */
/* The block_old and block_new scopes are never closed.          */
/* "dds_block" identifies the current/active compiler scope.     */
/* "sym_tag" STACK provides private API symbol tags.             */
/* The BIN struct for each binary stream contains                */
/*    DDS_TABLE symbol;                                          */
/*    DDS_BLOCK *block_old;                                      */
/*    DDS_BLOCK *block_new;                                      */
/*    DDS_STACK sym_tag;                                         */
/*                                                               */
/* When looking for named "objects" while compiling formats,     */
/* If the object is defined within the current scope,            */
/*    use it !                                                   */
/* else the scanner calls ddsYYdepend and attempts to create it. */
/*    If DDS_YACC_CREATE is set (write create mode),             */
/*       If "fmt:-:-.name" is defined,                           */
/*          call ddsYYparser recursively, which should create a  */
/*             "name" object within the current scope.           */
/*          If "name" object is created, use it !                */
/*       else if object "name" is defined within the block_old   */
/*          scope, use it !                                      */
/*       else if "fmt:-.name is defined,                         */
/*          re-open the block_old scope and call ddsYYparser.    */
/*          This "should" create the "name" object.              */
/*          If "name" object is created, use it !                */
/*          The block_old scope is closed before continuing.     */
/*       else                                                    */
/*          fail !                                               */
/*    else (read existing mode),                                 */
/*       If "fmt:-.name is defined,                              */
/*          call ddsYYparser recursively, which should create a  */
/*             "name" object within the current scope.           */
/*          If "name" object is created, use it !                */
/*       else if "fmt:-:-.name" is defined,                      */
/*          call ddsYYparser recursively, which should create a  */
/*             "name" object within the current scope.           */
/*          If "name" object is created, use it !                */
/*       else                                                    */
/*          fail !                                               */
/*****************************************************************/


/************************  CHEESEY DIAGRAM  *****************************/
/*              ***  CONNECTIVITY OF SYMBOL ENTRIES  ***                */
/************************************************************************/
/*     :.......:  :                                  :             :    */
/*             |  |                                  |             |    */
/*     +------      -------------->  BLOCK  <--------+------<------+    */
/*     |       |  |                     hash/name                       */
/*     |       |  |   outer hash <----  chain                           */
/*     |       |  +----------<--------  older                           */
/*     |       |                        newer  ---------->---------+    */
/*     |       +---------<------------  block/flag                 |    */
/*     |              type  <---------  type                       |    */
/*     |                                first  -->---+             |    */
/*     |                                last  ------   ----->--+   |    */
/*     |                                             |         |   |    */
/*     :                                             :         :   :    */
/*     |    +-->------------------>  OBJECT  <-------+         |   |    */
/*     |    |                           hash/name              |   |    */
/*     |    |         local hash <----  chain                  |   |    */
/*     |    |              0 <--------  older                  |   |    */
/*     |    |                           newer  -->---+         |   |    */
/*     +---   -----------<------------  block/flag   |         |   |    */
/*     |    |         type  <---------  type         |         |   |    */
/*     |    +--<--+                     ptr          |         |   |    */
/*     |          |   stack  <--------  reloc        |         |   |    */
/*     |          |   value  <--------  track        |         |   |    */
/*     :          :                                  :         :   :    */
/*     |    +-->-   ---------------> MEMBER  <-------+         |   |    */
/*     |    |     |                     hash/name              |   |    */
/*     |    |     |   local hash <----  chain                  |   |    */
/*     |    |     +----------<--------  older                  |   |    */
/*     |    |                           newer  -->---+         |   |    */
/*     +---   -----------<------------  block/flag   |         |   |    */
/*     |    |         type  <---------  type         |         |   |    */
/*     |    |                           offset       |         |   |    */
/*     |    |                           adjust       |         |   |    */
/*     |    +--<--+   value  <--------  value_b      |         |   |    */
/*     :          :                                  :         :   :    */
/*     |    +-->-   ---------------> TYPE  <---------+         |   |    */
/*     |    |     |                     hash/name              |   |    */
/*     |    |     |            0 <----  chain                  |   |    */
/*     |    |     +----------<--------  older                  |   |    */
/*     |    |                           newer  -->---+         |   |    */
/*     +---   -----------<------------  block/flag   |         |   |    */
/*     |    |         type  <---------  type         |         |   |    */
/*     |    |                           ptype        |         |   |    */
/*     |    |                           align        |         |   |    */
/*     |    |                           prec/count   |         |   |    */
/*     |    |                           stroke       |         |   |    */
/*     |    +--<--+                     c/a/trail    |         |   |    */
/*     :          :                                  :         :   :    */
/*     |    +-->-   ---------------> EXPR  <---------+         |   |    */
/*     |    |     |                     hash/name              |   |    */
/*     |    |     |   local hash <----  chain                  |   |    */
/*     |    |     +----------<--------  older                  |   |    */
/*     |    |                           newer  -->---+         |   |    */
/*     +---   -----------<------------  block/flag   |         |   |    */
/*     |    |         type    <-------  type         |         |   |    */
/*     |    |                           ptr          |         |   |    */
/*     |    |         stack  <--------  reloc        |         |   |    */
/*     |    |                           opcode       |         |   |    */
/*     |    |         expr  <---------  next         |         |   |    */
/*     |    |         value  <--------  args...      |         |   |    */
/*     :    :.....:                                  :         :   :    */
/************************************************************************/


typedef enum DDS_SYM_FLAG
{  /* symbol table flags, shared by all "dds_symbol" entries */
   /***********************************************************************/
   /* Flags that distinguish between the basic symbol entries.            */
   /* These flags are mutually exclusive.                                 */
   /***********************************************************************/
   DDS_SYM_TYPE         = 1 <<  0,   /* type description (DDS_TYPE)       */
                                     /*    Name space not applicable.     */
                                     /*    Binary data representation.    */
   DDS_SYM_OBJECT       = 1 <<  1,   /* storage description (DDS_OBJECT)  */
                                     /*    Identifier name space.         */
                                     /*    Variables, constants,          */
                                     /*    enumerations and functions.    */
                                     /*    Implies value and storage,     */
                                     /*    unless TYPEDEF is also set.    */
   DDS_SYM_EXPR         = 1 <<  2,   /* expression description (DDS_EXPR).*/
                                     /*    Map expression name space.     */
                                     /*    Results, operators, and args.  */
   DDS_SYM_MEMBER       = 1 <<  3,   /* member description (DDS_MEMBER)   */
                                     /*    struct or union name space.    */
                                     /*    Bit fields may be anonymous.   */
                                     /*    Members subdivide objects,     */
                                     /*    but have no storage themself.  */
   DDS_SYM_BLOCK        = 1 <<  4,   /* nested scope (DDS_BLOCK)          */
                                     /*    "tag" name space (struct,      */
                                     /*    union, enum, initializer).     */
                                     /*    Symbols delimited by "{}".     */
   DDS_SYM_LABEL        = 1 <<  5,   /* statement markers (DDS_LABEL)     */
                                     /*    "label" name space.            */

   /*************************************************************/
   /* Flags shared by most symbol descriptors.                  */
   /*************************************************************/
   DDS_SYM_ERROR        = 1 <<  6,   /* symbol descriptor error */
   DDS_SYM_MULTIPLE     = 1 <<  7,   /* multiple declarations   */
   DDS_SYM_UNDEFINED    = 1 <<  8,   /* used before declaration */

   /*********************************************************/
   /* Flags (storage class) for DDS_OBJECT.                 */
   /* DDS_DECLARE usage is transient (declaration parsing). */
   /* DDS_BLOCK usage is for local objects and expressions. */
   /* These flags are mutually exclusive in DDS_DECLARE.    */
   /* DDS_SYM_REGISTER allowed in DDS_EXPR (deviant).       */
   /*********************************************************/
   DDS_SYM_AUTO         = 1 <<  9,   /* automatic           */
      /* Default for objects declared within functions.     */
   DDS_SYM_REGISTER     = 1 << 10,   /* register (auto)     */
      /* Unary "&" may not be applied to register object.   */
      /* If set, value is NOT aligned (deviant symantics),  */
      /* i.e. non-aligned assignments/coercion required.    */
      /* Example: map buffer objects created by ddsOpenMap. */
   DDS_SYM_STATIC       = 1 << 11,   /* static              */
   DDS_SYM_EXTERN       = 1 << 12,   /* external (static).  */
      /* Default for objects declared within file scope,    */
      /* and non-static function declarations.              */
   DDS_SYM_TYPEDEF      = 1 << 13,   /* typedef             */
      /* Syntactic convenience.  no storage is reserved.    */
      /* If set, DDS_SYM_LVALUE and PREP should be reset.   */
 
   /*******************************************************************/
   /* Flags (lex context) for DDS_DECLARE.                            */
   /* Declare usage is transient (declaration parsing).               */
   /*******************************************************************/
   DDS_SYM_IDENT        = 1 << 27,   /* IDENTIFIER_TOKEN wanted from  */
                                     /* scanner (not existing object).*/

   /*************************************************************/
   /* Flags (scope usage) for DDS_OBJECT, DDS_BLOCK, DDS_EXPR.  */
   /*************************************************************/
   DDS_SYM_FUNC         = 1 << 19,   /* function                */

   /*******************************************************/
   /* Flags (scope usage) for DDS_TYPE, DDS_OBJECT,       */
   /* BLOCK, MEMBER, and EXPR.                            */
   /*******************************************************/
   DDS_SYM_ARRAY        = 1 << 20,   /* array declaration */

   /*******************************************************/
   /* Flags (scope usage) for DDS_TYPE, DDS_OBJECT,       */
   /* BLOCK, MEMBER, and EXPR.                            */
   /* These flags are mutually exclusive.                 */
   /*******************************************************/
   DDS_SYM_STRUCT       = 1 << 21,   /* structure         */
   DDS_SYM_UNION        = 1 << 22,   /* union             */
   DDS_SYM_ENUM         = 1 << 23,   /* enumerated        */

   /*****************************************************/
   /* Flags (member modifiers) for DDS_MEMBER           */
   /*****************************************************/
   DDS_SYM_FIELD        = 1 << 27,   /* ":N", bit field */

   /****************************************************************/
   /* Flags (member composition) for DDS_DECLARE.                  */
   /* DDS_DECLARE usage is transient (declaration parsing).        */
   /* Used to "edit" members within struct/union lists.            */
   /****************************************************************/
   DDS_SYM_DELETE       = 1 << 14,   /* "-"  delete, if it exists  */
   DDS_SYM_REPLACE      = 1 << 15,   /* "*"  replace, if it exists */
   DDS_SYM_APPEND       = 1 << 16,   /* "+"  append, if unique     */
   DDS_SYM_DAPPEND      = 1 << 17,   /* "-+" delete and append     */
   DDS_SYM_RAPPEND      = 1 << 18,   /* "*+" replace or append     */

   /***********************************************/
   /* Flags (variations) for DDS_TYPE descriptors */
   /***********************************************/
   DDS_SYM_SHORT        = 1 <<  9,   /* short     */
   DDS_SYM_LONG         = 1 << 10,   /* long      */
   DDS_SYM_LLONG        = 1 << 11,   /* long long */
   DDS_SYM_SIGNED       = 1 << 12,   /* signed    */
   DDS_SYM_UNSIGNED     = 1 << 13,   /* unsigned  */

   /*********************************************************************/
   /* Flags (type genus) for DDS_TYPE descriptors                       */
   /*********************************************************************/
   DDS_SYM_VOID         = 1 << 14,   /* void                            */
   DDS_SYM_CHAR         = 1 << 15,   /* character                       */
   DDS_SYM_INT          = 1 << 16,   /* integer                         */
   DDS_SYM_FLOAT        = 1 << 17,   /* floating point                  */
   DDS_SYM_DOUBLE       = 1 << 18,   /* double precision floating point */
   DDS_SYM_COMPLEX      = 1 << 19,   /* complex (real, imaginary pair)  */

   /************************************************************************/
   /* Flags (attributes) for DDS_OBJECT, DDS_EXPR, and DDS_MEMBER.         */
   /* Set in DDS_EXPR iff it computes a pointer (P+n, A[n], P->m, S.m).    */
   /*    The computed pointer is save in DDS_EXPR value ptr itself, unlike */
   /*    other expressions which change the value pointed to by the ptr.   */
   /*    Dependencies are relocated when the expression is evaluated.      */
   /* Set in DDS_OBJECT iff the value ptr can change after compilation.    */
   /*    For example, the value ptr can change for map buffer objects.     */
   /*    Dependencies are relocated before evaluating map expressions.     */
   /* If DDS_SYM_RELOC is set and the value is used as an argument, then  */
   /*    the DDS_EARG value.ptr must be relocated when the original moves. */
   /*    An entry is appended to the original pointer's reloc list for     */
   /*    each DDS_EARG value.ptr that is dependent upon the value.         */
   /* Expressions with INHIBIT set are not threaded for evaluation.        */
   /*    If RELOC is also set (as with member references), dependencies   */
   /*    can not be relocated when the expression is evaluated.            */
   /*    Instead, the relocation information is propagated upwards.        */
   /* Set in DDS_MEMBER iff it is a pointer ???                            */
   /************************************************************************/
   DDS_SYM_RELOC       = 1 << 14,   /* value "ptr" type is (void**)       */

   /***********************************************************************/
   /* Flags (attributes) for DDS_OBJECT.                                  */
   /* Set in DDS_OBJECT iff value is a literal.  If set, "name" is the    */
   /* unparsed text for uncompile ("123", "3.14", "0xf", "string", etc.). */
   /* If reset, "name" is taken as an object identifer by uncompile.      */
   /***********************************************************************/
   DDS_SYM_LITERAL      = 1 << 15,   /* literal value  */

   /***********************************************************************/
   /* Flags (attributes) for DDS_EXPR.                                    */
   /* Set in DDS_EXPR iff the expression is not threaded for evaluation,  */
   /* i.e. the results are computed via another mechanism.                */
   /* Expression examples: foo.member, sizeof(type_name), axis_size("t"). */
   /***********************************************************************/
   DDS_SYM_INHIBIT      = 1 << 15,   /* inhibit threading and evaluation. */

   /***********************************************************************/
   /* Flags (attributes) for DDS_OBJECT and DDS_EXPR.                     */
   /* Implicitly set/reset in DDS_EXPR, depending upon operator and args. */
   /***********************************************************************/
   DDS_SYM_LVALUE       = 1 << 16,   /* value pointie is assignable       */

   /********************************************************/
   /* Flags (uncompile) for DDS_OBJECT and DDS_MEMBER.     */
   /* Set iff uncompile has already processed the object.  */
   /********************************************************/
   DDS_SYM_UNCOMP       = 1 << 17,   /* uncompiled done.   */

   /************************************************************************/
   /* Flags (opcode modifier) for DDS_EXPR.                                */
   /* If set, "ddsUncompile" must perform special processing.              */
   /************************************************************************/
   DDS_SYM_CAST         = 1 << 17,   /* explicit cast, DDS_ASSIGN_OP only. */
   DDS_SYM_COERCE       = 1 << 18,   /* implicit cast, DDS_ASSIGN_OP only. */
                                     /*    CAST and COERCE are optional    */
                                     /*    and mutually exclusive.         */
                                     /*    If neither CAST nor COERCE are  */
                                     /*    set, its a normal assignment.   */

   /************************************************************************/
   /* Flags (type qualifiers)  DDS_TYPE, DDS_OBJECT, DDS_MEMBER, DDS_EXPR. */
   /* Propagated to DDS_OBJECT from its declared DDS_TYPE.                 */
   /* Propagated to DDS_MEMBER from its declared DDS_TYPE.                 */
   /* Propagated to DDS_EXPR from DDS_MEMBER by "->" and "." operators.    */
   /* Propagated to DDS_EXPR from DDS_OBJECT by "=" operator.              */
   /************************************************************************/
   DDS_SYM_CONST        = 1 << 24,   /* value not assignable               */
      /* Set in DDS_TYPE via declaration keyword "const".                  */
      /* Implicitly set in DDS_EXPR for literal (constant) values.         */
      /* Warning if CONST is set and DDS_EXPR is used as an lvalue.        */
      /* Implicitly set/reset in DDS_EXPR, depending upon operator.        */
      /* (K&R 2nd Ed, A7.19)                                               */
   DDS_SYM_VOLATILE     = 1 << 25,   /* volatile, unpredictable value.     */
      /* Set in DDS_TYPE via declaration keyword "volatile".               */
      /* Optimization is suppressed if VOLATILE is set in an DDS_EXPR.     */
   DDS_SYM_ALIGN        = 1 << 26,   /* non default alignment              */
      /* Set in DDS_TYPE via declaration keyword "align(N)".               */
      /* Implicitly reset in DDS_EXPR for literal (constant) values.       */
      /* Implicitly set in DDS_OBJECT for map buffer references.           */

   /************************************************************************/
   /* Flags (CONST variations) DDS_OBJECT, DDS_EXPR.                       */
   /* Implicitly set/reset in DDS_EXPR, depending upon operator and args.  */
   /* Implicitly set in DDS_EXPR for literal (constant) values.            */
   /************************************************************************/
   DDS_SYM_CONST_PREP   = 1 << 27,   /* preprocessor constant.             */
   DDS_SYM_CONST_DECL   = 1 << 28,   /* declaration constant.              */
   DDS_SYM_CONST_INIT   = 1 << 29,   /* initializer constant.              */

   /***********************************************************************/
   /* Flags (attributes) for DDS_TYPE, DDS_OBJECT, DDS_BLOCK, DDS_MEMBER, */
   /* and DDS_LABEL.  */
   /***********************************************************************/
   DDS_SYM_INCOMPLETE   = 1 << 30    /* incomplete type                   */
      /*    Set in DDS_LABEL, if usage is a forward reference.            */
      /*    Reset in DDS_LABEL, when forward reference is back patched.   */
      /*    Set in DDS_TYPE, if array bound is unknown, i.e. "[]".        */
      /*    Set in DDS_TYPE, if struct/union/enum is a forward reference. */
      /*    Propagated to DDS_MEMBER from its declared DDS_TYPE.          */
      /*    Propagated to DDS_TYPE from struct/union DDS_MEMBER.          */
      /*    Propagated to DDS_OBJECT from DDS_TYPE.                       */
      /*    Reset in DDS_OBJECT, when forward reference is back patched.  */
      /*    If set in DDS_TYPE, ERROR is returned by "sizeof" operator.   */
      /*    If set in DDS_TYPE, ERROR if member of struct or union.       */

} DDS_SYM_FLAG;


   /***********************************************************************/
   /* bit   TYPE      OBJECT    EXPR      MEMBER    BLOCK     DECLARE     */
   /***********************************************************************/
   /*  0    TYPE                                                          */
   /*  1              OBJECT                                              */
   /*  2                        EXPR                                      */
   /*  3                                  MEMBER                          */
   /*  4                                            BLOCK                 */
   /*  6    ERROR     ERROR     ERROR     ERROR     ERROR     ERROR       */
   /*  7              MULTIPLE            MULTIPLE  MULTIPLE              */
   /*  8              UNDEFINED UNDEFINED UNDEFINED UNDEFINED             */
   /*  9    SHORT     AUTO                          AUTO      AUTO        */
   /* 10    LONG      REGISTER                      REGISTER  REGISTER    */
   /* 11    LLONG     STATIC                        STATIC    STATIC      */
   /* 12    SIGNED    EXTERN                        EXTERN    EXTERN      */
   /* 13    UNSIGNED  TYPEDEF                       TYPEDEF   TYPEDEF     */
   /* 14    VOID      RELOC     RELOC     RELOC               DELETE      */
   /* 15    CHAR      LITERAL   INHIBIT                       REPLACE     */
   /* 16    INT       LVALUE    LVALUE                        APPEND      */
   /* 17    FLOAT     UNCOMP    CAST      UNCOMP              DAPPEND     */
   /* 18    DOUBLE              COERCE                        RAPPEND     */
   /* 19    COMPLEX   FUNC      FUNC      FUNC      FUNC                  */
   /* 20    ARRAY     ARRAY     ARRAY     ARRAY                           */
   /* 21    STRUCT    STRUCT    STRUCT    STRUCT    STRUCT                */
   /* 22    UNION     UNION     UNION     UNION     UNION                 */
   /* 23    ENUM      ENUM      ENUM      ENUM      ENUM                  */
   /* 24    CONST     CONST     CONST     CONST                           */
   /* 25    VOLATILE  VOLATILE  VOLATILE  VOLATILE                        */
   /* 26    ALIGN     ALIGN     ALIGN     ALIGN                           */
   /* 27              PREP      PREP      FIELD               IDENT       */
   /* 28              DECL      DECL                                      */
   /* 29              INIT      INIT                                      */
   /* 30  INCOMPLETE INCOMPLETE          INCOMPLETE INCOMPLETE            */
   /***********************************************************************/
   /* DDS_SYM_FLAG flow between DDS_SYM entries       */
   /***************************************************/
   /*                                                 */
   /*                       parser                    */
   /*                         |                       */
   /*                         +------------+          */
   /*                         |            |          */
   /*                         | BASE       |          */
   /*                         | MODIFY     | STORE    */
   /*                         | ASUE       | EDIT     */
   /*                         | QUAL       |          */
   /*                         v            v          */
   /*                        TYPE       DECLARE       */
   /*                         |            |          */
   /*      +--------+---------+            |          */
   /*      |        |         |            |          */
   /*      | SUE    | ASUE    | ASUE       | STORE    */
   /*      |        | QUAL    | QUAL       |          */
   /*      v        v         v            |          */
   /*    BLOCK    MEMBER    OBJECT <-------+          */
   /*               |         |                       */
   /*               |         | LVALUE                */
   /*               | ASUE    | ASUE                  */
   /*               | QUAL    | QUAL                  */
   /*               |         | P/D/I                 */
   /*               |         v                       */
   /*               +------> EXPR <---+               */
   /*                         |       |               */
   /*                         +-------+               */
   /*                                                 */
   /***************************************************/

/*** "poor mans" inheritance scheme ***/
#define DDS_SYM_CLASS       /********************************************/ \
   DDS_ITEM_CLASS           /* hash table item symbol                   */ \
                            /*    (hash, name, chain, older, newer)     */ \
   int tag;                 /* API serial number for this symbol.       */ \
                            /*    Non-zero if exported by API.          */ \
                            /*    Subscript into bin->sym_tag stack.    */ \
   DDS_SYM_FLAG flag;       /* symbol table flags (see above)           */ \
   struct DDS_BLOCK *block; /* enclosing block/scope.                   */ \
                            /*    NULL, if dds_prime_block or block_old.*/ \
   struct DDS_TYPE *type;   /* type of this symbol.                     */ \
                            /*    If DDS_SYM_BLOCK is set, "type" is    */ \
                            /*       NULL, unless this block defines a  */ \
                            /*       struct, union, or enum.            */ \
                            /*    If DDS_SYM_TYPE is set, "type" is     */ \
                            /*       NULL, unless type is a pointer     */ \
                            /*       or an array. ??? what ???          */ \
                            /*    If DDS_SYM_OBJECT, MEMBER, or EXPR is */ \
                            /*       set, "type" is always non-NULL.    */ \
                            /********************************************/

typedef struct DDS_SYM
{  /* generic symbol table item (extended hash table entry) */
   DDS_SYM_CLASS            /* hash table item symbol (hash, name, chain, */
                            /*    older, newer, tag, flag, block, type) */
} DDS_SYM;

typedef struct DDS_SYM_TAG
{  /* symbol tag table for API */
   DDS_SYM *sym;
#if defined(_WIN32) || defined(MINGW)
   size_t offset;
#else
   long offset;
#endif
} DDS_SYM_TAG;

typedef struct DDS_BLOCK
{  /* "block" descriptor (DDS_SYM_BLOCK) */
   /******************************************************************/
   /* DDS_BLOCK begins a unique name space, i.e. function body,      */
   /* nested block, or members of a "struct", "union", or "enum".    */
   /* Local (nested) symbols are delimited by "first" and "last".    */
   /* The complete list is found via (nested) "older" and "newer".   */
   /* "first", "last", "local" and "size" are initialized when the   */
   /* parser exits a block.   They are used by "ddsFind*" functions. */
   /******************************************************************/
   DDS_SYM_CLASS        /* hash table item symbol (hash, name, chain,  */
                        /*    older, newer, tag, flag, block, type)    */
                        /*    "struct", "union", and "enum" blocks.    */
                        /*    Identifiers are in "tag" name space.     */
                        /*    "parameter", "function", and "statement" */
                        /*    blocks are always anonymous.             */
   DDS_SYM *first;      /* Link to first symbol within block.          */
   DDS_SYM *last;       /* Link to last symbol within block.           */
} DDS_BLOCK;

typedef struct DDS_RELOC
{  /* "relocation" descriptor for pointers within symbol tables. */
   void **ptrptr;       /* pointer to relocateable pointer   */
   size_t offset;       /* relative offset for this pointer  */
} DDS_RELOC;

/*** "poor mans" inheritance scheme ***/
#define DDS_VALUE_CLASS /************************************************/ \
   DDS_SYM_CLASS        /* hash table item symbol (hash, name, chain,   */ \
                        /*    older, newer, tag, flag, block, type,     */ \
                        /*    ptr, reloc)                               */ \
   void *ptr;           /* Pointer to storage for object/expr value.    */ \
                        /*   If this "ptr" changes dynamically, then    */ \
                        /*   references to the value are relocatable.   */ \
                        /*   Relocation is needed if DDS_SYM_OBJECT     */ \
                        /*   and RELOC are both set, or if              */ \
                        /*   DDS_SYM_EXPR, RELOC, and INHIBIT are set.  */ \
   DDS_STACK *reloc;    /* Array of DDS_RELOC (relocation descriptors). */ \
                        /*   NULL if nothing is dependent upon "ptr",   */ \
                        /*   or if dependencies are propagated upward.  */ \
                        /************************************************/

typedef struct DDS_VALUE
{  /* "expression" descriptor (DDS_SYM_EXPR) */
   DDS_VALUE_CLASS      /* hash table item symbol (hash, name, chain, */
                        /*    older, newer, tag, flag, block, type, */
                        /*    ptr, reloc) */
} DDS_VALUE;

typedef struct DDS_OBJECT
{  /* "object" descriptor (storage description) */
   DDS_VALUE_CLASS      /* hash table item symbol (hash, name, chain, */
                        /*    older, newer, tag, flag, block, type, */
                        /*    ptr, reloc) */
                        /*    Identifier name space. */
                        /*    Constant objects are anonymous. */
                        /*    If DDS_SYM_FUNC, function object. */
                        /*    func->older is return value DDS_TYPE. */
                        /*    func->newer is parameter DDS_BLOCK. */ 

         /* the following are needed to uncompile the symbol table */

   struct DDS_VALUE *track;/* initializer expression. */
} DDS_OBJECT;

typedef struct DDS_MEMBER
{  /* struct or union "field" descriptor */
   DDS_SYM_CLASS        /* hash table item symbol (hash, name, chain, */
                        /*    older, newer, tag, flag, block, type) */
                        /*    Bit fields may be named or anonymous. */
                        /*    Unique name space for each struct/union. */
#if defined(_WIN32) || defined(MINGW)
   size_t offset;
#else
   long offset;         /* relative position of value (byte offset).  */
                        /*    If union, offset == 0 by definition. */
#endif
   int adjust;          /* field adjustment (bit size or byte align). */
                        /*    -1 implies default adjustment. */
                        /*    Bit field size, if DDS_SYM_FIELD is set. */
   struct DDS_VALUE *value_b;/* "bias" initializer. */
} DDS_MEMBER;

typedef struct DDS_LABEL
{  /* "label" descriptor (DDS_SYM_LABEL) */
   DDS_SYM_CLASS        /* hash table item symbol (hash, name, chain, */
                        /*    older, newer, tag, flag, block, type) */
   struct DDS_BLOCK *func; /* DDS_BLOCK pointer */
} DDS_LABEL;

/***********************************************************************/
/*  Expression pointers reference values and args.                    */
/*  They are computed when compiling formats and map expressions.      */
/***********************************************************************/

typedef int DDS_OPCODE;

/*** "poor mans" inheritance scheme ***/
#define DDS_EXPR_CLASS     /**********************************************/ \
   DDS_VALUE_CLASS         /* hash table item symbol (hash, name, chain, */ \
                           /*    older, newer, tag, flag, block, type,   */ \
                           /*    ptr, reloc)                             */ \
                           /*    Expression name space (enumerations     */ \
                           /*    and fully qualified map defn names).    */ \
                           /*    Intermediate values are anonymous.      */ \
   DDS_OPCODE opcode;      /* operation code (+, -, *, /, ...)           */ \
                           /*    See "dds_opnote"                        */ \
   struct DDS_EXPR4 *next; /* next expression to evaluate.               */ \
                           /*    Order reflects expression dependency.   */ \
                           /*    Not threaded for evaluation if          */ \
                           /*    DDS_SYM_INHIBIT is set.                 */ \
                           /**********************************************/

typedef struct DDS_EXPR
{  /* "expression" descriptor (DDS_SYM_EXPR) */
   DDS_EXPR_CLASS          /* hash table item symbol (hash, name, chain, */
                           /*    older, newer, tag, flag, block, type, */
                           /*    ptr, reloc, opcode, next) */
} DDS_EXPR;

typedef struct DDS_EARG
{  /* "expression arg" descriptor */
   union {
#if defined(_WIN32) || defined(MINGW)
      size_t idx;
#else
      long idx;            /* if index */
#endif
      void *ptr;           /* if pointer */
   } value;
            /* the following is needed to uncompile symbol table */
   DDS_VALUE *track;       /* DDS_VALUE associated with operand */
                           /*    deviant usage if "sizeof(type_name)". */
} DDS_EARG;

typedef struct DDS_EXPR1
{  /* "expression" descriptor (DDS_SYM_EXPR) */
   DDS_EXPR_CLASS          /* hash table item symbol (hash, name, chain, */
                           /*    older, newer, tag, flag, block, type, */
                           /*    ptr, reloc, opcode, next) */
   DDS_EARG arg1;          /* 1st argument */
} DDS_EXPR1;

typedef struct DDS_EXPR2
{  /* "expression" descriptor (DDS_SYM_EXPR) */
   DDS_EXPR_CLASS          /* hash table item symbol (hash, name, chain, */
                           /*    older, newer, tag, flag, block, type, */
                           /*    ptr, reloc, opcode, next) */
   DDS_EARG arg1;          /* 1st argument */
   DDS_EARG arg2;          /* 2nd argument */
} DDS_EXPR2;

typedef struct DDS_EXPR3
{  /* "expression" descriptor (DDS_SYM_EXPR) */
   DDS_EXPR_CLASS          /* hash table item symbol (hash, name, chain, */
                           /*    older, newer, tag, flag, block, type, */
                           /*    ptr, reloc, opcode, next) */
   DDS_EARG arg1;          /* 1st argument */
   DDS_EARG arg2;          /* 2nd argument */
   DDS_EARG arg3;          /* 3rd argument */
} DDS_EXPR3;

typedef struct DDS_EXPR4
{  /* "expression" descriptor (DDS_SYM_EXPR) */
   DDS_EXPR_CLASS          /* hash table item symbol (hash, name, chain, */
                           /*    older, newer, tag, flag, block, type, */
                           /*    ptr, reloc, opcode, next) */
   DDS_EARG arg1;          /* 1st argument */
   DDS_EARG arg2;          /* 2nd argument */
   DDS_EARG arg3;          /* 3rd argument */
   DDS_EARG arg4;          /* 4th argument */
} DDS_EXPR4;

typedef struct DDS_EXPRA
{  /* assignment "expression" descriptor (DDS_SYM_EXPR) */
   DDS_EXPR_CLASS          /* hash table item symbol (hash, name, chain, */
                           /*    older, newer, tag, flag, block, type, */
                           /*    ptr, reloc, opcode, next) */
   DDS_EARG arg1;          /* 1st argument */
   DDS_EARG arg2;          /* 2nd argument */
   DDS_PTYPE stype1;       /* 1st argument, stream type */
   DDS_PTYPE stype2;       /* 2nd argument, stream type */
#if defined(_WIN32) || defined(MINGW)
   size_t count;
#else
   long count;             /* element count */
#endif
} DDS_EXPRA;

typedef struct DDS_EXPRC
{  /* copy op "expression" descriptor (DDS_SYM_EXPR) */
   DDS_EXPR_CLASS          /* hash table item symbol (hash, name, chain, */
                           /*    older, newer, tag, flag, block, type, */
                           /*    ptr, reloc, opcode, next) */
   DDS_EARG arg1;          /* 1st argument */
   DDS_EARG arg2;          /* 2nd argument */
   DDS_EARG arg3;          /* 3nd argument */
#if defined(_WIN32) || defined(MINGW)
   size_t lhs_bias;
   size_t rhs_bias;
#else
   long lhs_bias;          /* LHS starting byte offset */
   long rhs_bias;          /* RHS starting byte offset */
#endif
} DDS_EXPRC;

typedef struct DDS_CONDITION
{  /* DDS_CONDITIONAL_OP expression descriptor */
   DDS_EXPR *expr1;      /* condition thread */
   DDS_EXPR *expr2;      /* true  thread */
   DDS_EXPR *expr3;      /* false thread */
} DDS_CONDITION;

typedef struct DDS_TYPE
{  /* DDS "type" descriptor (DDS_SYM_TYPE) */
   DDS_SYM_CLASS           /* hash table item symbol (hash, name, chain, */
                           /*    older, newer, tag, flag, block, type) */
                           /*    Name space is not applicable. */
                           /*    The names of prime types correspond to */
                           /*    declaration keywords (other types are */
                           /*    anonymous). */
                           /*    Note deviant usage of the "block" link */
                           /*    when "ptype" is DDS_SYM_STRUCT or UNION. */
   DDS_PTYPE ptype;        /* DDS "prime" type ("C", stream, or DDS). */
                           /*    Binary types intrinsicly known to DDS. */
                           /*    If DDS_SYM_ENUM is set, ptype must be */
                           /*    DDS_CTYPE_INT or DDS_STYPE_INTEGER1, */
                           /*    2, 4, or 8 (enum, enum1, 2, 4, 8). */
                           /*    If ptype is DDS_STYPE_STRUCT or UNION, */
                           /*    or if DDS_SYM_ENUM is set, then */
                           /*    "block" link references the local */
                           /*    scope of the struct/union/enum. */
   int align;              /* minimum byte alignment (non-zero required). */
                           /*    By definition, prec % align == 0. */
                           /*    If DDS_SYM_ALIGN is set, "align(N)" */
                           /*    type qualifier overrode default. */
#if defined(_WIN32) || defined(MINGW)
   size_t prec;
   size_t count;
   size_t stroke;
#else
   long prec;              /* byte precision of type. */
                           /*    This includes all array elements */
                           /*    of *lower* dimensions (via "type" link). */
                           /*    Precision is *not* affected by the */
                           /*    "count" in *this* DDS_TYPE struct. */
   long count;             /* array element count, for one dimension. */
                           /*    Zero, iff DDS_SYM_ARRAY is set and */
                           /*    dimension is INCOMPLETE, i.e. "[]". */
                           /*    N, if DDS_SYM_ARRAY is set and "[N]". */
                           /*    One, if DDS_SYM_ARRAY is reset. */
   long stroke;            /* pre-computed (count * prec) */
#endif
         /* the following are needed to uncompile symbol table */

   DDS_VALUE *value_c;     /* "count" initializer. */
   DDS_VALUE *value_a;     /* "align" initializer. */
   DDS_OBJECT *trail;      /* if created via typedef. */
                           /*    Needed to reconstruct syntax. */
} DDS_TYPE;

typedef struct DDS_DECLARE
{  /* parser temporaries for nested declarations */
            /* Nested example: struct{union{...}foo;} bar; */
            /* Nested example: int foo[sizeof(float)];     */
   struct DDS_DECLARE *outer;   /* outer scope chain */
   struct DDS_DECLARE *inner;   /* inner scope chain */
   DDS_SYM_FLAG flag;           /* DDS_SYM_AUTO, REGISTER, */
                                /* EXTERN, TYPEDEF, IDENT */
   DDS_TYPE *type;              /* type being constructed */
   DDS_TYPE *more;              /* initial type declaration */
                                /*    needed for declarator lists */
                                /*    Example: int foo, *bar;  */
} DDS_DECLARE;


typedef enum DDS_SHAPE
{  /* left and right operand SHAPE. */
   DDS_SHAPE_NON,     /* non-conformable shapes */
   DDS_SHAPE_SUB,     /* sub-conformable arrays */
   DDS_SHAPE_S,       /* scalar */
   DDS_SHAPE_V,       /* vector */
   DDS_SHAPE_SS,      /* to scalar from vector */
   DDS_SHAPE_VV,      /* to vector from vector */
   DDS_SHAPE_VS,      /* to vector from scalar */
   DDS_SHAPE_SV,      /* to scalar from vector */
   DDS_SHAPE_MAX
} DDS_SHAPE;


typedef enum DDS_MISC_SYMBOL
{  /* miscellaneous constants, non-preprocessor and debugable */
   DDS_ALIGN_MAX = 32,  /* maximum allowed byte alignment */

   DDS_RELOC_MIN = 32,  /* minimum number of relocatable pointers */

   DDS_INTEGRAL_GENUS =
        (1 << DDS_TYPE_CHARS    | 1 << DDS_TYPE_UNSIGNEDS |
         1 << DDS_TYPE_INTEGERS),

   DDS_ARITHMETIC_GENUS =
        (1 << DDS_TYPE_CHARS    | 1 << DDS_TYPE_UNSIGNEDS |
         1 << DDS_TYPE_INTEGERS | 1 << DDS_TYPE_FLOATS),

   DDS_ARITHMETIC_PTR_GENUS =
        (1 << DDS_TYPE_CHARS    | 1 << DDS_TYPE_UNSIGNEDS |
         1 << DDS_TYPE_INTEGERS | 1 << DDS_TYPE_FLOATS    |
         1 << DDS_TYPE_PTRS),

   DDS_COMPLEX_GENUS =
        (1 << DDS_TYPE_CHARS    | 1 << DDS_TYPE_UNSIGNEDS |
         1 << DDS_TYPE_INTEGERS | 1 << DDS_TYPE_FLOATS    |
         1 << DDS_TYPE_COMPLEXS),

   DDS_COMPLEX_OR_PTR_GENUS =
        (1 << DDS_TYPE_CHARS    | 1 << DDS_TYPE_UNSIGNEDS |
         1 << DDS_TYPE_INTEGERS | 1 << DDS_TYPE_FLOATS    |
         1 << DDS_TYPE_COMPLEXS | 1 << DDS_TYPE_PTRS),

   DDS_COMPLEX_STRUCT_UNION_PTR_GENUS =
        (1 << DDS_TYPE_CHARS    | 1 << DDS_TYPE_UNSIGNEDS |
         1 << DDS_TYPE_INTEGERS | 1 << DDS_TYPE_FLOATS    |
         1 << DDS_TYPE_COMPLEXS | 1 << DDS_TYPE_STRUCTS   |
         1 << DDS_TYPE_UNIONS   | 1 << DDS_TYPE_PTRS),

   DDS_STRUCT_OR_UNION_GENUS =
        (1 << DDS_TYPE_STRUCTS  | 1 << DDS_TYPE_UNIONS),

   DDS_HOST_CTYPE = (             1 << DDS_CTYPE_CHAR    |
         1 << DDS_CTYPE_SCHAR   | 1 << DDS_CTYPE_UCHAR   |
         1 << DDS_CTYPE_SHORT   | 1 << DDS_CTYPE_INT     |
         1 << DDS_CTYPE_LONG    | 1 << DDS_CTYPE_LLONG   |
         1 << DDS_CTYPE_USHORT  | 1 << DDS_CTYPE_UINT    |
         1 << DDS_CTYPE_ULONG   | 1 << DDS_CTYPE_ULLONG  |
         1 << DDS_CTYPE_FLOAT   | 1 << DDS_CTYPE_DOUBLE  |
         1 << DDS_CTYPE_LDOUBLE | 1 << DDS_CTYPE_CPX     |
         1 << DDS_CTYPE_DBLCPX  | 1 << DDS_CTYPE_LDBLCPX |
         1 << DDS_CTYPE_PTR),

   /* mask for class DDS_SYM */
   DDS_SYM_GENUS = (DDS_SYM_BLOCK | DDS_SYM_OBJECT | DDS_SYM_MEMBER |
      DDS_SYM_TYPE | DDS_SYM_EXPR | DDS_SYM_LABEL),

   /* mask for type storage qualifier flags. */
   DDS_SYM_STORE = (DDS_SYM_AUTO | DDS_SYM_REGISTER | DDS_SYM_STATIC |
      DDS_SYM_EXTERN | DDS_SYM_TYPEDEF),

   /* mask for type struct/union/enum flags. */
   DDS_SYM_SUE = (DDS_SYM_STRUCT | DDS_SYM_UNION | DDS_SYM_ENUM),

   /* mask for type array/struct/union/enum flags. */
   DDS_SYM_ASUE = (DDS_SYM_STRUCT | DDS_SYM_UNION | DDS_SYM_ENUM |
      DDS_SYM_ARRAY),

   /* mask for type base flags. */
   DDS_SYM_BASE = (DDS_SYM_VOID | DDS_SYM_CHAR | DDS_SYM_INT |
      DDS_SYM_FLOAT | DDS_SYM_DOUBLE | DDS_SYM_COMPLEX),

   /* mask for type modifier flags. */
   DDS_SYM_MODIFY = (DDS_SYM_SHORT | DDS_SYM_LONG | DDS_SYM_LLONG |
      DDS_SYM_SIGNED | DDS_SYM_UNSIGNED),

   /* mask for type qualifier flags. */
   DDS_SYM_QUAL = (DDS_SYM_CONST | DDS_SYM_VOLATILE | DDS_SYM_ALIGN),

   /* mask for preprocessor constants. */
   DDS_SYM_PREP = (DDS_SYM_CONST_INIT | DDS_SYM_CONST_DECL |
      DDS_SYM_CONST_PREP),

   /* mask for declaration constants. */
   DDS_SYM_DECL = (DDS_SYM_CONST_INIT | DDS_SYM_CONST_DECL),

   /* mask for initializer constants. */
   DDS_SYM_INIT = (DDS_SYM_CONST_INIT),

   /* mask for member editing flags. */
   DDS_SYM_EDIT = (DDS_SYM_DELETE | DDS_SYM_REPLACE | DDS_SYM_APPEND |
      DDS_SYM_DAPPEND | DDS_SYM_RAPPEND),

   /* mask for type specifier flags. */
   DDS_SYM_SPEC = (DDS_SYM_BASE | DDS_SYM_MODIFY | DDS_SYM_ASUE),

   /* mask for objects that require explicit relocation */
   DDS_RELOCATE_OBJECT = (DDS_SYM_OBJECT | DDS_SYM_RELOC),

   /* mask for expressions that require explicit relocation */
   DDS_RELOCATE_EXPR = (DDS_SYM_EXPR | DDS_SYM_RELOC),

   /* mask for inhibited expressions that require explicit relocation */
   DDS_RELOCATE_INHIBIT = (DDS_SYM_EXPR | DDS_SYM_RELOC | DDS_SYM_INHIBIT)

} DDS_MISC_SYMBOL;


typedef enum DDS_OPNOTE_FLAG
{  /* opcode flags */
   DDS_OPNOTE_S        =   0,  /* scalar */
   DDS_OPNOTE_V        =   1,  /* vector */
   DDS_OPNOTE_SS       =   2,  /* scalar-scalar */
   DDS_OPNOTE_VV       =   3,  /* vector-vector */
   DDS_OPNOTE_VS       =   4,  /* vector-scalar */
   DDS_OPNOTE_SV       =   5,  /* scalar-vector ? reduction ? */
   DDS_OPNOTE_SHAPE    = 0xff, /* mask for S, V, SS, VV, VS, SV flags */
   DDS_OPNOTE_ARG1     = 1 <<  8,  /* arg1 is relocatable */
   DDS_OPNOTE_ARG2     = 1 <<  9,  /* arg2 is relocatable */
   DDS_OPNOTE_ARG3     = 1 << 10,  /* arg3 is relocatable */
   DDS_OPNOTE_ARG4     = 1 << 11,  /* arg4 is relocatable */
   DDS_OPNOTE_RELOC    =   0xf00   /* mask for relocation */
} DDS_OPNOTE_FLAG;
 
#define ARGXXXX (0)
#define ARG1XXX (DDS_OPNOTE_ARG1)
#define ARG12XX (DDS_OPNOTE_ARG1 | DDS_OPNOTE_ARG2)
#define ARG123X (DDS_OPNOTE_ARG1 | DDS_OPNOTE_ARG2 | DDS_OPNOTE_ARG3)
#define ARGX2XX (DDS_OPNOTE_ARG2)
 
typedef struct DDS_OPNOTE
{  /* opcode annotation */
   const char *name;      /* enumeration symbol */
   DDS_OPNOTE_FLAG flag;  /* bit flags */
   int args;              /* number of args */
   int precedence;        /* operator percedence */
   const char *prefix;    /* prefix lexical symbol */
   const char *infix;     /* infix  lexical symbol */
   const char *suffix;    /* suffix lexical symbol */
} DDS_OPNOTE;
 

typedef struct DDS_DUMP_ITEM
{  /* "field" descriptor in tabular dump list */
   DDS_TYPE *type;    /* field type. */
                      /*    zero, iff field doesn't exist */
                      /*    in the default "trace" type. */
#if defined(_WIN32) || defined(MINGW)
   size_t offset;
#else
   long offset;       /* byte offset within buffer to field. */
#endif
} DDS_DUMP_ITEM;

typedef enum DDS_LEX_START
{  /* lex start state changes */
   DDS_LEX_CONTINUE,  /* continue (current lex processing state) */
   DDS_LEX_SHARED,    /* begin INITIAL   state (shared processing) */
                      /*    (identifier_list, constant_expression) */
   DDS_LEX_TRANSLATE, /* begin INITIAL   state (return TRANSLATION_UNIT) */
   DDS_LEX_DEFN,      /* begin INITIAL   state (return DEFN_UNIT) */
   DDS_LEX_IDENT,     /* begin INITIAL   state (return IDENT_UNIT) */
   DDS_LEX_MAP        /* begin INITIAL   state (return MAP_EXPR) */
} DDS_LEX_START;

typedef struct DDS_SCANNER
{  /* scanner input stack (analogous to nested "#include"). */
   struct DDS_SCANNER *outer;   /* outer scope chain */
   struct DDS_SCANNER *inner;   /* inner scope chain */
   DDS_BOOLEAN leof;            /* logical end-of-file */
   const char *dict;            /* input dictionary name */
   const char *defn;            /* input definition name */
   const char *start;           /* starting input character */
   const char *next;            /* next input character */
   const char *stop;            /* last + 1 input character */
   int line;                    /* starting line within dictionary */
   void *yy_buf;                /* flex buffer state YY_BUFFER_STATE */
} DDS_SCANNER;

typedef enum DDS_CON_STATUS
{  /* conversion status return code */
   DDS_CON_OK=0,
   DDS_CON_BUG,
   DDS_CON_ERROR,
   DDS_CON_OVERFLOW
} DDS_CON_STATUS;


/*****************************************/
/* extern global variables and constants */
/*****************************************/

/* temporary for PUSH_SYM and PUSH_VALUE macro */
extern DDS_SYM *dds_push_sym;

/* current declaration scope */
extern DDS_DECLARE *dds_declare;

/* Graveyard for unused pointers computed by expressions. */
extern void **DDS_PTR_CEMETERY;

/* Used to evaluate one conversion */
extern DDS_EXPRA DDS_CON_EXPRA;

/* Opcode descriptor table */
extern const DDS_OPNOTE *dds_opnote;

/* DDS_TRUE iff DDS_ASSIGN_OP is effectively DDS_COPY_OP. */
extern DDS_BOOLEAN dds_copy_op
   [DDS_STYPE_MAX - DDS_STYPE_MIN]
   [DDS_STYPE_MAX - DDS_STYPE_MIN];

/* parser keyword hash table */
extern DDS_TABLE dds_keyword;

/* pointer to active symbol table. */
extern DDS_TABLE *dds_symbol;

/* pointer to current block in active symbol table. */
extern DDS_BLOCK *dds_block;

/* Pointer to first and last expression to evaluate. */
/* All expressions are initially chained onto this list. */
/* The list is evaluated at compile time to obtain constants. */
/* Its moved to DDS_MAP, when compiling map expressions. */
extern DDS_EXPR *dds_thread;
extern DDS_EXPR *dds_needle;

/* lex start state */
extern DDS_LEX_START dds_lex_ss;

/* prime block for in symbol table */
extern DDS_BLOCK   *dds_prime_block;
extern DDS_OBJECT  *dds_nop_object;

/* error recovery entries in prime symbol table */
extern DDS_BLOCK   *dds_error_block;
extern DDS_TYPE    *dds_error_type;
extern DDS_MEMBER  *dds_error_member;
extern DDS_VALUE   *dds_error_value;
extern DDS_OBJECT  *dds_error_object;
extern DDS_EXPR    *dds_error_expr;
extern DDS_EXPR    *dds_error_thread;

/* history dictionary for compiler "hist:" references */
extern const char *dds_hist;
 
/* list of shape names */
extern const char *dds_shape_name[DDS_SHAPE_MAX];

/* Handle into scanner state */
extern DDS_SCANNER *dds_lexer;
extern DDS_SCANNER dds_lexer_root;

/***********************/
/* function prototypes */
/***********************/

DDS_BLOCK * ddsSymBlockBegin(
   const char *name,
   DDS_SYM_FLAG flag,
   DDS_TYPE *merge_type)
;
void ddsSymBlockEnd(void)
;
DDS_OBJECT * ddsSymEnumName(
   const char *name)
;
DDS_OBJECT * ddsSymEnumValue(
   DDS_OBJECT *object,
   DDS_VALUE *value)
;
DDS_VALUE * ddsSymExpr0A(
   const char *name,
   DDS_OPCODE opcode)
;
DDS_VALUE * ddsSymExpr1A(
   const char *name,
   DDS_OPCODE opcode,
   DDS_VALUE *value1)
;
DDS_VALUE * ddsSymExpr1A2(
   const char *name)
;
DDS_VALUE * ddsSymExpr1A3(
   const char *name,
   DDS_OPCODE opcode,
   DDS_TYPE *type1)
;
DDS_VALUE * ddsSymExpr1A4(
   const char *name,
   DDS_VALUE *value)
;
DDS_VALUE * ddsSymExpr1A1(
   const char *name,
   DDS_OPCODE opcode,
   DDS_VALUE *value1)
;
DDS_VALUE * ddsSymExpr2A(
   const char *name,
   DDS_OPCODE opcode,
   DDS_VALUE *value1,  /* lvalue */
   DDS_VALUE *value2)  /* rvalue */
;
DDS_VALUE * ddsSymExpr2B(
   const char *name,
   DDS_OPCODE opcode,
   DDS_VALUE *value1,
   DDS_VALUE *value2)
;
DDS_VALUE * ddsSymExpr2C(
   const char *name,
   DDS_OPCODE opcode,
   DDS_VALUE *value1,
   const char *name2)
;
DDS_VALUE * ddsSymExpr2D(
   const char *name,
   DDS_OPCODE opcode,
   DDS_VALUE *value1,
   DDS_VALUE *value2)
;
DDS_VALUE * ddsSymExpr3A(
   const char *name,
   DDS_OPCODE opcode,
   DDS_VALUE *value1,
   DDS_VALUE *value2,
   DDS_VALUE *value3,
   DDS_EXPR *needle1,
   DDS_EXPR *needle2,
   DDS_EXPR *needle3)
;
void ddsSymValuePtr(
   void **reloc_ptr,
   DDS_VALUE *value)
;
DDS_EXPR * ddsSymExprDone(
   DDS_EXPR *expr)
;
void ddsSymRelocate(void)
;
void ddsSymRelocValue(
   DDS_VALUE *value)
;


DDS_SYM * ddsSymLookup(
   const DDS_TABLE *table,
   const char *name,
   DDS_SYM_FLAG sym_flag,
   DDS_BLOCK *first_block,
   DDS_BLOCK *last_block)
;

#if defined(_WIN32) || defined(MINGW)
DDS_MEMBER * ddsSymLookupMem(
	const DDS_TABLE *table,
	const char *name,
	DDS_BLOCK *block,
	size_t *bias)
;
#else
DDS_MEMBER * ddsSymLookupMem(
   const DDS_TABLE *table,
   const char *name,
   DDS_BLOCK *block,
   long *bias)
;
#endif

DDS_VALUE * ddsSymObjectLit(
   const char *name,
   DDS_SYM_FLAG flag,
   DDS_TYPE *copy_type,
   void *value_ptr,
   size_t count)
;
DDS_MEMBER * ddsSymMember(
   const char *name)
;
DDS_MEMBER * ddsSymMemberBit(
   DDS_MEMBER *member,
   int adjust)
;
void ddsSymMemberEdit(
   DDS_SYM_FLAG flag,
   DDS_MEMBER *member)
;
DDS_BOOLEAN ddsSymNullValue(
   DDS_VALUE *value)
;
DDS_BOOLEAN ddsSymSameType(
   DDS_TYPE *type1,
   DDS_TYPE *type2)
;
DDS_TYPE * ddsSymTypeCopy(
   DDS_TYPE *copy_type)
;
DDS_OBJECT * ddsSymTypedefObj(
   const char *name,
   DDS_PTYPE ptype,
   int align)
;
DDS_BLOCK * ddsSymTypedefSB(void)
;
DDS_OBJECT * ddsSymTypedefSE(
   DDS_BLOCK *block,
   const char *name)
;
void ddsSymTypedefSM(
   const char *name,
   DDS_OBJECT *object,
   long count)
;
DDS_BLOCK * ddsSymTypedefEB(
   DDS_PTYPE ptype)
;
void ddsSymTypedefEV(
   const char *name,
   int value)
;
void ddsSymTypedefEE(
   DDS_BLOCK *block,
   const char *name)
;
DDS_TYPE * ddsSymTypeArray(
   DDS_VALUE *value)
;
DDS_VALUE * ddsSymTypeCast(
   DDS_SYM_FLAG expr_flag,
   DDS_TYPE *type,
   DDS_VALUE *value1)
;
DDS_OBJECT * ddsSymDeclare(
   const char *name)
;
DDS_TYPE * ddsSymTypeMerge(
   DDS_TYPE *merge)
;
DDS_TYPE * ddsSymTypeQual(
   DDS_SYM_FLAG flag,
   DDS_VALUE *value)
;
DDS_SHAPE ddsSymTypeShape(
   DDS_TYPE **type1a)
;
DDS_SHAPE ddsSymTypeShape2(
   DDS_TYPE **type1a,
   DDS_TYPE **type2a)
;
DDS_TYPE * ddsSymTypeSpec(
   DDS_SYM_FLAG flag,
   DDS_SYM_FLAG mask)
;
DDS_TYPE * ddsSymTypeStore(
   DDS_SYM_FLAG flag)
;
DDS_TYPE * ddsSymTypeSUE(
   DDS_BLOCK *sue_block)
;
DDS_BOOLEAN ddsSymConstType(
   DDS_TYPE *type)
;


void ddsLexStr(
   DDS_BOOLEAN leof,
   const char *dict,
   const char *defn,
   const char *start,
   const char *stop,
   size_t line)
;
 
 
void ddsDefnValue(
   DDS_VALUE *dict_value,
   DDS_VALUE *defn_value)
;


void ddsEvaluate(
   DDS_EXPR *expr)
;

void ddsDate2Int(
   void *dateint,
   void *datestruct)
;

void ddsDate2Struct(
   void *datestruct,
   double *dateint)
; 
 
int ddsSymConstInt(
   DDS_VALUE *expr,
   int value,
   DDS_SYM_FLAG mask)
;
HOST_LLONG_T ddsSymConstLLong(
   DDS_VALUE *expr,
   HOST_LLONG_T value,
   DDS_SYM_FLAG mask)
;
long ddsSymConstLong(
   DDS_VALUE *expr,
   long value,
   DDS_SYM_FLAG mask)
;
size_t ddsSymConstSizet(
   DDS_VALUE *expr,
   size_t value,
   DDS_SYM_FLAG mask)
;
unsigned ddsSymConstUInt(
   DDS_VALUE *expr,
   unsigned value,
   DDS_SYM_FLAG mask)
;
HOST_ULLONG_T ddsSymConstULLong(
   DDS_VALUE *expr,
   HOST_ULLONG_T value,
   DDS_SYM_FLAG mask)
;
unsigned long ddsSymConstULong(
   DDS_VALUE *expr,
   unsigned long value,
   DDS_SYM_FLAG mask)
;


int ddsYYparse(
   void *)
;

const char * ddsSymIdent(
   char *str)
;


#endif /* __DDS_SYMBOL_H */
