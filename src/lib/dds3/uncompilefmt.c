static char rcsid[]="$Id: uncompilefmt.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, uncompile binary data format

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

/* enumeration alias stack */
static DDS_STACK enum_alias;

#ifdef DDS_UNCOMPILE_CHECK
   #define PUSH_TOKEN(STACK, PTR) \
      if(dds_debug_api & DDS_DBG_UNCOMPILE) { \
         if(NULL != (PTR)) { \
            fprintf(stderr, "ddsUncompileFmt: stack= %p; str= \"%s\"\n", \
               (void*)(STACK), (char*)(PTR)); \
         } else { \
            fprintf(stderr, "ddsUncompileFmt: stack= %p; str= \"<NULL>\"\n", \
               (void*)(STACK)); \
         } \
      } \
      ddsStackPushPtr((STACK), (PTR));
#else
   #define PUSH_TOKEN(STACK, PTR)  ddsStackPushPtr((STACK), (PTR));
#endif

/* prototype */
static DDS_TYPE * UnDeclare(
   DDS_STACK *token,
   DDS_TYPE *type)
;
static void UnObject(
   DDS_OBJECT *object)
;
static void Uncompile(
   DDS_SYM *sym)
;

static int ScanAlias(
   const char *name,
   DDS_STACK *token)
{  /* compare defn and token values for alias equivalence */
   static DDS_STRING defn_name = NULL_STRING;
   DDS_DICT *scan_dict;
   int status = DDS_OK;
   const DDS_DEFN *scan_defn;
   const char **ptrptr, **stop_ptrptr, *ptr;

   /* Find the target of the alias definitions */
   cdds_scanf(name, "");
   scan_dict = dds_scan_dict;
   scan_defn = dds_scan_defn;

   /* scan the list of alias names */
   stop_ptrptr = (const char**)(token->ptr + token->top);
   for(ptrptr = (const char**)token->ptr;
      ptrptr != stop_ptrptr;
      ptrptr++) {
      ptr = *ptrptr;
      ddsStrCpyCat2(&defn_name, "fmt:", ptr);
      cdds_scanf(defn_name.ptr, "");
      if(scan_dict == dds_scan_dict && scan_defn == dds_scan_defn) {
         /* alias is still valid, zap the name pointer */
         *ptrptr = NULL;
      } else {
         /* one or more alias definitions need to be updated */
         status = DDS_BAD;
      }
   }

   return status;
}

static void PrintAlias(
   const char *name,
   DDS_STACK *token)
{  /* append alias defn, given name and alias value. */
   static DDS_STRING defn_name = NULL_STRING;
   const char **ptrptr, **stop_ptrptr, *ptr;

   stop_ptrptr = (const char**)(token->ptr + token->top);
   for(ptrptr = (const char**)token->ptr;
      ptrptr != stop_ptrptr;
      ptrptr++) {
      ptr = *ptrptr;
      if(! ptr) continue; /* name has been zapped (alias still valid) */

      /* append alias defn to another name */
      ddsStrCpyCat2(&defn_name, "alias:fmt:", ptr);
      cdds_printf(defn_name.ptr, " %s\n", name);
   }

   return;
}

static int ScanStack(
   const char *name,
   DDS_STACK *token)
{  /* compare defn and token values for equivalence */
   typedef const char DUMP[16];
   const char **ptrptr, **stop_ptrptr, *ptr, *value;
   char test;
   size_t len;

   if(0 >= cdds_scanv(name, &value)) {
      /* nothing defined (nothing to compare) */
      return DDS_BAD;
   }

   /* compare all tokens to the value of this definition */
   test = ' ';
   stop_ptrptr = (const char**)(token->ptr + token->top);
   for(ptrptr = (const char**)token->ptr;
      ptrptr != stop_ptrptr;
      ptrptr++) {
      ptr = *ptrptr;
      if(ptr) {
         len = strlen(ptr);
         if(len) {
            if(isalnum(test) && isalnum(*ptr)) {
               /* white space required within defn value */
               if(! isspace(*value)) return DDS_BAD;
            }
            while(isspace(*value)) value++; /* ignore white space */
            if(memcmp(ptr, value, len)) {
               /* difference exists between tokens and old defn */
               return DDS_BAD;
            }
            value += len; /* walk past non-white space characters */
            test = ptr[len - 1];
         }
      }
   }

   while(isspace(*value)) value++; /* ignore trailing white space */

   if(*value) return DDS_BAD; /* trailing difference */

   return DDS_OK;
}

