static char rcsid[]="$Id: global.c 229 2011-06-02 18:53:57Z user $";
/*
RCS-t=DDS, initialize global variables and constants

*/

/*****************************************/
/***   Copyright (C) 2006              ***/
/***   BP America, Houston, TX         ***/
/***   Jerry Ehlers, August 2006       ***/
/*****************************************/

#include "dds_sys.h"
#include "dds_dump.h"
#include "dds_opcode.h"

/******************************************/
/* global variables, visible via "cdds.h" */
/******************************************/

/* This should be the first global symbol */
DDS_HEAP dds_global_start = {NULL};
 
/**********************************************/
/* global variables, visible to DDS internals */
/**********************************************/

int dds_api_next = 0;
char *dds_api_call[DDS_API_MAX] = {NULL};

DDS_STACK dds_heap = {0};
double dds_mem_use = 0.0;
double dds_mem_max = 0.0;
double dds_samples_in = 0.0;
double dds_samples_out = 0.0;

int dds_map_samples = 1;
unsigned dds_bin_max = 0;
DDS_BIN **dds_bin = NULL;
DDS_MAP **dds_map = NULL;

DDS_TABLE dds_dict = {0};

DDS_DICT *dds_scan_first = NULL;
DDS_DICT *dds_scan_last = NULL;
DDS_DICT *dds_scan_dict = NULL;
const DDS_DEFN *dds_scan_defn = NULL;
const char *dds_scan_name = NULL;
size_t dds_scan_value_bias = 0;
DDS_TOKEN dds_scan_token = NULL_TOKEN;
char dds_scan_value_save = '\0';
const DDS_DEFN *dds_scan_newer = NULL;

DDS_DICT *dds_print_first = NULL;
DDS_DICT *dds_print_last = NULL;
DDS_DICT *dds_log_dict = NULL;

DDS_ALIAS *dds_alias_free = NULL;
DDS_ALIAS *dds_alias_done = NULL;
unsigned dds_alias_count = 0;

DDS_STACK dds_dump_que = {0};
DDS_DUMP_DIR *dds_dump_dir = NULL;
int dds_dump_max = 8;

DDS_MEDIA *dds_media_list = NULL;
const char * const dds_media_genus[DDS_MEDIA_GENUS_MAX] = {
   "<err>",
   "<not>",
   "<tty>",
   "tape",
   "<block>",
   "file",
   "<directory>",
   "pipe",
   "<null>"
};
const char * const dds_fmt_name[DDS_MEDIA_FMT_MAX] = {
   "<unknown_format>",
   "<dictionary>",
   "generic",
   "usp",
   "segy",
   "disco",
   "geoquest",
   "landmark",
   "su",
   "segy1"
};

int dds_error = 0;

int dds_warn_count = 0;
int dds_error_count = 0;
int dds_debug_count = 0;

DDS_STRING dds_warn_msg = NULL_STRING;
DDS_STRING dds_error_msg = NULL_STRING;
DDS_STRING dds_debug_msg = NULL_STRING;

FILE* dds_print_fp = NULL;


int dds_print_lu = -1;

DDS_EXPRA DDS_CON_EXPRA =
   {0, NULL, NULL, NULL, NULL,        /* hash, name, chain, older, newer, */
   0, DDS_SYM_EXPR, NULL, NULL,       /* tag, flag, block, type, */
   NULL, NULL, DDS_ASSIGN_OP, NULL};  /* ptr, reloc, opcode, next, */

#undef  DDS_OPDEF
#define DDS_OPDEF(NAME, FLAG, ARGS, PREC, PREFIX, INFIX, SUFFIX)  \
   {#NAME, (DDS_OPNOTE_FLAG)(DDS_OPNOTE_SS | FLAG),  \
      ARGS, PREC, PREFIX, INFIX, SUFFIX},

const DDS_OPNOTE dds_opnote_list[DDS_OPCODE_MAX] =
{
   #include "dds_oplist.h"
};

const DDS_OPNOTE *dds_opnote = dds_opnote_list;

DDS_PTYPE dds_mutate[DDS_STYPE_MAX] = {DDS_CTYPE_VOID};

DDS_PTYPE dds_concur[DDS_STYPE_MAX][DDS_STYPE_MAX] = {{DDS_CTYPE_VOID}};

DDS_PRIME *dds_prime = NULL;

const DDS_PRIME_GENUS dds_prime_genus[DDS_TYPE_GENUS_MAX] = {
   "void",
   "character",
   "integer",
   "unsigned",
   "float",
   "complex",
   "structure",
   "union",
   "pointer"
};

const char *dds_shape_name[DDS_SHAPE_MAX] = {
   "non-conformable vector operands",
   "scalar operand deferred",
   "vector operand deferred",
   "scalar-vector operand deferred",
   "vector-vector operand deferred",
   "vector-scalar operand deferred",
   "scalar-vector operand deferred"
};

DDS_BOOLEAN dds_initialized = DDS_FALSE;
DDS_STATE dds_state = (DDS_STATE)(DDS_ERROR_ABORT | DDS_WARN_PRINT |
   DDS_ERROR_PRINT | DDS_DEBUG_PRINT | DDS_OPERATOR);

DDS_DEBUG dds_debug_user = (DDS_DEBUG)0;
DDS_DEBUG dds_debug_api = (DDS_DEBUG)DDS_DEBUG_INIT;
int dds_yy_flex_debug = 0;

