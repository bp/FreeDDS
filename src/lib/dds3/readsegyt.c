static char rcsid[]="$Id: readsegyt.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, read and edit segy trace binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "segy_hdr.h"
#include "dds_dump.h"

int ddsReadSegyT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buffer,
   int count)
{
   DDS_MEDIA *media = bin->media;
   DDS_BOOLEAN dump;
   int n_read, n_bytes, residual;
   int green, prefix, suffix, n_prefix, n_suffix;
   int test_char, TrcNumSmp;
   int n_trace;
   char *buf;
   const char *rbuf, *ebuf;

   errno = 0;

   buf = buffer;
   n_bytes = 0;   /* number of bytes in rbuf */
   residual = 0;

   if((dds_state & DDS_DUMP_DATA) && (dds_state & DDS_DUMP_READ)) {
      dump = DDS_TRUE;
   } else {
      dump = DDS_FALSE;
   }

   if(bin->state & DDS_green_prefix) {
      n_prefix = bin->prefix.type->prec;
      if(bin->state & DDS_green_suffix) {
         n_suffix = bin->suffix.type->prec;
         green = type->stroke - n_prefix - n_suffix;
      } else {
         n_suffix = 0;
         green = type->stroke - n_prefix;
      }
   } else {
      n_prefix = n_suffix = 0;
   }

   for(n_read = 0; n_read < count; n_read++) {

      reject_trace:
      if(! n_bytes) {
         /* attempt to read another trace */
         if(bin->state & DDS_BIN_UNBLOCK) {
            /* Some segy flavors have line headers embed among the traces. */
            /* The segy card header may be longer or shorter than a trace. */
            /* If the tape has variable length records (UNBLOCK) and */
            /* does not support backspace, then read must request the maximum */
            /* possible size to preserve unexpected changes. */
            #ifdef HOST_SGI
               /* SGI hates read size != actual record size. */
               n_bytes = greada(media->fp, (const void**)&rbuf,
                  type->stroke, GIO_CHAR);
            #else
               n_bytes = greada(media->fp, (const void**)&rbuf,
                  bin->block_size, GIO_CHAR);
            #endif
         } else {
            n_bytes = greada(media->fp, (const void**)&rbuf,
               type->stroke, GIO_CHAR);
         }

         if(n_bytes != type->stroke && 
            (n_bytes < bin->std.segy.card_green_min ||
             n_bytes > bin->std.segy.card_green_max)) {
            /* no input trace nor segy card header */
            if(n_bytes < SEGY_TRACE_MIN) {
               if(geof(media->fp)) errno = 0;
               residual = n_bytes;
               break;
            }
         }
      }

      if(n_prefix) {
         /* validate green word prefix */
         CON_ANSS(DDS_CTYPE_INT, bin->prefix.type->ptype,
            &prefix, rbuf + bin->prefix.offset);

         if(! prefix && ! (bin->state & DDS_BIN_UNBLOCK)) {
            /* green word is zero on blocked media, simulated EOF ? */
            ddsWarn("segy trace green word prefix= EOF ?\n\tbinary= %s, ReelTrc= %lu\n",
               bin->name, (unsigned long)bin->ReelTrc);
            gunread(media->fp, rbuf + n_prefix, n_bytes - n_prefix);
            n_bytes = 0;
            goto reject_trace;
         } else if(n_suffix) {
            /* validate green word suffix */
            CON_ANSS(DDS_CTYPE_INT, bin->suffix.type->ptype,
               &suffix, rbuf + bin->suffix.offset);

            if(green != prefix || green != suffix) {
               ddsWarn("segy trace green word prefix= %i, suffix= %i\n\tbinary= %s, ReelTrc= %lu\n",
                  prefix, suffix,
                  bin->name, (unsigned long)bin->ReelTrc);
            }
         } else if(green != prefix) {
            ddsWarn("segy trace green word prefix= %i\n\tbinary= %s, ReelTrc= %lu\n",
               prefix, bin->name, (unsigned long)bin->ReelTrc);
         }
      }

      if(bin->std.segy.flags & DDS_SEGY_charisma) {
         test_char = rbuf[n_prefix];

         if(test_char == DDS_ASCII_C ||
            test_char == DDS_ASCII_c ||
            test_char == DDS_ASCII_BLANK ||
            test_char == DDS_EBCDIC_C ||
            test_char == DDS_EBCDIC_c ||
            test_char == DDS_EBCDIC_BLANK) {

            ddsVerbose("Schlumberger Charisma segy line header ?\n\tbinary= %s, ReelTrc= %lu\n",
                  bin->name, (unsigned long)bin->ReelTrc);

            gunread(media->fp, rbuf, n_bytes);

            if(1 != cdds_read(bin->tag, bin->std.segy.card.object->tag,
               bin->std.segy.card_buf, 1)) {
               count = n_read;
               break;
            }

                 /* validate card header ? */

            if(1 != cdds_read(bin->tag, bin->std.segy.binary.object->tag,
               bin->std.segy.binary_buf, 1)) {
               count = n_read;
               break;
            }

            /* validate binary header ? */
  

            n_bytes = 0;
            goto reject_trace;
         }
      }

      if(n_bytes != type->stroke) {
         if(!(bin->std.segy.flags & DDS_SEGY_NumSmp) ||
            n_bytes < SEGY_TRACE_MIN) {
            /* no input trace */
            residual = n_bytes;
            break;
         } else if(bin->state & DDS_BIN_UNBLOCK && bin->FileTrc <= 1) {
            CON_ANSS(DDS_CTYPE_INT, bin->std.segy.TrcNumSmp.type->ptype,
               &TrcNumSmp,  rbuf + bin->std.segy.TrcNumSmp.offset);
            n_trace = type->stroke + bin->std.segy.sample_prec
               *(TrcNumSmp - bin->cube.axis[1].size);
            if(n_bytes != n_trace &&
               ((n_bytes >= bin->std.segy.card_green_min &&
                 n_bytes <= bin->std.segy.card_green_max) ||
                (n_bytes >= bin->std.segy.binary_green_min &&
                 n_bytes <= bin->std.segy.binary_green_max))) {
               /* no input trace */
               residual = n_bytes;
               break;
            }
         }
      }

      /* Edit the input trace in rbuf. */
      /* This "(void*)" violates "const". */
      ebuf = ddsEditTrace(bin, type, (void*)rbuf, DDS_BIN_MSG_READ);

      if(ebuf == rbuf) {
         /* trace is accepted by the API */
         if (n_bytes <= type->stroke) {
            memcpy(buf, ebuf, n_bytes);
            if (n_bytes < type->stroke) {
               memset(buf+n_bytes, 0, (size_t)(type->stroke - n_bytes));
            }
         } else {
            memcpy(buf, ebuf, type->stroke);
         }

         /* process TrcNumSmp field */
         CON_ANSS(DDS_CTYPE_INT, bin->std.segy.TrcNumSmp.type->ptype,
            &TrcNumSmp,  rbuf + bin->std.segy.TrcNumSmp.offset);
         n_trace = type->stroke + bin->std.segy.sample_prec
           *(TrcNumSmp - bin->cube.axis[1].size);

         /* check for variable length trace */
         if(bin->std.segy.flags & DDS_SEGY_NumSmp) {
            if((bin->state & DDS_BIN_UNBLOCK)) {
               if (n_trace != n_bytes) {
                  ddsWarn("segy n_bytes= %i, expected %i\n\tbinary= %s, ReelTrc= %lu\n",
                    n_bytes, n_trace,
                    bin->name, (unsigned long)bin->ReelTrc);
               }
            } else if (n_trace > n_bytes) {
               greada(media->fp, (const void**)&rbuf, n_trace-n_bytes, 
                      GIO_CHAR);
            } else if (n_trace < n_bytes) {
               gunread(media->fp, rbuf+n_trace, n_bytes-n_trace);
            }
         } else if(TrcNumSmp != TrcNumSmp && bin->cube.axis[1].size) {
            ddsWarn("segy TrcNumSmp= %i, expected %li\n\tbinary= %s, ReelTrc= %lu\n",
                    TrcNumSmp, (long)bin->cube.axis[1].size,
                    bin->name, (unsigned long)bin->ReelTrc);
         }

         if(dump) ddsDumpDataBuf(bin, "read", (DDS_SYM*)type, rbuf);
         n_bytes = 0;
      } else if(ebuf == bin->dead_trace) {
         /* pass a dead trace to the API */
         memcpy(buf, ebuf, n_bytes);

         /* Counters reflect the media (not API). */
         bin->LineTrc--;
         bin->ReelTrc--;
         bin->FileTrc--;
      } else if(! ebuf) {
         /* Discard trace and read another. */
         /* process TrcNumSmp field */
         CON_ANSS(DDS_CTYPE_INT, bin->std.segy.TrcNumSmp.type->ptype,
            &TrcNumSmp,  rbuf + bin->std.segy.TrcNumSmp.offset);

         /* check for variable length trace */
         if(bin->std.segy.flags & DDS_SEGY_NumSmp) {
            n_trace = type->stroke + bin->std.segy.sample_prec
               *(TrcNumSmp - bin->cube.axis[1].size);
            if (n_trace > n_bytes) {
               greada(media->fp, (const void**)&rbuf, n_trace-n_bytes, 
                       GIO_CHAR);
            } else if (n_trace < n_bytes) {
               gunread(media->fp, rbuf+n_trace, n_bytes-n_trace);
            }
         } else if(TrcNumSmp != TrcNumSmp && bin->cube.axis[1].size) {
            ddsWarn("segy TrcNumSmp= %i, expected %li\n\tbinary= %s, ReelTrc= %lu\n",
                    TrcNumSmp, (long)bin->cube.axis[1].size,
                    bin->name, (unsigned long)bin->ReelTrc);
         }

         if(dump) ddsDumpDataBuf(bin, "read", (DDS_SYM*)type, rbuf);

         /* Counters reflect the media (not API). */
         bin->LineTrc++;
         bin->ReelTrc++;
         bin->FileTrc++;
         n_bytes = 0;
         goto reject_trace;
      } else {
         /* edit error, terminate API */
         count = n_read;
         break;
      }

      buf += type->stroke;
      ddsCubeInc(bin);
   }

   if(n_bytes) {
      /* un-read some input data */
      gunread(media->fp, rbuf, n_bytes);
   }

   return ddsReadCheck(bin, type, buffer, count, n_read, residual);
}
