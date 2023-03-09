#ifndef __SEP_HDR_H
#define __SEP_HDR_H

/*
RCS-t=DDS, "sep line header" description

$Id: sep_hdr.h 16 2009-01-06 21:52:47Z user $
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/


typedef struct SEP_LINE
{  /* sep "line" header values */
   int   n1, n2, n3;		/* dimension sizes */
   float o1, o2, o3;		/* origins */
   float d1, d2, d3;		/* deltas */
   char label1[AXISNAME_MAX+1],	/* labels */
        label2[AXISNAME_MAX+1],
        label3[AXISNAME_MAX+1];
   int esize;			/* bytes per element */
} SEP_LINE;

#endif /* __SEP_HDR_H */