static void PrintStack(
   const char *name,
   DDS_STACK *token)
{  /* append format defn, given its name and token values. */
   const char **ptrptr, **stop_ptrptr, *ptr;
   const char *ptr1, *ptr2, *ptr3;
   char last;
   size_t len, len1, len2, len3;
   int line, nest;

   nest = 0;
   last = ' ';
   line = strlen(name) + 2;
   cdds_printf(name, " ");

   /* print all token values */
   stop_ptrptr = (const char**)(token->ptr + token->top);
   for(ptrptr = (const char**)token->ptr;
      ptrptr != stop_ptrptr;
      ptrptr++) {
      ptr = *ptrptr;
      if(ptr) {
         /* nominal token processing */
         len = strlen(ptr);
         if(len) {
            /* non-trivial token */
            if(len == 1) {
               /* token is a single character (special processing) */
               if(line > DDS_UNCOMP_LINE) {
                  /* maximum line length has been reached */
                  last = *ptr;
                  if(last == ',' || last == '}') {
                     /* let the line grow some more ? */
                     last = ' ';
                  } else {
                     line = (nest + 1) * DDS_UNCOMP_DENT;
                     cdds_printf("", "\n%*s", line, "");
                  }
               } else if(last == ',' || (isalnum(last) && isalnum(*ptr))) {
                  /* want/need white space between tokens */
                  last = *ptr;
                  cdds_printv("", " ");
                  line++;
               } else if(last == ';') {
                  /* nominal new line indicator */
                  last = *ptr;
                  if(last != '}') {
                     /* last character was ";" (start a new line) */
                     /* and this is not a special case, i.e. ";}" */
                     last = ' ';
                     line = (nest + 1) * DDS_UNCOMP_DENT;
                     cdds_printf("", "\n%*s", line, "");
                  }
               } else {
                  last = *ptr;
               }

               if(last == '{') {
                  /* cosmetic white space before token */
                  nest++;
                  cdds_printf("", " %s", ptr);
                  line += 2;
               } else if(last == '}') {
                  /* cosmetic white space after token */
                  nest--;
                  cdds_printf("", "%s ", ptr);
                  line += 2;
               } else {
                  /* just print the damn token ! */
                  cdds_printv("", ptr);
                  line++;
               }
            } else {
               /* token contains multiple characters */
               if(last == ';') {
                  /* nominal new line indicator */
                  line = nest * DDS_UNCOMP_DENT;
                  cdds_printf("", "\n%*s", line, "");
               } else if(line + len + 4 > DDS_UNCOMP_LINE) {
                  /* maximum line length has been (almost) reached */
                  line = (nest + 1) * DDS_UNCOMP_DENT;
                  cdds_printf("", "\n%*s", line, "");
               } else if(last == ',' || (isalnum(last) && isalnum(*ptr))) {
                  /* want/need white space between tokens */
                  cdds_printv("", " ");
                  line++;
               }
               last = ptr[len - 1];
               line += len;
               cdds_printv("", ptr);
            }
         }
      } else {
         /* token triplet, i.e. "quote string" or 'c' or ident("name") */
         ptr1 = *++ptrptr;
         ptr2 = *++ptrptr;
         ptr3 = *++ptrptr;
         len1 = strlen(ptr1);
         len2 = strlen(ptr2);
         len3 = strlen(ptr3);
         len = len1 + len2 + len3;
         if(line + len + 2 > DDS_UNCOMP_LINE) {
            /* maximum line length has been (almost) reached */
            line = (nest + 1) * DDS_UNCOMP_DENT;
            cdds_printf("", "\n%*s", line, "");
         } else if(last == ',' || (isalnum(last) && isalnum(*ptr1))) {
            /* want/need white space between tokens */
            cdds_printv("", " ");
            line++;
         }
         last = ptr3[len3 - 1];
         line += len;
         cdds_printv("", ptr1);
         cdds_printv("", ptr2);
         cdds_printv("", ptr3);
      }
   }
   cdds_printf("", "\n\n");
   return;
}

