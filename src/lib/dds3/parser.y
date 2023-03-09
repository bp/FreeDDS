   /************************************************/
   /***   "definitions" 1st section of parser.   ***/
   /************************************************/

%{
static char rcsid[]="$Id: parser.y 50 2009-02-10 21:29:01Z user $";
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

static long bias;

static char tmp_str[64];

%}


%pure_parser


   /* YYSTYPE typedef */
%union {
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
}

%type <YYvoid> combined
%type <YYvalue> void_or_conditional_expression

%type <YYvoid> translation_unit
%type <YYvoid> external_declaration
%type <YYvoid> declaration
%type <YYtype> declaration_specifiers
%type <YYtype> storage_class_specifier
%type <YYtype> type_specifier
%type <YYtype> dds_type_specifier
%type <YYtype> type_qualifier
%type <YYtype> struct_or_union_specifier
%type <YYptype> struct_or_union
%type <YYvoid> struct_declaration_list
%type <YYvoid> init_declarator_list
%type <YYvoid> init_declarator
%type <YYvoid> struct_declaration
%type <YYedit> dds_member_edit
%type <YYtype> specifier_qualifier_list
%type <YYmember> struct_declarator_list
%type <YYmember> struct_declarator
%type <YYmember> dds_edit_declaration
%type <YYtype> enum_specifier
%type <YYptype> dds_enum_or_enumn
%type <YYvoid> enumerator_list
%type <YYvoid> enumerator_list_FIX
%type <YYobject> enumerator
%type <YYname> declarator
%type <YYname> direct_declarator
%type <YYvoid> parameter_type_list
%type <YYvoid> parameter_list
%type <YYvoid> parameter_declaration
%type <YYvoid> identifier_list
%type <YYvoid> initializer
%type <YYvoid> initializer_list
%type <YYtype> type_name
%type <YYvoid> abstract_declarator
%type <YYvoid> direct_abstract_declarator
%type <YYvalue> expression
%type <YYvalue> assignment_expression
%type <YYopcode> assignment_operator
%type <YYvalue> conditional_expression
%type <YYvalue> constant_expression
%type <YYvalue> logical_OR_expression
%type <YYvalue> logical_AND_expression
%type <YYvalue> inclusive_OR_expression
%type <YYvalue> exclusive_OR_expression
%type <YYvalue> AND_expression
%type <YYvalue> equality_expression
%type <YYvalue> relational_expression
%type <YYvalue> shift_expression
%type <YYvalue> additive_expression
%type <YYvalue> multiplicative_expression
%type <YYvalue> cast_expression
%type <YYvalue> unary_expression
%type <YYopcode> unary_operator
%type <YYvalue> postfix_expression
%type <YYvalue> primary_expression
%type <YYvalue> constant

%type <YYname> identifier

%token <YYvoid> TRANSLATION_UNIT
%token <YYvoid> DEFN_UNIT
%token <YYvoid> IDENT_UNIT
%token <YYvoid> MAP_EXPR

   /* undefined terminal symbols in K&R grammer for ANSI C */
%token <YYvalue> INTEGER_TOKEN
%token <YYvalue> FLOATING_TOKEN
%token <YYvalue> CHARACTER_TOKEN
%token <YYvalue> ENUMERATION_TOKEN
%token <YYname> IDENTIFIER_TOKEN
%token <YYtype> TYPEDEF_TOKEN
%token <YYvalue> STRING_TOKEN

%token  ASM  ASSIGN_OP  AUTO  BIT_AND_ASSIGN_OP  BIT_OR_ASSIGN_OP
%token  BIT_XOR_ASSIGN_OP  BREAK  CASE  CHAR  COMPLEX  CONST  CONTINUE
%token  DECREMENT_OP  DEFAULT  DIVIDE_ASSIGN_OP  DO  DOUBLE
%token  ELIPSE  ELSE  ENUM  EQUAL_TO_OP  EXTERN  FLOAT  FOR  FORTRAN
%token  GOTO  GREATER_EQUAL_OP  IF INCREMENT_OP  INT
%token  LEFT_SHIFT_ASSIGN_OP  LEFT_SHIFT_OP  LESS_EQUAL_OP
%token  LOGICAL_AND_OP  LOGICAL_OR_OP  LONG
%token  MINUS_ASSIGN_OP  MODULO_ASSIGN_OP  MULTIPLY_ASSIGN_OP
%token  NOT_EQUAL_TO_OP  PLUS_ASSIGN_OP
%token  REGISTER  RETURN  RIGHT_SHIFT_ASSIGN_OP  RIGHT_SHIFT_OP
%token  SHORT  SIGNED  SIZEOF  STATIC  STRUCT  STRUCT_POINTER_OP  SWITCH
%token  TYPEDEF  UNION  UNSIGNED  VOID  VOLATILE  WHILE

   /* keyword extension for DDS compiler functions */
