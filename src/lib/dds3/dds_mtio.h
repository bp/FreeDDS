#ifndef __DDS_MTIO_H
#define __DDS_MTIO_H

/*
RCS-t=DDS, include file for mag tape functions.

$Id: dds_mtio.h 39 2009-01-13 23:00:15Z user $
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/
 
#include "dds_sys.h"
#include "dds_magtape.h"

#if defined(__sun__) && !defined(__SVR4) && defined(__GNUC__)
#include <sys/ioccom.h>
#endif
#if defined(_WIN32) || defined(MINGW)
#include <conio.h>
size_t MTIOCGET;
#elif defined(Darwin)
size_t MTIOCGET;
#else
#include <sys/ioctl.h>
#include <sys/mtio.h>
#endif 
#endif /* __DDS_MTIO_H */
