#ifndef __DDS_MAGTAPE_H
#define __DDS_MAGTAPE_H

/*
RCS-t=DDS, mag tape "utility" functions.

$Id: dds_magtape.h 16 2009-01-06 21:52:47Z user $
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/
 
/***********************/
/* function prototypes */
/***********************/

int ddsMTBSF(
   int fd,
   int count)
;
DDS_BOOLEAN ddsMTEOF(
   int fd)
;
DDS_BOOLEAN ddsMTEOM(
   int fd)
;
int ddsMTFSF(
   int fd,
   int count)
;
int ddsMTLOAD(
   int fd)
;
int ddsMTUNLOAD(
   int fd)
;
int ddsMTREW(
   int fd)
;
void ddsMTStatus(
   int fd,
   const char *msg)
;
int ddsMTWEOF(
   int fd,
   int count)
;


const char * ddsDeviceCode(
   int fd)
;
 
 
#endif /* __DDS_MAGTAPE_H */