%token  IDENT  DEFN_VALUE  ALIGN  AXIS_SIZE
%token  EXIST  NOP  FIELD  ORDINAL  LINE_TRC  REEL_TRC
%token  SQRT  SIN  COS  TAN  ASIN  ACOS  ATAN  ATAN2  EXP  LOG  LOG10  POW  ABS  SIGN
%token  CEIL  FLOOR  DATEINT  DATESTRUCT

   /* keyword extension for DDS declarations */
%token ASCII1  EBCDIC1
%token UNSIGNED1  UNSIGNED2  UNSIGNED4  UNSIGNED8
%token INTEGER1  INTEGER2  INTEGER4  INTEGER8
%token FLOAT4  FLOAT8  FLOAT16
%token FLOAT4IBM  FLOAT8IBM
%token COMPLEX8  COMPLEX16  COMPLEX32
%token COMPLEX8IBM  COMPLEX16IBM
%token ENUM1  ENUM2  ENUM4  ENUM8

%token UNSIGNED2X  UNSIGNED4X  UNSIGNED8X
%token INTEGER2X  INTEGER4X  INTEGER8X
%token FLOAT4X  FLOAT8X  FLOAT16X
%token COMPLEX8X  COMPLEX16X  COMPLEX32X

%start combined


%%

   /******************************************/
   /***   "rules" 2nd section of parser.   ***/
   /******************************************/


   /* grammar derived from "The C Programming Language" */
   /* Second Edition, Kernighan and Ritchie */

   /* Non-terminal type names for DDS extensions include */
   /*    dds_type_specifier */
   /*    dds_member_edit */
   /*    dds_edit_declaration */
   /*    dds_enum_or_enumn */

combined:
      TRANSLATION_UNIT translation_unit
         {$$ = VOIDED}
   |  DEFN_UNIT '(' STRING_TOKEN ',' STRING_TOKEN ')'
         {ddsDefnValue($3, $5); $$ = VOIDED; YYACCEPT}
   |  IDENT_UNIT '(' STRING_TOKEN ')'
         {*((DDS_VALUE**)parm) = $3; $$ = VOIDED; YYACCEPT}
   |  MAP_EXPR void_or_conditional_expression
         {*((DDS_VALUE**)parm) = ($2 && $2->type->ptype != DDS_CTYPE_VOID) ?
            $2 : NULL}
   ;

void_or_conditional_expression:
      VOID {$$ = VOIDED}
   |  conditional_expression {$$ = $1}
   ;

translation_unit:
      external_declaration {$$ = VOIDED}
   |  translation_unit external_declaration {$$ = VOIDED}
   ;

/***********************/
/* declaration grammar */
/***********************/

external_declaration:
      declaration {$$ = VOIDED}
   ;
declaration:
      declaration_specifiers init_declarator_list ';'
         {DECLARE_CLEAR; needle = dds_needle; $$ = VOIDED}
   |  declaration_specifiers                      ';'
         {DECLARE_CLEAR; needle = dds_needle; $$ = VOIDED}
   ;
declaration_specifiers:
      storage_class_specifier declaration_specifiers {$$ = $1}
   |  storage_class_specifier                        {$$ = $1}
   |  type_specifier declaration_specifiers          {$$ = $1}
   |  type_specifier                                 {$$ = $1}
   |  type_qualifier declaration_specifiers          {$$ = $1}
   |  type_qualifier                                 {$$ = $1}
   ;
storage_class_specifier:
      TYPEDEF   {$$ = ddsSymTypeStore(DDS_SYM_TYPEDEF)}
   ;
