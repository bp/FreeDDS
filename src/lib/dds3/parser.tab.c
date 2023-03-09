/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse ddsYYparse
#define yylex   ddsYYlex
#define yyerror ddsYYerror
#define yylval  ddsYYlval
#define yychar  ddsYYchar
#define yydebug ddsYYdebug
#define yynerrs ddsYYnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TRANSLATION_UNIT = 258,
     DEFN_UNIT = 259,
     IDENT_UNIT = 260,
     MAP_EXPR = 261,
     INTEGER_TOKEN = 262,
     FLOATING_TOKEN = 263,
     CHARACTER_TOKEN = 264,
     ENUMERATION_TOKEN = 265,
     IDENTIFIER_TOKEN = 266,
     TYPEDEF_TOKEN = 267,
     STRING_TOKEN = 268,
     ASM = 269,
     ASSIGN_OP = 270,
     AUTO = 271,
     BIT_AND_ASSIGN_OP = 272,
     BIT_OR_ASSIGN_OP = 273,
     BIT_XOR_ASSIGN_OP = 274,
     BREAK = 275,
     CASE = 276,
     CHAR = 277,
     COMPLEX = 278,
     CONST = 279,
     CONTINUE = 280,
     DECREMENT_OP = 281,
     DEFAULT = 282,
     DIVIDE_ASSIGN_OP = 283,
     DO = 284,
     DOUBLE = 285,
     ELIPSE = 286,
     ELSE = 287,
     ENUM = 288,
     EQUAL_TO_OP = 289,
     EXTERN = 290,
     FLOAT = 291,
     FOR = 292,
     FORTRAN = 293,
     GOTO = 294,
     GREATER_EQUAL_OP = 295,
     IF = 296,
     INCREMENT_OP = 297,
     INT = 298,
     LEFT_SHIFT_ASSIGN_OP = 299,
     LEFT_SHIFT_OP = 300,
     LESS_EQUAL_OP = 301,
     LOGICAL_AND_OP = 302,
     LOGICAL_OR_OP = 303,
     LONG = 304,
     MINUS_ASSIGN_OP = 305,
     MODULO_ASSIGN_OP = 306,
     MULTIPLY_ASSIGN_OP = 307,
     NOT_EQUAL_TO_OP = 308,
     PLUS_ASSIGN_OP = 309,
     REGISTER = 310,
     RETURN = 311,
     RIGHT_SHIFT_ASSIGN_OP = 312,
     RIGHT_SHIFT_OP = 313,
     SHORT = 314,
     SIGNED = 315,
     SIZEOF = 316,
     STATIC = 317,
     STRUCT = 318,
     STRUCT_POINTER_OP = 319,
     SWITCH = 320,
     TYPEDEF = 321,
     UNION = 322,
     UNSIGNED = 323,
     VOID = 324,
     VOLATILE = 325,
     WHILE = 326,
     IDENT = 327,
     DEFN_VALUE = 328,
     ALIGN = 329,
     AXIS_SIZE = 330,
     EXIST = 331,
     NOP = 332,
     FIELD = 333,
     ORDINAL = 334,
     LINE_TRC = 335,
     REEL_TRC = 336,
     SQRT = 337,
     SIN = 338,
     COS = 339,
     TAN = 340,
     ASIN = 341,
     ACOS = 342,
     ATAN = 343,
     ATAN2 = 344,
     EXP = 345,
     LOG = 346,
     LOG10 = 347,
     POW = 348,
     ABS = 349,
     SIGN = 350,
     CEIL = 351,
     FLOOR = 352,
     DATEINT = 353,
     DATESTRUCT = 354,
     ASCII1 = 355,
     EBCDIC1 = 356,
     UNSIGNED1 = 357,
     UNSIGNED2 = 358,
     UNSIGNED4 = 359,
     UNSIGNED8 = 360,
     INTEGER1 = 361,
     INTEGER2 = 362,
     INTEGER4 = 363,
     INTEGER8 = 364,
     FLOAT4 = 365,
     FLOAT8 = 366,
     FLOAT16 = 367,
     FLOAT4IBM = 368,
     FLOAT8IBM = 369,
     COMPLEX8 = 370,
     COMPLEX16 = 371,
     COMPLEX32 = 372,
     COMPLEX8IBM = 373,
     COMPLEX16IBM = 374,
     ENUM1 = 375,
     ENUM2 = 376,
     ENUM4 = 377,
     ENUM8 = 378,
     UNSIGNED2X = 379,
     UNSIGNED4X = 380,
     UNSIGNED8X = 381,
     INTEGER2X = 382,
     INTEGER4X = 383,
     INTEGER8X = 384,
     FLOAT4X = 385,
     FLOAT8X = 386,
     FLOAT16X = 387,
     COMPLEX8X = 388,
     COMPLEX16X = 389,
     COMPLEX32X = 390
   };
#endif
/* Tokens.  */
#define TRANSLATION_UNIT 258
#define DEFN_UNIT 259
#define IDENT_UNIT 260
#define MAP_EXPR 261
#define INTEGER_TOKEN 262
#define FLOATING_TOKEN 263
#define CHARACTER_TOKEN 264
#define ENUMERATION_TOKEN 265
#define IDENTIFIER_TOKEN 266
#define TYPEDEF_TOKEN 267
#define STRING_TOKEN 268
#define ASM 269
#define ASSIGN_OP 270
#define AUTO 271
#define BIT_AND_ASSIGN_OP 272
#define BIT_OR_ASSIGN_OP 273
#define BIT_XOR_ASSIGN_OP 274
#define BREAK 275
#define CASE 276
#define CHAR 277
#define COMPLEX 278
#define CONST 279
#define CONTINUE 280
#define DECREMENT_OP 281
#define DEFAULT 282
#define DIVIDE_ASSIGN_OP 283
#define DO 284
#define DOUBLE 285
#define ELIPSE 286
#define ELSE 287
#define ENUM 288
#define EQUAL_TO_OP 289
#define EXTERN 290
#define FLOAT 291
#define FOR 292
#define FORTRAN 293
#define GOTO 294
#define GREATER_EQUAL_OP 295
#define IF 296
#define INCREMENT_OP 297
#define INT 298
#define LEFT_SHIFT_ASSIGN_OP 299
#define LEFT_SHIFT_OP 300
#define LESS_EQUAL_OP 301
#define LOGICAL_AND_OP 302
#define LOGICAL_OR_OP 303
#define LONG 304
#define MINUS_ASSIGN_OP 305
#define MODULO_ASSIGN_OP 306
#define MULTIPLY_ASSIGN_OP 307
#define NOT_EQUAL_TO_OP 308
#define PLUS_ASSIGN_OP 309
#define REGISTER 310
#define RETURN 311
#define RIGHT_SHIFT_ASSIGN_OP 312
#define RIGHT_SHIFT_OP 313
#define SHORT 314
#define SIGNED 315
#define SIZEOF 316
#define STATIC 317
#define STRUCT 318
#define STRUCT_POINTER_OP 319
#define SWITCH 320
#define TYPEDEF 321
#define UNION 322
#define UNSIGNED 323
#define VOID 324
#define VOLATILE 325
#define WHILE 326
#define IDENT 327
#define DEFN_VALUE 328
#define ALIGN 329
#define AXIS_SIZE 330
#define EXIST 331
#define NOP 332
#define FIELD 333
#define ORDINAL 334
#define LINE_TRC 335
#define REEL_TRC 336
#define SQRT 337
#define SIN 338
#define COS 339
#define TAN 340
#define ASIN 341
#define ACOS 342
#define ATAN 343
#define ATAN2 344
#define EXP 345
#define LOG 346
#define LOG10 347
#define POW 348
#define ABS 349
#define SIGN 350
#define CEIL 351
#define FLOOR 352
#define DATEINT 353
#define DATESTRUCT 354
#define ASCII1 355
#define EBCDIC1 356
#define UNSIGNED1 357
#define UNSIGNED2 358
#define UNSIGNED4 359
#define UNSIGNED8 360
#define INTEGER1 361
#define INTEGER2 362
#define INTEGER4 363
#define INTEGER8 364
#define FLOAT4 365
#define FLOAT8 366
#define FLOAT16 367
#define FLOAT4IBM 368
#define FLOAT8IBM 369
#define COMPLEX8 370
#define COMPLEX16 371
#define COMPLEX32 372
#define COMPLEX8IBM 373
#define COMPLEX16IBM 374
#define ENUM1 375
#define ENUM2 376
#define ENUM4 377
#define ENUM8 378
#define UNSIGNED2X 379
#define UNSIGNED4X 380
#define UNSIGNED8X 381
#define INTEGER2X 382
#define INTEGER4X 383
#define INTEGER8X 384
#define FLOAT4X 385
#define FLOAT8X 386
#define FLOAT16X 387
#define COMPLEX8X 388
#define COMPLEX16X 389
#define COMPLEX32X 390




/* Copy the first part of user declarations.  */


static char rcsid[]="$Id: parser.tab.c 121 2009-10-21 20:40:59Z user $";
/*
DDS, GNU bison parser for binary formats.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

/*** #define alloca ddsMalloc ***/

#define VOIDED  0

#define YYPARSE_PARAM parm

/********************************************************/
/*  Bison creates a bad proto type in parser.tab.c and  */
/*  gcc activates the bad code by defining __GNUC__.    */
/*  Solution: edit parser.tab.c to disable the code...  */
/*  #ifdef __GNUC__BISON_BUG                            */
/*  int yyparse (void);                                 */
/*  #endif                                              */
/********************************************************/

#ifdef HOST_LLONG_C
   #define MAKE_LLONG DDS_TRUE
#else
   #define MAKE_LLONG DDS_FALSE
#endif

#ifdef HOST_LDOUBLE_C
   #define MAKE_LDOUBLE DDS_TRUE
#else
   #define MAKE_LDOUBLE DDS_FALSE
#endif

