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
/* Line 1447 of yacc.c.  */

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif





