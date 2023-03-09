#ifndef __SEGY_HDR_H
#define __SEGY_HDR_H

/*
RCS-t=DDS, "segy line header" description

$Id: segy_hdr.h 16 2009-01-06 21:52:47Z user $
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

typedef enum SEGY_MISC
{  /* miscellaneous constants, non-preprocessor and debugable */
   SEGY_TRACE_DATA = 1,
   SEGY_TRACE_DEAD = 2,
   SEGY_TRACE_DUMMY = 3,
   SEGY_TRACE_TIME_BREAK = 4,
   SEGY_TRACE_UPHOLE = 5,
   SEGY_TRACE_SWEEP = 6,
   SEGY_TRACE_TIMING = 7,
   SEGY_TRACE_WATER_BREAK = 8,
   SEGY_GREEN_WORD_MAX = 8,
   SEGY_UNITSFLAG_other = 0,
   SEGY_UNITSFLAG_METERS = 1,
   SEGY_UNITSFLAG_FEET = 2,
   SEGY_FMTCODE_FLOAT4IBM  = 1,
   SEGY_FMTCODE_INTEGER4   = 2,
   SEGY_FMTCODE_INTEGER2   = 3,
   SEGY_FMTCODE_INTEGER2a  = 4,
   SEGY_FMTCODE_FLOAT4     = 5,
   SEGY_FMTCODE_INTEGER1   = 8,
   SEGY_CARD_MIN = 3200,
   SEGY_CARD_MAX = 3200,
   SEGY_BINARY_MIN = 400,
   SEGY_BINARY_MAX = 400,
   SEGY_TRACE_MIN = 240,
   SEGY_TRACE_MAX = 32758
} SEGY_MISC;
 
typedef struct SEGY_LINE
{  /* SEGY "line" (card and binary) header values */
   char		card3200[40][80];
   int		JobNum;
   int		LineNum;
   int		ReelNum;
   int		NumTrc;
   int		NumAux;
   int		SmpInt;
   int		OrigSmpInt;
   int		NumSmp;
   int		OrigNumSmp;
   int		FmtCode;
   int		CdpFold;
   int		SortCode;
   int		VertSumFlag;
   int		SwpFreqStart;
   int		SwpFreqEnd;
   int		SwpLength;
   int		SwpCode;
   int		SwpChan;
   int		SwpTprStart;
   int		SwpTprEnd;
   int		SwpTprType;
   int		CorrFlag;
   int		GainFlag;
   int		AmpFlag;
   int		UnitsFlag;
   int		PolarityFlag;
   int		VibLagFlag;
   int		SpareBinI2[20];
   int      SegyFmt;
   int      FixedLength;
   int      NumHdr;
   int      Unassigned;
   int		Spare2BinI2[146];
} SEGY_LINE;

typedef struct SEGY_EXT
{  /* SEGY "Extended" (card) header values */
   char		card3200[40][80];
} SEGY_EXT;

#endif /* __SEGY_HDR_H */