static DDS_TYPE * BaseType(
   DDS_TYPE *type)
{
   /* walk down to the base type */
   for( /* empty */ ;
      type->type && (type->flag & DDS_SYM_ARRAY) && ! type->trail;
      type = type->type) /* empty */ ;

   return type;
}

static void UnIdentify(
   DDS_STACK *token,
   const char *sym_name)
{  /* append symbol name to uncompile stack */
   DDS_BOOLEAN special = DDS_FALSE;
   const char *name = sym_name;
   char test;

   test = *name++;
   if(test != '_' && ! isalpha(test)) {
      special = DDS_TRUE;
   } else {
      for(test = *name++; test; test = *name++) {
         if(test != '_' && ! isalnum(test)) {
            special = DDS_TRUE;
            break;
         }
      }
   }

   if(special) {
      /* special syntax, i.e. indent("sym_name") */
      PUSH_TOKEN(token, NULL); /* token triplet */
      PUSH_TOKEN(token, "ident(\"");
      PUSH_TOKEN(token, sym_name);
      PUSH_TOKEN(token, "\")");
   } else {
      /* simple syntax, i.e. sym_name */
      PUSH_TOKEN(token, sym_name);
   }

   return;
}

static void UnValue(
   DDS_STACK *token,
   DDS_VALUE *value)
{
   DDS_OBJECT *object, *enum_object;
   DDS_EXPR4 *expr;
   DDS_OPCODE opcode;
   DDS_TYPE *type;

   if(value->flag & DDS_SYM_OBJECT) {
      object = (DDS_OBJECT*)value;
      if(object->flag & DDS_SYM_LITERAL) {
         if(object->type == dds_prime[DDS_CTYPE_CHAR].type) {
            if(object->type->flag & DDS_SYM_ARRAY) {
               PUSH_TOKEN(token, NULL); /* token triplet */
               PUSH_TOKEN(token, "\"");
               PUSH_TOKEN(token, object->name);
               PUSH_TOKEN(token, "\"");
            } else {
               PUSH_TOKEN(token, NULL); /* token triplet */
               PUSH_TOKEN(token, "'");
               PUSH_TOKEN(token, object->name);
               PUSH_TOKEN(token, "'");
            }
         } else {
            PUSH_TOKEN(token, object->name);
         }
      } else if(object->flag & DDS_SYM_ENUM) {
         PUSH_TOKEN(token, object->name);
         assert(object->flag & DDS_SYM_CONST_DECL);
         enum_object = object;
         type = object->type;
         object = UNIQUE_OBJECT(dds_symbol, type->name, type->block->block);
         assert(object && "Uncompile: confusion 1");
         PUSH_TOKEN(&enum_alias, type->name);
         PUSH_TOKEN(&enum_alias, enum_object->name);
         if(! (object->flag & DDS_SYM_UNCOMP)) {
            /* unravel enumeration before continuing */
            Uncompile((DDS_SYM*)object);
         }
      } else {
         ddsPanic("ddsUncompileFmt:UnValue: confusion 1\n");
      }
   } else if(value->flag & DDS_SYM_EXPR) {
      expr = (DDS_EXPR4*)value;
      opcode = (DDS_OPCODE)expr->opcode;
      switch(dds_opnote[opcode].args) {
      case 4:
         /* conditional operator, i.e. "test ? true : false" */
         UnValue(token, expr->arg1.track);
         PUSH_TOKEN(token, dds_opnote[opcode].infix);
         UnValue(token, expr->arg2.track);
         PUSH_TOKEN(token, dds_opnote[opcode].suffix);
         UnValue(token, expr->arg3.track);
         break;
      case 3:
         break;
      case 2:
         /* binary operators, i.e. +, -, *, /, [], ., -> */
         PUSH_TOKEN(token, dds_opnote[opcode].prefix);
         UnValue(token, expr->arg1.track);
         PUSH_TOKEN(token, dds_opnote[opcode].infix);
         UnValue(token, expr->arg2.track);
         PUSH_TOKEN(token, dds_opnote[opcode].suffix);
         break;
      case 1:
         /* unary operators and functions */
         PUSH_TOKEN(token, dds_opnote[opcode].prefix);
         UnValue(token, expr->arg1.track);
         PUSH_TOKEN(token, dds_opnote[opcode].suffix);
         break;
      case 0:
         /* implicit operators, i.e. linetrc(), reeltrc() */
         PUSH_TOKEN(token, dds_opnote[opcode].prefix);
         break;
      default:
         ddsPanic("ddsUncompileFmt:UnValue: confusion 7\n");
      }
   } else {
      ddsPanic("ddsUncompileFmt:UnValue: confusion 2\n");
   }
   return;
}

