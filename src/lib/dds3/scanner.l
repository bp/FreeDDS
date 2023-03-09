static char rcsid[]="$Id: scanner.l 16 2009-01-06 21:52:47Z user $";

   /*************************************************/
   /***   "definitions" 1st section of scanner.   ***/
   /*************************************************/

%{
/*
RCS-t=DDS, GNU flex scanner for binary format and map definitions.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

   #include <limits.h>
   #include "dds_sys.h"
   #include "parser.tab.h"

   #undef YY_INPUT
   #define YY_INPUT(buf, size, max) \
      ((size) = ddsYYinput((char*)(buf), (max)))

   #define DEFN_NEST_MAX 10

   /* length of character string (used with STRING_TOKEN) */
   static size_t str_size;

   #define YY_SKIP_YYWRAP
   #undef yywrap
   static int yywrap(void);

   static size_t ddsYYinput(char *buf, size_t max);

   static DDS_OBJECT * ddsYYdepend(const char *name);

   /* Push scanner stack (nested input) */
   #define SCANNER_PUSH(LEOF, DICT, DEFN, START, STOP, LINE)  \
      dds_lexer->yy_buf = YY_CURRENT_BUFFER;  \
      dds_lexer->leof = LEOF;  \
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
      dds_lexer->start = (START);  \
      dds_lexer->next = (START);  \
      dds_lexer->stop = (STOP);  \
      dds_lexer->line = (LINE);  \
      yy_buf = yy_create_buffer(NULL, YY_BUF_SIZE);  \
      yy_switch_to_buffer(yy_buf);

   /* Pop scanner stack (compliment of PUSH) */
   #define SCANNER_POP  \
      if(dds_lexer->defn) ddsFree((void*)dds_lexer->defn);  \
      yy_delete_buffer(YY_CURRENT_BUFFER);  \
      dds_lexer = dds_lexer->outer;  \
      yy_switch_to_buffer(dds_lexer->yy_buf);

   DDS_SCANNER *dds_lexer = NULL;
   DDS_SCANNER dds_lexer_root = {NULL};

   /*** extern YYSTYPE dds_yylval; ***/
   /*** #define yylval(member)  dds_yylval.member ***/

   /* following required for bison "pure" parsers */
   #undef  YY_DECL
   #define YY_DECL   int ddsYYlex(YYSTYPE *lvalp)
   #define yylval(member) lvalp->member

%}

%x  X_COMMENT

%%

   /*******************************************/
   /***   "rules" 2nd section of scanner.   ***/
   /*******************************************/

   #undef YY_BREAK
   #define YY_BREAK

   DDS_KEYWORD *key;
   DDS_OBJECT *object;
   DDS_VALUE *value;
   DDS_PTYPE ptype;

   const char *name, *name_ptr;
   char *end_ptr;
   size_t size;
   void *value_ptr, *text_ptr;
   int char_in, char_const, char_value;
   int parse_rc;
   int comment_state;
   unsigned long ul_con;        /* unsigned long integer constant */
   double d_con;                /* double constant */

   /* propagate debug trace flag */
   yy_flex_debug = dds_yy_flex_debug;

   /* process lex start state and its changes */
   switch(dds_lex_ss) {
   case DDS_LEX_CONTINUE:
      break;
   case DDS_LEX_SHARED:
      dds_lex_ss = DDS_LEX_CONTINUE;
      BEGIN(INITIAL);
      break;
   case DDS_LEX_TRANSLATE:
      dds_lex_ss = DDS_LEX_CONTINUE;
      BEGIN(INITIAL);
      yylval(YYvoid) = 0;
      return TRANSLATION_UNIT;
   case DDS_LEX_IDENT:
      dds_lex_ss = DDS_LEX_CONTINUE;
      BEGIN(INITIAL);
      yylval(YYvoid) = 0;
      return IDENT_UNIT;
   case DDS_LEX_DEFN:
      dds_lex_ss = DDS_LEX_CONTINUE;
      BEGIN(INITIAL);
      yylval(YYvoid) = 0;
      return DEFN_UNIT;
   case DDS_LEX_MAP:
      dds_lex_ss = DDS_LEX_CONTINUE;
      BEGIN(INITIAL);
      yylval(YYvoid) = 0;
      return MAP_EXPR;
   default:
      ddsPanic("dds_yylex: bad dds_lex_ss\n");
   }

%{
   /* end of preprocessing */
   /* empty */
%}

"..."       {yylval(YYvoid) = 0; return ELIPSE;}
"->"        {yylval(YYvoid) = 0; return STRUCT_POINTER_OP;}
"++"        {yylval(YYvoid) = 0; return INCREMENT_OP;}
"--"        {yylval(YYvoid) = 0; return DECREMENT_OP;}
"<<"        {yylval(YYvoid) = 0; return LEFT_SHIFT_OP;}
">>"        {yylval(YYvoid) = 0; return RIGHT_SHIFT_OP;}
"<\\="      {yylval(YYvoid) = 0; return LESS_EQUAL_OP;}
">\\="      {yylval(YYvoid) = 0; return GREATER_EQUAL_OP;}
"\\=\\="    {yylval(YYvoid) = 0; return EQUAL_TO_OP;}
"!\\="      {yylval(YYvoid) = 0; return NOT_EQUAL_TO_OP;}
"&&"        {yylval(YYvoid) = 0; return LOGICAL_AND_OP;}
"||"        {yylval(YYvoid) = 0; return LOGICAL_OR_OP;}
"\\="       {yylval(YYvoid) = 0; return ASSIGN_OP;}
"*\\="      {yylval(YYvoid) = 0; return MULTIPLY_ASSIGN_OP;}
"/\\="      {yylval(YYvoid) = 0; return DIVIDE_ASSIGN_OP;}
"%\\="      {yylval(YYvoid) = 0; return MODULO_ASSIGN_OP;}
"+\\="      {yylval(YYvoid) = 0; return PLUS_ASSIGN_OP;}
"-\\="      {yylval(YYvoid) = 0; return MINUS_ASSIGN_OP;}
"<<\\="     {yylval(YYvoid) = 0; return LEFT_SHIFT_ASSIGN_OP;}
">>\\="     {yylval(YYvoid) = 0; return RIGHT_SHIFT_ASSIGN_OP;}
"&\\="      {yylval(YYvoid) = 0; return BIT_AND_ASSIGN_OP;}
"^\\="      {yylval(YYvoid) = 0; return BIT_XOR_ASSIGN_OP;}
"|\\="      {yylval(YYvoid) = 0; return BIT_OR_ASSIGN_OP;}


[0-9]+[uU]?[lL]?		|
0x[0-9a-fA-F]+[uU]?[lL]?	|
0X[0-9a-fA-F]+[uU]?[lL]? {
      /* integral constant */
      errno = 0;
      ul_con = strtoul(yytext, &end_ptr, (int)0);
      if(errno) ddsYYError("'%s' overflows type 'unsigned long int'\n", yytext);
      size = strlen(yytext);
      if(! strchr("uUlL", yytext[size - 1])) {
         /* unsuffixed */
         if(yytext[0] == '0') {
            /* octal or hexadecimal */
            if(ul_con <= INT_MAX) {
               ptype = DDS_CTYPE_INT;
            } else if(ul_con <= UINT_MAX) {
               ptype = DDS_CTYPE_UINT;
            } else if(ul_con <= LONG_MAX) {
               ptype = DDS_CTYPE_LONG;
            } else {
               ptype = DDS_CTYPE_ULONG;
            }
         } else {
            /* decimal */
            if(ul_con <= INT_MAX) {
               ptype = DDS_CTYPE_INT;
            } else if(ul_con <= LONG_MAX) {
               ptype = DDS_CTYPE_LONG;
            } else {
               ptype = DDS_CTYPE_ULONG;
            }
         }
      } else if(strchr("uU", yytext[size - 1]) ||
         strchr("uU", yytext[size - 2])) {
         /* unsigned */
         if(ul_con <= UINT_MAX &&
            ! (strchr("lL", yytext[size - 1]) ||
            strchr("lL", yytext[size - 2]))) {
            ptype = DDS_CTYPE_UINT;
         } else {
            ptype = DDS_CTYPE_ULONG;
         }
      } else if(strchr("lL", yytext[size - 1]) ||
         strchr("lL", yytext[size - 2])) {
         /* long */
         if(ul_con <= LONG_MAX) {
            ptype = DDS_CTYPE_LONG;
         } else {
            ptype = DDS_CTYPE_ULONG;
         }
      }

      value_ptr = ddsTablePush(dds_symbol, dds_prime[ptype].prec,
         dds_prime[ptype].align);
      name_ptr = ddsTablePushStr(dds_symbol, yytext);
      value = ddsSymObjectLit(name_ptr, (DDS_SYM_FLAG)DDS_SYM_PREP,
         dds_prime[ptype].type, value_ptr, 0);

      switch(ptype) {
      case DDS_CTYPE_INT:
         *(int*)(value_ptr) = ul_con;
         break;
      case DDS_CTYPE_UINT:
         *(unsigned*)(value_ptr) = ul_con;
         break;
      case DDS_CTYPE_LONG:
         *(long*)(value_ptr) = ul_con;
         break;
      case DDS_CTYPE_ULONG:
         *(unsigned long*)(value_ptr) = ul_con;
         break;
      default:
         ddsPanic("scanner.l: integral constant, bad ptype\n");
      }
      yylval(YYvalue) = value;
      return INTEGER_TOKEN;
   }


(([0-9]+)|([0-9]*\.[0-9]+)([eE][-+]?[0-9]+)?)[FfLl]? {
      /* floating point token */
      errno = 0;
      d_con = strtod(yytext, &end_ptr);
      size = strlen(yytext);
      if(! strchr("fF", yytext[size - 1])) {
         ptype = DDS_CTYPE_FLOAT;
      } else if(! strchr("lL", yytext[size - 1])) {
         ptype = DDS_CTYPE_LDOUBLE;
      } else {
         ptype = DDS_CTYPE_DOUBLE;
      }
 
      value_ptr = ddsTablePush(dds_symbol,
         dds_prime[ptype].prec, dds_prime[ptype].align);
      name_ptr = ddsTablePushStr(dds_symbol, yytext);
      value = ddsSymObjectLit(name_ptr, (DDS_SYM_FLAG)DDS_SYM_PREP,
         dds_prime[ptype].type, value_ptr, 0);

      switch(ptype) {
      case DDS_CTYPE_FLOAT:
         *(float*)(value_ptr) = d_con;
         break;
      case DDS_CTYPE_DOUBLE:
         *(double*)(value_ptr) = d_con;
         break;
      case DDS_CTYPE_LDOUBLE:
         *(HOST_LDOUBLE_T*)(value_ptr) = d_con;
         break;
      default:
         ddsPanic("scanner.l: float constant, bad ptype\n");
      }
      yylval(YYvalue) = value;
      return FLOATING_TOKEN;
   }


