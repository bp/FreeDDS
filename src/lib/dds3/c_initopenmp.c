static char rcsid[]="$Id: c_initopenmp.c 234 2011-08-06 08:55:25Z user $";
/*
RCS-t=DDS API, initialize OpenMP; this allows changing availability of
OpenMP in this one routine instead of in each application.

*/

/**********************************************/
/***   Copyright (C) 2006                   ***/
/***   BP America, Inc, Houston, Texas      ***/
/***   Jerry Ehlers, 24 FEB 2006            ***/
/**********************************************/

#ifdef OpenMP
   extern void omp_set_num_threads(int);
   #ifdef MLIB
      extern void mlib_setnumthreads(int*);
   #endif
#endif

#include "dds_sys.h"

int cdds_initopenmp()
{
   int nproc;

   DDS_API_ENTRY("initopenmp")


   nproc = 1;

#ifdef OpenMP
   cdds_dict("par:", "scan");
   cdds_scanf("nproc", "%d", &nproc);
   omp_set_num_threads(nproc);
#ifdef MLIB
   mlib_setnumthreads(&nproc);
#endif
#endif

   DDS_API_EXIT(nproc);
}