type_specifier:
      VOID {$$ = ddsSymTypeSpec(DDS_SYM_VOID,
         (DDS_SYM_FLAG)(0))}
   |  CHAR {$$ = ddsSymTypeSpec(DDS_SYM_CHAR,
         (DDS_SYM_FLAG)(DDS_SYM_SIGNED | DDS_SYM_UNSIGNED))}
   |  SHORT {$$ = ddsSymTypeSpec(DDS_SYM_SHORT,
         (DDS_SYM_FLAG)(DDS_SYM_SIGNED | DDS_SYM_UNSIGNED | DDS_SYM_INT))}
   |  INT {$$ = ddsSymTypeSpec(DDS_SYM_INT,
         (DDS_SYM_FLAG)(DDS_SYM_SIGNED | DDS_SYM_UNSIGNED | DDS_SYM_SHORT |
            DDS_SYM_LONG | DDS_SYM_LLONG))}
   |  LONG {if(MAKE_LLONG && dds_declare->type &&
               (dds_declare->type->flag & DDS_SYM_LONG)) {
               $$ = ddsSymTypeSpec(DDS_SYM_LLONG,
                  (DDS_SYM_FLAG)(DDS_SYM_SIGNED | DDS_SYM_UNSIGNED |
                     DDS_SYM_LONG | DDS_SYM_INT));
            } else {
               $$ = ddsSymTypeSpec(DDS_SYM_LONG,
                  (DDS_SYM_FLAG)(DDS_SYM_SIGNED | DDS_SYM_UNSIGNED |
                     DDS_SYM_INT | DDS_SYM_DOUBLE | DDS_SYM_COMPLEX));
            }
         }
   |  FLOAT {$$ = ddsSymTypeSpec(DDS_SYM_FLOAT,
         (DDS_SYM_FLAG)(DDS_SYM_COMPLEX))}
   |  DOUBLE {if(MAKE_LDOUBLE) {
               $$ = ddsSymTypeSpec(DDS_SYM_DOUBLE,
                  (DDS_SYM_FLAG)(DDS_SYM_LONG | DDS_SYM_COMPLEX));
            } else {
               $$ = ddsSymTypeSpec(DDS_SYM_DOUBLE,
                  (DDS_SYM_FLAG)(DDS_SYM_COMPLEX));
            }
         }
   |  SIGNED {$$ = ddsSymTypeSpec(DDS_SYM_SIGNED,
         (DDS_SYM_FLAG)(DDS_SYM_CHAR | DDS_SYM_SHORT | DDS_SYM_INT |
            DDS_SYM_LONG | DDS_SYM_LLONG))}
   |  UNSIGNED {$$ = ddsSymTypeSpec(DDS_SYM_UNSIGNED,
         (DDS_SYM_FLAG)(DDS_SYM_CHAR | DDS_SYM_SHORT | DDS_SYM_INT |
            DDS_SYM_LONG | DDS_SYM_LLONG))}
   |  struct_or_union_specifier {$$ = $1}
   |  enum_specifier {$$ = $1}
   |  TYPEDEF_TOKEN {$$ = $1}
   |  COMPLEX {if(MAKE_LDOUBLE) {
               $$ = ddsSymTypeSpec(DDS_SYM_COMPLEX,
                  (DDS_SYM_FLAG)(DDS_SYM_FLOAT | DDS_SYM_DOUBLE | DDS_SYM_LONG));
            } else {
               $$ = ddsSymTypeSpec(DDS_SYM_COMPLEX,
                  (DDS_SYM_FLAG)(DDS_SYM_FLOAT | DDS_SYM_DOUBLE));
            }
         }
   |  dds_type_specifier {$$ = $1}
   ;
