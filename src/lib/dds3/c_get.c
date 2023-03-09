static char rcsid[]="$Id: c_get.c 130 2010-01-08 22:59:37Z user $";
/*
RCS-t=DDS API, get byte(s)

*/

/**********************************************/
/***   Copyright (C) 2010                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 Jan 2010            ***/
/**********************************************/

#include "dds_sys.h"

int cdds_get(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   const void *buf,
   void *array,
   int count) 
{ 
  DDS_BIN *bin;  
  DDS_SYM *sym; 
  int num;
#if defined(_WIN32) || defined(MINGW)
  size_t *bias;
#else
  long *bias;  
#endif
  DDS_API_ENTRY("get") 
  BIN_TAG_TO_BIN(bin, (bin_tag), DDS_API_EXIT(0));  
  NON_NULL((buf), bin, DDS_API_EXIT(0));  
  NON_NULL((array), bin, DDS_API_EXIT(0));  
  NON_NEGATIVE((count), bin, DDS_API_EXIT(0));  
  SYM_TAG_TO_SYM(sym, bin, (sym_tag), DDS_API_EXIT(0));  
  SYM_TAG_TO_BIAS(bias, bin, (sym_tag), DDS_API_EXIT(0));  
  num = 0; 
  if(!sym->type) {  
    ddsDebug("cdds_get: type not defined\n\tbin=%s\n\tsymbol= %s\n",
             bin->name, sym->name ? sym->name : "<anonymous>"); 
  } if(*bias < 0) {  
    ddsDebug("cdds_get: offset not set\n\tbin=%s\n\tsymbol= %s\n",
             bin->name, sym->name ? sym->name : "<anonymous>"); 
  } else {  
    num = count;
    memcpy((char*)(array), (char*)(buf) + *bias, num);  
  }  
  DDS_API_EXIT(num); 
}