static void UnArray(
   DDS_STACK *token,
   DDS_TYPE *type)
{
   /* walk down to the base type */
   for( /* empty */ ;
      (type->flag & DDS_SYM_ARRAY) && ! type->trail;
      type = type->type) {
      PUSH_TOKEN(token, "[");
      UnValue(token, type->value_c);
      PUSH_TOKEN(token, "]");
   }

   return;
}

static void UnSUEBlock(
   DDS_STACK *token,
   DDS_BLOCK *block)
{
   DDS_SYM *sym, *sym1;
   DDS_MEMBER *member, *member1;
   DDS_OBJECT *object;
   DDS_TYPE *block_type, *base1_type, *base_type;
   DDS_BOOLEAN more;

   if(block->name) {
      /* unravel tag name */
      UnIdentify(token, block->name);
   }

   /* opening brace for this block */
   PUSH_TOKEN(token, "{");

   if(block->flag & (DDS_SYM_STRUCT | DDS_SYM_UNION)) {
      /* unravel struct or union member declarations */
      for(sym1 = block->first;
         sym1;
         sym1 = (DDS_SYM*)sym1->newer) {

         if(! (sym1->flag & DDS_SYM_MEMBER) ||
            (sym1->flag & DDS_SYM_UNCOMP)) continue;

         member1 = (DDS_MEMBER*)sym1;
         member1->flag |= DDS_SYM_UNCOMP;

         base1_type = UnDeclare(token, member1->type);
         UnIdentify(token, member1->name);
         UnArray(token, member1->type);

         /* walk the list of remaining symbols in the member's scope */
         for(sym = (DDS_SYM*)sym1->newer;
            sym;
            sym = (DDS_SYM*)sym->newer) {

            /* skip symbols that are not members of this block */
            if(sym->block != block ||
               ! (sym->flag & DDS_SYM_MEMBER)) continue;

            base_type = BaseType(sym->type);
            if(base_type != base1_type) {
               sym1 = (DDS_SYM*)sym->older;
               break;
            }

            assert(! (sym->flag & DDS_SYM_UNCOMP) && "what the heck");

            member = (DDS_MEMBER*)sym;
            member->flag |= DDS_SYM_UNCOMP;

            PUSH_TOKEN(token, ",");
            UnIdentify(token, member->name);
            UnArray(token, member->type);
         }
         PUSH_TOKEN(token, ";");
      }
   } else {
      /* unravel enumerated list */
      more = DDS_FALSE;
      block_type = block->type;
      for(sym = (DDS_SYM*)block->block->newer;
         sym;
         sym = (DDS_SYM*)sym->newer) {

         if(! (sym->flag & DDS_SYM_OBJECT) ||
            (sym->flag & DDS_SYM_TYPEDEF) ||
            sym->type != block_type) continue;

         object = (DDS_OBJECT*)sym;
         object->flag |= DDS_SYM_UNCOMP;

         if(more) {
            PUSH_TOKEN(token, ",");
         } else {
            more = DDS_TRUE;
         }

         PUSH_TOKEN(token, object->name);
         if(object->track) {
            PUSH_TOKEN(token, " \\= ");
            UnValue(token, object->track);
         }
      }
   }

   /* closing brace for this block */
   PUSH_TOKEN(token, "}");
   return;
}