dds_type_specifier:
      ASCII1         {$$ = TYPE_MERGE(ASCII1)}
   |  EBCDIC1        {$$ = TYPE_MERGE(EBCDIC1)}
   |  INTEGER1       {$$ = TYPE_MERGE(INTEGER1)}
   |  INTEGER2       {$$ = TYPE_MERGE(INTEGER2)}
   |  INTEGER2X      {$$ = TYPE_MERGE(INTEGER2X)}
   |  INTEGER4       {$$ = TYPE_MERGE(INTEGER4)}
   |  INTEGER4X      {$$ = TYPE_MERGE(INTEGER4X)}
   |  INTEGER8       {$$ = TYPE_MERGE(INTEGER8)}
   |  INTEGER8X      {$$ = TYPE_MERGE(INTEGER8X)}
   |  UNSIGNED1      {$$ = TYPE_MERGE(UNSIGNED1)}
   |  UNSIGNED2      {$$ = TYPE_MERGE(UNSIGNED2)}
   |  UNSIGNED2X     {$$ = TYPE_MERGE(UNSIGNED2X)}
   |  UNSIGNED4      {$$ = TYPE_MERGE(UNSIGNED4)}
   |  UNSIGNED4X     {$$ = TYPE_MERGE(UNSIGNED4X)}
   |  UNSIGNED8      {$$ = TYPE_MERGE(UNSIGNED8)}
   |  UNSIGNED8X     {$$ = TYPE_MERGE(UNSIGNED8X)}
   |  FLOAT4         {$$ = TYPE_MERGE(FLOAT4)}
   |  FLOAT4X        {$$ = TYPE_MERGE(FLOAT4X)}
   |  FLOAT4IBM      {$$ = TYPE_MERGE(FLOAT4IBM)}
   |  FLOAT8         {$$ = TYPE_MERGE(FLOAT8)}
   |  FLOAT8X        {$$ = TYPE_MERGE(FLOAT8X)}
   |  FLOAT8IBM      {$$ = TYPE_MERGE(FLOAT8IBM)}
   |  FLOAT16        {$$ = TYPE_MERGE(FLOAT16)}
   |  FLOAT16X       {$$ = TYPE_MERGE(FLOAT16X)}
   |  COMPLEX8       {$$ = TYPE_MERGE(COMPLEX8)}
   |  COMPLEX8X      {$$ = TYPE_MERGE(COMPLEX8X)}
   |  COMPLEX8IBM    {$$ = TYPE_MERGE(COMPLEX8IBM)}
   |  COMPLEX16      {$$ = TYPE_MERGE(COMPLEX16)}
   |  COMPLEX16X     {$$ = TYPE_MERGE(COMPLEX16X)}
   |  COMPLEX16IBM   {$$ = TYPE_MERGE(COMPLEX16IBM)}
   |  COMPLEX32      {$$ = TYPE_MERGE(COMPLEX32)}
   |  COMPLEX32X     {$$ = TYPE_MERGE(COMPLEX32X)}
   ;
type_qualifier:
      CONST   
            {$$ = ddsSymTypeQual(DDS_SYM_CONST, NULL)}
   |  VOLATILE
            {$$ = ddsSymTypeQual(DDS_SYM_VOLATILE, NULL)}
   |  ALIGN '(' constant_expression ')'
            {$$ = ddsSymTypeQual(DDS_SYM_ALIGN, $3)}
   ;
struct_or_union_specifier:
      struct_or_union identifier
         '{' {$<YYblock>$ = ddsSymBlockBegin($2, (DDS_SYM_FLAG)(
                (($1 == DDS_STYPE_STRUCT) ? DDS_SYM_STRUCT : DDS_SYM_UNION)),
                dds_prime[$1].type)}
         struct_declaration_list
         '}' {ddsSymBlockEnd();
            $$ = ddsSymTypeSUE($<YYblock>4)}
   |  struct_or_union identifier
             {$<YYblock>$ = ddsSymBlockBegin($2, (DDS_SYM_FLAG)(
                DDS_SYM_INCOMPLETE |
                (($1 == DDS_STYPE_STRUCT) ? DDS_SYM_STRUCT : DDS_SYM_UNION)),
                dds_prime[$1].type);
                ddsSymBlockEnd();
            $$ = ddsSymTypeSUE($<YYblock>$)}
   |  struct_or_union
         '{' {$<YYblock>$ = ddsSymBlockBegin(NULL, (DDS_SYM_FLAG)(
                (($1 == DDS_STYPE_STRUCT) ? DDS_SYM_STRUCT : DDS_SYM_UNION)),
                dds_prime[$1].type)}
         struct_declaration_list
         '}' {ddsSymBlockEnd();
            $$ = ddsSymTypeSUE($<YYblock>3)}
   ;
struct_or_union:
      STRUCT {$$ = DDS_STYPE_STRUCT}
   |  UNION  {$$ = DDS_STYPE_UNION}
   ;
struct_declaration_list:
      struct_declaration {$$ = VOIDED}
   |  struct_declaration_list struct_declaration {$$ = VOIDED}
   ;
init_declarator_list:
      init_declarator {$$ = VOIDED}
   |  init_declarator_list ',' {DECLARE_MORE}
         init_declarator {$$ = VOIDED}
   ;
init_declarator:
      declarator {ddsSymDeclare($1); $$ = VOIDED}
   |  declarator ASSIGN_OP {ddsSymDeclare($1)}
         initializer
           {ddsYYError("parser.y: initializers are deferred\n"); $$ = VOIDED}
   ;
struct_declaration:
      specifier_qualifier_list struct_declarator_list
         ';' {DECLARE_CLEAR; needle = dds_needle; $$ = VOIDED}
   |  dds_member_edit dds_edit_declaration
         ';' {ddsSymMemberEdit($1, $2);
              DECLARE_CLEAR; needle = dds_needle; $$ = VOIDED}
   ;