[_a-zA-Z][_a-zA-Z0-9]* {
      /* "name" found (keyword, TYPEDEF_TOKEN,  */
      /*    ENUMERATION_TOKEN, or IDENTIFIER_TOKEN) */
      key = (DDS_KEYWORD*)ddsTableLookup(&dds_keyword, yytext);
      if(key) {
         /* name is a keyword */
         if(key->token == IDENT) {
            /* prognosis: ident("SHOT-X"), or other funky name */
            /* Support for arbitrary identifier names. */
            dds_lex_ss = DDS_LEX_IDENT;
            if(dds_yy_flex_debug) fprintf(stderr, "==ddsYYlex: parse ident(\"...\")\n");
            parse_rc = ddsYYparse(&value);
            if(dds_yy_flex_debug) fprintf(stderr, "##ddsYYlex: parse ident(\"...\")\n");
            if(parse_rc) {
               /* parse error */
               name = ddsTablePushStr(dds_symbol, "ident(<bad>)");
            } else {
               name = value->ptr;
            }
            /* fall through and process like an identifier */
         } else if(key->token == DEFN_VALUE) {
            /* prognosis: defn_value("dict_name","defn_alias") */
            /* Support for nested input from defn and dict. */
            dds_lex_ss = DDS_LEX_DEFN;
            if(dds_yy_flex_debug) fprintf(stderr, "==ddsYYlex: parse ident(\"...\")\n");
            parse_rc = ddsYYparse(NULL);
            if(dds_yy_flex_debug) fprintf(stderr, "##ddsYYlex: parse ident(\"...\")\n");
            break; /* continue scanning more tokens */
         } else {
            /* prognosis: reserved keyword in grammer. */
            yylval(YYvoid) = 0;
            return key->token; /* token associated with keyword */
         }
      } else {
         /* push "name/identifier" onto table */
         name = ddsTablePushStr(dds_symbol, yytext);
      }

      if(! (dds_declare->flag & DDS_SYM_IDENT)) {
         /* The parser does NOT want an IDENTIFIER_TOKEN, i.e. */
         /* its not expecting a tag name or declarator for a type. */
         object = LOOKUP_OBJECT_NEW(dds_symbol, name, dds_block);

         if(! dds_map_value && ! object) {
            /* We are NOT compiling an rvalue for a map expression */
            /* and this "name/identifier" is NOT a defined object. */
            /* A syntax error is pending, unless ddsYYdepend */
            /* can compile an object into the symbol table. */
            object = ddsYYdepend(name);
         }

         if(object) {
            /* "name" identifier is declared */
            if(object->flag & DDS_SYM_TYPEDEF) {
               /* Use existing typdef (don't declare new one). */
               yylval(YYtype) = ddsSymTypeMerge(object->type);
               yylval(YYtype)->trail = object;
               return TYPEDEF_TOKEN;
            } else if(object->flag & DDS_SYM_ENUM) {
               /* use existing object (enumerated constant) */
               yylval(YYvalue) = (DDS_VALUE*)object;
               return ENUMERATION_TOKEN;
            }
         }
      }
      yylval(YYname) = name;
      return IDENTIFIER_TOKEN;
   }


\" {
      /* Opening double quote.   Scan the string constant and */
      /* save it in the symbol table. */
      value_ptr = dds_symbol->page.ptr + dds_symbol->page.top;

      /* consume all characters in all adjacent strings. */
      while(DDS_TRUE) {
         char_in = input();
         while(char_in == '"') {
            /* closing double quote */
            do {char_in = input();}
               while(char_in != EOF &&
                  isspace(char_in)); /* eat white space */

            if(char_in == '"') {
               /* opening quote for adjacent string, concantenate. */
               char_in = input();
            } else {
               /* next token is not a string. */
               unput(char_in);
               goto string_done;
            }
         }
         if(char_in == '\\') {
            /* decode string escape sequence '\t', '\n', etc. */
            char_in = input();
            switch(char_in) {
            case '\n':
               /* line splicing (eat '\\' and '\n') */
               dds_lexer->line++;
               continue;
            case '\\': char_in = '\\'; break;
            case  'n': char_in = '\n'; break;
            case  't': char_in = '\t'; break;
            case  'v': char_in = '\v'; break;
            case  'b': char_in = '\b'; break;
            case  'r': char_in = '\r'; break;
            case  'f': char_in = '\f'; break;
            /* silence warnings from Sun acc regarding '\a' usage */
            case  'a': char_in = DDS_ASCII_ALERT; break;
            }
         } else if(char_in == '\n') {
            ddsYYError("newline within string constant\n");
            dds_lexer->line++;
            break;
         } else if(char_in == EOF) {
            ddsYYError("EOF within string constant\n");
            break;
         }

         /* append another character to string */
         value_ptr = (void*)ddsTablePushChar(dds_symbol, value_ptr, char_in);
      }

      /* closing quote found (or syntax error detected). */
      string_done:
      str_size = dds_symbol->page.top -
         ((char*)value_ptr - dds_symbol->page.ptr);
      value_ptr = (void*)ddsTablePushChar(dds_symbol, value_ptr, '\0');

      value = ddsSymObjectLit(value_ptr, (DDS_SYM_FLAG)DDS_SYM_PREP,
         dds_prime[DDS_CTYPE_CHAR].type, value_ptr, str_size);
      yylval(YYvalue) = value;
      return STRING_TOKEN;
   }