static DDS_TYPE * UnDeclare(
   DDS_STACK *token,
   DDS_TYPE *type)
{
   DDS_PTYPE stype;
   DDS_OBJECT *object;

   /* walk down to the base type */
   for( /* empty */ ;
      type->type && (type->flag & DDS_SYM_ARRAY) && ! type->trail;
      type = type->type) /* empty */ ;

   /**************************/
   /* append type qualifiers */
   /**************************/

   if((type->flag & DDS_SYM_CONST) && ! (type->flag & DDS_SYM_ENUM)) {
      if(! type->trail || ! (type->trail->flag & DDS_SYM_CONST)) {
         PUSH_TOKEN(token, "const");
      }
   }
   if(type->flag & DDS_SYM_VOLATILE) {
      if(! type->trail || ! (type->trail->flag & DDS_SYM_VOLATILE)) {
         PUSH_TOKEN(token, "volatile");
      }
   }
   if(type->flag & DDS_SYM_ALIGN) {
      if(! type->trail || ! (type->trail->flag & DDS_SYM_ALIGN) ||
         type->value_a != type->trail->type->value_a) {
         PUSH_TOKEN(token, "align(");
         UnValue(token, type->value_a);
         PUSH_TOKEN(token, ") ");
      }
   }

   /**************************/
   /* append type specifiers */
   /**************************/

   if(type->trail) {
      object = type->trail;
      PUSH_TOKEN(token, object->name);
      if(! (object->flag & DDS_SYM_UNCOMP)) {
         /* unravel this object before continuing */
         Uncompile((DDS_SYM*)object);
      }
   } else {
      if(type->flag & DDS_SYM_STRUCT) {
         PUSH_TOKEN(token, "struct");
         UnSUEBlock(token, type->block);
      } else if(type->flag & DDS_SYM_UNION) {
         PUSH_TOKEN(token, "union");
         UnSUEBlock(token, type->block);
      } else if(type->flag & DDS_SYM_ENUM) {
         if(type == dds_prime[DDS_CTYPE_INT].type) {
            /* This is NOT a host independent type. */
            /* It should never be used within a struct or union */
            PUSH_TOKEN(token, "enum");
         } else if(type->prec == 1) {
            PUSH_TOKEN(token, "enum1");
         } else if(type->prec == 2) {
            PUSH_TOKEN(token, "enum2");
         } else if(type->prec == 4) {
            PUSH_TOKEN(token, "enum4");
         } else if(type->prec == 8) {
            PUSH_TOKEN(token, "enum8");
         } else {
            ddsPanic("ddsUncompileFmt:UnDeclare: enum precision\n");
         }
         if(type->block->name) {
            /* unravel tag name */
            UnIdentify(token, type->block->name);
         }
         UnSUEBlock(token, type->block);
      } else {
         /* Avoid host dependent specifiers (int, float, etc.) */
         /* Use host independent "stream" type name. */
         stype = dds_prime[type->ptype].stype;
         PUSH_TOKEN(token, dds_prime[stype].name);
      }
   }
   return type;
}

static void UnObject(
   DDS_OBJECT *object)
{
   DDS_STRING defn_name;
   DDS_STACK alias;
   DDS_STACK token;
   DDS_SYM *sym;
   DDS_OBJECT *object1;
   DDS_BLOCK *block;

   /* initialize stacks and strings */
   ddsStrSetMin(&defn_name, "", 64);
   ddsStackInit(&alias, 100 * sizeof(void*));
   ddsStackInit(&token, 100 * sizeof(void*));

   object1 = object;
   object->flag |= DDS_SYM_UNCOMP;

   /************************/
   /* append storage class */
   /************************/

   if(object->flag & DDS_SYM_TYPEDEF) {
      PUSH_TOKEN(&token, "typedef");
   }

   UnDeclare(&token, object->type);
   UnIdentify(&token, object->name);
   UnArray(&token, object->type);

   /* walk the list of symbols in the object's scope */
   block = object->block;
   for(sym = block->first ? (DDS_SYM*)block->first : (DDS_SYM*)block->newer;
      sym;
      sym = (DDS_SYM*)sym->newer) {

      /* only uncompile "interesting" symbols */
      if(sym->block != block ||
         sym->type != object1->type ||
         ! (sym->flag & DDS_SYM_OBJECT)) continue;

      if(sym->flag & (DDS_SYM_ERROR | DDS_SYM_LITERAL | DDS_SYM_UNCOMP))
         continue;

      object = (DDS_OBJECT*)sym;
      object->flag |= DDS_SYM_UNCOMP;

      /* subsequent objects must be aliased to primary name */
      PUSH_TOKEN(&alias, object->name);

      PUSH_TOKEN(&token, ",");
      UnIdentify(&token, object->name);
      UnArray(&token, object->type);
   }

   /* terminate declaration */
   PUSH_TOKEN(&token, ";");

   /* scan history and compare to token stack */
   cdds_dict(dds_compile->dict_open, "scan");
   ddsStrCpyCat2(&defn_name, "fmt:", object1->name);
   if(ScanStack(defn_name.ptr, &token)) {
      /* update format defn in the history. */
      cdds_dict(dds_compile->dict_open, "print");
      PrintStack(defn_name.ptr, &token);

      /* update alias defn to primary name */
      PrintAlias(defn_name.ptr, &alias);
   } else {
      cdds_dict(dds_compile->dict_open, "scan");
      if(ScanAlias(defn_name.ptr, &alias)) {
         /* update alias defn to primary name */
         cdds_dict(dds_compile->dict_open, "print");
         PrintAlias(defn_name.ptr, &alias);
      }
   }

   /* terminate stacks and strings */
   ddsStackTerm(&token);
   ddsStackTerm(&alias);
   ddsStrFree(&defn_name);

   return;
}