dds_edit_declaration:
      specifier_qualifier_list struct_declarator_list {$$ = $2}
   |  specifier_qualifier_list {$$ = 0}
   ;
dds_member_edit:
      '-'     {$$ = DDS_SYM_DELETE}
   |  '*'     {$$ = DDS_SYM_REPLACE}
   |  '+'     {$$ = DDS_SYM_APPEND}
   |  '-' '+' {$$ = DDS_SYM_DAPPEND}
   |  '*' '+' {$$ = DDS_SYM_RAPPEND}
   ;
specifier_qualifier_list:
      type_specifier specifier_qualifier_list {$$ = $1}
   |  type_specifier {$$ = $1}
   |  type_qualifier specifier_qualifier_list {$$ = $1}
   |  type_qualifier {$$ = $1}
   ;
struct_declarator_list:
      struct_declarator {$$ = $1}
   |  struct_declarator_list ',' {DECLARE_MORE; $$ = $1}
         struct_declarator {$$ = $1}
   ;
struct_declarator:
      declarator {$$ = ddsSymMember($1)}
   |  declarator ':' {$<YYmember>$ = ddsSymMember($1)}
         constant_expression
            {$$ = ddsSymMemberBit($<YYmember>3,
               ddsSymConstInt($4, 0, DDS_SYM_CONST_DECL))}
   |             ':' {$<YYmember>$ = ddsSymMember(NULL)}
         constant_expression
            {$$ = ddsSymMemberBit($<YYmember>2,
               ddsSymConstSizet($3, 0, DDS_SYM_CONST_DECL))}
   ;
enum_specifier:
      dds_enum_or_enumn identifier
         '{' {$<YYblock>$ = ddsSymBlockBegin($2,
               (DDS_SYM_FLAG)(DDS_SYM_ENUM),
               dds_prime[$1].type);
               ddsSymBlockEnd();
               dds_declare->flag |= DDS_SYM_IDENT}
         enumerator_list_FIX
         '}' {$$ = ddsSymTypeSUE($<YYblock>4)}
   |  dds_enum_or_enumn
         '{' {$<YYblock>$ = ddsSymBlockBegin(NULL,
               (DDS_SYM_FLAG)(DDS_SYM_ENUM),
               dds_prime[$1].type);
               ddsSymBlockEnd();
               dds_declare->flag |= DDS_SYM_IDENT}
         enumerator_list_FIX
         '}' {$$ = ddsSymTypeSUE($<YYblock>3)}
   |  dds_enum_or_enumn identifier
             {$<YYblock>$ = ddsSymBlockBegin($2,
               (DDS_SYM_FLAG)(DDS_SYM_ENUM | DDS_SYM_INCOMPLETE),
               dds_prime[$1].type);
               ddsSymBlockEnd();
              $$ = ddsSymTypeSUE($<YYblock>$)}
   ;
dds_enum_or_enumn:
      ENUM  {$$ = DDS_CTYPE_INT}
   |  ENUM1 {$$ = DDS_STYPE_INTEGER1}
   |  ENUM2 {$$ = DDS_STYPE_INTEGER2}
   |  ENUM4 {$$ = DDS_STYPE_INTEGER4}
   |  ENUM8 {$$ = DDS_STYPE_INTEGER8}
   ;
enumerator_list_FIX:
      enumerator_list {$$ = VOIDED}
   |  enumerator_list ',' {$$ = VOIDED}
   ;
enumerator_list:
      enumerator {$$ = VOIDED}
   |  enumerator_list ',' {dds_declare->flag |= DDS_SYM_IDENT}
         enumerator {$$ = VOIDED}
   ;
enumerator:
      identifier {$$ = ddsSymEnumName($1)}
   |  identifier ASSIGN_OP {$<YYobject>$ = ddsSymEnumName($1)}
         constant_expression
            {$$ = ddsSymEnumValue($<YYobject>3, $4)}
   ;
declarator:
              direct_declarator {$$ = $1}
   ;
direct_declarator:
      identifier {dds_declare->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_IDENT; $$ = $1}
   |  '(' declarator ')' {$$ = $2}
   |  direct_declarator '['
         constant_expression ']'
            {ddsSymTypeArray($3); $$ = $1}
   |  direct_declarator '['
                             ']'
            {ddsSymTypeArray(0); $$ = $1}
   |  direct_declarator '('
         parameter_type_list ')' {$$ = $1}
   |  direct_declarator '('
         identifier_list ')' {$$ = $1}
   |  direct_declarator '('
                         ')' {$$ = $1}
   ;
