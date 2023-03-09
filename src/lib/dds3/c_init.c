static char rcsid[]="$Id: c_init.c 50 2009-02-10 21:29:01Z user $";
/*
RCS-t=DDS API, initialize DDS root control structure.

*/

/**********************************************/
/***   Copyright (C) 1996,2009              ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/***   Jerry Ehlers,  20 Jan 2009           ***/
/**********************************************/

#include <limits.h>
#if defined(_WIN32) || defined(MINGW)
/*
#include "utsname.h"
*/
#else
#include <pwd.h>
#include <sys/utsname.h>
#endif
#include <time.h>
#include <sys/stat.h>

#include "dds_sys.h"
#include "dds_version.h"
#include "dds_opcode.h"
#include "dds_dump.h"
#include "parser.tab.h"

/* Initial size for binary descriptor array */
#define BIN_ARRAY_SIZE  64
 
/* Initial size for temporary stack. */
#define TMP_STACK_SIZE (32 * DDS_KB)

#define NOM_TOKEN_MAX (2 * DDS_KB)

typedef int GRAPH_CONCUR[DDS_STYPE_MAX][DDS_STYPE_MAX];

GRAPH_CONCUR *graph_concur = (GRAPH_CONCUR*)&dds_concur;

static void PushKey(
   const char *name,
   int token)
{
   DDS_KEYWORD *key;

   key = (DDS_KEYWORD*)ddsTablePushItem(&dds_keyword, name,
      sizeof(DDS_KEYWORD));
   key->token = token;
   return;
}

static DDS_TYPE * prime(
   const char *name,
   DDS_PTYPE ptype,
   size_t prec,
   size_t align)
{
   DDS_TYPE *type;
   SYM_TAG tag;

   type = PUSH_TYPE(&dds_prime_table, name, (DDS_SYM_FLAG)0,
      dds_prime_block, NULL);
   type->ptype = ptype;
   type->align = align;
   type->prec = prec;
   type->count = 1;
   type->stroke = prec;

   SYM_TO_SYM_TAG(tag, &dds_prime_tag, type)

   return type;
}

static void ddsCleanup(void)
{  /* ddsCleanup is executed when application calls "exit" */
   int n;
   DDS_BIN *bin;
   DDS_DICT *dict, *dict_newer;
   DDS_MEDIA *media, *media_right;
   DDS_ALIAS *alias, *alias_right;
   BIN_TAG bin_tag;

   /* check for cleanup recursion */
   if(dds_state & DDS_CLEANUP) {
      /* panic or cleanup already in progress ? */
      return;
   }

   /* prevent recursive cleanup, and aborts during cleanup */
   dds_state = (DDS_STATE)(DDS_CLEANUP |
      (dds_state & ~ (DDS_WARN_ABORT | DDS_ERROR_ABORT | DDS_DEBUG_ABORT)));

   /* shutdown dictionary scans and prints */
   ddsScanDone();
   ddsPrintDone();

   /* reset (close) all open dictionaries */
   for(dict = (DDS_DICT*)dds_dict.oldest;
      dict;
      dict = dict_newer) {
      dict_newer = (DDS_DICT*)dict->newer;
      if(dict->state & DDS_DICT_ACTIVE) ddsDictDelete(dict);
   }

   /* close all open binaries */
   for(bin_tag = 0; bin_tag < dds_bin_max; bin_tag++) {
      bin = dds_bin[bin_tag];
      if(bin) ddsCloseBin(bin);
   }

   /* delete all media descriptors */
   for(media = dds_media_list; media; media = media_right) {
      media_right = media->right;
      ddsMediaDelete(media);
   }

   /* free storage ... */
   for(alias = dds_alias_free; alias; alias = alias_right) {
      alias_right = alias->right;
      ddsStrFree(&alias->name);
      ddsFree(alias);
   }

   ddsFree(dds_bin);

   ddsFree((void*)dds_cmd_name);
   ddsFree((void*)dds_cmd_rev);
   ddsFree((void*)dds_cmd_ddsrev);
   ddsFree((void*)dds_cmd_ddsdat);
   ddsFree((void*)dds_cmd_user);
   ddsFree((void*)dds_cmd_date);
   ddsFree((void*)dds_cmd_host);
   ddsFree((void*)dds_cmd_cwd);
   ddsFree((void*)dds_cmd_home);

   if(dds_project_dict) ddsFree((void*)dds_project_dict);
   if(dds_ddsrc_dict) ddsFree((void*)dds_ddsrc_dict);
   ddsFree((void*)dds_base_dict);
   ddsStrFree(&dds_path);

   ddsTableTerm(&dds_dict);

   ddsStrFree(&dds_scan_token.str);

   ddsTableTerm(&dds_keyword);

   ddsFree(dds_map);

   ddsFree(dds_prime);

   ddsStrFree(&dds_warn_msg);
   ddsStrFree(&dds_error_msg);
   ddsStrFree(&dds_debug_msg);

   ddsStrFree(&dds_data_path);
   ddsStrFree(&dds_data_suffix);
         
   if (dds_dump_dir) {
      for(n=0;n<dds_dump_max;n++) ddsFree((void*)dds_dump_dir[n].name);
      ddsFree(dds_dump_dir);
   }

   free(dds_heap.ptr);

   return;
}