static void Uncompile(
   DDS_SYM *sym)
{
   DDS_OBJECT *object;

   if(sym->flag & DDS_SYM_OBJECT) {
      object = (DDS_OBJECT*)sym;
      UnObject(object);
   } else {
      ddsPanic("Uncompile: confusion 2\n");
   }
   return;
}

int ddsUncompileFmt(
   DDS_BIN *bin)
{  /* uncompile the binary format */
   static DDS_STRING root_value = NULL_STRING;
   static DDS_STRING defn_name = NULL_STRING;
   static DDS_STRING alias_name = NULL_STRING;
   DDS_SYM *sym;
   DDS_BLOCK *block_new;
   const char **ptrptr, **stop_ptrptr, *ptr;

   dds_compile = bin;
   dds_symbol = &bin->symbol;
   block_new = bin->block_new;

   if(! enum_alias.ptr) {
      ddsStackInit(&enum_alias, 100 * sizeof(void*));
   } else {
      enum_alias.top = 0;
   }

   ddsStrCpyMin(&root_value, "", 128);

   cdds_dict(dds_compile->dict_open, "print");
   cdds_printf("", "\n");

   /* walk the list of symbols in block_new scope */
   for(sym = (DDS_SYM*)block_new->newer;
      sym;
      sym = (DDS_SYM*)sym->newer) {

      /* only uncompile "interesting" symbols */
      if(sym->block != block_new ||      /* wrong block */
         ! (sym->flag & DDS_SYM_OBJECT)  /* not an object */
         ) continue;

      if(sym->flag & (
         DDS_SYM_ERROR |       /* don't touch the leper */
         DDS_SYM_LITERAL |     /* literal, i.e. 123, 3.14, "string", ... */
         DDS_SYM_CONST_DECL |  /* enumeration constant */
         DDS_SYM_UNCOMP))      /* already uncompiled */
         continue;

      if(sym->name &&
         (! strcmp(sym->name, "SEQUENCE") ||
          ! strncmp(sym->name, "PREAMBLE", 8) ||
          ! strncmp(sym->name, "EPILOGUE", 8))) {
         /* root of format, i.e. typedef USP_TRACE SEQUENCE; */
         ddsStrCat5(&root_value, "   typedef ", sym->type->trail->name,
            " ", sym->name, ";\n");
      }

      Uncompile(sym);
   }

   /* update root defn for this format, i.e. */
   /* fmt:usp= typedef USP_LINE PREAMBLE; typedef USP_TRACE SEQUENCE; */
   ddsStrCpyCat2(&defn_name, "fmt:", bin->fmt_name);
   cdds_dict(dds_compile->dict_open, "scan");
   if(0 >= cdds_scanv(defn_name.ptr, &ptr) ||
      strcmp(ptr, root_value.ptr)) {
      cdds_dict(dds_compile->dict_open, "print");
      cdds_printf(defn_name.ptr, "%s", root_value.ptr);
   } else {
      cdds_dict(dds_compile->dict_open, "print");
   }

   /* create alias defn for enum references */
   stop_ptrptr = (const char**)(enum_alias.ptr + enum_alias.top);
   for(ptrptr = (const char**)enum_alias.ptr;
      ptrptr != stop_ptrptr;
      ptrptr++) {
      ddsStrCpyCat2(&alias_name, "fmt:", *ptrptr);
      ptrptr++;
      ddsStrCpyCat2(&defn_name, "alias:fmt:", *ptrptr);
      cdds_printf(defn_name.ptr, " %s\n", alias_name.ptr);
   }

   return DDS_OK;
}
