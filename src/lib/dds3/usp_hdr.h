#ifndef __USP_HDR_H
#define __USP_HDR_H

/*
RCS-t=DDS, "usp line header" description

$Id: usp_hdr.h 39 2009-01-13 23:00:15Z user $
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#define DEF_Dx1000 TmMsSl
#define DEF_Dy1000 DtInFl
#define DEF_Dz1000 TmSlIn


typedef enum USP_MISC
{  /* miscellaneous constants, non-preprocessor and debugable */
   USP_GREEN_WORD_MAX = 8,
   USP_TRACE_DEAD = 30000,
   USP_USEC_LIMIT = 32,     /* SmpInt "msec" to "usec" threshold */
   USP_UnitFl_FEET = 0,
   USP_UnitFl_METERS = 1,
   USP_UnitFl_other = 2,
   USP_LINE_IBM_MIN = 1004,
   USP_LINE_IBM_MAX = 32758,
   USP_TRACE_IBM_MIN = 256,
   USP_LINE_IEEE_MIN = 1004,
   USP_LINE_IEEE_MAX = 32758,
   USP_TRACE_IEEE_MIN = 256,
} USP_MISC;

typedef struct USP_LINE
{  /* USP "line" header values */
   char		EqpCod[1];
   char		CrwNam[6];
   char		DatTyp[1];
   char		DgTrkS[2];
   char		PrcNam[10];
   char		PrcDat[8];
   char		OACLin[8];
   char		JobNum[8];
   int		NumTrc;
   int		NumRec;
   int		SmpInt;
   int		NumSmp;
   int		Format;
   int		IndAdj;
   int		OpGrFl;
   char		SrtTyp[2];
   int		CDPFld;
   char		GrpInt[4];
   int		DtInFl;
   int		MxSPEl;
   int		MnSPEl;
   int		MxRSEL;
   int		MnRSEL;
   int		MxGrEl;
   int		MnGrEl;
   int		MxTrSt;
   int		MnTrSt;
   char		LinDir[4];
   int		AERcPr;
   int		OrNTRC;
   int		OrNREC;
   float	MutVel;
   int		ZDMuTm;
   int		IntInc;
   int		WatVel;
   int		MxShDp;
   int		MnShDp;
   int		MxUHTm;
   int		MnUHTm;
   int		MxTrOf;
   int		MnTrOf;
   int		RATTrc;
   int		RATFld;
   int		UnitFl;
   int		FrstSP;
   int		DpN1SP;
   int		NmDpIn;
   int		StWdFl;
   int		DptInt;
   int		TmMsSl;
   int		TmSlIn;
   float	TmMsFS;
   int		NTrLnS;
   int		PltDir;
   int		LgRfEl;
   int		MnLnIn;
   int		MxLnIn;
   int		MnDpIn;
   int		MxDpIn;
   float	ILClIn;
   float	CLClIn;
   int		ReSpFm;
   int		VlFnRF;
   char		OACUsr[4];
   char		LinNam[8];
   int		MutFlg;
   char		SPBias[2];
   int		SpBiFl;
   char		APIWNo[12];
   float	NmSpMi;
   int		VelFun[200];
   int		HlhEnt;
   int		HlhByt;
/* ZERO COUNT, HlhBuf */
/* ZERO COUNT, PadLine */
} USP_LINE;

/*** unions in the line header
   int          Dy1000;
   int          RefVel;
   int          AnglSt;
   int          AngInc;
   int          Dx1000;
   int          Dz1000;

   int          MinVel;
   int          MaxVel;

   int          Nx_Pad;
   int          Nx_Win;
   int          Nx_Rec;
   int          NxLine;
   int          Nx_Trc;
   int          Ny_Pad;
   int          Ny_Win;
   int          Ny_Rec;
   int          NyLine;
   int          Ny_Trc;
   int          MCTrSp;
   int          NumCmp;
   int          MCLE01;

   int          OrNSMP;
   float        AziIln;
   float        AziXln;
   float        HrzNul;
   float        UnitSc;
   float        SmpFlt;
   float        MaxAmp;
   float        AvgAmp;
   float        RmsAmp;
***/

#endif /* __USP_HDR_H */
