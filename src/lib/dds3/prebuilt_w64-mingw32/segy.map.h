"segy.map:= $Id: segy.map.dds 228 2011-05-05 23:13:45Z user $\n"
"RCS-t=DDS, segy.map.dds\n"
"\n"
"map:in_fmt:out_fmt.out_field= expression\n"
"\n"
"map:*:segy.TrcIdCode=   1\n"
"map:*:segy.ElevScaler=  1\n"
"map:*:segy.XYScaler=    1\n"
"\n"
"map:segy:segy.TrcIdCode=  TrcIdCode\n"
"map:segy:segy.ElevScaler= ElevScaler\n"
"map:segy:segy.XYScaler=   XYScaler\n"
"\n"
"comment= USP to SEGY trace header map.\n"
"   Derived  04/30/93 by Randy Selzler\n"
"   Modified 08/27/01 by Jerry Ehlers for segy Rev 1.0\n"
"   Modified 09/09/04 by Jerry Ehlers for su\n"
"   Modified 11/18/04 by Jerry Ehlers put segy1 as separate format\n"
"   Modified 11/01/06 by Jerry Ehlers\n"
"\n"
"map:usp:segy.Date=        datestruct(SGRDat)\n"
"map:usp:segy.MuteEnd=     SGRNum\n"
"map:usp:segy.DateType=    SGRAmp\n"
"map:usp:segy.SrcStatic=   InStAp\n"
"map:usp:segy.GrpStatic=   RcStAp\n"
"map:usp:segy.SrcX=        SrPtXC\n"
"map:usp:segy.SrcY=        SrPtYC\n"
"map:usp:segy.GrpX=        RcPtXC\n"
"map:usp:segy.GrpY=        RcPtYC\n"
"map:usp:segy.SrcDepth=    ShtDep\n"
"map:usp:segy.UpholeTmSrc= UphlTm\n"
"map:usp:segy.HorzStack=   FoldNm\n"
"map:usp:segy.CdpTrcNum=   TrcNum\n"
"map:usp:segy.FieldRecNum= PrRcNm\n"
"map:usp:segy.FieldTrcNum= PrTrNm\n"
"map:usp:segy.SrcElev=     SrPtEl\n"
"map:usp:segy.GrpElev=     GrpElv\n"
"map:usp:segy.CdpNum=      DphInd\n"
"map:usp:segy.GrpDatum=    RfSrEl\n"
"map:usp:segy.TotalStatic= StaCor\n"
"map:usp:segy.TrcIdCode=   (StaCor - 30000) ? 1 : 2\n"
"\n"
"map:usp:su.Date=          datestruct(SGRDat)\n"
"map:usp:su.MuteEnd=       SGRNum\n"
"map:usp:su.DateType=      SGRAmp\n"
"map:usp:su.SrcStatic=     InStAp\n"
"map:usp:su.GrpStatic=     RcStAp\n"
"map:usp:su.SrcX=          SrPtXC\n"
"map:usp:su.SrcY=          SrPtYC\n"
"map:usp:su.GrpX=          RcPtXC\n"
"map:usp:su.GrpY=          RcPtYC\n"
"map:usp:su.SrcDepth=      ShtDep\n"
"map:usp:su.UpholeTmSrc=   UphlTm\n"
"map:usp:su.HorzStack=     FoldNm\n"
"map:usp:su.CdpTrcNum=     TrcNum\n"
"map:usp:su.FieldRecNum=   PrRcNm\n"
"map:usp:su.FieldTrcNum=   PrTrNm\n"
"map:usp:su.SrcElev=       SrPtEl\n"
"map:usp:su.GrpElev=       GrpElv\n"
"map:usp:su.CdpNum=        DphInd\n"
"map:usp:su.GrpDatum=      RfSrEl\n"
"map:usp:su.TotalStatic=   StaCor\n"
"map:usp:su.TrcIdCode=     (StaCor - 30000) ? 1 : 2\n"
"\n"
"map:asp:segy.Date=        datestruct(field(SGRDat,0))\n"
"map:asp:segy.MuteEnd=     field(SGRNum, nop)\n"
"map:asp:segy.DateType=    field(SGRAmp, nop)\n"
"map:asp:segy.SrcStatic=   field(InStAp, nop)\n"
"map:asp:segy.GrpStatic=   field(RcStAp, nop)\n"
"map:asp:segy.SrcX=        field(SrPtXC, nop)\n"
"map:asp:segy.SrcY=        field(SrPtYC, nop)\n"
"map:asp:segy.GrpX=        field(RcPtXC, nop)\n"
"map:asp:segy.GrpY=        field(RcPtYC, nop)\n"
"map:asp:segy.SrcDepth=    field(ShtDep, nop)\n"
"map:asp:segy.UpholeTmSrc= field(UphlTm, nop)\n"
"map:asp:segy.HorzStack=   field(FoldNm, nop)\n"
"map:asp:segy.CdpTrcNum=   field(TrcNum, nop)\n"
"map:asp:segy.FieldRecNum= field(PrRcNm, nop)\n"
"map:asp:segy.FieldTrcNum= field(PrTrNm, nop)\n"
"map:asp:segy.SrcElev=     field(SrPtEl, nop)\n"
"map:asp:segy.GrpElev=     field(GrpElv, nop)\n"
"map:asp:segy.CdpNum=      field(DphInd, nop)\n"
"map:asp:segy.GrpDatum=    field(RfSrEl, nop)\n"
"map:asp:segy.TotalStatic= field(StaCor, nop)\n"
"map:asp:segy.TrcIdCode=   (field(StaCor, 0) - 30000) ? field(TrcIdCode, 1) : 2\n"
"map:asp:segy.ElevScaler=  field(ElevScaler, 1)\n"
"map:asp:segy.XYScaler=    field(XYScaler, 1)\n"
"\n"
"map:segy1:segy.Spare.Hdr.CDPBCX=     CDPBCX\n"
"map:segy1:segy.Spare.Hdr.CDPBCY=     CDPBCY\n"
"map:segy1:segy.Spare.Hdr.LinInd=     LinInd\n"
"map:segy1:segy.Spare.Hdr.SrcLoc=     SrcLoc\n"
"map:segy1:segy.CdpNum=               DphInd\n"
"\n"
"map:asp:su.Date=        datestruct(field(SGRDat,0))\n"
"map:asp:su.MuteEnd=     field(SGRNum, nop)\n"
"map:asp:su.DateType=    field(SGRAmp, nop)\n"
"map:asp:su.SrcStatic=   field(InStAp, nop)\n"
"map:asp:su.GrpStatic=   field(RcStAp, nop)\n"
"map:asp:su.SrcX=        field(SrPtXC, nop)\n"
"map:asp:su.SrcY=        field(SrPtYC, nop)\n"
"map:asp:su.GrpX=        field(RcPtXC, nop)\n"
"map:asp:su.GrpY=        field(RcPtYC, nop)\n"
"map:asp:su.SrcDepth=    field(ShtDep, nop)\n"
"map:asp:su.UpholeTmSrc= field(UphlTm, nop)\n"
"map:asp:su.HorzStack=   field(FoldNm, nop)\n"
"map:asp:su.CdpTrcNum=   field(TrcNum, nop)\n"
"map:asp:su.FieldRecNum= field(PrRcNm, nop)\n"
"map:asp:su.FieldTrcNum= field(PrTrNm, nop)\n"
"map:asp:su.SrcElev=     field(SrPtEl, nop)\n"
"map:asp:su.GrpElev=     field(GrpElv, nop)\n"
"map:asp:su.CdpNum=      field(DphInd, nop)\n"
"map:asp:su.GrpDatum=    field(RfSrEl, nop)\n"
"map:asp:su.TotalStatic= field(StaCor, nop)\n"
"map:asp:su.TrcIdCode=   (field(StaCor, 0) - 30000) ? field(TrcIdCode, 1) : 2\n"
"map:asp:su.ElevScaler=  field(ElevScaler, 1)\n"
"map:asp:su.XYScaler=    field(XYScaler, 1)\n"
"\n"
"comment= segy to segy SPARE trace header mapping is tricky.\n"
"   Care must be taken to preserve the fidelity of all fields\n"
"   even though their type (integer, float, etc) is unknown.\n"
"   The default mapping should never apply float arithmetic\n"
"   to fields which may actually contain integer data.\n"
"   If we assume that simple integer assignments will preserve\n"
"   all data types, then a solution is possible...\n"
"   The following defaults disables all spare header mapping,\n"
"   except for one simple vector of integers (SpareI4).\n"
"   This is applied prior to any explicit mapping of spare\n"
"   header fields that may be provided by the end user.\n"
"\n"
"#write:segy.LineTrcNum= linetrc()\n"
"#write:segy.ReelTrcNum= reeltrc()\n"
"\n"
"#write:segy.card_green_prefix= sizeof(SEGY_CARD)\n"
"   - (field(GREEN_SUFFIX) ? 2 : 1) * sizeof(SEGY_INT)\n"
"#write:segy.card_green_suffix= sizeof(SEGY_CARD)\n"
"   - (field(GREEN_SUFFIX) ? 2 : 1) * sizeof(SEGY_INT)\n"
"\n"
"#write:segy.binary_green_prefix= sizeof(SEGY_BINARY)\n"
"   - (field(GREEN_SUFFIX) ? 2 : 1) * sizeof(SEGY_INT)\n"
"#write:segy.binary_green_suffix= sizeof(SEGY_BINARY)\n"
"   - (field(GREEN_SUFFIX) ? 2 : 1) * sizeof(SEGY_INT)\n"
"\n"
"#write:segy.green_prefix= sizeof(SEGY_TRACE)\n"
"   - (field(GREEN_SUFFIX) ? 2 : 1) * sizeof(SEGY_INT)\n"
"#write:segy.green_suffix= sizeof(SEGY_TRACE)\n"
"   - (field(GREEN_SUFFIX) ? 2 : 1) * sizeof(SEGY_INT)\n"
"\n"
"";