\' {
      /* Opening single quote.   Scan the character constant and */
      /* save it in the symbol table. */
      text_ptr = dds_symbol->page.ptr + dds_symbol->page.top;
      text_ptr = (void*)ddsTablePushChar(dds_symbol, text_ptr, '\'');
      char_const = 0;

      /* consume all characters in the constant. */
      for(char_in = input();
         char_in != '\'' && char_in != EOF;
         char_in = input()) {
         text_ptr = (void*)ddsTablePushChar(dds_symbol, text_ptr, char_in);
         /* translate escape sequences */
         if(char_in == '\\') {
            /* decode escape sequence '\ooo', '\xhh', '\t', '\n', etc. */
            char_in = input();
            text_ptr = (void*)ddsTablePushChar(dds_symbol, text_ptr, char_in);
            switch(char_in) {
            case '0': case '1': case '2': case '3':
            case '4': case '5': case '6': case '7':
               /* octal sequence of 1, 2, or 3 characters. */
               char_value = char_in - '0';
               char_in = input();
               text_ptr = (void*)ddsTablePushChar(dds_symbol, text_ptr, char_in);
               if(char_in >= '0' && char_in <= '7') {
                  char_value = (char_value << 3) + char_in - '0';
                  char_in = input();
                  text_ptr = (void*)ddsTablePushChar(dds_symbol, text_ptr, char_in);
                  if(char_in >= '0' && char_value <= '7') {
                     char_value = (char_value << 3) + char_in - '0';
                  } else {
                     unput(char_in);
                  }
               } else {
                  unput(char_in);
               }
               break;
            case 'x': case 'X':
               /* hexidecimal sequence of N characters. */
               char_value = 0;
               while(DDS_TRUE) {
                  char_in = input();
                  text_ptr = (void*)ddsTablePushChar(dds_symbol, text_ptr, char_in);
                  if(char_in >= '0' && char_in <= '9') {
                     char_value = (char_value << 4) + char_in - '0';
                  } else if(char_in >= 'a' && char_in <= 'f') {
                     char_value = (char_value << 4) + 10 + char_in - 'a';
                  } else if(char_in >= 'A' && char_in <= 'F') {
                     char_value = (char_value << 4) + 10 + char_in - 'A';
                  } else {
                     break;
                  }
               }
               unput(char_in);
               break;
            case '\\': char_value = '\\'; break;
            case  'n': char_value = '\n'; break;
            case  't': char_value = '\t'; break;
            case  'v': char_value = '\v'; break;
            case  'b': char_value = '\b'; break;
            case  'r': char_value = '\r'; break;
            case  'f': char_value = '\f'; break;
            /* silence warnings from Sun acc regarding '\a' usage */
            case  'a': char_value = DDS_ASCII_ALERT; break;
            }
         } else {
            char_value = char_in;
         }
         /* accumulate another character into the constant */
         char_const = (char_const << CHAR_BIT) + char_value;
      }
      text_ptr = (void*)ddsTablePushChar(dds_symbol, text_ptr, '\'');
      text_ptr = (void*)ddsTablePushChar(dds_symbol, text_ptr, '\0');
      value_ptr = ddsTablePush(dds_symbol, HOST_INT_P, HOST_INT_A);
      *(int*)value_ptr= char_const;
      value = ddsSymObjectLit(text_ptr, (DDS_SYM_FLAG)DDS_SYM_PREP,
         dds_prime[DDS_CTYPE_INT].type, value_ptr, 0);
      yylval(YYvalue) = value;
      return CHARACTER_TOKEN;
   }


"\\\n" {
      /* BROKEN, if splice should join token pieces ? */
      /* line splicing, count newline*/ dds_lexer->line++; break;}


"/*" {
      /* begin comment */ comment_state = YYSTATE; BEGIN(X_COMMENT); break;}


<X_COMMENT>\n {
      /* count newline in comment */ dds_lexer->line++; break;}
<X_COMMENT>([^*\n]*|\*[^/\n])* {
      /* discard comment text */ break;}
<X_COMMENT>"*/" {
      /* terminate comment */ BEGIN(comment_state); break;}


[ \t]+ {
      break; /* ignore white space */ }


\n {
      /* newline processing. */
      dds_lexer->line++;
      break;}


. {
      /* all other characters, return their pristene value */
      yylval(YYvoid) = 0;
      return *yytext;}


%%

   /***********************************************/
   /***   "user code" 3rd section of scanner.   ***/
   /***********************************************/

