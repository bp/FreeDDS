static char rcsid[]="$Id: opennewusp.c 211 2011-01-11 14:32:42Z user $";
/*
RCS-t=DDS, open new binary data, usp format

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "usp_hdr.h"

static int hlh_size, HlhByt_prec, usp_line_max;
static DDS_STRING tmp_str = NULL_STRING;
static char *hlh_ptr;
static DDS_PTYPE short_stype;
static USP_LINE bin_hdr;

static int storeHlh(
   const char *history,
   int flag)
{
   /* flag=0: remove leading/trailing space */
   /*     =1: leave 1 extra leading space   */
   /*     =2: leave all leading spaces      */
   int i, j, n, m;
   char *hlh;

   if(!history) return(DDS_TRUE);

   /* remove leading spaces */
   if(flag != 2) {
      while (*history && isspace(*history)) history++;
      if(*history == '"') history++;
      while (*history && isspace(*history)) history++;
   } else {
      if(*history == '"') history++;
   }

   /* remove trailing spaces */
   n = strlen(history);
   while (n>0 && isspace(history[n-1])) n--;
   if(history[n-1] == '"') n--;
   while (n>0 && isspace(history[n-1])) n--;

   if(n <= 0) return(DDS_TRUE);

   /* limit each entry to 80 bytes */
   m = n;
   if(n > 80) n = 80;

   /* limit total line header size such that an old usp application */
   /* declaring a buffer with 15,000 halfwords does not overflow.   */
   /* Joe Wade said to use 19000 byte limit July 21, 2005           */
   if(USP_LINE_IEEE_MIN + hlh_size + HlhByt_prec + n > 19000)
      return(DDS_FALSE);

   /* replace "\=" with "=" */
   hlh = (char*)malloc(n+1);
   for(i=j=0;i<n;i++,j++) {
      if (history[i]=='\\' && history[i+1]=='=') i++;
      hlh[j] = history[i];
   }
   n = j;

   /* add leading space for continuation entry */
   if(flag==1) {
      memcpy(hlh_ptr + HlhByt_prec, " ", 1);
      memcpy(hlh_ptr + HlhByt_prec + 1, hlh, n);
      n++;
   } else {
      memcpy(hlh_ptr + HlhByt_prec, hlh, n);
   }
   if(m > n) {
      memcpy(hlh_ptr + HlhByt_prec + n, "...", 3);
      n = n + 3;
   }

   free(hlh);
      
   CON_NASS(short_stype, DDS_CTYPE_INT, hlh_ptr, &n);

   bin_hdr.HlhEnt++;
   if(HlhByt_prec > 2) n = HlhByt_prec * (1 + (n-1)/HlhByt_prec);
   hlh_ptr  = hlh_ptr  + HlhByt_prec + n;
   hlh_size = hlh_size + HlhByt_prec + n;

   return(DDS_TRUE);
}

int getHlhPars()
{
   const char *defn, *parm, *str;

   defn = cdds_newer("dds_history{ dds_history");
   if(defn) defn = cdds_newer("");
   while(defn) {
      ddsStrCpyCat2(&tmp_str, defn, "=");
      if(!strncmp(defn, "dds_history", 11)) return(DDS_TRUE);
      if(!strcmp(defn, "cmd_args") && cdds_scanv("", &parm) > 0) {
         if(!storeHlh(parm, 1)) break;
      } else if(strncmp(defn, "dds_par", 7) && strcmp(defn, "par")
                 && strcmp(defn, "data_path") && strcmp(defn, "DATA_PATH")
                 && !strstr(defn, "_format") && strcmp(defn, "_mpi_dummy")) { 
         if(cdds_scant("", &parm) > 0) {
#if defined(_WIN32) || defined(MINGW)
            str = strrchr(parm, '\\');
#else
            str = strrchr(parm, '/');
#endif
            if(str) {
               str++;
               ddsStrCat(&tmp_str, str);
            } else {
               ddsStrCat(&tmp_str, parm);
            }
            if(cdds_scant("", &parm) > 0) ddsStrCat(&tmp_str, " ...");
         }
         if(!storeHlh(tmp_str.ptr, 1)) break;
      }
      defn = cdds_newer("");
   }

   return(DDS_FALSE);
}