int (cdds_init)(void)
{  /* initialize global variables (one time only) */
   DDS_STRING tmp_string = NULL_STRING;
   DDS_TOKEN value_save = NULL_TOKEN;
   DDS_PTYPE ptype, ptype1, ptype2;
   DDS_PTYPE ctype, ctype1, ctype2, concur;
   int status = DDS_OK;
   int n;
   char *ctmp;
   const char *value, *thing;
   const char *dump_alias;
   size_t global_size;

   time_t calendar_time;
#if !defined(_WIN32) && !defined(MINGW)
   struct utsname name;
#endif
   STRUCTSTAT stat_buf;

   static DDS_HEAP global_fix[3];

   dds_print_fp = stderr;

   /* one time only initialization */
   if(dds_initialized) {
      /* check the heap to see if its been corrupted */
      ddsHeapCheck(NULL);
      if(dds_debug_api & DDS_DEBUG_INIT) {
         dds_debug_api &= (DDS_DEBUG) ~ DDS_DEBUG_INIT;
         ddsDebug("cdds_init: flaky DDS_DEBUG flag\n");
      }
      return DDS_OK;
   }

   dds_debug_api &= (DDS_DEBUG) ~ DDS_DEBUG_INIT;

   /* ensure "dump" linkage (with debugger), without execution */
   ddsDumpDataBuf(NULL, NULL, NULL, NULL);
   ddsDumpBinary();
   ddsDumpS(NULL);
   ddsDumpSym(0, NULL);
   ddsDumpBinSym(0, 0);
   ddsDumpSymbol();
   ddsDumpGlobal();
   ddsDumpMapIn(0, 0);
   ddsDumpMapOut(0, 0);
   ddsDumpStack(0, NULL);
   ddsDumpDataType(0, NULL, 0, NULL, NULL, NULL);
   ddsDumpDataBlock(0, NULL, 0, NULL, NULL, NULL);
   ddsDumpDataArray(0, NULL, 0, NULL, NULL, NULL);
   ddsDumpDataDiff(0, NULL, 0, NULL, NULL, NULL);
   ddsDumpDataValue(0, 0, NULL, NULL);
   ddsDumpDataOpen(NULL);
   ddsDumpDataList(NULL, NULL, NULL, NULL);
   ddsDumpBlock(0, NULL);
   ddsDumpBlockR(DDS_FALSE, 0, NULL);
   ddsDumpBlockRS(0, NULL);
   ddsDumpExpr(0, NULL);
   ddsDumpMember(0, NULL);
   ddsDumpItem(0, NULL);
   ddsDumpObject(0, NULL);
   ddsDumpSymFlag(0, NULL);
   ddsDumpTable(0, NULL);
   ddsDumpPage(0, NULL);
   ddsDumpScanner(NULL);
   ddsDumpDeclare(0, NULL);
   ddsDumpType(0, NULL);

   dds_initialized = DDS_TRUE;

   /* assert correctness of configuration */
#if !defined(_WIN32) && !defined(MINGW)
   assert(sizeof(HOST_PTRDIFF_T_T) == sizeof(HOST_LONG_T));
   assert(sizeof(HOST_SIZE_T_T) == sizeof(HOST_ULONG_T));
#endif
   assert(DDS_TYPE_GENUS_MAX < CHAR_BIT * sizeof(DDS_MISC_SYMBOL));
   assert(DDS_ASSIGN_OP == 0);

   /* Initialize heap validation stack */

   dds_heap.top = 0;
   dds_heap.max = 2000 * sizeof(DDS_HEAP);
   dds_heap.ptr = malloc(dds_heap.max);

   /* synthesize heap descriptors surrounding global data */
   dds_heap.top += sizeof(DDS_HEAP);
   global_size = (char*)&global_fix[2] - (char*)&global_fix[0];
   global_fix[0].ptr = &global_fix[0];
   global_fix[0].size = global_size + DDS_HEAP_CREEP;
   global_fix[0].idx = 0;
   global_fix[0].ptr2 = global_fix[0].ptr;
   global_fix[0].size2 = global_fix[0].size;
   global_fix[0].idx2 = global_fix[0].idx;

   memcpy((void*)&global_fix[2], (void*)&global_fix[0], sizeof(DDS_HEAP));
   memcpy((void*)dds_heap.ptr,    (void*)&global_fix[0], sizeof(DDS_HEAP));

   /***********************************************************/
   /*  Initialize buffers used by message utilities.          */
   /*  This is a "crash" window because ddsStrSetMin could    */
   /*  fail and attempt to report it before things are ready. */
   /***********************************************************/

   ddsStrSetMin(&dds_warn_msg, "", DDS_MSG_MAX);
   ddsStrSetMin(&dds_error_msg, "", DDS_MSG_MAX);
   ddsStrSetMin(&dds_debug_msg, "", DDS_MSG_MAX);

   /***********************************************************/
   /*  OK, message utilities now have the buffers they need.  */
   /*  Finish initializing globals, if not already done.      */
   /***********************************************************/

   DDS_API_ENTRY("init")

   ddsStrSetMin(&dds_scan_token.str, "", NOM_TOKEN_MAX);

   ddsStackInit(&dds_tmp, TMP_STACK_SIZE);

   ddsStrSet(&dds_data_path, "DATA_PATH data_path");
   ddsStrSet(&dds_data_suffix, "DATA_SUFFIX data_suffix");

   /* process id number */
   dds_cmd_pid = getpid();

   /* command name used to invoke this process */
   thing = GetArgCmdV(0);
   if(! thing) thing = "<cmd_name>";
   dds_cmd_name = ddsString(thing);

   /* command revision (setup in c_print.c */
   dds_cmd_rev = ddsString("<cmd_rev>");

   /* dds revision */
   ctmp = (char*)ddsString(dds_rcs_rev);
   ctmp[strlen(ctmp)-1] = '\0';
   dds_cmd_ddsrev = ddsString(ctmp+6);
   ddsFree((void*)(ctmp));

   /* dds date */
   ctmp = (char*)ddsString(dds_rcs_date);
   ctmp[strlen(ctmp)-1] = '\0';
   dds_cmd_ddsdat = ddsString(ctmp+7);
   ddsFree((void*)(ctmp));

   /* user name */

#if defined(_WIN32) || defined(MINGW)
   thing = getenv("LOGNAME");
#else
   thing = getlogin();
#endif   
   if(! thing) thing = "<user_name>";
   dds_cmd_user = ddsString(thing);

   /* determine current calendar time */
   calendar_time = time(NULL);
   thing = ctime(&calendar_time);
   if(! thing || ! *thing) thing = "<date>\n";
   dds_cmd_date = ddsStringN(thing, strlen(thing) - 1);

   /* host name */
#if defined(_WIN32) || defined(MINGW)
   thing = getenv("USERNAME");
   if (!thing) {
      ddsWarn("init: uname call, errno= %i\n", errno);
      thing = "<hostname>";
   }
#else
   if (0 <= uname(&name)) {
      thing = name.nodename;
   } else {
      ddsWarn("init: uname call, errno= %i\n", errno);
      thing = "<hostname>";
   }
#endif

   dds_cmd_host = ddsString(thing);

   /* user's current working directory */
   ddsStrSetMin(&tmp_string, "<cwd>", (PATHNAME_MAX + 1));
   if (!getcwd(tmp_string.ptr, (size_t)PATHNAME_MAX)) {
      ddsWarn("init: getcwd call, errno= %i\n",errno);
   }
   dds_cmd_cwd = ddsString(ddsPathClean(tmp_string.ptr));

   /* initialize dictionary hash table. */
   ddsTableInit(&dds_dict, DICT_HASH_SIZE,
      (DICT_HASH_ALPHA * DICT_HASH_SIZE * sizeof(DDS_DICT)),
      DICT_HASH_ALPHA, DICT_HASH_BETA);

   /* initialize binary descriptor table */
   dds_bin_max = BIN_ARRAY_SIZE;
   dds_bin = (DDS_BIN**)ddsZalloc(dds_bin_max * sizeof(DDS_BIN*));

   dds_declare = (DDS_DECLARE*)ddsZalloc(sizeof(DDS_DECLARE));

   /* initialize map descriptor table */
   dds_map = (DDS_MAP**)ddsZalloc(dds_bin_max * dds_bin_max * sizeof(DDS_MAP*));

   /* lex-yacc keyword tokens */
   ddsTableInit(&dds_keyword, KEYS_HASH_SIZE,
      (KEYS_HASH_ALPHA * KEYS_HASH_SIZE * sizeof(DDS_KEYWORD)),
      KEYS_HASH_ALPHA, KEYS_HASH_BETA);

   PushKey("ident"             , IDENT);
   PushKey("defn_value"        , DEFN_VALUE);
   PushKey("align"             , ALIGN);

   PushKey("if"                , IF);
   PushKey("else"              , ELSE);
   PushKey("do"                , DO);
   PushKey("while"             , WHILE);
   PushKey("for"               , FOR);
   PushKey("switch"            , SWITCH);
   PushKey("case"              , CASE);
   PushKey("break"             , BREAK);
   PushKey("continue"          , CONTINUE);
   PushKey("default"           , DEFAULT);
   PushKey("goto"              , GOTO);
   PushKey("return"            , RETURN);

   PushKey("static"            , STATIC);
   PushKey("extern"            , EXTERN);
   PushKey("const"             , CONST);
   PushKey("volatile"          , VOLATILE);
   PushKey("auto"              , AUTO);
   PushKey("register"          , REGISTER);

   PushKey("void"              , VOID);
   PushKey("char"              , CHAR);
   PushKey("signed"            , SIGNED);
   PushKey("unsigned"          , UNSIGNED);
   PushKey("short"             , SHORT);
   PushKey("int"               , INT);
   PushKey("long"              , LONG);
   PushKey("float"             , FLOAT);
   PushKey("double"            , DOUBLE);
   PushKey("complex"           , COMPLEX);

   PushKey("typedef"           , TYPEDEF);
   PushKey("enum"              , ENUM);
   PushKey("struct"            , STRUCT);
   PushKey("union"             , UNION);

   PushKey("sizeof"            , SIZEOF);
   PushKey("axis_size"         , AXIS_SIZE);
   PushKey("exist"             , EXIST);
   PushKey("nop"               , NOP);
   PushKey("field"             , FIELD);
   PushKey("ordinal"           , ORDINAL);
   PushKey("sqrt"              , SQRT);
   PushKey("sin"               , SIN);
   PushKey("cos"               , COS);
   PushKey("tan"               , TAN);
   PushKey("asin"              , ASIN);
   PushKey("acos"              , ACOS);
   PushKey("atan"              , ATAN);
   PushKey("atan2"             , ATAN2);
   PushKey("exp"               , EXP);
   PushKey("log"               , LOG);
   PushKey("log10"             , LOG10);
   PushKey("pow"               , POW);
   PushKey("abs"               , ABS);
   PushKey("sign"              , SIGN);
   PushKey("ceil"              , CEIL);
   PushKey("floor"             , FLOOR);
   PushKey("dateint"           , DATEINT);
   PushKey("datestruct"        , DATESTRUCT);
   PushKey("linetrc"           , LINE_TRC);
   PushKey("reeltrc"           , REEL_TRC);

   PushKey("asm"               , ASM);
   PushKey("fortran"           , FORTRAN);

   PushKey("ascii1"            , ASCII1);
   PushKey("ebcdic1"           , EBCDIC1);
   PushKey("integer1"          , INTEGER1);
   PushKey("integer2"          , INTEGER2);
   PushKey("integer2x"         , INTEGER2X);
   PushKey("integer4"          , INTEGER4);
   PushKey("integer4x"         , INTEGER4X);
   PushKey("integer8"          , INTEGER8);
   PushKey("integer8x"         , INTEGER8X);
   PushKey("unsigned1"         , UNSIGNED1);
   PushKey("unsigned2"         , UNSIGNED2);
   PushKey("unsigned2x"        , UNSIGNED2X);
   PushKey("unsigned4"         , UNSIGNED4);
   PushKey("unsigned4x"        , UNSIGNED4X);
   PushKey("unsigned8"         , UNSIGNED8);
   PushKey("unsigned8x"        , UNSIGNED8X);
   PushKey("float4"            , FLOAT4);
   PushKey("float4x"           , FLOAT4X);
   PushKey("float4ibm"         , FLOAT4IBM);
   PushKey("float8"            , FLOAT8);
   PushKey("float8x"           , FLOAT8X);
   PushKey("float8ibm"         , FLOAT8IBM);
   PushKey("float16"           , FLOAT16);
   PushKey("float16x"          , FLOAT16X);
   PushKey("complex8"          , COMPLEX8);
   PushKey("complex8x"         , COMPLEX8X);
   PushKey("complex8ibm"       , COMPLEX8IBM);
   PushKey("complex16"         , COMPLEX16);
   PushKey("complex16x"        , COMPLEX16X);
   PushKey("complex16ibm"      , COMPLEX16IBM);
   PushKey("complex32"         , COMPLEX32);
   PushKey("complex32x"        , COMPLEX32X);

   PushKey("enum1"             , ENUM1);
   PushKey("enum2"             , ENUM2);
   PushKey("enum4"             , ENUM4);
   PushKey("enum8"             , ENUM8);

   /**********************************************/
   /* initialize array of prime type descriptors */
   /**********************************************/
   dds_prime = (DDS_PRIME*)ddsMalloc(DDS_STYPE_MAX * sizeof(DDS_PRIME));
   ddsPrimeTypes();

   /**************************************************/
   /* initialize compiler symbol table (prime block) */
   /**************************************************/
   ddsStackInit(&dds_prime_tag, 8 * sizeof(DDS_SYM_TAG*));


   /* First tag is reserved for the default type */
   ddsStackPush(&dds_prime_tag, sizeof(DDS_SYM_TAG), 1);
   memset(dds_prime_tag.ptr, '\0', sizeof(DDS_SYM_TAG));

   /* "+10" accommidates block and error recovery symbols */
   ddsTableInit(&dds_prime_table, SYM_HASH_SIZE,
      ((DDS_STYPE_MAX + 10) * sizeof(DDS_TYPE)),
      SYM_HASH_ALPHA, SYM_HASH_BETA);

   /* "prime" block for DDS_PTYPE in symbol table */
   dds_symbol = &dds_prime_table;
   dds_block = NULL;
   dds_prime_block = ddsSymBlockBegin("<prime_block>", (DDS_SYM_FLAG)0, NULL);

   for(ptype = DDS_CTYPE_VOID; ptype < DDS_STYPE_MAX; ptype++) {
      dds_prime[ptype].type = prime(dds_prime[ptype].name, ptype,
         dds_prime[ptype].prec, dds_prime[ptype].align);
   }

   dds_nop_object = PUSH_OBJECT(&dds_prime_table, "nop",
      (DDS_SYM_FLAG)(DDS_SYM_LITERAL | DDS_SYM_CONST | DDS_SYM_DECL),
      dds_prime_block, dds_prime[DDS_CTYPE_VOID].type);

   /*****************************************/
   /* error recovery symbols in prime block */
   /*****************************************/
   dds_error_block = ddsSymBlockBegin("<error_block>", DDS_SYM_ERROR, NULL);

   dds_error_type = PUSH_TYPE(&dds_prime_table, "<error_type>",
      (DDS_SYM_FLAG)(DDS_SYM_ERROR | DDS_SYM_VOID), dds_error_block, NULL);
   dds_error_type->ptype = DDS_CTYPE_VOID;

   dds_error_member = PUSH_MEMBER(&dds_prime_table, "<error_member>",
      DDS_SYM_ERROR, dds_error_block, dds_error_type);

   dds_error_object = PUSH_OBJECT(&dds_prime_table, "<error_object>",
      DDS_SYM_ERROR, dds_error_block, dds_error_type);

   dds_error_value = (DDS_VALUE*)dds_error_object;
   dds_error_value->ptr = (void*)-1;

   dds_error_expr = PUSH_EXPR(&dds_prime_table, "<error_expr>",
      DDS_SYM_ERROR, dds_error_block, dds_error_type);
   dds_error_expr->ptr = (void*)-1;
   dds_error_expr->opcode = DDS_ERROR_OP;

   dds_error_thread = dds_error_expr;

   /* terminate <error_block> */
   ddsSymBlockEnd();

   /* terminate <prime_block> */
   ddsSymBlockEnd();

   /******************************************************/
   /* mutate DDS_PTYPE to host "C" type and expressions  */
   /******************************************************/

   for(ptype = DDS_PTYPE_ZERO; ptype < DDS_STYPE_MAX; ptype++) {
      dds_mutate[ptype] = DDS_CTYPE_VOID;
   }

   /* conversion from "C" to "C" type is trivial */
   assert(DDS_CTYPE_MAX - DDS_CTYPE_MIN == 19);
   for(ctype = DDS_CTYPE_MIN; ctype < DDS_CTYPE_MAX; ctype++) {
      dds_mutate[ctype] = ctype;
   }

   /* special case for character */
   dds_mutate[DDS_STYPE_ASCII1] = DDS_CTYPE_CHAR;
   dds_mutate[DDS_STYPE_EBCDIC1] = DDS_CTYPE_CHAR;

   /* signed integers */
   dds_mutate[DDS_STYPE_INTEGER1] = DDS_CTYPE_SCHAR;
   dds_mutate[DDS_STYPE_INTEGER2] =
      (HOST_SHORT_P >= 2) ? DDS_CTYPE_SHORT : DDS_CTYPE_INT;
   dds_mutate[DDS_STYPE_INTEGER2X] =
      (HOST_SHORT_P >= 2) ? DDS_CTYPE_SHORT : DDS_CTYPE_INT;
   dds_mutate[DDS_STYPE_INTEGER4] =
      (HOST_INT_P >= 4) ? DDS_CTYPE_INT : DDS_CTYPE_LONG;
   dds_mutate[DDS_STYPE_INTEGER4X] =
      (HOST_INT_P >= 4) ? DDS_CTYPE_INT : DDS_CTYPE_LONG;
   dds_mutate[DDS_STYPE_INTEGER8] =
      (HOST_LONG_P >= 8) ? DDS_CTYPE_LONG : DDS_CTYPE_LLONG;
   dds_mutate[DDS_STYPE_INTEGER8X] =
      (HOST_LONG_P >= 8) ? DDS_CTYPE_LONG : DDS_CTYPE_LLONG;

   /* unsigned integers */
   dds_mutate[DDS_STYPE_UNSIGNED1] = DDS_CTYPE_UCHAR;
   dds_mutate[DDS_STYPE_UNSIGNED2] =
      (HOST_USHORT_P >= 2) ? DDS_CTYPE_USHORT : DDS_CTYPE_UINT;
   dds_mutate[DDS_STYPE_UNSIGNED2X] =
      (HOST_USHORT_P >= 2) ? DDS_CTYPE_USHORT : DDS_CTYPE_UINT;
   dds_mutate[DDS_STYPE_UNSIGNED4] =
      (HOST_UINT_P >= 4) ? DDS_CTYPE_UINT : DDS_CTYPE_ULONG;
   dds_mutate[DDS_STYPE_UNSIGNED4X] =
      (HOST_UINT_P >= 4) ? DDS_CTYPE_UINT : DDS_CTYPE_ULONG;
   dds_mutate[DDS_STYPE_UNSIGNED8] =
      (HOST_ULONG_P >= 8) ? DDS_CTYPE_ULONG : DDS_CTYPE_ULLONG;
   dds_mutate[DDS_STYPE_UNSIGNED8X] =
      (HOST_ULONG_P >= 8) ? DDS_CTYPE_ULONG : DDS_CTYPE_ULLONG;

   /* floating point */
   dds_mutate[DDS_STYPE_FLOAT4] =
      (HOST_FLOAT_P >= 4) ? DDS_CTYPE_FLOAT :
         ((HOST_DOUBLE_P >= 4) ? DDS_CTYPE_DOUBLE : DDS_CTYPE_LDOUBLE);
   dds_mutate[DDS_STYPE_FLOAT4X] =
      (HOST_FLOAT_P >= 4) ? DDS_CTYPE_FLOAT :
         ((HOST_DOUBLE_P >= 4) ? DDS_CTYPE_DOUBLE : DDS_CTYPE_LDOUBLE);
   dds_mutate[DDS_STYPE_FLOAT4IBM] =
      (HOST_FLOAT_P >= 4) ? DDS_CTYPE_FLOAT :
         ((HOST_DOUBLE_P >= 4) ? DDS_CTYPE_DOUBLE : DDS_CTYPE_LDOUBLE);
   dds_mutate[DDS_STYPE_FLOAT8] =
      (HOST_FLOAT_P >= 8) ? DDS_CTYPE_FLOAT :
         ((HOST_DOUBLE_P >= 8) ? DDS_CTYPE_DOUBLE : DDS_CTYPE_LDOUBLE);
   dds_mutate[DDS_STYPE_FLOAT8X] =
      (HOST_FLOAT_P >= 8) ? DDS_CTYPE_FLOAT :
         ((HOST_DOUBLE_P >= 8) ? DDS_CTYPE_DOUBLE : DDS_CTYPE_LDOUBLE);
   dds_mutate[DDS_STYPE_FLOAT8IBM] =
      (HOST_FLOAT_P >= 8) ? DDS_CTYPE_FLOAT :
         ((HOST_DOUBLE_P >= 8) ? DDS_CTYPE_DOUBLE : DDS_CTYPE_LDOUBLE);
   dds_mutate[DDS_STYPE_FLOAT16] =
      (HOST_FLOAT_P >= 16) ? DDS_CTYPE_FLOAT :
         ((HOST_DOUBLE_P >= 16) ? DDS_CTYPE_DOUBLE : DDS_CTYPE_LDOUBLE);
   dds_mutate[DDS_STYPE_FLOAT16X] =
      (HOST_FLOAT_P >= 16) ? DDS_CTYPE_FLOAT :
         ((HOST_DOUBLE_P >= 16) ? DDS_CTYPE_DOUBLE : DDS_CTYPE_LDOUBLE);

   /* complex floats */
   dds_mutate[DDS_STYPE_COMPLEX8] =
      (HOST_FLOAT_P >= 4) ? DDS_CTYPE_CPX :
         ((HOST_DOUBLE_P >= 4) ? DDS_CTYPE_DBLCPX : DDS_CTYPE_LDBLCPX);
   dds_mutate[DDS_STYPE_COMPLEX8X] =
      (HOST_FLOAT_P >= 4) ? DDS_CTYPE_CPX :
         ((HOST_DOUBLE_P >= 4) ? DDS_CTYPE_DBLCPX : DDS_CTYPE_LDBLCPX);
   dds_mutate[DDS_STYPE_COMPLEX8IBM] =
      (HOST_FLOAT_P >= 4) ? DDS_CTYPE_CPX :
         ((HOST_DOUBLE_P >= 4) ? DDS_CTYPE_DBLCPX : DDS_CTYPE_LDBLCPX);
   dds_mutate[DDS_STYPE_COMPLEX16] =
      (HOST_FLOAT_P >= 8) ? DDS_CTYPE_CPX :
         ((HOST_DOUBLE_P >= 8) ? DDS_CTYPE_DBLCPX : DDS_CTYPE_LDBLCPX);
   dds_mutate[DDS_STYPE_COMPLEX16X] =
      (HOST_FLOAT_P >= 8) ? DDS_CTYPE_CPX :
         ((HOST_DOUBLE_P >= 8) ? DDS_CTYPE_DBLCPX : DDS_CTYPE_LDBLCPX);
   dds_mutate[DDS_STYPE_COMPLEX16IBM] =
      (HOST_FLOAT_P >= 8) ? DDS_CTYPE_CPX :
         ((HOST_DOUBLE_P >= 8) ? DDS_CTYPE_DBLCPX : DDS_CTYPE_LDBLCPX);
   dds_mutate[DDS_STYPE_COMPLEX32] =
      (HOST_FLOAT_P >= 16) ? DDS_CTYPE_CPX :
         ((HOST_DOUBLE_P >= 16) ? DDS_CTYPE_DBLCPX : DDS_CTYPE_LDBLCPX);
   dds_mutate[DDS_STYPE_COMPLEX32X] =
      (HOST_FLOAT_P >= 16) ? DDS_CTYPE_CPX :
         ((HOST_DOUBLE_P >= 16) ? DDS_CTYPE_DBLCPX : DDS_CTYPE_LDBLCPX);

   /***********************************************************/
   /* "usual" arithmetic conversions, given operand types     */
   /***********************************************************/

/**************************************************************************
   NOTE: concur[x][x] provides integral promotion of ptype "x".

   ptype2  C  SC  UC   S  US   I   U   L  UL  LL  ULL  F   D  LD   X  DX  LDX
ptype1   ++++----++++----++++----++++----++++----++++----++++----++++----++++
CHAR      I/U
SCHAR      I   I
UCHAR     I/U I/U I/U
SHORT      I   I  I/U  I
USHORT    I/U I/U I/U I/U I/U                       SYMMETRIC
INT        I   I  I/U  I  I/U  I
UINT       U   U   U   U   U   U   U
LONG     L/UL  L  L/UL L L/UL  L  L/UL L
ULONG      UL  UL  UL  UL  UL  UL  UL  UL UL
LLONG     *LL  LL *LL  LL *LL  LL *LL  LL *LL  LL         *LL  ==>  LL/ULL
ULLONG    ULL ULL ULL ULL ULL ULL ULL ULL ULL ULL ULL
FLOAT      F   F   F   F   F   F   F   F   F   F   F   F
DOUBLE     D   D   D   D   D   D   D   D   D   D   D   D   D
LDOUBLE    LD  LD  LD  LD  LD  LD  LD  LD  LD  LD  LD LD  LD  LD
CPX        X   X   X   X   X   X   X   X   X   X   X   X   X   X   X
DBLCPX     DX  DX  DX  DX  DX  DX  DX  DX  DX  DX  DX  DX  DX  DX  DX  DX
LDBLCPX   LDX LDX LDX LDX LDX LDX LDX LDX LDX LDX LDX LDX LDX LDX LDX LDX LDX
ASCII
EBCDIC
INTEGER1
INTEGER2
...
ptype1   ++++----++++----++++----++++----++++----++++----++++----++++----++++
   ptype2  C  SC  UC   S  US   I   U   L  UL  LL  ULL  F   D  LD   X  DX  LDX
**************************************************************************/

   /*********************************/
   /* initialize everything to void */
   /*********************************/

   for(ptype1 = DDS_PTYPE_ZERO; ptype1 < DDS_STYPE_MAX; ptype1++) {
      for(ptype2 = DDS_PTYPE_ZERO; ptype2 < DDS_STYPE_MAX; ptype2++) {
         dds_concur[ptype1][ptype2] = DDS_CTYPE_VOID;
      }
   }

   /*********************************/
   /* now do the standard "C" types */
   /*********************************/

   ptype1 = DDS_CTYPE_CHAR;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_INT;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_INT;
   }

   ptype1 = DDS_CTYPE_SCHAR;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_INT;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_INT;
   }

   ptype1 = DDS_CTYPE_UCHAR;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_INT;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_INT;
   }

   ptype1 = DDS_CTYPE_SHORT;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_INT;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_INT;
   }

   ptype1 = DDS_CTYPE_USHORT;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_INT;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_INT;
   }

   ptype1 = DDS_CTYPE_INT;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_INT;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_INT;
   }

   ptype1 = DDS_CTYPE_UINT;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_UINT;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_UINT;
   }

   ptype1 = DDS_CTYPE_LONG;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_LONG;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_LONG;
   }

   if(HOST_LONG_P == HOST_UINT_P) {
      dds_concur[DDS_CTYPE_LONG][DDS_CTYPE_UINT] = DDS_CTYPE_ULONG;
      dds_concur[DDS_CTYPE_LONG][DDS_CTYPE_UINT] = DDS_CTYPE_ULONG;
   }

   ptype1 = DDS_CTYPE_ULONG;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_ULONG;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_ULONG;
   }

   ptype1 = DDS_CTYPE_LLONG;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_LLONG;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_LLONG;
   }

   if(HOST_LLONG_P == HOST_ULONG_P) {
      dds_concur[DDS_CTYPE_LLONG][DDS_CTYPE_ULONG] = DDS_CTYPE_ULLONG;
      dds_concur[DDS_CTYPE_LLONG][DDS_CTYPE_ULONG] = DDS_CTYPE_ULLONG;
   }

   ptype1 = DDS_CTYPE_ULLONG;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_ULLONG;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_ULLONG;
   }

   ptype1 = DDS_CTYPE_FLOAT;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_FLOAT;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_FLOAT;
   }

   ptype1 = DDS_CTYPE_DOUBLE;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_DOUBLE;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_DOUBLE;
   }

   ptype1 = DDS_CTYPE_LDOUBLE;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_LDOUBLE;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_LDOUBLE;
   }

   ptype1 = DDS_CTYPE_CPX;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_CPX;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_CPX;
   }

   ptype1 = DDS_CTYPE_DBLCPX;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_DBLCPX;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_DBLCPX;
   }

   ptype1 = DDS_CTYPE_LDBLCPX;
   for(ptype2 = DDS_CTYPE_CHAR; ptype2 <= ptype1; ptype2++) {
      dds_concur[ptype1][ptype2] = DDS_CTYPE_LDBLCPX;
      dds_concur[ptype2][ptype1] = DDS_CTYPE_LDBLCPX;
   }

   /*******************************/
   /* now do the dds stream types */
   /*******************************/

   for(ptype1 = DDS_STYPE_MIN; ptype1 < DDS_STYPE_MAX; ptype1++) {
      for(ptype2 = DDS_CTYPE_MIN; ptype2 < DDS_STYPE_MAX; ptype2++) {
         ctype1 = dds_mutate[ptype1];
         ctype2 = dds_mutate[ptype2];
         concur = dds_concur[ctype1][ctype2];
         dds_concur[ptype1][ptype2] = concur;
         dds_concur[ptype2][ptype1] = concur;
      }
   }

   for(ptype1 = DDS_CTYPE_MIN; ptype1 < DDS_CTYPE_MAX; ptype1++) {
      for(ptype2 = DDS_STYPE_MIN; ptype2 < DDS_STYPE_MAX; ptype2++) {
         ctype1 = dds_mutate[ptype1];
         ctype2 = dds_mutate[ptype2];
         concur = dds_concur[ctype1][ctype2];
         dds_concur[ptype1][ptype2] = concur;
         dds_concur[ptype2][ptype1] = concur;
      }
   }

   /******************************************************/
   /* preferred cleanup sequencing                       */
   /******************************************************/
   gio_init();
   atexit(&ddsCleanup);

   /* stress test the GIO Cache File Descriptor support. */
   /*gopenmax(GIO_OPEN_MAX_SET,1);*/

   /* initialize control dictionaries */
   cdds_dict("cmd:", "scan");
   cdds_dict("par:", "scan");
   cdds_dict("env:", "scan");

   #ifdef DDS_FAST_INIT
      /* avoid the slow automounter */
   #else
      /* Sun debugger rui (read from uninitialized) snafu */
      memset((void*)&stat_buf, '\0', sizeof(stat_buf));

      if(0 < cdds_scant("home HOME", &value) &&
         ! ddsSTAT(value,  &stat_buf) &&
         S_ISDIR(stat_buf.st_mode)) {
         dds_cmd_home = ddsString(value);
         ddsStrCpyCat2(&tmp_string, value, "/.ddsrc");
         if(! ddsSTAT(tmp_string.ptr, &stat_buf) &&
            stat_buf.st_size &&
            S_ISREG(stat_buf.st_mode)) {
            dds_ddsrc_dict = ddsString(tmp_string.ptr);
         }
      }
   #endif

   if(! dds_cmd_home) dds_cmd_home = ddsString("<home_dir>");

   ddsStrCpyCat2(&tmp_string, dds_cmd_cwd, "/project.dds");
   if(! ddsSTAT(tmp_string.ptr, &stat_buf) &&
      stat_buf.st_size &&
      S_ISREG(stat_buf.st_mode)) {
      dds_project_dict = ddsString(tmp_string.ptr);
   }

   /* create and select base dictionary "par:  env:  ./project  ~/.ddsrc" */
   /* deprecate dds_base_dict */
   ddsStrCpy(&tmp_string, "par: env:");
   if(dds_project_dict) ddsStrCat2(&tmp_string, " ", dds_project_dict);
   if(dds_ddsrc_dict) ddsStrCat2(&tmp_string, " ", dds_ddsrc_dict);
   dds_base_dict = ddsString(tmp_string.ptr);
   cdds_dict(dds_base_dict, "scan");

   /* retrieve dump value */
   dump_alias = "dds_dump DDS_DUMP dds_debug DDS_DEBUG";
   if(! cdds_scanf(dump_alias, "")) {

      dds_state |= (DDS_STATE)(DDS_VERBOSE | DDS_DEBUG_PRINT);

      cdds_scanv("", &value);
      if(strstr(value, "-verbose"))
         dds_state &= (DDS_STATE) ~ DDS_VERBOSE;
      if(strstr(value, "-debug"))
         dds_state &= (DDS_STATE) ~ DDS_DEBUG_PRINT;

      if(strstr(value, "+tmp"))
         dds_state |= DDS_SAVE_TMP;

      if(0 < cdds_scank(dump_alias, "diff0 diff diff1 diff2")) {
         cdds_scanv("", &value);
         if(strstr(value, "diff0"))  dds_state |= DDS_DUMP_DIFF0;
         if(strstr(value, "diff1"))  dds_state |= DDS_DUMP_DIFF1;
         if(strstr(value, "diff "))  dds_state |= DDS_DUMP_DIFF1;
         if(strstr(value, "diff\n")) dds_state |= DDS_DUMP_DIFF1;
         if(strstr(value, "diff\t")) dds_state |= DDS_DUMP_DIFF1;
         if(strstr(value, "diff2"))  dds_state |= DDS_DUMP_DIFF2;

         dds_dump_max = 30;
         if(0 < cdds_scank(dump_alias, "max")) {
            cdds_scanf("", "%*s %d", &dds_dump_max);
            if(dds_dump_max < 0) dds_dump_max = 0;
            if(dds_dump_max > DDS_DUMP_LIMIT) dds_dump_max = DDS_DUMP_LIMIT;
         }

         ddsStackInit(&dds_dump_que, DDS_DUMP_QUE);
      } else if(0 < cdds_scank(dump_alias, "list usp segy")) {
         cdds_scant("", &value);
         if(! strcmp(value, "list")) {
            value = "";
            cdds_scant("", &value);
            ddsStrCpy(&tmp_string, value);
         } else if(! strcmp(value, "usp")) {
            ddsStrCpy(&tmp_string, "RecNum TrcNum RecInd DphInd SrcLoc "
               "SoPtNm SrcPnt LinInd DstSgn StaCor");
         } else if(! strcmp(value, "segy")) {
            ddsStrCpy(&tmp_string, "LineTrcNum ReelTrcNum FieldRecNum "
               "FieldTrcNum SrcPtNum CdpNum CdpTrcNum");
         } else {
            ddsStrCpy(&tmp_string, "");
         }

         dds_dump_max = TOKEN_COUNT(tmp_string.ptr);

         if(dds_dump_max) {
            dds_state |= DDS_DUMP_LIST;
            dds_dump_dir = (DDS_DUMP_DIR*)ddsMalloc(dds_dump_max *
               sizeof(DDS_DUMP_DIR));
            for(n = 0, value = TOKEN_INIT(value_save, tmp_string.ptr);
               value;
               n++, value = TOKEN_NEXT(value_save)) {
               assert(n < dds_dump_max);
               dds_dump_dir[n].name = ddsString(value);
               dds_dump_dir[n].width = strlen(value);
               if(dds_dump_dir[n].width < DDS_DUMP_WIDTH) {
                  dds_dump_dir[n].width = DDS_DUMP_WIDTH;
               }
            }
            ddsFree(value_save.str.ptr);
            assert(n == dds_dump_max);
         } else {
            /* empty list ? */
            dds_state &= (DDS_STATE) ~ DDS_DUMP_LIST;
         }
      }

      if(0 < cdds_scank(dump_alias, "read write seek")) {
         cdds_scanv("", &value);
         if(strstr(value, "read"))
            dds_state |= DDS_DUMP_READ;
         if(strstr(value, "write"))
            dds_state |= DDS_DUMP_WRITE;
         if(strstr(value, "seek"))
            dds_state |= DDS_DUMP_SEEK;
      } else {
         dds_state |= (DDS_STATE)(DDS_DUMP_READ | DDS_DUMP_WRITE |
            DDS_DUMP_SEEK);
      }

      if(0 < cdds_scank(dump_alias,
         " dbg_heap  dbg_alloc  dbg_free  dbg_call "
         " dbg_trace  dbg_scanner "
         " dbg_symbol  dbg_binary "
         " dbg_uncompile dbg_panic "
         " dbg_all ")) {

         cdds_scanv("", &value);

         if(strstr(value, "dbg_heap"))
            dds_debug_user |= (DDS_DEBUG)(DDS_DBG_ALLOC | DDS_DBG_FREE);

         if(strstr(value, "dbg_alloc"))
            dds_debug_user |= DDS_DBG_ALLOC;

         if(strstr(value, "dbg_free"))
            dds_debug_user |= DDS_DBG_FREE;

         if(strstr(value, "dbg_call"))
            dds_debug_user |= DDS_DBG_CALL;

         if(strstr(value, "dbg_trace"))
            dds_debug_user |= DDS_DBG_TRACE;

         if(strstr(value, "dbg_scanner")) {
            dds_debug_user |= DDS_DBG_SCANNER;
            dds_yy_flex_debug = 1;
         }

         if(strstr(value, "dbg_symbol"))
            dds_debug_user |= DDS_DBG_SYMBOL;

         if(strstr(value, "dbg_uncompile"))
            dds_debug_user |= DDS_DBG_UNCOMPILE;

         if(strstr(value, "dbg_binary"))
            dds_debug_user |= DDS_DBG_BINARY;

         if(strstr(value, "dbg_panic"))
            dds_debug_user |= DDS_DBG_PANIC;

         if(strstr(value, "dbg_all")) {
            dds_debug_user |= (DDS_DEBUG)(DDS_DBG_HEAP | DDS_DBG_ALLOC |
               DDS_DBG_FREE | DDS_DBG_CALL |
               DDS_DBG_SCANNER | DDS_DBG_SYMBOL | DDS_DBG_PANIC);
         }

         if(dds_debug_user & DDS_DBG_SYMBOL) ddsDumpGlobal();

         dds_debug_api = dds_debug_user;
      }
   }

   /* retrieve DDS_PATH3 overrride */
   ddsStrSetMin(&dds_path, "", 2 * PATHNAME_MAX);

   ddsStrFree(&tmp_string);

   DDS_API_EXIT(status);
}
  
