#ifndef __DDS_MESSAGE_H
#define __DDS_MESSAGE_H

/*
RCS-t=DDS, print "utility" functions

$Id: dds_message.h 16 2009-01-06 21:52:47Z user $
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

/*****************************************/
/* extern global variables and constants */
/*****************************************/

extern int dds_error;                /* last non-zero errno */
 
extern int dds_warn_count;           /* warning counter */
extern int dds_error_count;          /* error counter */
extern int dds_debug_count;          /* debug counter */
 
extern DDS_STRING dds_warn_msg;      /* last warning message */
extern DDS_STRING dds_error_msg;     /* last error   message */
extern DDS_STRING dds_debug_msg;     /* last debug   message */
 
extern FILE* dds_print_fp;           /* print C file pointer */
extern int dds_print_lu;             /* print fortran logical unit */

/***********************/
/* function prototypes */
/***********************/

const char * ddsHint(
   const char *text)
;
const char * ddsHint2(
   const char *text)
;
const char * ddsHint3(
   const char *text)
;
const char * ddsHint4(
   const char *text)
;
const char * ddsHintN(
   const char *text,
   size_t size)
;
const char * ddsHintN2(
   const char *text,
   size_t size)
;


int ddsPanic(
   const char *fmt,
   ...)
;
void ddsAbort(void)
;
int ddsDebug(
   const char *fmt,
   ...)
;
int ddsError(
   const char *fmt,
   ...)
;
int ddsWarn(
   const char *fmt,
   ...)
;
int ddsVerbose(
   const char *fmt,
   ...)
;
int ddsOperator(
   const char *fmt,
   ...)
;


int dds_yyerror(
   const char *msg)
;
int ddsYYError(
   const char *fmt,
   ...)
;
int ddsYYWarn(
   const char *fmt,
   ...)
;


void ddsPrintStr(
   const char *msg)
;
char* ddsVsmprintf(
   const char *fmt, 
   va_list args)
;

#endif /* ifndef __DDS_MESSAGE_H */