void ddsLexStr(
   DDS_BOOLEAN leof,
   const char *dict,
   const char *defn,
   const char *start,
   const char *stop,
   size_t line)
{
   YY_BUFFER_STATE yy_buf;
   int size;

   /* If "leof" is DDS_TRUE, this input is NOT nested, */
   /* i.e. scanner returns EOF when this string is consumed. */
   /* If "leof" is DDS_FALSE, this input IS nested, */
   /* i.e. scanner continues processing previous string */
   /* after this string has been consumed. */

   if(dds_yy_flex_debug) {
      size = stop - start;
      if(size < 100)
         fprintf(stderr, "==ddsLexStr: input '%.*s'\n",   size, start);
      else
         fprintf(stderr, "==ddsLexStr: input '%.*s...'\n", 100, start);
   }

   SCANNER_PUSH(leof, dict, defn, start, stop, line);

   return;
}

static int yywrap(void)
{  /* scanner input "eof" processing */

   if(dds_lexer == &dds_lexer_root) {
      ddsPanic("yywrap: multiple calls\n");
      return 1;
   }

   SCANNER_POP;

   if(dds_lexer->leof) return 1; /* EOF on buffer chain */

   return 0;
}


static size_t ddsYYinput(
   char *buf, 
   size_t max)
{  /* lex input from string (setup by ddsLexStr) */
   size_t size;

   size = dds_lexer->stop - dds_lexer->next;

   if(max < size) size = max;

   memcpy(buf, dds_lexer->next, size);

   dds_lexer->next += size;
   
   return size ? size : YY_NULL;
}

void ddsDefnValue(
   DDS_VALUE *dict_value,
   DDS_VALUE *defn_value)
{  /* nested input from another definitions */
   const char *dict, *defn;

   dict = dict_value->ptr;
   defn = ddsString(defn_value->ptr);

   if(! cdds_dict(dict, "scan") && ! cdds_scanf(defn, "")) {
      ddsLexStr(DDS_FALSE,
         dict, defn, dds_scan_token.next,
         dds_scan_dict->buf.ptr + dds_scan_newer->start,
         dds_scan_defn->line);
   } else {
      ddsVerbose("not found 'defn_value(\"%s\",\"%s\")'\n",
         dict, defn);
   }
   return;
}

#define ALIAS_MAX 1024