void ddsOpenNewUsp(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
{
   USP_LINE dict_hdr;
   DDS_PTYPE char_stype, int_stype, float_stype;
   DDS_PTYPE green_stype, samples_stype;
   DDS_TYPE *type;
   DDS_MEMBER *member;
   DDS_BLOCK *block, *hdr_block;
   DDS_MEDIA *media = bin->media;
   DDS_AXIS *axis;
   char *hdr_buf, *line_buf;
   int i, i_value;
   int rank, axis_t, axis_x, axis_y, axis_z;
   const char *usp_units, *cube_units;
   double cube_to_usp, cube_to_sec, delta_t;
   float FakeFloat;
   int usp_line_min, usp_trace_min;
   int green_prec, green_line, green_trace;
   int Format, NumRec, NumRec4;
   float UnitSc;
   double UnitSc_dbl;
   int sample_prec;
   int line_pad, trace_pad;
   const char *defn, *history;
   char tmp[64];

   /* check for any Format override */
   /* (so format can be 199 for compress data) */
   Format = 3;
   cdds_dict("override: par: default:", "scan");
   cdds_scanf("usp_Format", "%d", &Format);

   /***************************************************/
   /* DEFault header values from dictionary.          */
   /*                                                 */
   /* If the dictionary defines a particular header,  */
   /* then its value is retrieved and saved in the    */
   /* corresponding dict_hdr and bin_hdr members.     */
   /* If not defined, the members are defaulted       */
   /* (zero if numeric, blank if character).          */
   /***************************************************/

   /* setup implicit arguments used by HDR_DEF */
   cdds_dict(bin->dict_open, "scan");

   HDR_DEF_CHAR(usp_, EqpCod, 1)
   HDR_DEF_CHAR(usp_, CrwNam, 1)
   HDR_DEF_CHAR(usp_, DatTyp, 1)
   HDR_DEF_CHAR(usp_, DgTrkS, 1)
   HDR_DEF_CHAR(usp_, PrcNam, 1)
   HDR_DEF_CHAR(usp_, PrcDat, 1)
   HDR_DEF_CHAR(usp_, OACLin, 1)
   HDR_DEF_CHAR(usp_, JobNum, 1)
   HDR_DEF_INT(usp_, NumTrc)
   HDR_DEF_INT(usp_, NumRec)
   HDR_DEF_INT(usp_, SmpInt)
   HDR_DEF_INT(usp_, NumSmp)
   HDR_DEF_INT(usp_, Format)
   HDR_DEF_INT(usp_, IndAdj)
   HDR_DEF_INT(usp_, OpGrFl)
   HDR_DEF_CHAR(usp_, SrtTyp, 1)
   HDR_DEF_INT(usp_, CDPFld)
   HDR_DEF_CHAR(usp_, GrpInt, 1)
   HDR_DEF_INT(usp_, DtInFl)
   HDR_DEF_INT(usp_, MxSPEl)
   HDR_DEF_INT(usp_, MnSPEl)
   HDR_DEF_INT(usp_, MxRSEL)
   HDR_DEF_INT(usp_, MnRSEL)
   HDR_DEF_INT(usp_, MxGrEl)
   HDR_DEF_INT(usp_, MnGrEl)
   HDR_DEF_INT(usp_, MxTrSt)
   HDR_DEF_INT(usp_, MnTrSt)
   HDR_DEF_CHAR(usp_, LinDir, 1)
   HDR_DEF_INT(usp_, AERcPr)
   HDR_DEF_INT(usp_, OrNTRC)
   HDR_DEF_INT(usp_, OrNREC)
   HDR_DEF_FLOAT(usp_, MutVel)
   HDR_DEF_INT(usp_, ZDMuTm)
   HDR_DEF_INT(usp_, IntInc)
   HDR_DEF_INT(usp_, WatVel)
   HDR_DEF_INT(usp_, MxShDp)
   HDR_DEF_INT(usp_, MnShDp)
   HDR_DEF_INT(usp_, MxUHTm)
   HDR_DEF_INT(usp_, MnUHTm)
   HDR_DEF_INT(usp_, MxTrOf)
   HDR_DEF_INT(usp_, MnTrOf)
   HDR_DEF_INT(usp_, RATTrc)
   HDR_DEF_INT(usp_, RATFld)
   HDR_DEF_INT(usp_, UnitFl)
   HDR_DEF_INT(usp_, FrstSP)
   HDR_DEF_INT(usp_, DpN1SP)
   HDR_DEF_INT(usp_, NmDpIn)
   HDR_DEF_INT(usp_, StWdFl)
   HDR_DEF_INT(usp_, DptInt)
   HDR_DEF_INT(usp_, TmMsSl)
   HDR_DEF_INT(usp_, TmSlIn)
   HDR_DEF_FLOAT(usp_, TmMsFS)
   HDR_DEF_INT(usp_, NTrLnS)
   HDR_DEF_INT(usp_, PltDir)
   HDR_DEF_INT(usp_, LgRfEl)
   HDR_DEF_INT(usp_, MnLnIn)
   HDR_DEF_INT(usp_, MxLnIn)
   HDR_DEF_INT(usp_, MnDpIn)
   HDR_DEF_INT(usp_, MxDpIn)
   HDR_DEF_FLOAT(usp_, ILClIn)
   HDR_DEF_FLOAT(usp_, CLClIn)
   HDR_DEF_INT(usp_, ReSpFm)
   HDR_DEF_INT(usp_, VlFnRF)
   HDR_DEF_CHAR(usp_, OACUsr, 1)
   HDR_DEF_CHAR(usp_, LinNam, 1)
   HDR_DEF_INT(usp_, MutFlg)
   HDR_DEF_CHAR(usp_, SPBias, 1)
   HDR_DEF_INT(usp_, SpBiFl)
   HDR_DEF_CHAR(usp_, APIWNo, 1)
   HDR_DEF_FLOAT(usp_, NmSpMi)

   HDR_DEF_INT(usp_, VelFun)

   /*******************************************************/
   /* Scan the dictionary for these union values, but     */
   /* ONLY AFTER VelFun is retrieved and ONLY if it comes */
   /* AFTER the VelFin definition in the dictionary.      */
   /*******************************************************/

   if(1 == cdds_scanf("usp_VelFun usp_MinVel", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[0] = i_value;
      dict_hdr.VelFun[0] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_MaxVel", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[1] = i_value;
      dict_hdr.VelFun[1] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Nx_Pad", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[2] = i_value;
      dict_hdr.VelFun[2] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Nx_Win", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[3] = i_value;
      dict_hdr.VelFun[3] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Nx_Rec", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[4] = i_value;
      dict_hdr.VelFun[4] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_NxLine", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[5] = i_value;
      dict_hdr.VelFun[5] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Nx_Trc", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[6] = i_value;
      dict_hdr.VelFun[6] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Ny_Pad", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[7] = i_value;
      dict_hdr.VelFun[7] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Ny_Win", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[8] = i_value;
      dict_hdr.VelFun[8] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Ny_Rec", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[9] = i_value;
      dict_hdr.VelFun[9] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_NyLine", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[10] = i_value;
      dict_hdr.VelFun[10] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Ny_Trc", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[11] = i_value;
      dict_hdr.VelFun[11] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_MCTrSp", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[12] = i_value;
      dict_hdr.VelFun[12] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_NumCmp", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[13] = i_value;
      dict_hdr.VelFun[13] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_MCLE01", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[14] = i_value;
      dict_hdr.VelFun[14] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_OrNSMP", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[40] = i_value;
      dict_hdr.VelFun[40] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_AziIln", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[41] = 0;
      bin_hdr.VelFun[42] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+41, bin_hdr.VelFun+42);
      dict_hdr.VelFun[41] = bin_hdr.VelFun[41];
      dict_hdr.VelFun[42] = bin_hdr.VelFun[42];
   }
   if(1 == cdds_scanf("usp_VelFun usp_AziXln", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[43] = 0;
      bin_hdr.VelFun[44] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+43, bin_hdr.VelFun+44);
      dict_hdr.VelFun[43] = bin_hdr.VelFun[43];
      dict_hdr.VelFun[44] = bin_hdr.VelFun[44];
   }
   if(1 == cdds_scanf("usp_VelFun usp_HrzNul", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[45] = 0;
      bin_hdr.VelFun[46] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+45, bin_hdr.VelFun+46);
      dict_hdr.VelFun[45] = bin_hdr.VelFun[45];
      dict_hdr.VelFun[46] = bin_hdr.VelFun[46];
   }
   if(1 == cdds_scanf("usp_VelFun usp_UnitSc", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[48] = 0;
      bin_hdr.VelFun[49] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+48, bin_hdr.VelFun+49);
      dict_hdr.VelFun[48] = bin_hdr.VelFun[48];
      dict_hdr.VelFun[49] = bin_hdr.VelFun[49];
   }
   if(1 == cdds_scanf("usp_VelFun usp_SmpFlt", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[50] = 0;
      bin_hdr.VelFun[51] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+50, bin_hdr.VelFun+51);
      dict_hdr.VelFun[50] = bin_hdr.VelFun[50];
      dict_hdr.VelFun[51] = bin_hdr.VelFun[51];
   }
   if(1 == cdds_scanf("usp_VelFun usp_MaxAmp", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[52] = 0;
      bin_hdr.VelFun[53] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+52, bin_hdr.VelFun+53);
      dict_hdr.VelFun[52] = bin_hdr.VelFun[52];
      dict_hdr.VelFun[53] = bin_hdr.VelFun[53];
   }
   if(1 == cdds_scanf("usp_VelFun usp_AvgAmp", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[54] = 0;
      bin_hdr.VelFun[55] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+54, bin_hdr.VelFun+55);
      dict_hdr.VelFun[54] = bin_hdr.VelFun[54];
      dict_hdr.VelFun[55] = bin_hdr.VelFun[55];
   }
   if(1 == cdds_scanf("usp_VelFun usp_RmsAmp", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[56] = 0;
      bin_hdr.VelFun[57] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+56, bin_hdr.VelFun+57);
      dict_hdr.VelFun[56] = bin_hdr.VelFun[56];
      dict_hdr.VelFun[57] = bin_hdr.VelFun[57];
   }
   if(1 == cdds_scanf("usp_VelFun usp_FreQst", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[68] = 0;
      bin_hdr.VelFun[69] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+68, bin_hdr.VelFun+69);
      dict_hdr.VelFun[68] = bin_hdr.VelFun[68];
      dict_hdr.VelFun[68] = bin_hdr.VelFun[69];
   }
   if(1 == cdds_scanf("usp_VelFun usp_FreQnd", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[70] = 0;
      bin_hdr.VelFun[71] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+70, bin_hdr.VelFun+71);
      dict_hdr.VelFun[70] = bin_hdr.VelFun[70];
      dict_hdr.VelFun[71] = bin_hdr.VelFun[71];
   }
   if(1 == cdds_scanf("usp_VelFun usp_RaMmin", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[72] = 0;
      bin_hdr.VelFun[73] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+72, bin_hdr.VelFun+73);
      dict_hdr.VelFun[72] = bin_hdr.VelFun[72];
      dict_hdr.VelFun[73] = bin_hdr.VelFun[73];
   }
   if(1 == cdds_scanf("usp_VelFun usp_RaMmax", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[74] = 0;
      bin_hdr.VelFun[75] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+74, bin_hdr.VelFun+75);
      dict_hdr.VelFun[74] = bin_hdr.VelFun[74];
      dict_hdr.VelFun[75] = bin_hdr.VelFun[75];
   }
   if(1 == cdds_scanf("usp_VelFun usp_RaXmax", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[76] = 0;
      bin_hdr.VelFun[77] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+76, bin_hdr.VelFun+77);
      dict_hdr.VelFun[76] = bin_hdr.VelFun[76];
      dict_hdr.VelFun[77] = bin_hdr.VelFun[77];
   }
   if(1 == cdds_scanf("usp_VelFun usp_RaZref", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[78] = 0;
      bin_hdr.VelFun[79] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+78, bin_hdr.VelFun+79);
      dict_hdr.VelFun[78] = bin_hdr.VelFun[78];
      dict_hdr.VelFun[79] = bin_hdr.VelFun[79];
   }
   HDR_DEF_INT(usp_, HlhEnt)
   HDR_DEF_INT(usp_, HlhByt)

/*** bad idea ? 7/9/98 rls
   if(1 == cdds_scanf("usp_UnitSc", "%f", &UnitSc)) {
      ddsFloat2Fake(&UnitSc, dict_hdr.VelFun+48, dict_hdr.VelFun+49);
   }
***/

   /*************************************/
   /* force rational line header values */
   /*************************************/

   NumRec = cdds_axis_prod(3); /* product of axis size 3 and above */
   NumRec4 = cdds_axis_prod(4); /* product of axis size 4 and above */

   bin->cube.axis[1].flag |= DDS_AXIS_size;
   if(bin->cube.axis[1].size < 1)
      bin->cube.axis[1].size = 1;
   bin_hdr.NumSmp = bin->cube.axis[1].size;

   if(bin->cube.rank >= 2) {
      bin->cube.axis[2].flag |= DDS_AXIS_size;
      if(bin->cube.axis[2].size < 1)
         bin->cube.axis[2].size = 1;
      bin_hdr.NumTrc = bin->cube.axis[2].size;
   } else if(bin_hdr.NumTrc < 1) {
      bin_hdr.NumTrc = 1;
   }

   if(bin->cube.rank >= 3) {
      bin->cube.axis[3].flag |= DDS_AXIS_size;
      if(bin->cube.axis[3].size < 1)
         bin->cube.axis[3].size = 1;
   }

   bin_hdr.NumRec = NumRec;
   bin_hdr.TmMsFS = 0;
   bin_hdr.Format = Format;
   bin_hdr.HlhEnt = 0;
   bin_hdr.VelFun[13] = NumRec4;

   hlh_size = 0;
   line_pad = 0;
   trace_pad = 0;

   /***************************************************/
   /* Merge line header and nominal cube description. */
   /***************************************************/

   /* Discover rank of standard axis names, if they exist. */
   usp_units = NULL;
   axis_t = axis_x = axis_y = axis_z = 0;

   for(rank = 1; rank <= bin->cube.rank; rank++) {
      axis = bin->cube.axis + rank;

      if(! strcmp(axis->name, "t")) {
         axis_t = rank;
      } else if(! strcmp(axis->name, "x")) {
         axis_x = rank;
         if(! usp_units && axis->units) usp_units = axis->units;
      } else if(! strcmp(axis->name, "y")) {
         axis_y = rank;
         if(! usp_units && axis->units) usp_units = axis->units;
      } else if(! strcmp(axis->name, "z")) {
         axis_z = rank;
         if(! usp_units && axis->units) usp_units = axis->units;
      }
   }

   if(! usp_units) usp_units = "usp_xy_units?";

   if(axis_t) {
      /* "time" axis DOES exist. */
      if(strstr(bin->cube.axis[axis_t].units, "?")) {
         cube_to_sec = 1.;
         UnitSc = 1.e-3; 
      } else {
         cube_units = bin->cube.axis[axis_t].units;
         cube_to_sec = cdds_scale("sec", cube_units);

         if(! cube_to_sec) {
            ddsWarn("sec unknown, given delta.t %s\n\t%s\n",
               cube_units, media->title.ptr);
            cube_to_sec = 1.;
         }
         delta_t = fabs(cube_to_sec * bin->cube.axis[axis_t].delta);
         if(! delta_t) delta_t = 1.0e+9; /* an obvious bogus value */

/*********************************************************************/
/* This was an attempt to achieve powers of 3 multiples              */
/* i.e. engineering style 10**-0, 10**-3, 10**-6.                    */
/*                                                                   */
/* UnitSc = pow(10., 3. * ROUND((log10(delta_t) + 66.) / 3.) - 69.); */
/*                                                                   */
/* The implementation is broken and the concept is defective         */
/* because the effective precision has large jumps for a small       */
/* change in sample rate.                                            */
/*                                                                   */
/* If "delta.t= 0.002" and "UnitSc= 10**-3" is choosen,              */
/* that also means "delta.t= 0.0027231" will lose precision.         */
/*********************************************************************/
/*** 18 Aug 1998 RLS, yet another bad idea
         UnitSc = pow(10.0, ROUND(log10(delta_t) + 60.5) - 66.0);
***/
         /***********************************************************/
         /* UnitSc power should be a multiple of 3 (engineer style) */
         /* The initial guess preserves 7 digits of accuracy,       */
         /* and insignificant digits are dropped for nominal usp.   */
         /*       delta_t     SmpInt   UnitSc                       */
         /*    0.008123456   8123460   1.e-9                        */
         /*    0.008123000      8123   1.e-6                        */
         /*    0.008000000         8   1.e-3   nominal              */
         /***********************************************************/
         UnitSc_dbl = pow(10., 3. * ROUND((log10(delta_t) + 60.5) / 3.) - 66.);
         sprintf(tmp, "%10i", (int)ROUND(delta_t / UnitSc_dbl));
         if(tmp[9] == '0' && tmp[8] == '0' && tmp[7] == '0') {
            if(tmp[6] == '0' && tmp[5] == '0' && tmp[4] == '0') {
               /* the last 6 digits are zero (not significant) */
               UnitSc_dbl *= 1.e6;
            } else {
               /* the last 3 digits are zero (not significant) */
               UnitSc_dbl *= 1.e3;
            }
         }
         UnitSc = UnitSc_dbl;
      }

      if(bin->cube.axis[axis_t].flag & DDS_AXIS_origin) {
         bin_hdr.TmMsFS = ROUND(bin->cube.axis[axis_t].origin *
            cube_to_sec / UnitSc);
      }

      if(bin->cube.axis[axis_t].flag & DDS_AXIS_delta) {
         bin_hdr.SmpInt = ROUND(bin->cube.axis[axis_t].delta *
            cube_to_sec / UnitSc);
      }

      ddsFloat2Fake(&UnitSc, bin_hdr.VelFun+48, bin_hdr.VelFun+49);
   } else if(! bin_hdr.SmpInt) {
      /* default the line header sample interval to 1 */
      bin_hdr.SmpInt = 1;
   }


   if(! strcmp(usp_units, "meters")) {
      bin_hdr.UnitFl = USP_UnitFl_METERS;
   } else if(! strcmp(usp_units, "feet")) {
      bin_hdr.UnitFl = USP_UnitFl_FEET;
   } else {
      bin_hdr.UnitFl = USP_UnitFl_other;
   }

   if(axis_x && (bin->cube.axis[axis_x].flag & DDS_AXIS_delta)) {
      /* "x-space" axis DOES exist (Note: Dx1000 == TmMsSl). */
      if(strstr(bin->cube.axis[axis_x].units, "?")) {
         cube_to_usp = 1.;
      } else {
         cube_units = bin->cube.axis[axis_x].units;
         cube_to_usp = cdds_scale(usp_units, cube_units);

         if(! cube_to_usp) {
            ddsWarn("usp Dx1000 %s unknown, given delta.x %s\n\t%s\n",
               usp_units, cube_units, media->title.ptr);
            cube_to_usp = 1.;
         }
      }
      bin_hdr.TmMsSl = ROUND(bin->cube.axis[axis_x].delta *
         1000. * cube_to_usp);
   }

   if(axis_y && (bin->cube.axis[axis_y].flag & DDS_AXIS_delta)) {
      /* "y-space" axis DOES exist (Note: Dy1000 == DtInFl). */
      if(strstr(bin->cube.axis[axis_y].units, "?")) {
         cube_to_usp = 1.;
      } else {
         cube_units = bin->cube.axis[axis_y].units;
         cube_to_usp = cdds_scale(usp_units, cube_units);

         if(! cube_to_usp) {
            ddsWarn("usp Dy1000 %s unknown, given delta.y %s\n\t%s\n",
               usp_units, cube_units, media->title.ptr);
            cube_to_usp = 1.;
         }
      }
      bin_hdr.DtInFl = ROUND(bin->cube.axis[axis_y].delta *
         1000. * cube_to_usp);
   }

   if(axis_z && (bin->cube.axis[axis_z].flag & DDS_AXIS_delta)) {
      /* "z-space" axis DOES exist (Note: Dz1000 == TmSlIn). */
      if(strstr(bin->cube.axis[axis_z].units, "?")) {
         cube_to_usp = 1.;
      } else {
         cube_units = bin->cube.axis[axis_z].units;
         cube_to_usp = cdds_scale(usp_units, cube_units);

         if(! cube_to_usp) {
            ddsWarn("usp Dz1000 %s unknown, given delta.z %s\n\t%s\n",
               usp_units, cube_units, media->title.ptr);
            cube_to_usp = 1.;
         }
      }
      bin_hdr.TmSlIn = ROUND(bin->cube.axis[axis_z].delta *
         1000. * cube_to_usp);
   }
      
   /******************************************************/
   /* Given a "format=" and media, determine usp flavor. */
   /******************************************************/

   bin->state  |= DDS_green_prefix;

#ifdef HOST_SWAP
   if(bin->media->genus == DDS_MEDIA_FIFO) {
#else
   if(0) {
#endif
      /* fifo: keep big endian first for byte swapped systems */
      usp_line_min = USP_LINE_IEEE_MIN;
      usp_line_max = USP_LINE_IEEE_MAX;
      usp_trace_min = USP_TRACE_IEEE_MIN;
      HlhByt_prec = 2;
      char_stype = DDS_STYPE_ASCII1;
      short_stype = DDS_STYPE_INTEGER2X;
      int_stype = DDS_STYPE_INTEGER4X;
      float_stype = DDS_STYPE_FLOAT4X;
      samples_stype = DDS_STYPE_FLOAT4X;
      green_stype = DDS_STYPE_INTEGER4X;
      if(cdds_scank("format", "complex")) {
         samples_stype = DDS_STYPE_COMPLEX8X;
         bin_hdr.NumSmp = 2*bin_hdr.NumSmp;
      }
   } else {
      usp_line_min = USP_LINE_IEEE_MIN;
      usp_line_max = USP_LINE_IEEE_MAX;
      usp_trace_min = USP_TRACE_IEEE_MIN;
      HlhByt_prec = 2;
      char_stype = DDS_STYPE_ASCII1;
      short_stype = DDS_STYPE_INTEGER2;
      int_stype = DDS_STYPE_INTEGER4;
      float_stype = DDS_STYPE_FLOAT4;
      samples_stype = DDS_STYPE_FLOAT4;
      green_stype = DDS_STYPE_INTEGER4;
      if(cdds_scank("format", "complex")) {
         samples_stype = DDS_STYPE_COMPLEX8;
         bin_hdr.NumSmp = 2*bin_hdr.NumSmp;
      }
   }

   bin_hdr.HlhByt = 2 * HlhByt_prec;

   if(0 < cdds_scank("format", "no_green_prefix")) {
      green_prec = 0;
   } else {
      green_prec = dds_prime[green_stype].prec;
   }

   sample_prec = dds_prime[samples_stype].prec;

   green_line = usp_line_min;
   green_trace = usp_trace_min + bin->cube.axis[1].size * sample_prec;

   /******************************************/
   /* build HLH into the new usp line header */
   /******************************************/

   line_buf = ddsZalloc(usp_line_max);

   hlh_size = 0;
   hlh_ptr = line_buf + green_line + green_prec;

   /* get all previous-history entries */
   cdds_dict(bin->dict_open, "scan");
   defn = cdds_newer("previous_history");
   while(defn) {
      if(cdds_scant("", &history) > 0) {
         if(!storeHlh(history,2)) break;
      }
      defn = cdds_newer("previous_history");
   }

   /* get all dds entries */
   ddsParCheck(0);
   cdds_dict(bin->dict_open, "scan");
   defn = cdds_newer("cmd_title");
   while(defn) {
      if(cdds_scant("", &history) > 0) {
         ddsStrCpy(&tmp_str, history);
         if(cdds_scank("", "modify") <= 0) {
            if(!storeHlh(tmp_str.ptr,0)) break;
            /* get user parameters */
            if(!getHlhPars()) break; 
         }
      }
      defn = cdds_newer("cmd_title");
   }
   ddsParCheck(1);
   
   /* add "dict: axis=..." first to work with openoldusp.c */
   /* do not set size.axis(1), size.axis(2) or delta.axis(1) */
   cdds_scanv("axis", &defn);
   ddsStrCpyCat2(&tmp_str, "dict: axis=", defn);
   storeHlh(tmp_str.ptr,1);
   
   rank = cdds_scank("axis", "");
   for (i=1;i<=rank;i++) {
      if(i > 2 && rank > 3) {
         sprintf(tmp, "size.%s", bin->cube.axis[i].name);
         if(cdds_scanv(tmp, &defn) > 0) {
            ddsStrCpyCat4(&tmp_str, "dict: ", tmp, "=", defn);
            storeHlh(tmp_str.ptr,1);
         }
      }
      
      if(i > 1) {
         sprintf(tmp, "delta.%s", bin->cube.axis[i].name);
         if(cdds_scanv(tmp, &defn) > 0) {
            ddsStrCpyCat4(&tmp_str, "dict: ", tmp, "=", defn);
            storeHlh(tmp_str.ptr,1);
         }
      }
      
      sprintf(tmp, "origin.%s", bin->cube.axis[i].name);
      if(cdds_scanv(tmp, &defn) > 0) {
         ddsStrCpyCat4(&tmp_str, "dict: ", tmp, "=", defn);
         storeHlh(tmp_str.ptr,1);
      }
      
      sprintf(tmp, "units.%s", bin->cube.axis[i].name);
      if(cdds_scanv(tmp, &defn) > 0) {
         ddsStrCpyCat4(&tmp_str, "dict: ", tmp, "=", defn);
         storeHlh(tmp_str.ptr,1);
      }

      sprintf(tmp, "base.%s", bin->cube.axis[i].name);
      if(cdds_scanv(tmp, &defn) > 0) {
         ddsStrCpyCat4(&tmp_str, "dict: ", tmp, "=", defn);
         storeHlh(tmp_str.ptr,1);
      }

      sprintf(tmp, "step.%s", bin->cube.axis[i].name);
      if(cdds_scanv(tmp, &defn) > 0) {
         ddsStrCpyCat4(&tmp_str, "dict: ", tmp, "=", defn);
         storeHlh(tmp_str.ptr,1);
      }
   }

   /* reset the line header parameters including the new HLH */
   bin_hdr.HlhByt = bin_hdr.HlhByt + hlh_size;
   green_line = green_line + hlh_size;

   /*************************************************/
   /* compile the description of the binary format. */
   /*************************************************/

   /* create explicit typedefs */
   ddsSymTypedefObj("GREEN_PREFIX", green_prec ? int_stype : DDS_CTYPE_VOID, 0);
   ddsSymTypedefObj("USP_CHAR", char_stype, 0);
   ddsSymTypedefObj("USP_SHORT", short_stype, 0);
   ddsSymTypedefObj("USP_INT", int_stype, 0);
   ddsSymTypedefObj("USP_FLOAT", float_stype, 0);
   ddsSymTypedefObj("SAMPLE_TYPE", samples_stype, 0);

   /* update USP_HLH_SIZE, USP_LINE_PAD, USP_TRACE_PAD */
   block = ddsSymTypedefEB(DDS_CTYPE_INT);
   ddsSymTypedefEV("USP_LINE_PAD", line_pad);
   ddsSymTypedefEV("USP_TRACE_PAD", trace_pad);
   ddsSymTypedefEV("USP_HLH_SIZE", hlh_size);
   ddsSymTypedefEE(block, "USP_ADJUST");

   if(ddsCompileFmt(bin, DDS_TRUE)) return;

   ddsUncompileFmt(bin);

   cdds_dict(bin->dict_open, "print");

   /*****************************************/
   /* link bin struct to line header fields */
   /*****************************************/

   if(ddsObjectTag(&bin->std.usp.line, bin->block_new, "PREAMBLE") ||
      bin->std.usp.line.type->ptype != DDS_STYPE_STRUCT ||
      ddsLinkObject(&bin->std.usp.line)) {
      ddsError("Bogus usp format line declaration\n\tbinary= %s\n",
         bin->name);
      return;
   }

   type = bin->std.usp.line.type;

   ddsLinkMember(&bin->std.usp.line_prefix, type->block, "line_green_prefix");
   ddsLinkMember(&bin->std.usp.line_HlhEnt, type->block, "HlhEnt");
   ddsLinkMember(&bin->std.usp.line_HlhByt, type->block, "HlhByt");
   ddsLinkMember(&bin->std.usp.line_HlhBuf, type->block, "HlhBuf");

   /********************************************/
   /* find trace object and create its API tag */
   /********************************************/

   if(ddsTraceTag(bin)) return;

   /*********************************************************/
   /* Update binary cube description.                       */
   /*                                                       */
   /* Cube print must follow HDR_MOD_* in order to maintain */
   /* maximum precision for space and time coordinates,     */
   /* i.e. sample rates and origins.                        */
   /* This is a problem for usp and segy.                   */
   /*********************************************************/

   ddsCubePrint(bin, old_cube);

   if(bin->trace.type->ptype != DDS_STYPE_STRUCT) {
      ddsError("Bogus usp format trace declaration\n\tbinary= %s\n",
         bin->name);
      return;
   }

   /*************************/
   /* link cube description */
   /*************************/

   ddsCubeLinker(bin);

   type = bin->trace.type;

   ddsLinkMember(&bin->prefix, type->block, "green_prefix");
   ddsLinkMember(&bin->TrcType, type->block, "StaCor");
   ddsLinkMember(&bin->Samples, type->block, "Samples");

   /******************************/
   /* update symbol table values */
   /******************************/

   type = bin->std.usp.line_HlhBuf.type;
   type->count = hlh_size;
   type->stroke = hlh_size;

   type = bin->std.usp.line.type;
   type->stroke = green_prec + green_line;
   type->prec = green_prec + green_line;
   bin->std.usp.line_buf = ddsRealloc(line_buf, type->stroke);

   /****************/
   /* sanity check */
   /****************/

   type = bin->std.usp.line.type;
   member = bin->std.usp.line_HlhBuf.member;
   if(member->offset != usp_line_min + green_prec) {
      ddsError("USP line HlhBuf offset= %lu, expected %i\n\tbinary= %s\n",
         (unsigned long)member->offset,
         (int)(usp_line_min + green_prec), bin->name);
   } else if(green_line != type->stroke - green_prec) {
      ddsError("USP line green word= %i, expected %i\n\tbinary= %s\n",
         (int)green_line, (int)(type->stroke - green_prec), bin->name);
   }

   /****************/
   /* sanity check */
   /****************/

   type = bin->trace.type;
   member = bin->Samples.member;
   if(member->offset != usp_trace_min + green_prec) {
      ddsError("USP trace Samples offset= %lu, expected %i\n\tbinary= %s\n",
         (unsigned long)member->offset,
         (int)(usp_trace_min + green_prec), bin->name);
   } else if(green_trace != type->stroke - green_prec) {
      ddsError("USP trace green word= %i, expected %i\n\tbinary= %s\n",
         (int)green_trace, (int)(type->stroke - green_prec), bin->name);
   }

   /*******************************************************/
   /* PUT header values into binary buffer.               */
   /*                                                     */
   /* The bin_hdr value is put into the binary header     */
   /* referenced by hdr_buf.                              */
   /*                                                     */
   /* API tags are created for each member and the offset */
   /* to each member in the buffer is returned in "bias". */
   /*******************************************************/

   bin->std.usp.line_green_min = usp_line_min;
   bin->std.usp.line_green_max = usp_line_max;

   /* setup implicit arguments used by HDR_PUT */
   hdr_buf = bin->std.usp.line_buf;
   type = bin->std.usp.line.type;
   hdr_block = type->block;
 
   HDR_PUT_CHAR("", EqpCod)
   HDR_PUT_CHAR("", CrwNam)
   HDR_PUT_CHAR("", DatTyp)
   HDR_PUT_CHAR("", DgTrkS)
   HDR_PUT_CHAR("", PrcNam)
   HDR_PUT_CHAR("", PrcDat)
   HDR_PUT_CHAR("", OACLin)
   HDR_PUT_CHAR("", JobNum)
   HDR_PUT_INT("", NumTrc)
   HDR_PUT_INT("", NumRec)
   HDR_PUT_INT("", SmpInt)
   HDR_PUT_INT("", NumSmp)
   HDR_PUT_INT("", Format)
   HDR_PUT_INT("", IndAdj)
   HDR_PUT_INT("", OpGrFl)
   HDR_PUT_CHAR("", SrtTyp)
   HDR_PUT_INT("", CDPFld)
   HDR_PUT_CHAR("", GrpInt)
   HDR_PUT_INT("_DtInFl.", DtInFl)
   HDR_PUT_INT("", MxSPEl)
   HDR_PUT_INT("", MnSPEl)
   HDR_PUT_INT("", MxRSEL)
   HDR_PUT_INT("", MnRSEL)
   HDR_PUT_INT("", MxGrEl)
   HDR_PUT_INT("", MnGrEl)
   HDR_PUT_INT("", MxTrSt)
   HDR_PUT_INT("", MnTrSt)
   HDR_PUT_CHAR("", LinDir)
   HDR_PUT_INT("", AERcPr)
   HDR_PUT_INT("", OrNTRC)
   HDR_PUT_INT("", OrNREC)
   HDR_PUT_FLOAT("", MutVel)
   HDR_PUT_INT("", ZDMuTm)
   HDR_PUT_INT("", IntInc)
   HDR_PUT_INT("_WatVel.", WatVel)
   HDR_PUT_INT("", MxShDp)
   HDR_PUT_INT("", MnShDp)
   HDR_PUT_INT("", MxUHTm)
   HDR_PUT_INT("", MnUHTm)
   HDR_PUT_INT("", MxTrOf)
   HDR_PUT_INT("", MnTrOf)
   HDR_PUT_INT("_RATTrc.", RATTrc)
   HDR_PUT_INT("_RATFld.", RATFld)
   HDR_PUT_INT("", UnitFl)
   HDR_PUT_INT("", FrstSP)
   HDR_PUT_INT("", DpN1SP)
   HDR_PUT_INT("", NmDpIn)
   HDR_PUT_INT("", StWdFl)
   HDR_PUT_INT("", DptInt)
   HDR_PUT_INT("_TmMsSl.", TmMsSl)
   HDR_PUT_INT("_TmSlIn.", TmSlIn)
   HDR_PUT_FLOAT("", TmMsFS)
   HDR_PUT_INT("", NTrLnS)
   HDR_PUT_INT("", PltDir)
   HDR_PUT_INT("", LgRfEl)
   HDR_PUT_INT("", MnLnIn)
   HDR_PUT_INT("", MxLnIn)
   HDR_PUT_INT("", MnDpIn)
   HDR_PUT_INT("", MxDpIn)
   HDR_PUT_FLOAT("", ILClIn)
   HDR_PUT_FLOAT("", CLClIn)
   HDR_PUT_INT("", ReSpFm)
   HDR_PUT_INT("", VlFnRF)
   HDR_PUT_CHAR("", OACUsr)
   HDR_PUT_CHAR("", LinNam)
   HDR_PUT_INT("", MutFlg)
   HDR_PUT_CHAR("", SPBias)
   HDR_PUT_INT("", SpBiFl)
   HDR_PUT_CHAR("", APIWNo)
   HDR_PUT_FLOAT("", NmSpMi)
   HDR_PUT_INT("_VelFun.", VelFun)
   HDR_PUT_INT("", HlhEnt)
   HDR_PUT_INT("", HlhByt)

   /*********************************************************/
   /* allocate buffer for one dead trace, and initialize it */
   /*********************************************************/

   type = bin->trace.type;
   bin->trace_buf = (char*)ddsZalloc(type->stroke);

   if(dds_state & DDS_DUMP_DIFF) {
      bin->diff1_buf = ddsMalloc(type->stroke);
      memset(bin->diff1_buf, DDS_NON_CHAR, type->stroke);
      bin->diff2_buf = ddsMalloc(type->stroke);
      memset(bin->diff2_buf, DDS_NON_CHAR, type->stroke);
   }

   bin->dead_trace = ddsZalloc(type->stroke);

   member = bin->TrcType.member;
   i_value = USP_TRACE_DEAD;
   CON_NASS(bin->TrcType.type->ptype, DDS_CTYPE_INT,
      bin->dead_trace + member->offset, &i_value);

   /***********************************************************/
   /* MODify header value within dictionary.                  */
   /*                                                         */
   /* If the values in dict_hdr and bin_hdr are different,    */
   /* then the dictionary is updated using the bin_hdr value. */
   /* If the values are the same, nothing is changed.         */
   /* This brings the dictionary up to date with the current  */
   /* values in the binary buffer.                            */
   /***********************************************************/

   /* setup implicit arguments used by HDR_MOD */
   cdds_dict(bin->dict_open, "print");

   HDR_MOD_CHAR(usp_, EqpCod, 1)
   HDR_MOD_CHAR(usp_, CrwNam, 1)
   HDR_MOD_CHAR(usp_, DatTyp, 1)
   HDR_MOD_CHAR(usp_, DgTrkS, 1)
   HDR_MOD_CHAR(usp_, PrcNam, 1)
   HDR_MOD_CHAR(usp_, PrcDat, 1)
   HDR_MOD_CHAR(usp_, OACLin, 1)
   HDR_MOD_CHAR(usp_, JobNum, 1)
   HDR_MOD_INT(usp_, NumTrc)
   HDR_MOD_INT(usp_, NumRec)
   HDR_MOD_INT(usp_, SmpInt)
   HDR_MOD_INT(usp_, NumSmp)
   HDR_MOD_INT(usp_, Format)
   HDR_MOD_INT(usp_, IndAdj)
   HDR_MOD_INT(usp_, OpGrFl)
   HDR_MOD_CHAR(usp_, SrtTyp, 1)
   HDR_MOD_INT(usp_, CDPFld)
   HDR_MOD_CHAR(usp_, GrpInt, 1)
   HDR_MOD_INT(usp_, DtInFl)

   if(bin_hdr.DtInFl != dict_hdr.DtInFl)
      cdds_printf("usp_Dy1000", " %i\n", bin_hdr.DtInFl);

   HDR_MOD_INT(usp_, MxSPEl)
   HDR_MOD_INT(usp_, MnSPEl)
   HDR_MOD_INT(usp_, MxRSEL)
   HDR_MOD_INT(usp_, MnRSEL)
   HDR_MOD_INT(usp_, MxGrEl)
   HDR_MOD_INT(usp_, MnGrEl)
   HDR_MOD_INT(usp_, MxTrSt)
   HDR_MOD_INT(usp_, MnTrSt)
   HDR_MOD_CHAR(usp_, LinDir, 1)
   HDR_MOD_INT(usp_, AERcPr)
   HDR_MOD_INT(usp_, OrNTRC)
   HDR_MOD_INT(usp_, OrNREC)
   HDR_MOD_FLOAT(usp_, MutVel)
   HDR_MOD_INT(usp_, ZDMuTm)
   HDR_MOD_INT(usp_, IntInc)
   HDR_MOD_INT(usp_, WatVel)

   if(bin_hdr.WatVel != dict_hdr.WatVel)
      cdds_printf("usp_RefVel", " %i\n", bin_hdr.WatVel);

   HDR_MOD_INT(usp_, MxShDp)
   HDR_MOD_INT(usp_, MnShDp)
   HDR_MOD_INT(usp_, MxUHTm)
   HDR_MOD_INT(usp_, MnUHTm)
   HDR_MOD_INT(usp_, MxTrOf)
   HDR_MOD_INT(usp_, MnTrOf)
   HDR_MOD_INT(usp_, RATTrc)

   if(bin_hdr.RATTrc != dict_hdr.RATTrc)
      cdds_printf("usp_AnglSt", " %i\n", bin_hdr.RATTrc);

   HDR_MOD_INT(usp_, RATFld)

   if(bin_hdr.RATFld != dict_hdr.RATFld)
      cdds_printf("usp_AngInc", " %i\n", bin_hdr.RATFld);

   HDR_MOD_INT(usp_, UnitFl)
   HDR_MOD_INT(usp_, FrstSP)
   HDR_MOD_INT(usp_, DpN1SP)
   HDR_MOD_INT(usp_, NmDpIn)
   HDR_MOD_INT(usp_, StWdFl)
   HDR_MOD_INT(usp_, DptInt)
   HDR_MOD_INT(usp_, TmMsSl)

   if(bin_hdr.TmMsSl != dict_hdr.TmMsSl)
      cdds_printf("usp_Dx1000", " %i\n", bin_hdr.TmMsSl);

   HDR_MOD_INT(usp_, TmSlIn)

   if(bin_hdr.TmSlIn != dict_hdr.TmSlIn)
      cdds_printf("usp_Dz1000", " %i\n", bin_hdr.TmSlIn);

   HDR_MOD_FLOAT(usp_, TmMsFS)
   HDR_MOD_INT(usp_, NTrLnS)
   HDR_MOD_INT(usp_, PltDir)
   HDR_MOD_INT(usp_, LgRfEl)
   HDR_MOD_INT(usp_, MnLnIn)
   HDR_MOD_INT(usp_, MxLnIn)
   HDR_MOD_INT(usp_, MnDpIn)
   HDR_MOD_INT(usp_, MxDpIn)
   HDR_MOD_FLOAT(usp_, ILClIn)
   HDR_MOD_FLOAT(usp_, CLClIn)
   HDR_MOD_INT(usp_, ReSpFm)
   HDR_MOD_INT(usp_, VlFnRF)
   HDR_MOD_CHAR(usp_, OACUsr, 1)
   HDR_MOD_CHAR(usp_, LinNam, 1)
   HDR_MOD_INT(usp_, MutFlg)
   HDR_MOD_CHAR(usp_, SPBias, 1)
   HDR_MOD_INT(usp_, SpBiFl)
   HDR_MOD_CHAR(usp_, APIWNo, 1)
   HDR_MOD_FLOAT(usp_, NmSpMi)

   /**********************************************************/
   /* The dictionary MUST be updated with these union floats */
   /* BEFORE updating the VelFun array, otherwise the floats */
   /* have precedence and may screw up fidelity if the field */
   /* is actually an integer (bogus float representation).   */
   /**********************************************************/

   if(bin_hdr.VelFun[41] != dict_hdr.VelFun[41] ||
      bin_hdr.VelFun[42] != dict_hdr.VelFun[42]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+41, bin_hdr.VelFun+42);
      cdds_printf("usp_AziIln", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[43] != dict_hdr.VelFun[43] ||
      bin_hdr.VelFun[44] != dict_hdr.VelFun[44]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+43, bin_hdr.VelFun+44);
      cdds_printf("usp_AziXln", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[45] != dict_hdr.VelFun[45] ||
      bin_hdr.VelFun[46] != dict_hdr.VelFun[46]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+45, bin_hdr.VelFun+46);
      cdds_printf("usp_HrzNul", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[48] != dict_hdr.VelFun[48] ||
      bin_hdr.VelFun[49] != dict_hdr.VelFun[49]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+48, bin_hdr.VelFun+49);
      cdds_printf("usp_UnitSc", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[50] != dict_hdr.VelFun[50] ||
      bin_hdr.VelFun[51] != dict_hdr.VelFun[51]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+50, bin_hdr.VelFun+51);
      cdds_printf("usp_SmpFlt", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[52] != dict_hdr.VelFun[52] ||
      bin_hdr.VelFun[53] != dict_hdr.VelFun[53]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+52, bin_hdr.VelFun+53);
      cdds_printf("usp_MaxAmp", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[54] != dict_hdr.VelFun[54] ||
      bin_hdr.VelFun[55] != dict_hdr.VelFun[55]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+54, bin_hdr.VelFun+55);
      cdds_printf("usp_AvgAmp", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[56] != dict_hdr.VelFun[56] ||
      bin_hdr.VelFun[57] != dict_hdr.VelFun[57]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+56, bin_hdr.VelFun+57);
      cdds_printf("usp_RmsAmp", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[68] != dict_hdr.VelFun[68] ||
      bin_hdr.VelFun[69] != dict_hdr.VelFun[69]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+68, bin_hdr.VelFun+69);
      cdds_printf("usp_FreQst", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[70] != dict_hdr.VelFun[70] ||
      bin_hdr.VelFun[71] != dict_hdr.VelFun[71]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+70, bin_hdr.VelFun+71);
      cdds_printf("usp_FreQnd", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[72] != dict_hdr.VelFun[72] ||
      bin_hdr.VelFun[73] != dict_hdr.VelFun[73]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+72, bin_hdr.VelFun+73);
      cdds_printf("usp_RaMmin", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[74] != dict_hdr.VelFun[74] ||
      bin_hdr.VelFun[75] != dict_hdr.VelFun[75]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+74, bin_hdr.VelFun+75);
      cdds_printf("usp_RaMmax", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[76] != dict_hdr.VelFun[76] ||
      bin_hdr.VelFun[77] != dict_hdr.VelFun[77]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+76, bin_hdr.VelFun+77);
      cdds_printf("usp_RaXmax", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[78] != dict_hdr.VelFun[78] ||
      bin_hdr.VelFun[79] != dict_hdr.VelFun[79]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+78, bin_hdr.VelFun+79);
      cdds_printf("usp_RaZref", " %g\n", FakeFloat);
   }

   HDR_MOD_INT(usp_, VelFun)

   if(bin_hdr.VelFun[0] != dict_hdr.VelFun[0])
      cdds_printf("usp_MinVel", " %i\n", bin_hdr.VelFun[0]);
   if(bin_hdr.VelFun[1] != dict_hdr.VelFun[1])
      cdds_printf("usp_MaxVel", " %i\n", bin_hdr.VelFun[1]);
   if(bin_hdr.VelFun[2] != dict_hdr.VelFun[2])
      cdds_printf("usp_Nx_Pad", " %i\n", bin_hdr.VelFun[2]);
   if(bin_hdr.VelFun[3] != dict_hdr.VelFun[3])
      cdds_printf("usp_Nx_Win", " %i\n", bin_hdr.VelFun[3]);
   if(bin_hdr.VelFun[4] != dict_hdr.VelFun[4])
      cdds_printf("usp_Nx_Rec", " %i\n", bin_hdr.VelFun[4]);
   if(bin_hdr.VelFun[5] != dict_hdr.VelFun[5])
      cdds_printf("usp_NxLine", " %i\n", bin_hdr.VelFun[5]);
   if(bin_hdr.VelFun[6] != dict_hdr.VelFun[6])
      cdds_printf("usp_Nx_Trc", " %i\n", bin_hdr.VelFun[6]);
   if(bin_hdr.VelFun[7] != dict_hdr.VelFun[7])
      cdds_printf("usp_Ny_Pad", " %i\n", bin_hdr.VelFun[7]);
   if(bin_hdr.VelFun[8] != dict_hdr.VelFun[8])
      cdds_printf("usp_Ny_Win", " %i\n", bin_hdr.VelFun[8]);
   if(bin_hdr.VelFun[9] != dict_hdr.VelFun[9])
      cdds_printf("usp_Ny_Rec", " %i\n", bin_hdr.VelFun[9]);
   if(bin_hdr.VelFun[10] != dict_hdr.VelFun[10])
      cdds_printf("usp_NyLine", " %i\n", bin_hdr.VelFun[10]);
   if(bin_hdr.VelFun[11] != dict_hdr.VelFun[11])
      cdds_printf("usp_Ny_Trc", " %i\n", bin_hdr.VelFun[11]);
   if(bin_hdr.VelFun[12] != dict_hdr.VelFun[12])
      cdds_printf("usp_MCTrSp", " %i\n", bin_hdr.VelFun[12]);
   if(bin_hdr.VelFun[13] != dict_hdr.VelFun[13])
      cdds_printf("usp_NumCmp", " %i\n", bin_hdr.VelFun[13]);
   if(bin_hdr.VelFun[14] != dict_hdr.VelFun[14])
      cdds_printf("usp_MCLE01", " %i\n", bin_hdr.VelFun[14]);
   if(bin_hdr.VelFun[15] != dict_hdr.VelFun[15])
      cdds_printf("usp_MCLE02", " %i\n", bin_hdr.VelFun[15]);
   if(bin_hdr.VelFun[16] != dict_hdr.VelFun[16])
      cdds_printf("usp_MCLE03", " %i\n", bin_hdr.VelFun[16]);
   if(bin_hdr.VelFun[17] != dict_hdr.VelFun[17])
      cdds_printf("usp_MCLE04", " %i\n", bin_hdr.VelFun[17]);
   if(bin_hdr.VelFun[18] != dict_hdr.VelFun[18])
      cdds_printf("usp_MCLE05", " %i\n", bin_hdr.VelFun[18]);
   if(bin_hdr.VelFun[19] != dict_hdr.VelFun[19])
      cdds_printf("usp_MCLE06", " %i\n", bin_hdr.VelFun[19]);
   if(bin_hdr.VelFun[20] != dict_hdr.VelFun[20])
      cdds_printf("usp_MCLE07", " %i\n", bin_hdr.VelFun[20]);
   if(bin_hdr.VelFun[21] != dict_hdr.VelFun[21])
      cdds_printf("usp_MCLE08", " %i\n", bin_hdr.VelFun[21]);
   if(bin_hdr.VelFun[22] != dict_hdr.VelFun[22])
      cdds_printf("usp_MCLE09", " %i\n", bin_hdr.VelFun[22]);
   if(bin_hdr.VelFun[23] != dict_hdr.VelFun[23])
      cdds_printf("usp_MCLE10", " %i\n", bin_hdr.VelFun[23]);
   if(bin_hdr.VelFun[24] != dict_hdr.VelFun[24])
      cdds_printf("usp_MCLE11", " %i\n", bin_hdr.VelFun[24]);
   if(bin_hdr.VelFun[25] != dict_hdr.VelFun[25])
      cdds_printf("usp_MCLE12", " %i\n", bin_hdr.VelFun[25]);
   if(bin_hdr.VelFun[26] != dict_hdr.VelFun[26])
      cdds_printf("usp_MCLE13", " %i\n", bin_hdr.VelFun[26]);
   if(bin_hdr.VelFun[27] != dict_hdr.VelFun[27])
      cdds_printf("usp_MCLE14", " %i\n", bin_hdr.VelFun[27]);
   if(bin_hdr.VelFun[28] != dict_hdr.VelFun[28])
      cdds_printf("usp_MCLE15", " %i\n", bin_hdr.VelFun[28]);
   if(bin_hdr.VelFun[29] != dict_hdr.VelFun[29])
      cdds_printf("usp_MCLE16", " %i\n", bin_hdr.VelFun[29]);
   if(bin_hdr.VelFun[40] != dict_hdr.VelFun[40])
      cdds_printf("usp_OrNSMP", " %i\n", bin_hdr.VelFun[40]);

   HDR_MOD_INT(usp_, HlhEnt)
   HDR_MOD_INT(usp_, HlhByt)

   return;
}
