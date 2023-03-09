#ifndef __DDS_HDR_H
#define __DDS_HDR_H
 
/*
RCS-t=DDS, line header "utility" declarations
 
$Id: dds_hdr.h 16 2009-01-06 21:52:47Z user $
*/
 
/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/
 
/***************************************************/
/* Macros to DEFault header value from dictionary. */
/*                                                 */
/* These macros are called by ddsOpenNew*.         */
/* The following setup is required before usage:   */
/*    cdds_dict(bin->dict_open, "scan");           */
/* If the dictionary defines a particular header,  */
/* then its value is retrieved and saved in the    */
/* corresponding dict_hdr and bin_hdr members.     */
/*    Example: if the format is usp and            */
/*       "usp_MxLnIn= 2000" is defined then        */
/*       dict_hdr.MxLnIn = bin_hdr.MxLnIn = 2000   */
/* If not defined, the members are defaulted       */
/* (zero if numeric, blank if character).          */
/***************************************************/
 
#define HDR_DEF_CHAR(PREFIX, NAME, COUNT) \
   ddsHdrDefChar((char*)&dict_hdr. NAME, (char*)&bin_hdr. NAME, \
      #PREFIX #NAME, (COUNT), (sizeof(dict_hdr. NAME) / (COUNT)));
 
#define HDR_DEF_INT(PREFIX, NAME) \
   ddsHdrDefInt((int*)&dict_hdr. NAME, (int*)&bin_hdr. NAME, \
      #PREFIX #NAME, sizeof(dict_hdr. NAME) / sizeof(int));
 
#define HDR_DEF_FLOAT(PREFIX, NAME) \
   ddsHdrDefFloat((float*)&dict_hdr. NAME, (float*)&bin_hdr. NAME, \
      #PREFIX #NAME, sizeof(dict_hdr. NAME) / sizeof(float));
 
/*******************************************************/
/* Macros to GET header value from dictionary or       */
/*    line header binary buffer.                       */
/*                                                     */
/* These macros are called by ddsOpenOld*.             */
/* The following (usp) setup is required before usage: */
/*    cdds_dict(bin->dict_open, "scan");               */
/*    hdr_buf = line_buf;                              */
/*    type = bin->std.usp.line.type;                   */
/*    hdr_block = type->block;                         */
/* If the dictionary defines a particular header,      */
/* then its value is retrieved and saved in the        */
/* corresponding dict_hdr and bin_hdr members.         */
/*    Example: if the format is usp and                */
/*       "usp_MxLnIn= 2000" is defined then            */
/*       dict_hdr.MxLnIn = bin_hdr.MxLnIn = 2000       */
/* If not defined, the dict_hdr member is defaulted    */
/* (zero if numeric, blank if character).              */
/* The bin_hdr value is taken from the binary header   */
/* referenced by hdr_buf.                              */
/*                                                     */
/* API tags are created for each member and the offset */
/* to each member in the buffer is returned in "bias". */
/*******************************************************/
 
#define HDR_GET_CHAR(PREFIX, AFFIX, NAME) \
   ddsHdrGetChar((char*)&dict_hdr. NAME, (char*)&bin_hdr. NAME, \
      AFFIX #NAME, #PREFIX #NAME, hdr_buf, hdr_block);
 
#define HDR_GET_INT(PREFIX, AFFIX, NAME) \
   ddsHdrGetInt((int*)&dict_hdr. NAME, (int*)&bin_hdr. NAME, \
      AFFIX #NAME, #PREFIX #NAME, hdr_buf, hdr_block);
 
#define HDR_GET_FLOAT(PREFIX, AFFIX, NAME) \
   ddsHdrGetFloat((float*)&dict_hdr. NAME, (float*)&bin_hdr. NAME, \
      AFFIX #NAME, #PREFIX #NAME, hdr_buf, hdr_block);
 
/***********************************************************/
/* Macros to MODify header value within dictionary.        */
/*                                                         */
/* These macros are called by ddsOpenOld* and ddsOpenNew*. */
/* The following (usp) setup is required before usage:     */
/*    cdds_dict(bin->dict_open, "print");                  */
/* If the values in dict_hdr and bin_hdr are different,    */
/* then the dictionary is updated using the bin_hdr value. */
/*    Example: if the format is usp and                    */
/*       dict_hdr.MxLnIn is not equal to bin_hdr.MxLnIn    */
/*       then "usp_MxLnIn= N" definition is appended,      */
/*       where "N" is provided by bin_hdr.MxLnIn.          */
/* If the values are the same, nothing is changed.         */
/* This brings the dictionary up to date with the current  */
/* values in the binary buffer.                            */
/***********************************************************/
 
#define HDR_MOD_CHAR(PREFIX, NAME, COUNT) \
   ddsHdrModChar((const char*)&dict_hdr. NAME, (const char*)&bin_hdr. NAME, \
      #PREFIX #NAME, (COUNT), (sizeof(dict_hdr. NAME) / (COUNT)));
 
#define HDR_MOD_INT(PREFIX, NAME) \
   ddsHdrModInt((const int*)&dict_hdr. NAME, (const int*)&bin_hdr. NAME, \
      #PREFIX #NAME, sizeof(dict_hdr. NAME) / sizeof(int));
 
#define HDR_MOD_FLOAT(PREFIX, NAME) \
   ddsHdrModFloat((const float*)&dict_hdr. NAME, (const float*)&bin_hdr. NAME, \
      #PREFIX #NAME, sizeof(dict_hdr. NAME) / sizeof(float));
 
/*******************************************************/
/* Macros to PUT header value into binary buffer.      */
/*                                                     */
/* These macros are called by ddsOpenNew*.             */
/* The following (usp) setup is required before usage: */
/*    hdr_buf = bin->std.usp.line_buf;                 */
/*    type = bin->std.usp.line.type;                   */
/*    hdr_block = type->block;                         */
/* The bin_hdr value is put into the binary header     */
/* referenced by hdr_buf.                              */
/*    Example: if the format is usp,                   */
/*       the bin_hdr.MxLnIn value is put into hdr_buf. */
/*                                                     */
/* API tags are created for each member and the offset */
/* to each member in the buffer is returned in "bias". */
/*******************************************************/
 
#define HDR_PUT_CHAR(AFFIX, NAME) \
   ddsHdrPutChar((char*)&bin_hdr. NAME, \
      AFFIX #NAME, hdr_buf, hdr_block);
 
#define HDR_PUT_INT(AFFIX, NAME) \
   ddsHdrPutInt((int*)&bin_hdr. NAME, \
      AFFIX #NAME, hdr_buf, hdr_block);
 
#define HDR_PUT_FLOAT(AFFIX, NAME) \
   ddsHdrPutFloat((float*)&bin_hdr. NAME, \
      AFFIX #NAME, hdr_buf, hdr_block);
 
/***********************/
/* function prototypes */
/***********************/


void ddsHdrDefChar(
   char *dict_hdr,
   char *bin_hdr,
   const char *defn,
   size_t count,
   size_t prec)
;
void ddsHdrDefFloat(
   float *dict_hdr,
   float *bin_hdr,
   const char *defn,
   size_t count)
;
void ddsHdrDefInt(
   int *dict_hdr,
   int *bin_hdr,
   const char *defn,
   size_t count)
;
 
 
void ddsHdrGetChar(
   char *dict_hdr,
   char *bin_hdr,
   const char *name,
   const char *defn,
   const char *buf,
   struct DDS_BLOCK *block)
;
void ddsHdrGetFloat(
   float *dict_hdr,
   float *bin_hdr,
   const char *name,
   const char *defn,
   const char *buf,
   struct DDS_BLOCK *block)
;
void ddsHdrGetInt(
   int *dict_hdr,
   int *bin_hdr,
   const char *name,
   const char *defn,
   const char *buf,
   struct DDS_BLOCK *block)
;
 
 
void ddsHdrModChar(
   const char *dict_hdr,
   const char *bin_hdr,
   const char *defn,
   size_t count,
   size_t prec)
;
void ddsHdrModFloat(
   const float *dict_hdr,
   const float *bin_hdr,
   const char *defn,
   size_t count)
;
void ddsHdrModInt(
   const int *dict_hdr,
   const int *bin_hdr,
   const char *defn,
   size_t count)
;
 
 
void ddsHdrPutChar(
   const char *bin_hdr,
   const char *name,
   char *buf,
   struct DDS_BLOCK *block)
;
void ddsHdrPutFloat(
   const float *bin_hdr,
   const char *name,
   char *buf,
   struct DDS_BLOCK *block)
;
void ddsHdrPutInt(
   const int *bin_hdr,
   const char *name,
   char *buf,
   struct DDS_BLOCK *block)
;

 
#endif /* __DDS_HDR_H */