static DDS_OBJECT * ddsYYdepend(
   const char *name)
{
   static DDS_STRING alias = NULL_STRING;
   DDS_STATE create_state;
   DDS_LEX_START save_lex_ss;
   DDS_BLOCK *save_block;
   DDS_SYM *save_newest, *save_newer;
   DDS_SCANNER *scan;
   DDS_OBJECT *object;
   const char *dict, *defn;
   int level;

   /**************************************************************/
   /* Parse definitions to declare struct/union dependencies.    */
   /*                                                            */
   /* This processing deviates from the nominal "C" model.       */
   /* "C" requires all symbols to be declared before use.        */
   /* "#include" explicitly references all input "C" source.     */
   /*                                                            */
   /* In DDS, declarations are scattered among many definitions. */
   /* Dictionary definitions are implicitly referenced, i.e. if  */
   /* an identifier is referenced that is not already declared,  */
   /* an attempt is made to find and compile the required defn.  */
   /**************************************************************/

   cdds_dict(dds_compile->dict_fmt, "scan");
   create_state = (DDS_STATE)(dds_state & DDS_YACC_CREATE);
   save_block = dds_block;

   if(dds_state & DDS_YACC_CREATE) {
      /* Compile format using "new" precedence. */
      /* Try format rule to CREATE new symbols (fmt:-:-.name). */
      ddsAliasName3(&alias, "fmt", dds_compile->old_format, 
         dds_compile->alias_name, name);
      if(cdds_scanf(alias.ptr, "")) {
         ddsAliasName3(&alias, "fmt", dds_compile->old_format,
            dds_compile->fmt_name, name);
         if(cdds_scanf(alias.ptr, "")) {
            /* "CREATE rule" is not defined. */
            /* Expand symbol table search into block_old. */
            object = LOOKUP_OBJECT(dds_symbol, name, dds_block);
            if(object) {
               /* Object is defined within block_old.  Use it ! */
               return object;
            } else {
               /* Try format rule for EXISTING symbols (fmt:-.name). */
               ddsStrCpyCat2(&alias, "fmt:", name);
               if(cdds_scanf(alias.ptr, "")) {
                  /* "EXISTING rule" is not defined. */
                  /* Fail! parser will have an undefined symbol error */
                  return NULL;
               }
               /* re-open the block_old scope */
               dds_block = dds_compile->block_old;
               dds_state &= (DDS_STATE) ~ DDS_YACC_CREATE;
            }
         } else {
            /* re-open the block_new scope */
            dds_block = dds_compile->block_new;
         }
      } else {
         /* re-open the block_new scope */
         dds_block = dds_compile->block_new;
      }
   } else {
      /* Compile format using "old" precedence. */
      /* Try format rule for EXISTING symbols (fmt:-.name). */
      ddsStrCpyCat2(&alias, "fmt:", name);
      if(cdds_scanf(alias.ptr, "")) {
         /* "EXISTING rule" is not defined. */
         /* Try format rule to CREATE new symbols (fmt:-:-.name). */
         ddsAliasName3(&alias, "fmt", dds_compile->old_format,
            dds_compile->alias_name, name);
         if(cdds_scanf(alias.ptr, "")) {
            ddsAliasName3(&alias, "fmt", dds_compile->old_format,
               dds_compile->fmt_name, name);
            if(cdds_scanf(alias.ptr, "")) {
               /* "CREATE rule" is not defined. */
               /* Fail ! parser will have an undefined symbol error */
               return NULL;
            }
         }
      }
      if(dds_compile->state & DDS_BIN_OPEN_CREATE) {
         /* re-open the block_old scope */
         dds_block = dds_compile->block_old;
      } else {
         /* re-open the block_new scope */
         dds_block = dds_compile->block_new;
      }
   }

   /*****************************************************************/
   /* Call ddsYYparser recursively, which should create the object. */
   /*****************************************************************/

   for(scan = dds_lexer, level = DEFN_NEST_MAX;
      scan && level;
      scan = scan->outer, level--) /* empty */ ;

   if(scan) {
      ddsError("binary format definition nesting > %i\n\tdict= %s, defn=%s\n\tbinary= %s\n",
         DEFN_NEST_MAX, dds_scan_dict->name,
         dds_scan_name, dds_compile->name);
      return NULL;
   }

   dict = dds_scan_dict->name;
   defn = ddsString(dds_scan_name);

   /* preserve current state */
   save_lex_ss = dds_lex_ss;
   DECLARE_INNER;
   save_newest = (DDS_SYM*)dds_symbol->newest;
   save_newer = (DDS_SYM*)dds_block->newer;

   /* disconnect the older/newer symbol chain. */
   dds_symbol->newest = (DDS_ITEM*)dds_block;
   dds_block->newer = NULL;
   if(save_newer) save_newer->older = NULL;

   /* initialize flags, blocks, and lex input */
   dds_lex_ss = DDS_LEX_TRANSLATE;

   /* tell the scanner where to find the input text */
   ddsLexStr(DDS_TRUE, dict, defn, dds_scan_token.next,
      dds_scan_dict->buf.ptr + dds_scan_newer->start,
      dds_scan_defn->line);

   if(dds_yy_flex_debug) fprintf(stderr,
      "==ddsYYdepend: parse dependency '%s'\n", defn);

   /* compile format definitions for binary data */
   ddsYYparse(NULL);

   if(dds_yy_flex_debug) fprintf(stderr,
      "##ddsYYdepend: parse dependency '%s'\n", defn);

   object = LOOKUP_OBJECT(dds_symbol, name, dds_block);

   if(save_newer) {
      /* reconnect the symbol chain */
      save_newer->older = (DDS_ITEM*)dds_symbol->newest;
      dds_symbol->newest->newer = (DDS_ITEM*)save_newer;
      dds_symbol->newest = (DDS_ITEM*)save_newest;
   }

   DECLARE_OUTER;
   dds_lex_ss = save_lex_ss;
   dds_block = save_block;
   dds_state |= create_state;

   return object;
}


int ddsYYerror(
   const char *msg)
{
   /* Only called by "bison", when it detects a syntax error. */
   /* It should NOT be called by any DDS functions directly. */
   return ddsYYError("%s%s", msg, "\n");
}