#if defined(_WIN32) || defined(MINGW)
size_t dds_cmd_pid = 0;
#else
pid_t dds_cmd_pid = 0;
#endif
const char *dds_cmd_name  = NULL;
const char *dds_cmd_rev   = NULL;
const char *dds_cmd_ddsrev= NULL;
const char *dds_cmd_ddsdat= NULL;
const char *dds_cmd_user  = NULL;
const char *dds_cmd_date  = NULL;
const char *dds_cmd_host  = NULL;
const char *dds_cmd_cwd   = NULL;
const char *dds_cmd_home  = NULL;

const char *dds_project_dict = NULL;
const char *dds_ddsrc_dict = NULL;
const char *dds_base_dict = NULL;
DDS_STRING dds_path = NULL_STRING;

DDS_BOOLEAN *dds_sislgbuf = NULL;

DDS_TABLE dds_prime_table = {NULL};

DDS_STACK dds_prime_tag = {0};

DDS_STRING dds_data_path = NULL_STRING;
DDS_STRING dds_data_suffix = NULL_STRING;

DDS_BOOLEAN dds_openpr = DDS_FALSE;

DDS_STACK dds_tmp = {0};

const DDS_STRING DDS_NULL_STRING = NULL_STRING;
const DDS_TOKEN DDS_NULL_TOKEN = NULL_TOKEN;

DDS_SYM *dds_push_sym = NULL;

DDS_DECLARE *dds_declare = NULL;

static void *ptr_grave;
void **DDS_PTR_CEMETERY = &ptr_grave;

#define DDS_CON2_COPY_OP

#include "dds_con2.h"

DDS_TABLE dds_keyword = {NULL};

DDS_BLOCK   *dds_prime_block = NULL;
DDS_OBJECT  *dds_nop_object = NULL;
DDS_BLOCK   *dds_error_block = NULL;
DDS_TYPE    *dds_error_type = NULL;
DDS_VALUE   *dds_error_value = NULL;
DDS_OBJECT  *dds_error_object = NULL;
DDS_MEMBER  *dds_error_member = NULL;
DDS_EXPR    *dds_error_expr = NULL;
DDS_EXPR    *dds_error_thread = NULL;

DDS_BIN *dds_compile = NULL;
DDS_TABLE *dds_symbol = NULL;
DDS_BLOCK *dds_block = NULL;
DDS_EXPR *dds_thread = NULL;
DDS_EXPR *dds_needle = NULL;
DDS_VALUE *dds_map_value = NULL;
DDS_LEX_START dds_lex_ss = DDS_LEX_CONTINUE;

const char *dds_hist = NULL;

/* global physical constants */
const double MM_PER_INCH      = 0.0393701;
const double CM_PER_INCH      = 0.393701;
const double METER_PER_FOOT   = 0.3048;
const int    INCHES_PER_FOOT  = 12;
const double INCHES_PER_METER = 39.37008;
const double INCHES_PER_CM    = 0.3937008;
const double FEET_PER_METER   = 3.281;
const double FEET_PER_INCH    = 0.0833333;
const int    USEC_PER_MSEC    = 1000;
const int    USEC_PER_SEC     = 1000000;
const double MSEC_PER_USEC    = 0.001;
const int    MSEC_PER_SEC     = 1000;
const double SEC_PER_MSEC     = 0.001;
const double SEC_PER_USEC     = 0.000001;

/* Initial size of hash table (rounded up to nearest prime). */
const int  KEYS_HASH_SIZE  = 293;
const int  SYM_HASH_SIZE   = 31;
const int  DICT_HASH_SIZE  = 31;
const int  DEFN_HASH_SIZE  = 31;
const int  STOP_HASH_SIZE  = 1;

/* ALPHA, rehash threshold (average number of items per hash bucket). */
/*    Rehash table if ALPHA < (count / hash_load) */
/* ALPHA << 1.00 simple ratio, good with duplicate names. */
/* ALPHA == 1.00 perfect hash, perpetual rehashing. */
/* ALPHA == 2.00 fast search, "frequent" rehashing. */
/* ALPHA == 20.0 slow search, "periodic" rehashing. */
/* ALPHA >> 200. (== 1.e+9) disable automatic rehashing. */
const double KEYS_HASH_ALPHA  = 1.15;
const double SYM_HASH_ALPHA   = 0.80;
const double DICT_HASH_ALPHA  = 1.50;
const double DEFN_HASH_ALPHA  = 0.80;
const double STOP_HASH_ALPHA  = 1.e+9;

/* BETA, items per bucket (most optimistic ALPHA after rehashing). */
/*    New table size == prime greater than old size >= (count / BETA). */
/* BETA  == 0.00 infinite hash table size (div and malloc fault). */
/* BETA  == 0.02 items per bucket,  51X "explosive" table growth. */
/* BETA  == 0.20 items per bucket,   6X "moderate"  table growth. */
/* BETA  == 2.00 item  per bucket, 1.5X "minimal"   table growth. */
/* BETA  >> 20.0 (== 1.e+9) increase old size to next prime number. */
/* Most optimistic population growth before rehash == (ALPHA / BETA). */
/* Large ratios minimize rehashing when tables grow rapidly. */
/* Small ratios minimize table size for a given search efficiency. */
const double KEYS_HASH_BETA   = 2.00;
const double SYM_HASH_BETA    = 0.20;
const double DICT_HASH_BETA   = 0.20;
const double DEFN_HASH_BETA   = 0.20;
const double STOP_HASH_BETA   = 1.e+9;

/* This should be the last global symbol */
DDS_HEAP dds_global_end = {NULL};