#define TYPE_MERGE(STYPE)  \
   ddsSymTypeMerge(dds_prime[DDS_STYPE_##STYPE].type)

static DDS_EXPR *needle;

#if defined(_WIN32) || defined(MINGW)
static size_t bias;
#else
static long bias;
#endif

static char tmp_str[64];



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)

typedef union YYSTYPE {
   void          *YYvoid;            /* DDS_SYM (useless info) */
   const char    *YYname;            /* name index (identifier) */
   DDS_TYPE      *YYtype;            /* DDS_TYPE */
   DDS_OBJECT    *YYobject;          /* DDS_OBJECT */
   DDS_MEMBER    *YYmember;          /* DDS_MEMBER */
   DDS_BLOCK     *YYblock;           /* DDS_BLOCK */
   DDS_VALUE     *YYvalue;           /* DDS_VALUE */
   DDS_EXPR      *YYexpr;            /* DDS_EXPR */
   DDS_PTYPE      YYptype;           /* DDS prime type (enum constant) */
   DDS_OPCODE     YYopcode;          /* operator code */
   DDS_SYM_FLAG   YYedit;            /* struct/union composition */
} YYSTYPE;
/* Line 196 of yacc.c.  */

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 219 of yacc.c.  */


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if defined (__STDC__) || defined (__cplusplus)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     define YYINCLUDED_STDLIB_H
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifdef __cplusplus
}
#  endif
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  132
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2112

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  159
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  82
/* YYNRULES -- Number of rules. */
#define YYNRULES  249
/* YYNRULES -- Number of states. */
#define YYNSTATES  422

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   390

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   157,     2,     2,     2,   155,   151,     2,
     136,   138,   143,   144,   137,   142,   158,   154,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   145,   139,
     152,     2,   153,   148,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   146,     2,   147,   150,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   140,   149,   141,   156,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     6,    13,    18,    21,    23,    25,    27,
      30,    32,    36,    39,    42,    44,    47,    49,    52,    54,
      56,    58,    60,    62,    64,    66,    68,    70,    72,    74,
      76,    78,    80,    82,    84,    86,    88,    90,    92,    94,
      96,    98,   100,   102,   104,   106,   108,   110,   112,   114,
     116,   118,   120,   122,   124,   126,   128,   130,   132,   134,
     136,   138,   140,   142,   144,   146,   148,   150,   152,   157,
     158,   165,   168,   169,   175,   177,   179,   181,   184,   186,
     187,   192,   194,   195,   200,   204,   208,   211,   213,   215,
     217,   219,   222,   225,   228,   230,   233,   235,   237,   238,
     243,   245,   246,   251,   252,   256,   257,   264,   265,   271,
     274,   276,   278,   280,   282,   284,   286,   289,   291,   292,
     297,   299,   300,   305,   307,   309,   313,   318,   322,   327,
     332,   336,   338,   342,   344,   345,   350,   353,   356,   358,
     360,   364,   366,   370,   375,   377,   381,   384,   386,   388,
     392,   397,   401,   405,   410,   414,   418,   420,   421,   426,
     428,   432,   434,   436,   437,   438,   446,   448,   450,   454,
     456,   460,   462,   466,   468,   472,   474,   478,   480,   484,
     488,   490,   494,   498,   502,   506,   508,   512,   516,   518,
     522,   526,   528,   532,   536,   540,   542,   543,   544,   551,
     553,   556,   559,   560,   566,   571,   572,   578,   579,   580,
     589,   596,   601,   605,   609,   614,   619,   624,   629,   634,
     639,   644,   651,   656,   661,   666,   671,   676,   681,   686,
     691,   696,   698,   700,   702,   704,   706,   711,   712,   717,
     719,   721,   723,   725,   729,   731,   733,   735,   737,   739
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     160,     0,    -1,     3,   162,    -1,     4,   136,    13,   137,
      13,   138,    -1,     5,   136,    13,   138,    -1,     6,   161,
      -1,    69,    -1,   213,    -1,   163,    -1,   162,   163,    -1,
     164,    -1,   165,   175,   139,    -1,   165,   139,    -1,   166,
     165,    -1,   166,    -1,   167,   165,    -1,   167,    -1,   169,
     165,    -1,   169,    -1,    66,    -1,    69,    -1,    22,    -1,
      59,    -1,    43,    -1,    49,    -1,    36,    -1,    30,    -1,
      60,    -1,    68,    -1,   170,    -1,   188,    -1,    12,    -1,
      23,    -1,   168,    -1,   100,    -1,   101,    -1,   106,    -1,
     107,    -1,   127,    -1,   108,    -1,   128,    -1,   109,    -1,
     129,    -1,   102,    -1,   103,    -1,   124,    -1,   104,    -1,
     125,    -1,   105,    -1,   126,    -1,   110,    -1,   130,    -1,
     113,    -1,   111,    -1,   131,    -1,   114,    -1,   112,    -1,
     132,    -1,   115,    -1,   133,    -1,   118,    -1,   116,    -1,
     134,    -1,   119,    -1,   117,    -1,   135,    -1,    24,    -1,
      70,    -1,    74,   136,   216,   138,    -1,    -1,   173,   240,
     140,   171,   174,   141,    -1,   173,   240,    -1,    -1,   173,
     140,   172,   174,   141,    -1,    63,    -1,    67,    -1,   179,
      -1,   174,   179,    -1,   177,    -1,    -1,   175,   137,   176,
     177,    -1,   197,    -1,    -1,   197,    15,   178,   204,    -1,
     182,   183,   139,    -1,   181,   180,   139,    -1,   182,   183,
      -1,   182,    -1,   142,    -1,   143,    -1,   144,    -1,   142,
     144,    -1,   143,   144,    -1,   167,   182,    -1,   167,    -1,
     169,   182,    -1,   169,    -1,   185,    -1,    -1,   183,   137,
     184,   185,    -1,   197,    -1,    -1,   197,   145,   186,   216,
      -1,    -1,   145,   187,   216,    -1,    -1,   191,   240,   140,
     189,   192,   141,    -1,    -1,   191,   140,   190,   192,   141,
      -1,   191,   240,    -1,    33,    -1,   120,    -1,   121,    -1,
     122,    -1,   123,    -1,   193,    -1,   193,   137,    -1,   195,
      -1,    -1,   193,   137,   194,   195,    -1,   240,    -1,    -1,
     240,    15,   196,   216,    -1,   198,    -1,   240,    -1,   136,
     197,   138,    -1,   198,   146,   216,   147,    -1,   198,   146,
     147,    -1,   198,   136,   199,   138,    -1,   198,   136,   203,
     138,    -1,   198,   136,   138,    -1,   200,    -1,   200,   137,
      31,    -1,   202,    -1,    -1,   200,   137,   201,   202,    -1,
     165,   197,    -1,   165,   207,    -1,   165,    -1,   240,    -1,
     203,   137,   240,    -1,   211,    -1,   140,   205,   141,    -1,
     140,   205,   137,   141,    -1,   204,    -1,   205,   137,   204,
      -1,   182,   207,    -1,   182,    -1,   208,    -1,   136,   207,
     138,    -1,   208,   146,   216,   147,    -1,   146,   216,   147,
      -1,   208,   146,   147,    -1,   208,   136,   199,   138,    -1,
     136,   199,   138,    -1,   208,   136,   138,    -1,   211,    -1,
      -1,   209,   137,   210,   211,    -1,   213,    -1,   230,   212,
     211,    -1,    15,    -1,   217,    -1,    -1,    -1,   217,   148,
     214,   209,   145,   215,   213,    -1,   213,    -1,   218,    -1,
     217,    48,   218,    -1,   219,    -1,   218,    47,   219,    -1,
     220,    -1,   219,   149,   220,    -1,   221,    -1,   220,   150,
     221,    -1,   222,    -1,   221,   151,   222,    -1,   223,    -1,
     222,    34,   223,    -1,   222,    53,   223,    -1,   224,    -1,
     223,   152,   224,    -1,   223,   153,   224,    -1,   223,    46,
     224,    -1,   223,    40,   224,    -1,   225,    -1,   224,    45,
     225,    -1,   224,    58,   225,    -1,   226,    -1,   225,   144,
     226,    -1,   225,   142,   226,    -1,   227,    -1,   226,   143,
     227,    -1,   226,   154,   227,    -1,   226,   155,   227,    -1,
     230,    -1,    -1,    -1,   136,   228,   206,   138,   229,   227,
      -1,   236,    -1,   235,   227,    -1,    61,   230,    -1,    -1,
      61,   136,   231,   206,   138,    -1,    75,   136,   216,   138,
      -1,    -1,    76,   136,   232,   240,   138,    -1,    -1,    -1,
      78,   136,   233,   240,   137,   234,   213,   138,    -1,    93,
     136,   209,   137,   209,   138,    -1,    79,   136,   216,   138,
      -1,    80,   136,   138,    -1,    81,   136,   138,    -1,    82,
     136,   209,   138,    -1,    83,   136,   209,   138,    -1,    84,
     136,   209,   138,    -1,    85,   136,   209,   138,    -1,    86,
     136,   209,   138,    -1,    87,   136,   209,   138,    -1,    88,
     136,   209,   138,    -1,    89,   136,   209,   137,   209,   138,
      -1,    90,   136,   209,   138,    -1,    91,   136,   209,   138,
      -1,    92,   136,   209,   138,    -1,    94,   136,   209,   138,
      -1,    95,   136,   209,   138,    -1,    96,   136,   209,   138,
      -1,    97,   136,   209,   138,    -1,    98,   136,   209,   138,
      -1,    99,   136,   209,   138,    -1,   144,    -1,   142,    -1,
     156,    -1,   157,    -1,   238,    -1,   236,   146,   209,   147,
      -1,    -1,   236,   158,   240,   237,    -1,   240,    -1,   239,
      -1,    77,    -1,    13,    -1,   136,   209,   138,    -1,     7,
      -1,     9,    -1,     8,    -1,    10,    -1,    11,    -1,    72,
     136,    13,   138,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   208,   208,   210,   212,   214,   220,   221,   225,   226,
     234,   237,   239,   243,   244,   245,   246,   247,   248,   251,
     254,   256,   258,   260,   263,   274,   276,   284,   287,   290,
     291,   292,   293,   301,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   338,   340,   342,   347,
     346,   353,   361,   360,   369,   370,   373,   374,   377,   378,
     378,   382,   383,   383,   388,   390,   395,   396,   399,   400,
     401,   402,   403,   406,   407,   408,   409,   412,   413,   413,
     417,   418,   418,   422,   422,   429,   428,   437,   436,   444,
     452,   453,   454,   455,   456,   459,   460,   463,   464,   464,
     468,   469,   469,   474,   477,   478,   479,   482,   485,   487,
     489,   493,   494,   497,   498,   498,   502,   504,   506,   510,
     511,   514,   515,   516,   519,   520,   523,   524,   527,   530,
     531,   533,   535,   537,   538,   539,   547,   548,   548,   552,
     553,   558,   561,   562,   563,   562,   569,   572,   573,   577,
     578,   582,   583,   587,   588,   592,   593,   597,   598,   600,
     604,   605,   607,   609,   611,   615,   616,   618,   622,   623,
     625,   629,   630,   632,   634,   638,   639,   639,   639,   643,
     644,   646,   648,   648,   650,   652,   652,   655,   656,   655,
     658,   660,   662,   664,   666,   668,   670,   672,   674,   676,
     678,   680,   682,   684,   686,   688,   690,   692,   694,   696,
     698,   702,   703,   704,   705,   708,   709,   711,   711,   715,
     753,   754,   755,   756,   759,   760,   761,   762,   770,   771
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TRANSLATION_UNIT", "DEFN_UNIT",
  "IDENT_UNIT", "MAP_EXPR", "INTEGER_TOKEN", "FLOATING_TOKEN",
  "CHARACTER_TOKEN", "ENUMERATION_TOKEN", "IDENTIFIER_TOKEN",
  "TYPEDEF_TOKEN", "STRING_TOKEN", "ASM", "ASSIGN_OP", "AUTO",
  "BIT_AND_ASSIGN_OP", "BIT_OR_ASSIGN_OP", "BIT_XOR_ASSIGN_OP", "BREAK",
  "CASE", "CHAR", "COMPLEX", "CONST", "CONTINUE", "DECREMENT_OP",
  "DEFAULT", "DIVIDE_ASSIGN_OP", "DO", "DOUBLE", "ELIPSE", "ELSE", "ENUM",
  "EQUAL_TO_OP", "EXTERN", "FLOAT", "FOR", "FORTRAN", "GOTO",
  "GREATER_EQUAL_OP", "IF", "INCREMENT_OP", "INT", "LEFT_SHIFT_ASSIGN_OP",
  "LEFT_SHIFT_OP", "LESS_EQUAL_OP", "LOGICAL_AND_OP", "LOGICAL_OR_OP",
  "LONG", "MINUS_ASSIGN_OP", "MODULO_ASSIGN_OP", "MULTIPLY_ASSIGN_OP",
  "NOT_EQUAL_TO_OP", "PLUS_ASSIGN_OP", "REGISTER", "RETURN",
  "RIGHT_SHIFT_ASSIGN_OP", "RIGHT_SHIFT_OP", "SHORT", "SIGNED", "SIZEOF",
  "STATIC", "STRUCT", "STRUCT_POINTER_OP", "SWITCH", "TYPEDEF", "UNION",
  "UNSIGNED", "VOID", "VOLATILE", "WHILE", "IDENT", "DEFN_VALUE", "ALIGN",
  "AXIS_SIZE", "EXIST", "NOP", "FIELD", "ORDINAL", "LINE_TRC", "REEL_TRC",
  "SQRT", "SIN", "COS", "TAN", "ASIN", "ACOS", "ATAN", "ATAN2", "EXP",
  "LOG", "LOG10", "POW", "ABS", "SIGN", "CEIL", "FLOOR", "DATEINT",
  "DATESTRUCT", "ASCII1", "EBCDIC1", "UNSIGNED1", "UNSIGNED2", "UNSIGNED4",
  "UNSIGNED8", "INTEGER1", "INTEGER2", "INTEGER4", "INTEGER8", "FLOAT4",
  "FLOAT8", "FLOAT16", "FLOAT4IBM", "FLOAT8IBM", "COMPLEX8", "COMPLEX16",
  "COMPLEX32", "COMPLEX8IBM", "COMPLEX16IBM", "ENUM1", "ENUM2", "ENUM4",
  "ENUM8", "UNSIGNED2X", "UNSIGNED4X", "UNSIGNED8X", "INTEGER2X",
  "INTEGER4X", "INTEGER8X", "FLOAT4X", "FLOAT8X", "FLOAT16X", "COMPLEX8X",
  "COMPLEX16X", "COMPLEX32X", "'('", "','", "')'", "';'", "'{'", "'}'",
  "'-'", "'*'", "'+'", "':'", "'['", "']'", "'?'", "'|'", "'^'", "'&'",
  "'<'", "'>'", "'/'", "'%'", "'~'", "'!'", "'.'", "$accept", "combined",
  "void_or_conditional_expression", "translation_unit",
  "external_declaration", "declaration", "declaration_specifiers",
  "storage_class_specifier", "type_specifier", "dds_type_specifier",
  "type_qualifier", "struct_or_union_specifier", "@1", "@2",
  "struct_or_union", "struct_declaration_list", "init_declarator_list",
  "@3", "init_declarator", "@4", "struct_declaration",
  "dds_edit_declaration", "dds_member_edit", "specifier_qualifier_list",
  "struct_declarator_list", "@5", "struct_declarator", "@6", "@7",
  "enum_specifier", "@8", "@9", "dds_enum_or_enumn", "enumerator_list_FIX",
  "enumerator_list", "@10", "enumerator", "@11", "declarator",
  "direct_declarator", "parameter_type_list", "parameter_list", "@12",
  "parameter_declaration", "identifier_list", "initializer",
  "initializer_list", "type_name", "abstract_declarator",
  "direct_abstract_declarator", "expression", "@13",
  "assignment_expression", "assignment_operator", "conditional_expression",
  "@14", "@15", "constant_expression", "logical_OR_expression",
  "logical_AND_expression", "inclusive_OR_expression",
  "exclusive_OR_expression", "AND_expression", "equality_expression",
  "relational_expression", "shift_expression", "additive_expression",
  "multiplicative_expression", "cast_expression", "@16", "@17",
  "unary_expression", "@18", "@19", "@20", "@21", "unary_operator",
  "postfix_expression", "@22", "primary_expression", "constant",
  "identifier", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,    40,    44,    41,    59,
     123,   125,    45,    42,    43,    58,    91,    93,    63,   124,
      94,    38,    60,    62,    47,    37,   126,    33,    46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   159,   160,   160,   160,   160,   161,   161,   162,   162,
     163,   164,   164,   165,   165,   165,   165,   165,   165,   166,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   169,   169,   169,   171,
     170,   170,   172,   170,   173,   173,   174,   174,   175,   176,
     175,   177,   178,   177,   179,   179,   180,   180,   181,   181,
     181,   181,   181,   182,   182,   182,   182,   183,   184,   183,
     185,   186,   185,   187,   185,   189,   188,   190,   188,   188,
     191,   191,   191,   191,   191,   192,   192,   193,   194,   193,
     195,   196,   195,   197,   198,   198,   198,   198,   198,   198,
     198,   199,   199,   200,   201,   200,   202,   202,   202,   203,
     203,   204,   204,   204,   205,   205,   206,   206,   207,   208,
     208,   208,   208,   208,   208,   208,   209,   210,   209,   211,
     211,   212,   213,   214,   215,   213,   216,   217,   217,   218,
     218,   219,   219,   220,   220,   221,   221,   222,   222,   222,
     223,   223,   223,   223,   223,   224,   224,   224,   225,   225,
     225,   226,   226,   226,   226,   227,   228,   229,   227,   230,
     230,   230,   231,   230,   230,   232,   230,   233,   234,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   235,   235,   235,   235,   236,   236,   237,   236,   238,
     238,   238,   238,   238,   239,   239,   239,   239,   240,   240
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     6,     4,     2,     1,     1,     1,     2,
       1,     3,     2,     2,     1,     2,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     4,     0,
       6,     2,     0,     5,     1,     1,     1,     2,     1,     0,
       4,     1,     0,     4,     3,     3,     2,     1,     1,     1,
       1,     2,     2,     2,     1,     2,     1,     1,     0,     4,
       1,     0,     4,     0,     3,     0,     6,     0,     5,     2,
       1,     1,     1,     1,     1,     1,     2,     1,     0,     4,
       1,     0,     4,     1,     1,     3,     4,     3,     4,     4,
       3,     1,     3,     1,     0,     4,     2,     2,     1,     1,
       3,     1,     3,     4,     1,     3,     2,     1,     1,     3,
       4,     3,     3,     4,     3,     3,     1,     0,     4,     1,
       3,     1,     1,     0,     0,     7,     1,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     3,
       1,     3,     3,     3,     3,     1,     3,     3,     1,     3,
       3,     1,     3,     3,     3,     1,     0,     0,     6,     1,
       2,     2,     0,     5,     4,     0,     5,     0,     0,     8,
       6,     4,     3,     3,     4,     4,     4,     4,     4,     4,
       4,     6,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     1,     1,     1,     1,     1,     4,     0,     4,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     1,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,     0,     0,     0,    31,    21,    32,    66,
      26,   110,    25,    23,    24,    22,    27,    74,    19,    75,
      28,    20,    67,     0,    34,    35,    43,    44,    46,    48,
      36,    37,    39,    41,    50,    53,    56,    52,    55,    58,
      61,    64,    60,    63,   111,   112,   113,   114,    45,    47,
      49,    38,    40,    42,    51,    54,    57,    59,    62,    65,
       2,     8,    10,     0,    14,    16,    33,    18,    29,     0,
      30,     0,     0,     0,   244,   246,   245,   247,   248,   242,
       0,     6,     0,     0,     0,   241,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   196,   232,
     231,   233,   234,     5,     7,   162,   167,   169,   171,   173,
     175,   177,   180,   185,   188,   191,   195,     0,   199,   235,
     240,   239,     1,     0,     9,     0,    12,     0,    78,    81,
     123,   124,    13,    15,    17,    72,    71,   107,   109,     0,
       0,   202,   201,     0,     0,   205,   207,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   156,
     159,     0,   195,     0,   163,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   200,     0,     0,   166,     0,     0,    79,    11,
      82,     0,     0,     0,    69,     0,   105,     0,     4,     0,
       0,     0,     0,     0,     0,   212,   213,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   157,   243,    94,    96,   147,
       0,   161,     0,   168,     0,   170,   172,   174,   176,   178,
     179,   184,   183,   181,   182,   186,   187,   190,   189,   192,
     193,   194,     0,   237,    68,   125,     0,     0,   130,   138,
       0,   131,   133,     0,   139,   127,     0,    88,    89,    90,
       0,    76,     0,     0,     0,     0,   115,   117,   120,     0,
       0,     0,   249,   204,     0,     0,   211,   214,   215,   216,
     217,   218,   219,   220,     0,   222,   223,   224,     0,   225,
     226,   227,   228,   229,   230,     0,    93,    95,     0,     0,
     146,   148,   197,   160,     0,   236,   238,    80,     0,    83,
     141,     0,   136,   137,   128,   134,     0,   129,   126,    91,
      92,    73,    77,     0,    87,   103,     0,    97,   100,     0,
     108,   118,   121,     0,     3,   203,   206,   208,     0,     0,
     158,     0,     0,     0,     0,     0,     0,   164,   144,     0,
     132,     0,   140,    85,    86,     0,    98,    84,   101,    70,
       0,     0,   106,     0,   221,   210,   154,   149,   151,   155,
       0,   152,     0,   198,     0,     0,   142,   135,   104,     0,
       0,   119,   122,     0,   153,   150,   165,   143,   145,    99,
     102,   209
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     5,   113,    60,    61,    62,   279,    64,    65,    66,
      67,    68,   294,   213,    69,   290,   137,   276,   138,   277,
     291,   353,   292,   293,   356,   409,   357,   410,   385,    70,
     299,   215,    71,   295,   296,   390,   297,   391,   358,   140,
     371,   281,   381,   282,   283,   339,   379,   250,   372,   331,
     178,   325,   179,   252,   180,   254,   404,   206,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   181,
     376,   126,   219,   222,   223,   393,   127,   128,   336,   129,
     130,   131
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -298
static const short int yypact[] =
{
     135,  1853,   -79,   -62,   425,    52,  -298,  -298,  -298,  -298,
    -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,
    -298,  -298,  -298,   -56,  -298,  -298,  -298,  -298,  -298,  -298,
    -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,
    -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,
    -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,
    1853,  -298,  -298,    29,  1853,  1853,  -298,  1853,  -298,    33,
    -298,    37,    87,   102,  -298,  -298,  -298,  -298,  -298,  -298,
     797,  -298,   -13,    -1,     6,  -298,    13,    20,    24,    28,
      36,    42,    47,    51,    53,    55,    57,    79,    82,    91,
     101,   107,   119,   122,   128,   129,   130,   131,   890,  -298,
    -298,  -298,  -298,  -298,  -298,   -16,   221,   120,   123,   124,
       0,     5,     4,   -81,   -84,  -298,  -298,   890,   -86,  -298,
    -298,  -298,  -298,   890,  -298,    31,  -298,    15,  -298,   255,
     -18,  -298,  -298,  -298,  -298,  -298,   132,  -298,   134,   139,
     133,   890,  -298,   264,   890,  -298,  -298,   890,   140,   141,
     890,   890,   890,   890,   890,   890,   890,   890,   890,   890,
     890,   890,   890,   890,   890,   890,   890,   890,   -17,  -298,
    -298,  1977,   265,   890,  -298,   890,   890,   890,   890,   890,
     890,   890,   890,   890,   890,   890,   890,   890,   890,   890,
     890,   890,  -298,   890,    39,  -298,   145,   146,  -298,  -298,
    -298,  1605,   518,  1480,  -298,    39,  -298,   272,  -298,  1977,
     149,   150,    39,    39,   151,  -298,  -298,     8,    58,    60,
      67,    69,    72,    74,   153,    76,    84,    86,   154,    88,
      92,    94,    97,   104,   108,  -298,  -298,  1977,  1977,   -12,
     155,  -298,   890,   221,   890,   120,   123,   124,     0,     5,
       5,     4,     4,     4,     4,   -81,   -81,   -84,   -84,  -298,
    -298,  -298,   -11,  -298,  -298,  -298,    31,   611,  -298,    -3,
     157,   159,  -298,   111,  -298,  -298,   152,   148,   156,  -298,
    1232,  -298,  1977,    35,  1480,   160,   161,  -298,   282,    39,
     164,   165,  -298,  -298,   166,   168,  -298,  -298,  -298,  -298,
    -298,  -298,  -298,  -298,   890,  -298,  -298,  -298,   890,  -298,
    -298,  -298,  -298,  -298,  -298,   890,  -298,  -298,  1107,   890,
    -298,    -9,  -298,  -298,  -102,  -298,  -298,  -298,   611,  -298,
    -298,   982,  -298,  -298,  -298,   275,    39,  -298,  -298,  -298,
    -298,  -298,  -298,   169,    35,  -298,    22,  -298,   162,  1356,
    -298,   170,  -298,   171,  -298,  -298,  -298,  -298,   113,   115,
    -298,   172,   175,   167,  1729,   704,   890,  -298,  -298,   -83,
    -298,  1853,  -298,  -298,   178,   890,  -298,  -298,  -298,  -298,
      39,   890,  -298,   890,  -298,  -298,  -298,  -298,  -298,  -298,
     179,  -298,   173,  -298,   890,   332,  -298,  -298,  -298,    35,
     890,  -298,  -298,   180,  -298,  -298,  -298,  -298,  -298,  -298,
    -298,  -298
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -298,  -298,  -298,  -298,   249,  -298,   121,  -298,  -180,  -298,
    -100,  -298,  -298,  -298,  -298,    25,  -298,  -298,    40,  -298,
    -243,  -298,  -298,  -117,   -33,  -298,   -87,  -298,  -298,  -298,
    -298,  -298,  -298,    27,  -298,  -298,   -67,  -298,   -59,  -298,
    -205,  -298,  -298,   -57,  -298,  -297,  -298,   109,  -213,  -298,
    -148,  -298,  -221,  -298,    -4,  -298,  -298,  -147,  -298,   163,
     144,   147,   174,   143,  -152,     9,    61,    64,  -122,  -298,
    -298,   -78,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,
    -298,   -60
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -117
static const short int yytable[] =
{
     114,   247,   152,   141,   139,   202,   280,   221,    78,   146,
     224,   148,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     182,   333,   183,   247,   189,   245,   330,   259,   260,   247,
      78,   378,    78,   377,    78,   191,    78,   352,    78,   195,
      78,   192,   132,   190,   405,   272,   340,    72,   406,   199,
     203,   197,   196,   198,   249,   286,   343,   247,   247,    82,
     200,   201,   204,   182,    73,   141,   207,   269,   270,   271,
     133,   248,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     149,    82,   249,    82,   370,    82,   334,    82,   418,    82,
     247,    82,   247,   248,   247,   150,   352,   340,   211,   248,
     245,   246,    63,   153,   328,   182,   245,   374,   212,   205,
     326,   327,   184,   341,   329,   154,   335,   375,     1,     2,
       3,     4,   155,   329,   273,   245,   307,   248,   248,   156,
     205,   284,   208,   205,   209,   298,   157,   193,   194,   386,
     158,   387,   304,   305,   159,   135,   368,   135,   136,   400,
     369,   135,   160,   145,   182,   354,   182,   147,   161,   247,
     355,    63,   373,   162,   340,   142,   143,   163,   144,   164,
     248,   165,   248,   166,   248,   245,   308,   245,   309,   182,
     261,   262,   263,   264,   245,   310,   245,   311,   205,   245,
     312,   245,   313,   245,   315,   167,   141,   139,   168,   141,
     342,   245,   316,   245,   317,   245,   319,   169,   402,   245,
     320,   245,   321,   141,   245,   322,   182,   170,   408,   298,
     182,   245,   323,   171,   412,   245,   324,   182,   346,   347,
     245,   394,   245,   395,   403,   172,   265,   266,   173,   248,
     182,   267,   268,   420,   174,   175,   176,   177,   185,   186,
     210,   218,   214,   187,   216,   188,   217,   220,   225,   226,
     251,   141,   207,   274,   275,   300,   382,   302,   303,   306,
     314,   318,   349,   332,   141,   344,   345,   362,   361,   348,
     350,   360,   364,   365,   366,   367,   380,   388,   383,   134,
     396,  -116,   392,   397,   398,   386,   337,   414,   421,   359,
     415,   384,   419,   411,   407,   205,   363,   182,   301,   255,
     298,   258,     0,   256,     0,     0,     0,     0,     0,    74,
      75,    76,    77,    78,     0,    79,   253,     0,     0,   141,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   257,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   205,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   205,     0,     0,     0,     0,     0,   205,     0,   413,
       0,     0,     0,    80,     0,     0,     0,     0,     0,     0,
     416,     0,     0,     0,    82,     0,   205,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,    74,    75,    76,    77,    78,     0,    79,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   108,     0,
       0,     0,   338,   417,   109,     0,   110,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    80,     0,   111,   112,
       0,     0,     0,     0,    81,     0,     0,    82,     0,     0,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,    74,    75,    76,    77,    78,
       0,    79,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   108,     0,     0,     0,     0,     0,   109,     0,   110,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    80,
       0,   111,   112,     0,     0,     0,     0,     0,     0,     0,
      82,     0,     0,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,    74,    75,
      76,    77,    78,     0,    79,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   108,     0,     0,     0,     0,     0,
     109,     0,   110,     0,     0,   285,     0,     0,     0,     0,
       0,     0,    80,     0,   111,   112,     0,     0,     0,     0,
       0,     0,     0,    82,     0,     0,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,    74,    75,    76,    77,    78,     0,    79,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   108,     0,     0,
       0,   338,     0,   109,     0,   110,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    80,     0,   111,   112,     0,
       0,     0,     0,     0,     0,     0,    82,     0,     0,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,    74,    75,    76,    77,    78,     0,
      79,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     108,     0,     0,     0,     0,     0,   109,     0,   110,     0,
       0,   401,     0,     0,     0,     0,     0,     0,    80,     0,
     111,   112,     0,     0,     0,     0,     0,     0,     0,    82,
       0,     0,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,    74,    75,    76,
      77,    78,     0,    79,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   151,     0,     0,     0,     0,     0,   109,
       0,   110,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    80,     0,   111,   112,     0,     0,     0,     0,     0,
       0,     0,    82,     0,     0,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
       0,     0,     0,    78,     6,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     7,     8,     9,     0,     0,     0,
       0,     0,    10,     0,     0,    11,     0,     0,    12,     0,
       0,     0,     0,     0,     0,    13,   108,     0,     0,     0,
       0,    14,   109,     0,   110,     0,     0,     0,     0,     0,
       0,    15,    16,     0,     0,    17,   111,   112,    18,    19,
      20,    21,    22,     0,    82,     0,    23,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,   341,     6,
       0,     0,     0,     0,     0,     0,     0,     0,   329,     7,
       8,     9,     0,     0,     0,     0,     0,    10,     0,     0,
      11,     0,     0,    12,     0,     0,     0,     0,     0,     0,
      13,     0,     0,     0,     0,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,    16,     0,     0,
      17,     0,     0,    18,    19,    20,    21,    22,     0,     0,
       0,    23,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,   328,     6,     0,     0,     0,     0,     0,
       0,     0,     0,   329,     7,     8,     9,     0,     0,     0,
       0,     0,    10,     0,     0,    11,     0,     0,    12,     0,
       0,     0,     0,     0,     0,    13,     0,     0,     0,     0,
       0,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    15,    16,     0,     0,    17,     0,     0,     0,    19,
      20,    21,    22,     0,     0,     0,    23,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,     6,     0,
       0,     0,     0,   351,   287,   288,   289,     0,     7,     8,
       9,     0,     0,     0,     0,     0,    10,     0,     0,    11,
       0,     0,    12,     0,     0,     0,     0,     0,     0,    13,
       0,     0,     0,     0,     0,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    15,    16,     0,     0,    17,
       0,     0,     0,    19,    20,    21,    22,     0,     0,     0,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,     6,     0,     0,     0,     0,   389,   287,   288,
     289,     0,     7,     8,     9,     0,     0,     0,     0,     0,
      10,     0,     0,    11,     0,     0,    12,     0,     0,     0,
       0,     0,     0,    13,     0,     0,     0,     0,     0,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    15,
      16,     0,     0,    17,     0,     0,     0,    19,    20,    21,
      22,     0,     0,     0,    23,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    78,     6,     0,     0,
       0,     0,   287,   288,   289,     0,     0,     7,     8,     9,
       0,     0,     0,     0,     0,    10,     0,     0,    11,     0,
       0,    12,     0,     0,     0,     0,     0,     0,    13,     0,
       0,     0,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    15,    16,     0,     0,    17,     0,
       0,    18,    19,    20,    21,    22,     0,    82,     0,    23,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,     6,     0,   278,     0,     0,     0,     0,     0,     0,
       0,     7,     8,     9,     0,     0,     0,     0,     0,    10,
       0,     0,    11,     0,     0,    12,     0,     0,     0,     0,
       0,     0,    13,     0,     0,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    15,    16,
       0,     0,    17,     0,     0,    18,    19,    20,    21,    22,
       0,     0,     0,    23,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,     6,     0,   399,     0,     0,
       0,     0,     0,     0,     0,     7,     8,     9,     0,     0,
       0,     0,     0,    10,     0,     0,    11,     0,     0,    12,
       0,     0,     0,     0,     0,     0,    13,     0,     0,     0,
       0,     0,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    15,    16,     0,     0,    17,     0,     0,    18,
      19,    20,    21,    22,     0,     0,     0,    23,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,     6,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     7,
       8,     9,     0,     0,     0,     0,     0,    10,     0,     0,
      11,     0,     0,    12,     0,     0,     0,     0,     0,     0,
      13,     0,     0,     0,     0,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,    16,     0,     0,
      17,     0,     0,     0,    19,    20,    21,    22,     0,     0,
       0,    23,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59
};

static const short int yycheck[] =
{
       4,   181,    80,    63,    63,   127,   211,   154,    11,    69,
     157,    71,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     108,   252,    48,   213,    34,   137,   249,   189,   190,   219,
      11,   338,    11,   145,    11,    40,    11,   290,    11,    45,
      11,    46,     0,    53,   137,   203,   277,   136,   141,   143,
     146,   142,    58,   144,   181,   212,   279,   247,   248,    72,
     154,   155,   158,   151,   136,   135,   135,   199,   200,   201,
     136,   181,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
      13,    72,   219,    72,   325,    72,   254,    72,   405,    72,
     290,    72,   292,   213,   294,    13,   359,   338,   136,   219,
     137,   138,     1,   136,   136,   203,   137,   136,   146,   133,
     247,   248,   148,   136,   146,   136,   147,   146,     3,     4,
       5,     6,   136,   146,   204,   137,   138,   247,   248,   136,
     154,   211,   137,   157,   139,   215,   136,   152,   153,   137,
     136,   139,   222,   223,   136,   136,   314,   136,   139,   374,
     318,   136,   136,   140,   252,   292,   254,   140,   136,   359,
     145,    60,   329,   136,   405,    64,    65,   136,    67,   136,
     290,   136,   292,   136,   294,   137,   138,   137,   138,   277,
     191,   192,   193,   194,   137,   138,   137,   138,   212,   137,
     138,   137,   138,   137,   138,   136,   276,   276,   136,   279,
     279,   137,   138,   137,   138,   137,   138,   136,   375,   137,
     138,   137,   138,   293,   137,   138,   314,   136,   385,   299,
     318,   137,   138,   136,   391,   137,   138,   325,   137,   138,
     137,   138,   137,   138,   376,   136,   195,   196,   136,   359,
     338,   197,   198,   410,   136,   136,   136,   136,    47,   149,
      15,   138,   140,   150,   140,   151,   137,    13,   138,   138,
      15,   341,   341,   138,   138,    13,   346,   138,   138,   138,
     137,   137,   144,   138,   354,   138,   137,    15,   137,   147,
     144,   141,   138,   138,   138,   137,    31,   145,   139,    60,
     138,   141,   141,   138,   147,   137,   276,   138,   138,   294,
     147,   354,   409,   390,   381,   329,   299,   405,   219,   185,
     390,   188,    -1,   186,    -1,    -1,    -1,    -1,    -1,     7,
       8,     9,    10,    11,    -1,    13,   183,    -1,    -1,   409,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   187,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   375,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   385,    -1,    -1,    -1,    -1,    -1,   391,    -1,   393,
      -1,    -1,    -1,    61,    -1,    -1,    -1,    -1,    -1,    -1,
     404,    -1,    -1,    -1,    72,    -1,   410,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,     7,     8,     9,    10,    11,    -1,    13,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   136,    -1,
      -1,    -1,   140,   141,   142,    -1,   144,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,   156,   157,
      -1,    -1,    -1,    -1,    69,    -1,    -1,    72,    -1,    -1,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,     7,     8,     9,    10,    11,
      -1,    13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   136,    -1,    -1,    -1,    -1,    -1,   142,    -1,   144,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      -1,   156,   157,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      72,    -1,    -1,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,     7,     8,
       9,    10,    11,    -1,    13,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   136,    -1,    -1,    -1,    -1,    -1,
     142,    -1,   144,    -1,    -1,   147,    -1,    -1,    -1,    -1,
      -1,    -1,    61,    -1,   156,   157,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    72,    -1,    -1,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,     7,     8,     9,    10,    11,    -1,    13,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   136,    -1,    -1,
      -1,   140,    -1,   142,    -1,   144,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,   156,   157,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    72,    -1,    -1,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,     7,     8,     9,    10,    11,    -1,
      13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     136,    -1,    -1,    -1,    -1,    -1,   142,    -1,   144,    -1,
      -1,   147,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,
     156,   157,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,
      -1,    -1,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,     7,     8,     9,
      10,    11,    -1,    13,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   136,    -1,    -1,    -1,    -1,    -1,   142,
      -1,   144,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    -1,   156,   157,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    72,    -1,    -1,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    22,    23,    24,    -1,    -1,    -1,
      -1,    -1,    30,    -1,    -1,    33,    -1,    -1,    36,    -1,
      -1,    -1,    -1,    -1,    -1,    43,   136,    -1,    -1,    -1,
      -1,    49,   142,    -1,   144,    -1,    -1,    -1,    -1,    -1,
      -1,    59,    60,    -1,    -1,    63,   156,   157,    66,    67,
      68,    69,    70,    -1,    72,    -1,    74,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   146,    22,
      23,    24,    -1,    -1,    -1,    -1,    -1,    30,    -1,    -1,
      33,    -1,    -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    59,    60,    -1,    -1,
      63,    -1,    -1,    66,    67,    68,    69,    70,    -1,    -1,
      -1,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,    12,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   146,    22,    23,    24,    -1,    -1,    -1,
      -1,    -1,    30,    -1,    -1,    33,    -1,    -1,    36,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    -1,    -1,    -1,    -1,
      -1,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    59,    60,    -1,    -1,    63,    -1,    -1,    -1,    67,
      68,    69,    70,    -1,    -1,    -1,    74,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,    12,    -1,
      -1,    -1,    -1,   141,   142,   143,   144,    -1,    22,    23,
      24,    -1,    -1,    -1,    -1,    -1,    30,    -1,    -1,    33,
      -1,    -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    59,    60,    -1,    -1,    63,
      -1,    -1,    -1,    67,    68,    69,    70,    -1,    -1,    -1,
      74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,    12,    -1,    -1,    -1,    -1,   141,   142,   143,
     144,    -1,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,
      30,    -1,    -1,    33,    -1,    -1,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    -1,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      60,    -1,    -1,    63,    -1,    -1,    -1,    67,    68,    69,
      70,    -1,    -1,    -1,    74,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,    11,    12,    -1,    -1,
      -1,    -1,   142,   143,   144,    -1,    -1,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    30,    -1,    -1,    33,    -1,
      -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,
      -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    59,    60,    -1,    -1,    63,    -1,
      -1,    66,    67,    68,    69,    70,    -1,    72,    -1,    74,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,    12,    -1,   138,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      -1,    -1,    33,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      -1,    -1,    43,    -1,    -1,    -1,    -1,    -1,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    60,
      -1,    -1,    63,    -1,    -1,    66,    67,    68,    69,    70,
      -1,    -1,    -1,    74,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,    12,    -1,   138,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    -1,    -1,    33,    -1,    -1,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,    -1,    -1,
      -1,    -1,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    59,    60,    -1,    -1,    63,    -1,    -1,    66,
      67,    68,    69,    70,    -1,    -1,    -1,    74,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    22,
      23,    24,    -1,    -1,    -1,    -1,    -1,    30,    -1,    -1,
      33,    -1,    -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    59,    60,    -1,    -1,
      63,    -1,    -1,    -1,    67,    68,    69,    70,    -1,    -1,
      -1,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,     4,     5,     6,   160,    12,    22,    23,    24,
      30,    33,    36,    43,    49,    59,    60,    63,    66,    67,
      68,    69,    70,    74,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   173,
     188,   191,   136,   136,     7,     8,     9,    10,    11,    13,
      61,    69,    72,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   136,   142,
     144,   156,   157,   161,   213,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   230,   235,   236,   238,
     239,   240,     0,   136,   163,   136,   139,   175,   177,   197,
     198,   240,   165,   165,   165,   140,   240,   140,   240,    13,
      13,   136,   230,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   209,   211,
     213,   228,   230,    48,   148,    47,   149,   150,   151,    34,
      53,    40,    46,   152,   153,    45,    58,   142,   144,   143,
     154,   155,   227,   146,   158,   213,   216,   197,   137,   139,
      15,   136,   146,   172,   140,   190,   140,   137,   138,   231,
      13,   216,   232,   233,   216,   138,   138,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   137,   138,   167,   169,   182,
     206,    15,   212,   218,   214,   219,   220,   221,   222,   223,
     223,   224,   224,   224,   224,   225,   225,   226,   226,   227,
     227,   227,   209,   240,   138,   138,   176,   178,   138,   165,
     199,   200,   202,   203,   240,   147,   216,   142,   143,   144,
     174,   179,   181,   182,   171,   192,   193,   195,   240,   189,
      13,   206,   138,   138,   240,   240,   138,   138,   138,   138,
     138,   138,   138,   138,   137,   138,   138,   138,   137,   138,
     138,   138,   138,   138,   138,   210,   182,   182,   136,   146,
     207,   208,   138,   211,   209,   147,   237,   177,   140,   204,
     211,   136,   197,   207,   138,   137,   137,   138,   147,   144,
     144,   141,   179,   180,   182,   145,   183,   185,   197,   174,
     141,   137,    15,   192,   138,   138,   138,   137,   209,   209,
     211,   199,   207,   216,   136,   146,   229,   145,   204,   205,
      31,   201,   240,   139,   183,   187,   137,   139,   145,   141,
     194,   196,   141,   234,   138,   138,   138,   138,   147,   138,
     199,   147,   216,   227,   215,   137,   141,   202,   216,   184,
     186,   195,   216,   213,   138,   147,   213,   141,   204,   185,
     216,   138
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname[yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()
    ;
#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 3:

    {ddsDefnValue((yyvsp[-3].YYvalue), (yyvsp[-1].YYvalue)); (yyval.YYvoid) = VOIDED; YYACCEPT;}
    break;

  case 4:

    {*((DDS_VALUE**)parm) = (yyvsp[-1].YYvalue); (yyval.YYvoid) = VOIDED; YYACCEPT;}
    break;

  case 5:

    {*((DDS_VALUE**)parm) = ((yyvsp[0].YYvalue) && (yyvsp[0].YYvalue)->type->ptype != DDS_CTYPE_VOID) ?
            (yyvsp[0].YYvalue) : NULL;}
    break;

  case 6:

    {(yyval.YYvalue) = VOIDED;}
    break;

  case 7:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 8:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 9:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 10:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 11:

    {DECLARE_CLEAR; needle = dds_needle; (yyval.YYvoid) = VOIDED;}
    break;

  case 12:

    {DECLARE_CLEAR; needle = dds_needle; (yyval.YYvoid) = VOIDED;}
    break;

  case 13:

    {(yyval.YYtype) = (yyvsp[-1].YYtype);}
    break;

  case 14:

    {(yyval.YYtype) = (yyvsp[0].YYtype);}
    break;

  case 15:

    {(yyval.YYtype) = (yyvsp[-1].YYtype);}
    break;

  case 16:

    {(yyval.YYtype) = (yyvsp[0].YYtype);}
    break;

  case 17:

    {(yyval.YYtype) = (yyvsp[-1].YYtype);}
    break;

  case 18:

    {(yyval.YYtype) = (yyvsp[0].YYtype);}
    break;

  case 19:

    {(yyval.YYtype) = ddsSymTypeStore(DDS_SYM_TYPEDEF);}
    break;

  case 20:

    {(yyval.YYtype) = ddsSymTypeSpec(DDS_SYM_VOID,
         (DDS_SYM_FLAG)(0));}
    break;

  case 21:

    {(yyval.YYtype) = ddsSymTypeSpec(DDS_SYM_CHAR,
         (DDS_SYM_FLAG)(DDS_SYM_SIGNED | DDS_SYM_UNSIGNED));}
    break;

  case 22:

    {(yyval.YYtype) = ddsSymTypeSpec(DDS_SYM_SHORT,
         (DDS_SYM_FLAG)(DDS_SYM_SIGNED | DDS_SYM_UNSIGNED | DDS_SYM_INT));}
    break;

  case 23:

    {(yyval.YYtype) = ddsSymTypeSpec(DDS_SYM_INT,
         (DDS_SYM_FLAG)(DDS_SYM_SIGNED | DDS_SYM_UNSIGNED | DDS_SYM_SHORT |
            DDS_SYM_LONG | DDS_SYM_LLONG));}
    break;

  case 24:

    {if(MAKE_LLONG && dds_declare->type &&
               (dds_declare->type->flag & DDS_SYM_LONG)) {
               (yyval.YYtype) = ddsSymTypeSpec(DDS_SYM_LLONG,
                  (DDS_SYM_FLAG)(DDS_SYM_SIGNED | DDS_SYM_UNSIGNED |
                     DDS_SYM_LONG | DDS_SYM_INT));
            } else {
               (yyval.YYtype) = ddsSymTypeSpec(DDS_SYM_LONG,
                  (DDS_SYM_FLAG)(DDS_SYM_SIGNED | DDS_SYM_UNSIGNED |
                     DDS_SYM_INT | DDS_SYM_DOUBLE | DDS_SYM_COMPLEX));
            }
         ;}
    break;

  case 25:

    {(yyval.YYtype) = ddsSymTypeSpec(DDS_SYM_FLOAT,
         (DDS_SYM_FLAG)(DDS_SYM_COMPLEX));}
    break;

  case 26:

    {if(MAKE_LDOUBLE) {
               (yyval.YYtype) = ddsSymTypeSpec(DDS_SYM_DOUBLE,
                  (DDS_SYM_FLAG)(DDS_SYM_LONG | DDS_SYM_COMPLEX));
            } else {
               (yyval.YYtype) = ddsSymTypeSpec(DDS_SYM_DOUBLE,
                  (DDS_SYM_FLAG)(DDS_SYM_COMPLEX));
            }
         ;}
    break;

  case 27:

    {(yyval.YYtype) = ddsSymTypeSpec(DDS_SYM_SIGNED,
         (DDS_SYM_FLAG)(DDS_SYM_CHAR | DDS_SYM_SHORT | DDS_SYM_INT |
            DDS_SYM_LONG | DDS_SYM_LLONG));}
    break;

  case 28:

    {(yyval.YYtype) = ddsSymTypeSpec(DDS_SYM_UNSIGNED,
         (DDS_SYM_FLAG)(DDS_SYM_CHAR | DDS_SYM_SHORT | DDS_SYM_INT |
            DDS_SYM_LONG | DDS_SYM_LLONG));}
    break;

  case 29:

    {(yyval.YYtype) = (yyvsp[0].YYtype);}
    break;

  case 30:

    {(yyval.YYtype) = (yyvsp[0].YYtype);}
    break;

  case 31:

    {(yyval.YYtype) = (yyvsp[0].YYtype);}
    break;

  case 32:

    {if(MAKE_LDOUBLE) {
               (yyval.YYtype) = ddsSymTypeSpec(DDS_SYM_COMPLEX,
                  (DDS_SYM_FLAG)(DDS_SYM_FLOAT | DDS_SYM_DOUBLE | DDS_SYM_LONG));
            } else {
               (yyval.YYtype) = ddsSymTypeSpec(DDS_SYM_COMPLEX,
                  (DDS_SYM_FLAG)(DDS_SYM_FLOAT | DDS_SYM_DOUBLE));
            }
         ;}
    break;

  case 33:

    {(yyval.YYtype) = (yyvsp[0].YYtype);}
    break;

  case 34:

    {(yyval.YYtype) = TYPE_MERGE(ASCII1);}
    break;

  case 35:

    {(yyval.YYtype) = TYPE_MERGE(EBCDIC1);}
    break;

  case 36:

    {(yyval.YYtype) = TYPE_MERGE(INTEGER1);}
    break;

  case 37:

    {(yyval.YYtype) = TYPE_MERGE(INTEGER2);}
    break;

  case 38:

    {(yyval.YYtype) = TYPE_MERGE(INTEGER2X);}
    break;

  case 39:

    {(yyval.YYtype) = TYPE_MERGE(INTEGER4);}
    break;

  case 40:

    {(yyval.YYtype) = TYPE_MERGE(INTEGER4X);}
    break;

  case 41:

    {(yyval.YYtype) = TYPE_MERGE(INTEGER8);}
    break;

  case 42:

    {(yyval.YYtype) = TYPE_MERGE(INTEGER8X);}
    break;

  case 43:

    {(yyval.YYtype) = TYPE_MERGE(UNSIGNED1);}
    break;

  case 44:

    {(yyval.YYtype) = TYPE_MERGE(UNSIGNED2);}
    break;

  case 45:

    {(yyval.YYtype) = TYPE_MERGE(UNSIGNED2X);}
    break;

  case 46:

    {(yyval.YYtype) = TYPE_MERGE(UNSIGNED4);}
    break;

  case 47:

    {(yyval.YYtype) = TYPE_MERGE(UNSIGNED4X);}
    break;

  case 48:

    {(yyval.YYtype) = TYPE_MERGE(UNSIGNED8);}
    break;

  case 49:

    {(yyval.YYtype) = TYPE_MERGE(UNSIGNED8X);}
    break;

  case 50:

    {(yyval.YYtype) = TYPE_MERGE(FLOAT4);}
    break;

  case 51:

    {(yyval.YYtype) = TYPE_MERGE(FLOAT4X);}
    break;

  case 52:

    {(yyval.YYtype) = TYPE_MERGE(FLOAT4IBM);}
    break;

  case 53:

    {(yyval.YYtype) = TYPE_MERGE(FLOAT8);}
    break;

  case 54:

    {(yyval.YYtype) = TYPE_MERGE(FLOAT8X);}
    break;

  case 55:

    {(yyval.YYtype) = TYPE_MERGE(FLOAT8IBM);}
    break;

  case 56:

    {(yyval.YYtype) = TYPE_MERGE(FLOAT16);}
    break;

  case 57:

    {(yyval.YYtype) = TYPE_MERGE(FLOAT16X);}
    break;

  case 58:

    {(yyval.YYtype) = TYPE_MERGE(COMPLEX8);}
    break;

  case 59:

    {(yyval.YYtype) = TYPE_MERGE(COMPLEX8X);}
    break;

  case 60:

    {(yyval.YYtype) = TYPE_MERGE(COMPLEX8IBM);}
    break;

  case 61:

    {(yyval.YYtype) = TYPE_MERGE(COMPLEX16);}
    break;

  case 62:

    {(yyval.YYtype) = TYPE_MERGE(COMPLEX16X);}
    break;

  case 63:

    {(yyval.YYtype) = TYPE_MERGE(COMPLEX16IBM);}
    break;

  case 64:

    {(yyval.YYtype) = TYPE_MERGE(COMPLEX32);}
    break;

  case 65:

    {(yyval.YYtype) = TYPE_MERGE(COMPLEX32X);}
    break;

  case 66:

    {(yyval.YYtype) = ddsSymTypeQual(DDS_SYM_CONST, NULL);}
    break;

  case 67:

    {(yyval.YYtype) = ddsSymTypeQual(DDS_SYM_VOLATILE, NULL);}
    break;

  case 68:

    {(yyval.YYtype) = ddsSymTypeQual(DDS_SYM_ALIGN, (yyvsp[-1].YYvalue));}
    break;

  case 69:

    {(yyval.YYblock) = ddsSymBlockBegin((yyvsp[-1].YYname), (DDS_SYM_FLAG)(
                (((yyvsp[-2].YYptype) == DDS_STYPE_STRUCT) ? DDS_SYM_STRUCT : DDS_SYM_UNION)),
                dds_prime[(yyvsp[-2].YYptype)].type);}
    break;

  case 70:

    {ddsSymBlockEnd();
            (yyval.YYtype) = ddsSymTypeSUE((yyvsp[-2].YYblock));}
    break;

  case 71:

    {(yyval.YYblock) = ddsSymBlockBegin((yyvsp[0].YYname), (DDS_SYM_FLAG)(
                DDS_SYM_INCOMPLETE |
                (((yyvsp[-1].YYptype) == DDS_STYPE_STRUCT) ? DDS_SYM_STRUCT : DDS_SYM_UNION)),
                dds_prime[(yyvsp[-1].YYptype)].type);
                ddsSymBlockEnd();
            (yyval.YYtype) = ddsSymTypeSUE((yyval.YYblock));}
    break;

  case 72:

    {(yyval.YYblock) = ddsSymBlockBegin(NULL, (DDS_SYM_FLAG)(
                (((yyvsp[-1].YYptype) == DDS_STYPE_STRUCT) ? DDS_SYM_STRUCT : DDS_SYM_UNION)),
                dds_prime[(yyvsp[-1].YYptype)].type);}
    break;

  case 73:

    {ddsSymBlockEnd();
            (yyval.YYtype) = ddsSymTypeSUE((yyvsp[-2].YYblock));}
    break;

  case 74:

    {(yyval.YYptype) = DDS_STYPE_STRUCT;}
    break;

  case 75:

    {(yyval.YYptype) = DDS_STYPE_UNION;}
    break;

  case 76:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 77:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 78:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 79:

    {DECLARE_MORE;}
    break;

  case 80:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 81:

    {ddsSymDeclare((yyvsp[0].YYname)); (yyval.YYvoid) = VOIDED;}
    break;

  case 82:

    {ddsSymDeclare((yyvsp[-1].YYname));}
    break;

  case 83:

    {ddsYYError("parser.y: initializers are deferred\n"); (yyval.YYvoid) = VOIDED;}
    break;

  case 84:

    {DECLARE_CLEAR; needle = dds_needle; (yyval.YYvoid) = VOIDED;}
    break;

  case 85:

    {ddsSymMemberEdit((yyvsp[-2].YYedit), (yyvsp[-1].YYmember));
              DECLARE_CLEAR; needle = dds_needle; (yyval.YYvoid) = VOIDED;}
    break;

  case 86:

    {(yyval.YYmember) = (yyvsp[0].YYmember);}
    break;

  case 87:

    {(yyval.YYmember) = 0;}
    break;

  case 88:

    {(yyval.YYedit) = DDS_SYM_DELETE;}
    break;

  case 89:

    {(yyval.YYedit) = DDS_SYM_REPLACE;}
    break;

  case 90:

    {(yyval.YYedit) = DDS_SYM_APPEND;}
    break;

  case 91:

    {(yyval.YYedit) = DDS_SYM_DAPPEND;}
    break;

  case 92:

    {(yyval.YYedit) = DDS_SYM_RAPPEND;}
    break;

  case 93:

    {(yyval.YYtype) = (yyvsp[-1].YYtype);}
    break;

  case 94:

    {(yyval.YYtype) = (yyvsp[0].YYtype);}
    break;

  case 95:

    {(yyval.YYtype) = (yyvsp[-1].YYtype);}
    break;

  case 96:

    {(yyval.YYtype) = (yyvsp[0].YYtype);}
    break;

  case 97:

    {(yyval.YYmember) = (yyvsp[0].YYmember);}
    break;

  case 98:

    {DECLARE_MORE; (yyval.YYmember) = (yyvsp[-1].YYmember);}
    break;

  case 99:

    {(yyval.YYmember) = (yyvsp[-3].YYmember);}
    break;

  case 100:

    {(yyval.YYmember) = ddsSymMember((yyvsp[0].YYname));}
    break;

  case 101:

    {(yyval.YYmember) = ddsSymMember((yyvsp[-1].YYname));}
    break;

  case 102:

    {(yyval.YYmember) = ddsSymMemberBit((yyvsp[-1].YYmember),
               ddsSymConstInt((yyvsp[0].YYvalue), 0, DDS_SYM_CONST_DECL));}
    break;

  case 103:

    {(yyval.YYmember) = ddsSymMember(NULL);}
    break;

  case 104:

    {(yyval.YYmember) = ddsSymMemberBit((yyvsp[-1].YYmember),
               ddsSymConstSizet((yyvsp[0].YYvalue), 0, DDS_SYM_CONST_DECL));}
    break;

  case 105:

    {(yyval.YYblock) = ddsSymBlockBegin((yyvsp[-1].YYname),
               (DDS_SYM_FLAG)(DDS_SYM_ENUM),
               dds_prime[(yyvsp[-2].YYptype)].type);
               ddsSymBlockEnd();
               dds_declare->flag |= DDS_SYM_IDENT;}
    break;

  case 106:

    {(yyval.YYtype) = ddsSymTypeSUE((yyvsp[-2].YYblock));}
    break;

  case 107:

    {(yyval.YYblock) = ddsSymBlockBegin(NULL,
               (DDS_SYM_FLAG)(DDS_SYM_ENUM),
               dds_prime[(yyvsp[-1].YYptype)].type);
               ddsSymBlockEnd();
               dds_declare->flag |= DDS_SYM_IDENT;}
    break;

  case 108:

    {(yyval.YYtype) = ddsSymTypeSUE((yyvsp[-2].YYblock));}
    break;

  case 109:

    {(yyval.YYblock) = ddsSymBlockBegin((yyvsp[0].YYname),
               (DDS_SYM_FLAG)(DDS_SYM_ENUM | DDS_SYM_INCOMPLETE),
               dds_prime[(yyvsp[-1].YYptype)].type);
               ddsSymBlockEnd();
              (yyval.YYtype) = ddsSymTypeSUE((yyval.YYblock));}
    break;

  case 110:

    {(yyval.YYptype) = DDS_CTYPE_INT;}
    break;

  case 111:

    {(yyval.YYptype) = DDS_STYPE_INTEGER1;}
    break;

  case 112:

    {(yyval.YYptype) = DDS_STYPE_INTEGER2;}
    break;

  case 113:

    {(yyval.YYptype) = DDS_STYPE_INTEGER4;}
    break;

  case 114:

    {(yyval.YYptype) = DDS_STYPE_INTEGER8;}
    break;

  case 115:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 116:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 117:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 118:

    {dds_declare->flag |= DDS_SYM_IDENT;}
    break;

  case 119:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 120:

    {(yyval.YYobject) = ddsSymEnumName((yyvsp[0].YYname));}
    break;

  case 121:

    {(yyval.YYobject) = ddsSymEnumName((yyvsp[-1].YYname));}
    break;

  case 122:

    {(yyval.YYobject) = ddsSymEnumValue((yyvsp[-1].YYobject), (yyvsp[0].YYvalue));}
    break;

  case 123:

    {(yyval.YYname) = (yyvsp[0].YYname);}
    break;

  case 124:

    {dds_declare->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_IDENT; (yyval.YYname) = (yyvsp[0].YYname);}
    break;

  case 125:

    {(yyval.YYname) = (yyvsp[-1].YYname);}
    break;

  case 126:

    {ddsSymTypeArray((yyvsp[-1].YYvalue)); (yyval.YYname) = (yyvsp[-3].YYname);}
    break;

  case 127:

    {ddsSymTypeArray(0); (yyval.YYname) = (yyvsp[-2].YYname);}
    break;

  case 128:

    {(yyval.YYname) = (yyvsp[-3].YYname);}
    break;

  case 129:

    {(yyval.YYname) = (yyvsp[-3].YYname);}
    break;

  case 130:

    {(yyval.YYname) = (yyvsp[-2].YYname);}
    break;

  case 131:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 132:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 133:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 134:

    {DECLARE_CLEAR;}
    break;

  case 135:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 136:

    {ddsSymDeclare((yyvsp[0].YYname)); (yyval.YYvoid) = VOIDED;}
    break;

  case 137:

    {ddsSymDeclare((yyvsp[0].YYvoid)); (yyval.YYvoid) = VOIDED;}
    break;

  case 138:

    {ddsSymDeclare(NULL); (yyval.YYvoid) = VOIDED;}
    break;

  case 139:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 140:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 141:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 142:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 143:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 144:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 145:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 146:

    {(yyval.YYtype) = dds_declare->type;}
    break;

  case 147:

    {(yyval.YYtype) = dds_declare->type;}
    break;

  case 148:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 149:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 150:

    {ddsSymTypeArray((yyvsp[-1].YYvalue)); (yyval.YYvoid) = VOIDED;}
    break;

  case 151:

    {ddsSymTypeArray((yyvsp[-1].YYvalue)); (yyval.YYvoid) = VOIDED;}
    break;

  case 152:

    {ddsSymTypeArray(0); (yyval.YYvoid) = VOIDED;}
    break;

  case 153:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 154:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 155:

    {(yyval.YYvoid) = VOIDED;}
    break;

  case 156:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 157:

    {DECLARE_CLEAR; needle = dds_needle;}
    break;

  case 158:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 159:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue); needle = dds_needle;}
    break;

  case 160:

    {(yyval.YYvalue) = ddsSymExpr2A(NULL, (yyvsp[-1].YYopcode), (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));
            needle = dds_needle;}
    break;

  case 161:

    {(yyval.YYopcode) = DDS_ASSIGN_OP;}
    break;

  case 162:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 163:

    {(yyval.YYexpr) = dds_needle;}
    break;

  case 164:

    {(yyval.YYexpr) = dds_needle;}
    break;

  case 165:

    {(yyval.YYvalue) = ddsSymExpr3A(NULL, DDS_CONDITIONAL_OP,
            (yyvsp[-6].YYvalue), (yyvsp[-3].YYvalue), (yyvsp[0].YYvalue), needle, (yyvsp[-4].YYexpr), (yyvsp[-1].YYexpr));}
    break;

  case 166:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 167:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 168:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_LOGICAL_OR_OP, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 169:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 170:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_LOGICAL_AND_OP, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 171:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 172:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_BIT_OR_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 173:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 174:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_BIT_XOR_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 175:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 176:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_BIT_AND_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 177:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 178:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_EQUAL_TO_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 179:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_NOT_EQUAL_TO_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 180:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 181:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_LESS_THAN_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 182:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_GREATER_THAN_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 183:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_LESS_EQUAL_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 184:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_GREATER_EQUAL_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 185:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 186:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_LEFT_SHIFT_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 187:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_RIGHT_SHIFT_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 188:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 189:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_PLUS_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 190:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_MINUS_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 191:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 192:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_MULTIPLY_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 193:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_DIVIDE_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 194:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_MODULO_OP_INT, (yyvsp[-2].YYvalue), (yyvsp[0].YYvalue));}
    break;

  case 195:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 196:

    {DECLARE_INNER;;}
    break;

  case 197:

    {DECLARE_OUTER;;}
    break;

  case 198:

    {(yyval.YYvalue) = ddsSymTypeCast(DDS_SYM_CAST, (yyvsp[-3].YYtype), (yyvsp[0].YYvalue));}
    break;

  case 199:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 200:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, (yyvsp[-1].YYopcode), (yyvsp[0].YYvalue));}
    break;

  case 201:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_SIZEOF_EXPR_OP, (yyvsp[0].YYvalue));}
    break;

  case 202:

    {DECLARE_INNER;;}
    break;

  case 203:

    {DECLARE_OUTER;
         (yyval.YYvalue) = ddsSymExpr1A3(NULL, DDS_SIZEOF_TYPE_OP, dds_declare->type);}
    break;

  case 204:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_AXIS_SIZE_OP, (yyvsp[-1].YYvalue));}
    break;

  case 205:

    {dds_declare->flag |= DDS_SYM_IDENT;}
    break;

  case 206:

    { dds_declare->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_IDENT;
            (yyval.YYvalue) = ddsSymExpr1A2((yyvsp[-1].YYname));}
    break;

  case 207:

    {dds_declare->flag |= DDS_SYM_IDENT;}
    break;

  case 208:

    {dds_declare->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_IDENT;}
    break;

  case 209:

    {(yyval.YYvalue) = ddsSymExpr1A4((yyvsp[-4].YYname), (yyvsp[-1].YYvalue));}
    break;

  case 210:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_POW_OP, (yyvsp[-3].YYvalue), (yyvsp[-1].YYvalue));}
    break;

  case 211:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_ORDINAL_OP, (yyvsp[-1].YYvalue));}
    break;

  case 212:

    {(yyval.YYvalue) = ddsSymExpr0A(NULL, DDS_LINE_TRC_OP);}
    break;

  case 213:

    {(yyval.YYvalue) = ddsSymExpr0A(NULL, DDS_REEL_TRC_OP);}
    break;

  case 214:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_SQRT_OP, (yyvsp[-1].YYvalue));}
    break;

  case 215:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_SIN_OP, (yyvsp[-1].YYvalue));}
    break;

  case 216:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_COS_OP, (yyvsp[-1].YYvalue));}
    break;

  case 217:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_TAN_OP, (yyvsp[-1].YYvalue));}
    break;

  case 218:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_ASIN_OP, (yyvsp[-1].YYvalue));}
    break;

  case 219:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_ACOS_OP, (yyvsp[-1].YYvalue));}
    break;

  case 220:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_ATAN_OP, (yyvsp[-1].YYvalue));}
    break;

  case 221:

    {(yyval.YYvalue) = ddsSymExpr2B(NULL, DDS_ATAN2_OP, (yyvsp[-3].YYvalue), (yyvsp[-1].YYvalue));}
    break;

  case 222:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_EXP_OP, (yyvsp[-1].YYvalue));}
    break;

  case 223:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_LOG_OP, (yyvsp[-1].YYvalue));}
    break;

  case 224:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_LOG10_OP, (yyvsp[-1].YYvalue));}
    break;

  case 225:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_ABS_OP, (yyvsp[-1].YYvalue));}
    break;

  case 226:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_SIGN_OP, (yyvsp[-1].YYvalue));}
    break;

  case 227:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_CEIL_OP, (yyvsp[-1].YYvalue));}
    break;

  case 228:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_FLOOR_OP, (yyvsp[-1].YYvalue));}
    break;

  case 229:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_DATEINT_OP, (yyvsp[-1].YYvalue));}
    break;

  case 230:

    {(yyval.YYvalue) = ddsSymExpr1A(NULL, DDS_DATESTRUCT_OP, (yyvsp[-1].YYvalue));}
    break;

  case 231:

    {(yyval.YYopcode) = DDS_UNARY_PLUS_OP;}
    break;

  case 232:

    {(yyval.YYopcode) = DDS_UNARY_MINUS_OP_INT;}
    break;

  case 233:

    {(yyval.YYopcode) = DDS_COMPLEMENT_OP_INT;}
    break;

  case 234:

    {(yyval.YYopcode) = DDS_NEGATION_OP_INT;}
    break;

  case 235:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 236:

    {(yyval.YYvalue) = ddsSymExpr2D(NULL, DDS_ARRAY_REF_OP_PTR_SUB, (yyvsp[-3].YYvalue), (yyvsp[-1].YYvalue));}
    break;

  case 237:

    {(yyval.YYvalue) = (yyvsp[-2].YYvalue);}
    break;

  case 238:

    {(yyval.YYvalue) = ddsSymExpr2C(NULL, DDS_STRUCT_MEMBER_OP, (yyvsp[-3].YYvalue), (yyvsp[-1].YYname));}
    break;

  case 239:

    {
         (yyval.YYvalue) = (DDS_VALUE*)LOOKUP_OBJECT(dds_symbol, (yyvsp[0].YYname), dds_block);
         if(dds_map_value && ! (yyval.YYvalue)) {
            /* We ARE compiling an rvalue for a map expression */
            /* and this "name/identifier" is NOT a defined object. */
            /* If the default type for the input binary is a struct/union */
            /* then its name space may provide an object. */
            if(ddsSymLookupMem(dds_symbol, (yyvsp[0].YYname),
               dds_map_value->type->block, &bias)) {
               /* name is a member of struct/union (auto dereference) */
               (yyval.YYvalue) = ddsSymExpr2C(NULL, DDS_STRUCT_MEMBER_OP,
                  dds_map_value, (yyvsp[0].YYname));
            } else {
               sprintf(tmp_str, "Spare.Hdr.%.32s", (yyvsp[0].YYname));
               if(ddsSymLookupMem(dds_symbol, tmp_str,
                  dds_map_value->type->block, &bias)) {
                  ddsYYError("map identifier not declared (%s)\n\t"
                     "try Rev 3 expression syntax, i.e. '%s'\n"
                     , (yyvsp[0].YYname), tmp_str);
               } else {
                  sprintf(tmp_str, "Spare.%.32s", (yyvsp[0].YYname) + 5);
                  if(ddsSymLookupMem(dds_symbol, tmp_str,
                     dds_map_value->type->block, &bias)) {
                     ddsYYError("map identifier not declared (%s)\n\t"
                        "try Rev 3 expression syntax, i.e. '%s'\n"
                        , (yyvsp[0].YYname), tmp_str);
                  } else {
                     ddsYYError("map identifier not declared (%s)\n", (yyvsp[0].YYname));
                  }
               }
               (yyval.YYvalue) = (DDS_VALUE*)dds_error_object;
            }
         }
         if(! (yyval.YYvalue)) {
            ddsYYError("identifier not declared (%s)\n", (yyvsp[0].YYname));
            (yyval.YYvalue) = (DDS_VALUE*)dds_error_object;
         }
      ;}
    break;

  case 240:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 241:

    {(yyval.YYvalue) = (DDS_VALUE*)dds_nop_object;}
    break;

  case 242:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 243:

    {(yyval.YYvalue) = (yyvsp[-1].YYvalue);}
    break;

  case 244:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 245:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 246:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 247:

    {(yyval.YYvalue) = (yyvsp[0].YYvalue);}
    break;

  case 248:

    {(yyval.YYname) = (yyvsp[0].YYname);}
    break;

  case 249:

    {(yyval.YYname) = ddsSymIdent((yyvsp[-1].YYvalue)->ptr);}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */


  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  int yytype = YYTRANSLATE (yychar);
	  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
	  YYSIZE_T yysize = yysize0;
	  YYSIZE_T yysize1;
	  int yysize_overflow = 0;
	  char *yymsg = 0;
#	  define YYERROR_VERBOSE_ARGS_MAXIMUM 5
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;

#if 0
	  /* This is so xgettext sees the translatable formats that are
	     constructed on the fly.  */
	  YY_("syntax error, unexpected %s");
	  YY_("syntax error, unexpected %s, expecting %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytname[yytype];
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytname[yyx];
		yysize1 = yysize + yytnamerr (0, yytname[yyx]);
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + yystrlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow && yysize <= YYSTACK_ALLOC_MAXIMUM)
	    yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg)
	    {
	      /* Avoid sprintf, as that infringes on the user's name space.
		 Don't have undefined behavior even if the translation
		 produced a string with the wrong number of "%s"s.  */
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (YY_("syntax error"));
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (0)
     goto yyerrorlab;

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK;
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}





   /**********************************************/
   /***   "user code" 3rd section of parser.   ***/
   /**********************************************/

#ifdef TEST
extern FILE *yyin;

int main(
   int argc,
   char **argv)
{
   while(! feof(yyin)) ddsYYparse();
   return 0;
}
#endif /* TEST */