int ddsYYError(
   const char *fmt,
   ...)
{
   DDS_BOOLEAN print_msg;
   const char *dict_name, *defn_name;
   int n, line_numb;
   char msg[DDS_MSG_MAX+1];
   va_list args;
 
   va_start(args, fmt);

   if(dds_state & (DDS_VERBOSE | DDS_ERROR_ABORT | DDS_ERROR_PRINT)) {
      print_msg = DDS_TRUE;
   } else {
      print_msg = DDS_FALSE;
   }

   dds_error_count++;

   if(dds_lexer->outer) {
      dict_name = dds_lexer->outer->dict;
      defn_name = dds_lexer->outer->defn;
      line_numb = dds_lexer->line;
   } else {
      dict_name = dds_compile->dict_open;
      defn_name = "<defn_name>";
      line_numb = 0;
   }
 
   sprintf(msg, "DDS SYNTAX ERROR: dict=%s, line= %i, defn= %s\n\t",
      dict_name, line_numb, defn_name);

   /* Message body supplied by caller. */
   /* Intermediate lines terminated by "\n\t", last line by "\n". */
   n = strlen(msg);
   vsprintf(msg + n, fmt, args);
   va_end(args);
   if(print_msg) ddsPrintStr(msg);
   ddsStrCpy(&dds_error_msg, msg);

   return errno ? errno : (dds_error ? dds_error : EINVAL) ;
}

int ddsYYWarn(
   const char *fmt,
   ...)
{
   DDS_BOOLEAN print_msg;
   const char *dict_name, *defn_name;
   int n, line_numb;
   char msg[DDS_MSG_MAX+1];
   va_list args;
 
   va_start(args, fmt);

   if(dds_state & (DDS_VERBOSE | DDS_WARN_ABORT | DDS_WARN_PRINT)) {
      print_msg = DDS_TRUE;
   } else {
      print_msg = DDS_FALSE;
   }

   dds_warn_count++;
 
   if(dds_lexer->outer) {
      dict_name = dds_lexer->outer->dict;
      defn_name = dds_lexer->outer->defn;
      line_numb = dds_lexer->line;
   } else {
      dict_name = dds_compile->dict_open;
      defn_name = "<defn_name>";
      line_numb = 0;
   }
 
   sprintf(msg, "DDS SYNTAX WARNING: dict=%s, line= %i, defn= %s\n\t",
      dict_name, line_numb, defn_name);

   /* Message body supplied by caller. */
   /* Intermediate lines terminated by "\n\t", last line by "\n". */
   n = strlen(msg);
   vsprintf(msg + n, fmt, args);
   va_end(args);
   if(print_msg) ddsPrintStr(msg);
   ddsStrCpy(&dds_warn_msg, msg);

   return errno ? errno : (dds_error ? dds_error : EINVAL) ;
}

#include "dds_dump.h"

void ddsDumpScanner(
   void *handle)
{
   DDS_SCANNER *scan = handle;

   if(! dds_initialized) return; /* no-op */

   if(! scan) {
      fprintf(dds_print_fp, "ddsDumpScanner: NULL SCAN ptr\n");
      return;
   }

   if(scan->outer) {
      ddsDumpScanner(scan->outer);
   }

   fprintf(dds_print_fp, "SCANNER &= %p; ", (void*)scan);
   if(! scan->outer)
      fprintf(dds_print_fp, "OLDEST ");
   else
      fprintf(dds_print_fp, "NEWER ");
   fprintf(dds_print_fp, "leof= %s; line= %i;\n",
      scan->leof ? "TRUE" : "FALSE",
      (int)scan->line);
   fprintf(dds_print_fp, "%*sdict= %s; defn= %s;\n",
      DDS_DENT, "", scan->dict ? scan->dict : "<nil>",
                    scan->defn ? scan->defn : "<nil>");
   fprintf(dds_print_fp, "%*sstart= %.*s...\n",
      DDS_DENT, "", 50, scan->start ? scan->start : "<nil>");
   fprintf(dds_print_fp, "%*snext= %.*s...\n",
      DDS_DENT, "", 50, scan->next ? scan->next : "<nil>");

   return;
}
