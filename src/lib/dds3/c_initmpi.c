static char rcsid[]="$Id: c_initmpi.c 199 2010-11-04 17:14:10Z user $";
/*
RCS-t=DDS API, initialize MPI, setup parms, and return node & nodes.

*/

/**********************************************/
/***   Copyright (C) 2006                   ***/
/***   BP America, Inc, Houston, Texas      ***/
/***   Jerry Ehlers, 24 FEB 2006            ***/
/**********************************************/

#include "dds_sys.h"
#ifdef MPI
#include "mpi.h"
#endif

/* initialize MPI when available */
int cdds_initmpi(
   int *node,
   int *nodes
)
{
#ifdef MPI
   int mpi_initialized;
   int argc, n, i, ndfn, nval, np;
   int GetArgCmdC();
   char **argv, *dfn, *val;
   const char *defn;
   const char *GetArgCmdV();

   DDS_API_ENTRY("initmpi")

   /* 
    * Initialize MPI 
    */
   argc = GetArgCmdC();
   argv = (char**)malloc(argc*sizeof(char*));
   for(i=0;i<argc;i++) argv[i] = (char*)GetArgCmdV(i);

   MPI_Initialized(&mpi_initialized);
   if ( ! mpi_initialized ) MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, node);
   MPI_Comm_size(MPI_COMM_WORLD, nodes);

   gio_mpi_node= *node;

   MPI_Barrier(MPI_COMM_WORLD);
   cdds_prtcon("starting node %3d of %d...\n", *node, *nodes);
   MPI_Barrier(MPI_COMM_WORLD);

   /* 
    * Pass Master par: to Slaves
    * (Must insure they are identical!)
    */
   if (*node == 0 && *nodes > 1) {
      /* master node send par: definitions */
      cdds_dict("par:", "scan");
      defn = cdds_newer("");
      while (defn) {
         n = 1+strlen(defn);
         MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
         MPI_Barrier(MPI_COMM_WORLD);
         MPI_Bcast((char*)defn, n, MPI_CHAR, 0, MPI_COMM_WORLD);
         n = 0;
         if (cdds_scanv("", &defn) > 0) n = 1+strlen(defn);
         MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
         if (n > 1) {
            MPI_Bcast((char*)defn, n, MPI_CHAR, 0, MPI_COMM_WORLD);
         }
         defn = cdds_newer("");
      }
      n = 0;
      MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
   } else if (*nodes > 1) {
      /* slave nodes receiver definitions so to "see" same exact
         characters */
      ndfn = nval = 1000;
      dfn = (char*)malloc(ndfn);
      val = (char*)malloc(nval);
      cdds_dict("par:", "reset");
      cdds_dict("par:", "print");
      MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
      while (n > 0) {
         if (n > ndfn) {
            ndfn = n + 100;
            dfn = (char*)realloc(dfn, (size_t)ndfn);
         }
         MPI_Barrier(MPI_COMM_WORLD);
         MPI_Bcast(dfn, n, MPI_CHAR, 0, MPI_COMM_WORLD);
         MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
         if (n >= nval) {
            nval = n + 100;
            val = (char*)realloc(val, (size_t)nval);
         }
         if (n > 0) {
            MPI_Bcast(val, n, MPI_CHAR, 0, MPI_COMM_WORLD);
         }
         val[n] = '\0';
         cdds_printv(dfn, val);
         MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
      }
      cdds_dict("par:", "scan");
   }
   cdds_scanf("np", "%d", &np);
#else
   int np;

   DDS_API_ENTRY("initmpi")

   *node = 0;
   *nodes = 1;
   cdds_dict("par:", "scan");
   np = 1;
   cdds_scanf("np", "%d", &np);
   if (np != 1) {
      cdds_prterr("MPI not available, cannot use 'np' > 1\n");
   }
#endif

   DDS_API_EXIT(DDS_OK);
}

/* initialize MPI when available and np > 1 */
int cdds_initmpix(
   int *node,
   int *nodes
)
{
   int np;

   DDS_API_ENTRY("initmpix")

   *node = 0;
   *nodes = 1;
   np = 1;
   cdds_dict("par:", "scan");
   cdds_scanf("np", "%d", &np);
   if (np != 1) {
#ifdef MPI
      cdds_initmpi(node,nodes);
#else
      cdds_prterr("MPI not available, cannot use 'np' > 1\n");
#endif
   }

   DDS_API_EXIT(DDS_OK);
}
