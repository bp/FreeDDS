static char rcsid[]="$Id: dumpdatabuf.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump binary data buffer (recursive)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

/*****************************************************************/
/* Example header, format of binary data dump.                   */
/* 123456789012345678901234567890123456789012345678901234567890  */
/*****************************************************************/
/* DDS:Dump(1) read  bin= 0, data= classic.usp                  */
/*     Dump(1) media= 0, bias= 0, trc= 123, cube= (*, 23, 5)    */
/*****************************************************************/
/* Example contents, format of binary data dump.                 */
/* 123456789012345678901234567890123456789012345678901234567890  */
/*****************************************************************/
/*     Dump(1)    0: SEQUENCE= struct                           */
/*        Dump(1)    0: usp_green_prefix= 1260                     */
/*        Dump(1)    4: SGRNum= 0                                  */
/*        Dump(1)    6: SGRAmp= 0                                  */
/*****************************************************************/
/*        Dump(1)   92: TiLiIx [6 of 6]= 1 2 3 4 5 6               */
/*        Dump(1)  104: TVPair [20 of 40]=                         */
/*           Dump(1)    0:  1  2  3  4  5  6  7  8  9 10           */
/*           Dump(1)   40: 11 12 13 14 15 16 17 18 19 20           */
/*****************************************************************/
/*        Dump(1)  260: Samples [20 of 250]=                       */
/*           Dump(1)    0:  1  2  3  4  5  6  7  8  9 10           */
/*           Dump(1)   40: 11 12 13 14 15 16 17 18 19 20           */
/*****************************************************************/
/*        Dump(1)  xxx thing2d [2 of 2][3 of 3]=                   */
/*           Dump(1)    0: 11 12 13                                */
/*           Dump(1)   12: 21 22 33                                */
/*****************************************************************/
/*        Dump(1)  xxx char1d [8 of 8]= "abcdefgh"                 */
/*****************************************************************/
/*        Dump(1)  xxx char2d [2 of 2][8 of 8]=                    */
/*        Dump(1)       0: "abcdefgh"                              */
/*        Dump(1)       8: "ABCDEFGH"                              */
/*****************************************************************/
/*        Dump(1)  xxx TupleABC struct=                            */
/*           Dump(1)    0: A= 111                                  */
/*           Dump(1)    2: B= 222                                  */
/*           Dump(1)    4: C= 333                                  */
/*****************************************************************/
/*        Dump(1)  yyy TupleXYZ struct [2 of 2]=                   */
/*           Dump(1)    0: X= 111                                  */
/*           Dump(1)    4: Y= 222                                  */
/*           Dump(1)    8: Z= 333                                  */
/*           Dump(1)   12: X= 444                                  */
/*           Dump(1)   16: Y= 555                                  */
/*           Dump(1)   20: Z= 666                                  */
/*****************************************************************/
/*        Dump(1)  yyy TupleXYZ2d struct [1 of 1][1 of 1]=         */
/*           Dump(1)    0: X= 111                                  */
/*           Dump(1)    4: Y= 222                                  */
/*           Dump(1)    8: Z= 333                                  */
/*****************************************************************/

void ddsDumpDataBuf(
   DDS_BIN *bin,
   const char *mode,
   DDS_SYM *sym,
   const char *buf)
{
   DDS_TYPE *type;
   char *buf1, *buf2;
   const char *name;
   int rc, indent;
   size_t offset;

   if(! dds_initialized) return; /* no-op */

   if(! sym) {
      fprintf(dds_print_fp, "ddsDumpDataBuf: NULL SYM ptr\n");
      return;
   }

   if(bin->trace.type == (DDS_TYPE*)sym) {
      /* dump type is the default "trace" */
      dds_state |= DDS_DUMP_DEFAULT;
   } else if(dds_state & DDS_DUMP_DIFF) {
      /* dump type is non-default (circumvent "diff") */
      dds_state &= (DDS_STATE) ~ DDS_DUMP_DEFAULT;
   } else {
      /* DDS_DUMP_LIST on non-default type ? */
      return;
   }

   if(dds_state & DDS_DUMP_DIFF) {
      fprintf(dds_print_fp, "DDS:Dump(0) %s bin= %i, data= %s\n",
         mode, (int)bin->tag, bin->name);
      fprintf(dds_print_fp, "DDS:Dump(0) bias= %lu, ReelTrc= %lu\n",
         (unsigned long)bin->media->first_trace,
         (unsigned long)bin->ReelTrc);

      buf1 = bin->diff1_buf;
      buf2 = bin->diff2_buf;
      indent = 0;
      offset = 0;
      name = sym->name;
      dds_dump_que.top = 0;

      if(sym->flag & DDS_SYM_TYPE) {
         type = (DDS_TYPE*)sym;
      } else {
         assert(sym->type);
         type = sym->type;
      }

      if(! name) {
         if(type->name) {
            name = type->name;
         } else if(type->trail && type->trail->name) {
            name = type->trail->name;
         } else {
            name = "<no_name>";
         }
      }

      rc = ddsDumpDataDiff(indent, type, offset,
         buf, buf1, buf2);

      ddsStackChar(&dds_dump_que, name, strlen(name));

      ddsDumpDataType(indent + DDS_DENT, type, offset,
         buf, bin->diff1_buf, bin->diff2_buf);

      if(rc == DDS_OK && dds_dump_que.top) {
         fwrite(dds_dump_que.ptr, 1, dds_dump_que.top, dds_print_fp);
      }

      if(dds_state & DDS_DUMP_DEFAULT) {
         bin->diff2_buf = buf1;
         bin->diff1_buf = buf2;
         memcpy(bin->diff1_buf, buf,  type->stroke);
      }
   } else if(dds_state & DDS_DUMP_LIST) {
      if(sym->flag & DDS_SYM_TYPE) {
         type = (DDS_TYPE*)sym;
      } else {
         assert(sym->type);
         type = sym->type;
      }
      ddsDumpDataList(bin, mode, type, buf);
   } else {
      ddsPanic("ddsDumpDataBuf: state flags bad\n");
   }

   return;
}