parameter_type_list:
      parameter_list {$$ = VOIDED}
   |  parameter_list ',' ELIPSE {$$ = VOIDED}
   ;
parameter_list:
      parameter_declaration {$$ = VOIDED}
   |  parameter_list ',' {DECLARE_CLEAR}
         parameter_declaration {$$ = VOIDED}
   ;
parameter_declaration:
      declaration_specifiers declarator
         {ddsSymDeclare($2); $$ = VOIDED}
   |  declaration_specifiers abstract_declarator
         {ddsSymDeclare($2); $$ = VOIDED}
   |  declaration_specifiers
         {ddsSymDeclare(NULL); $$ = VOIDED}
   ;
identifier_list:
      identifier {$$ = VOIDED}
   |  identifier_list ',' identifier {$$ = VOIDED}
   ;
initializer:
      assignment_expression {$$ = VOIDED}
   |  '{' initializer_list '}' {$$ = VOIDED}
   |  '{' initializer_list ',' '}' {$$ = VOIDED}
   ;
initializer_list:
      initializer {$$ = VOIDED}
   |  initializer_list ',' initializer {$$ = VOIDED}
   ;
type_name:
      specifier_qualifier_list abstract_declarator {$$ = dds_declare->type}
   |  specifier_qualifier_list {$$ = dds_declare->type}
   ;
abstract_declarator:
              direct_abstract_declarator {$$ = VOIDED}
   ;
direct_abstract_declarator:
      '(' abstract_declarator ')' {$$ = VOIDED}
   |  direct_abstract_declarator '[' constant_expression ']'
         {ddsSymTypeArray($3); $$ = VOIDED}
   |                             '[' constant_expression ']'
         {ddsSymTypeArray($2); $$ = VOIDED}
   |  direct_abstract_declarator '['                     ']'
         {ddsSymTypeArray(0); $$ = VOIDED}
   |  direct_abstract_declarator '(' parameter_type_list ')' {$$ = VOIDED}
   |                             '(' parameter_type_list ')' {$$ = VOIDED}
   |  direct_abstract_declarator '('                     ')' {$$ = VOIDED}
   ;

/**********************/
/* expression grammar */
/**********************/

expression:
      assignment_expression {$$ = $1}
   |  expression ',' {DECLARE_CLEAR; needle = dds_needle}
         assignment_expression {$$ = $4}
   ;
assignment_expression:
      conditional_expression {$$ = $1; needle = dds_needle}
   |  unary_expression assignment_operator assignment_expression
         {$$ = ddsSymExpr2A(NULL, $2, $1, $3);
            needle = dds_needle}
   ;
assignment_operator:
      ASSIGN_OP              {$$ = DDS_ASSIGN_OP}
   ;
conditional_expression:
      logical_OR_expression {$$ = $1}
   |  logical_OR_expression '?' {$<YYexpr>$ = dds_needle}
         expression ':' {$<YYexpr>$ = dds_needle}
         conditional_expression
         {$$ = ddsSymExpr3A(NULL, DDS_CONDITIONAL_OP,
            $1, $4, $7, needle, $<YYexpr>3, $<YYexpr>6)}
   ;
constant_expression:
      conditional_expression {$$ = $1}
   ;
logical_OR_expression:
      logical_AND_expression {$$ = $1}
   |  logical_OR_expression LOGICAL_OR_OP logical_AND_expression
         {$$ = ddsSymExpr2B(NULL, DDS_LOGICAL_OR_OP, $1, $3)}
   ;
logical_AND_expression:
      inclusive_OR_expression {$$ = $1}
   |  logical_AND_expression LOGICAL_AND_OP inclusive_OR_expression
         {$$ = ddsSymExpr2B(NULL, DDS_LOGICAL_AND_OP, $1, $3)}
   ;
inclusive_OR_expression:
      exclusive_OR_expression {$$ = $1}
   |  inclusive_OR_expression '|' exclusive_OR_expression
         {$$ = ddsSymExpr2B(NULL, DDS_BIT_OR_OP_INT, $1, $3)}
   ;
exclusive_OR_expression:
      AND_expression {$$ = $1}
   |  exclusive_OR_expression '^' AND_expression
         {$$ = ddsSymExpr2B(NULL, DDS_BIT_XOR_OP_INT, $1, $3)}
   ;
AND_expression:
      equality_expression {$$ = $1}
   |  AND_expression '&' equality_expression
         {$$ = ddsSymExpr2B(NULL, DDS_BIT_AND_OP_INT, $1, $3)}
   ;
