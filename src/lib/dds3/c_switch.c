static char rcsid[]="$Id: c_switch.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, standard method to fetch a command line switch

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_switch(
   const char	*swtch,
   const int	dflt
   )
{
   static char	yes[]="1 y Y yes Yes YES t T true True TRUE";
   static char	no[] ="0 n N no No NO f F false False FALSE";

   DDS_API_ENTRY("switch");

   if (cdds_scanf(swtch, "") < 0) {
      DDS_API_EXIT(dflt);
   } else if (cdds_key("")  <  1) {
      DDS_API_EXIT((int)1);
   } else if (cdds_key(yes) >= 1) {
      DDS_API_EXIT((int)1);
   } else if (cdds_key(no)  >= 1) {
      DDS_API_EXIT((int)0);
   }

   DDS_API_EXIT(dflt);
}