equality_expression:
      relational_expression {$$ = $1}
   |  equality_expression EQUAL_TO_OP relational_expression
         {$$ = ddsSymExpr2B(NULL, DDS_EQUAL_TO_OP_INT, $1, $3)}
   |  equality_expression NOT_EQUAL_TO_OP relational_expression
         {$$ = ddsSymExpr2B(NULL, DDS_NOT_EQUAL_TO_OP_INT, $1, $3)}
   ;
relational_expression:
      shift_expression {$$ = $1}
   |  relational_expression '<' shift_expression
         {$$ = ddsSymExpr2B(NULL, DDS_LESS_THAN_OP_INT, $1, $3)}
   |  relational_expression '>' shift_expression
         {$$ = ddsSymExpr2B(NULL, DDS_GREATER_THAN_OP_INT, $1, $3)}
   |  relational_expression LESS_EQUAL_OP shift_expression
         {$$ = ddsSymExpr2B(NULL, DDS_LESS_EQUAL_OP_INT, $1, $3)}
   |  relational_expression GREATER_EQUAL_OP shift_expression
         {$$ = ddsSymExpr2B(NULL, DDS_GREATER_EQUAL_OP_INT, $1, $3)}
   ;
shift_expression:
      additive_expression {$$ = $1}
   |  shift_expression LEFT_SHIFT_OP additive_expression
         {$$ = ddsSymExpr2B(NULL, DDS_LEFT_SHIFT_OP_INT, $1, $3)}
   |  shift_expression RIGHT_SHIFT_OP additive_expression
         {$$ = ddsSymExpr2B(NULL, DDS_RIGHT_SHIFT_OP_INT, $1, $3)}
   ;
additive_expression:
      multiplicative_expression {$$ = $1}
   |  additive_expression '+' multiplicative_expression
         {$$ = ddsSymExpr2B(NULL, DDS_PLUS_OP_INT, $1, $3)}
   |  additive_expression '-' multiplicative_expression
         {$$ = ddsSymExpr2B(NULL, DDS_MINUS_OP_INT, $1, $3)}
   ;
multiplicative_expression:
      cast_expression {$$ = $1}
   |  multiplicative_expression '*' cast_expression
         {$$ = ddsSymExpr2B(NULL, DDS_MULTIPLY_OP_INT, $1, $3)}
   |  multiplicative_expression '/' cast_expression
         {$$ = ddsSymExpr2B(NULL, DDS_DIVIDE_OP_INT, $1, $3)}
   |  multiplicative_expression '%' cast_expression
         {$$ = ddsSymExpr2B(NULL, DDS_MODULO_OP_INT, $1, $3)}
   ;
cast_expression:
      unary_expression {$$ = $1}
   |  '(' {DECLARE_INNER;} type_name ')' {DECLARE_OUTER;} cast_expression
         {$$ = ddsSymTypeCast(DDS_SYM_CAST, $3, $6)}
   ;
unary_expression:
      postfix_expression {$$ = $1}
   |  unary_operator cast_expression
         {$$ = ddsSymExpr1A(NULL, $1, $2)}
   |  SIZEOF unary_expression
         {$$ = ddsSymExpr1A(NULL, DDS_SIZEOF_EXPR_OP, $2)}
   |  SIZEOF '(' {DECLARE_INNER;} type_name ')' {DECLARE_OUTER;
         $$ = ddsSymExpr1A3(NULL, DDS_SIZEOF_TYPE_OP, dds_declare->type)}
   |  AXIS_SIZE '(' constant_expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_AXIS_SIZE_OP, $3)}
   |  EXIST '(' {dds_declare->flag |= DDS_SYM_IDENT} identifier ')'
         { dds_declare->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_IDENT;
            $$ = ddsSymExpr1A2($4)}
   |  FIELD '(' {dds_declare->flag |= DDS_SYM_IDENT} identifier ','
                {dds_declare->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_IDENT}
             conditional_expression ')' {$$ = ddsSymExpr1A4($4, $7)}
   |  POW '(' expression ',' expression ')'
         {$$ = ddsSymExpr2B(NULL, DDS_POW_OP, $3, $5)}
   |  ORDINAL '(' constant_expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_ORDINAL_OP, $3)}
   |  SQRT '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_SQRT_OP, $3)}
   |  SIN '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_SIN_OP, $3)}
   |  COS '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_COS_OP, $3)}
   |  TAN '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_TAN_OP, $3)}
   |  ASIN '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_ASIN_OP, $3)}
   |  ACOS '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_ACOS_OP, $3)}
   |  ATAN '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_ATAN_OP, $3)}
   |  ATAN2 '(' expression ',' expression ')'
         {$$ = ddsSymExpr2B(NULL, DDS_ATAN2_OP, $3, $5)}
   |  EXP '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_EXP_OP, $3)}
   |  LOG '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_LOG_OP, $3)}
   |  LOG10 '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_LOG10_OP, $3)}
   |  ABS '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_ABS_OP, $3)}
   |  SIGN '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_SIGN_OP, $3)}
   |  CEIL '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_CEIL_OP, $3)}
   |  FLOOR '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_FLOOR_OP, $3)}
   |  DATEINT '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_DATEINT_OP, $3)}
   |  DATESTRUCT '(' expression ')'
         {$$ = ddsSymExpr1A(NULL, DDS_DATESTRUCT_OP, $3)}
   ;
unary_operator:
      '+'   {$$ = DDS_UNARY_PLUS_OP}
   |  '-'   {$$ = DDS_UNARY_MINUS_OP_INT}
   |  '~'   {$$ = DDS_COMPLEMENT_OP_INT}
   |  '!'   {$$ = DDS_NEGATION_OP_INT}
   ;
postfix_expression:
      primary_expression {$$ = $1}
   |  postfix_expression '[' expression ']'
         {$$ = ddsSymExpr2D(NULL, DDS_ARRAY_REF_OP_PTR_SUB, $1, $3)}
   |  postfix_expression '.' identifier {$$ = $1}
         {$$ = ddsSymExpr2C(NULL, DDS_STRUCT_MEMBER_OP, $1, $3)}
   ;
primary_expression:
      identifier {
         $$ = (DDS_VALUE*)LOOKUP_OBJECT(dds_symbol, $1, dds_block);
         if(dds_map_value && ! $$) {
            /* We ARE compiling an rvalue for a map expression */
            /* and this "name/identifier" is NOT a defined object. */
            /* If the default type for the input binary is a struct/union */
            /* then its name space may provide an object. */
            if(ddsSymLookupMem(dds_symbol, $1,
               dds_map_value->type->block, &bias)) {
               /* name is a member of struct/union (auto dereference) */
               $$ = ddsSymExpr2C(NULL, DDS_STRUCT_MEMBER_OP,
                  dds_map_value, $1);
            } else {
               sprintf(tmp_str, "Spare.Hdr.%.32s", $1);
               if(ddsSymLookupMem(dds_symbol, tmp_str,
                  dds_map_value->type->block, &bias)) {
                  ddsYYError("map identifier not declared (%s)\n\t"
                     "try Rev 3 expression syntax, i.e. '%s'\n"
                     , $1, tmp_str);
               } else {
                  sprintf(tmp_str, "Spare.%.32s", $1 + 5);
                  if(ddsSymLookupMem(dds_symbol, tmp_str,
                     dds_map_value->type->block, &bias)) {
                     ddsYYError("map identifier not declared (%s)\n\t"
                        "try Rev 3 expression syntax, i.e. '%s'\n"
                        , $1, tmp_str);
                  } else {
                     ddsYYError("map identifier not declared (%s)\n", $1);
                  }
               }
               $$ = (DDS_VALUE*)dds_error_object;
            }
         }
         if(! $$) {
            ddsYYError("identifier not declared (%s)\n", $1);
            $$ = (DDS_VALUE*)dds_error_object;
         }
      }
   |  constant {$$ = $1}
   |  NOP {$$ = (DDS_VALUE*)dds_nop_object}
   |  STRING_TOKEN {$$ = $1}
   |  '(' expression ')' {$$ = $2}
   |  LINE_TRC '(' ')' {$$ = ddsSymExpr0A(NULL, DDS_LINE_TRC_OP);
         needle = dds_needle}
   |  REEL_TRC '(' ')' {$$ = ddsSymExpr0A(NULL, DDS_REEL_TRC_OP);
         needle = dds_needle}
   ;
constant:
      INTEGER_TOKEN {$$ = $1}
   |  CHARACTER_TOKEN {$$ = $1}
   |  FLOATING_TOKEN {$$ = $1}
   |  ENUMERATION_TOKEN {$$ = $1}
   ;

/**********************/
/* identifier grammar */
/**********************/

identifier:
      IDENTIFIER_TOKEN {$$ = $1}
   |  IDENT '(' STRING_TOKEN ')'
         {$$ = ddsSymIdent($3->ptr)}
   ;


%%

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
