static char rcsid[]="$Id: getargc.c 50 2009-02-10 21:29:01Z user $";
/*
HOST, C API, Command line arguments and environment variable information.
*/

#if defined(_WIN32) || defined(MINGW)
#include <windows.h>
#else
extern char **environ; /*!< system environmental variables array */
#endif
#include <stdio.h>


static int setargcv_initialized = 0;
static char dummy_argv0[] = "?";
static const char *dummy_argv[]= {dummy_argv0, 0};
static int _SetArgCmdC = 1;
static const char **_SetArgCmdV = dummy_argv;

int xargc = 1;
const char **xargv = dummy_argv;

int setargcv(
   int argc,		/*!< number of command line arguments */
   char *argv[])	/*!< command line argument values */
{
   /*! Set command line argument count and values */
   /*! Must be executed on some hosts prior to getting cmd line args */
   setargcv_initialized = 1;
   _SetArgCmdC = argc;
   _SetArgCmdV = (const char**)argv;
   xargc = argc;
   xargv = (const char**)argv;

   return 0;
}

int GetArgCmdC(
   void)
{
   /*! get command line argument count */
   if(! setargcv_initialized) {
      fprintf(stderr, "GetArgCmdC: application bug, cmd line not initialized.\n");
      fprintf(stderr, "\tIf the 'main' is Fortran, link with libfhost.\n");
      fprintf(stderr, "\tIf 'C', call setargcv and continue linking with libchost.\n");
      setargcv_initialized = 1;
   }
   return _SetArgCmdC;

}

const char *GetArgCmdV(
   int n)		/*!< command line argument number */
{
   /* get pointer to null terminated string, cmd line arg n */
   if(! setargcv_initialized) {
      fprintf(stderr, "GetArgCmdV: application bug, cmd line not initialized.\n");
      fprintf(stderr, "\tIf the 'main' is Fortran, link with libfhost.\n");
      fprintf(stderr, "\tIf 'C', call setargcv and continue linking with libchost.\n");
      setargcv_initialized = 1;
   }
   if(0 <= n && n < _SetArgCmdC) return _SetArgCmdV[n];
   return NULL;

}

/*! get environment variable count */
int GetArgEnvC(
   void)
{
#if defined(_WIN32) || defined(MINGW)
   LPTSTR lpszVariable;
   LPVOID lpvEnv;

   /* Get a pointer to the environment block. */
   lpvEnv = GetEnvironmentStrings();

   if (lpvEnv == NULL)
      return 0;
   /* Variable strings are separated by NULL byte, and the block is terminated by a NULL byte. */
   int i = 0;
   lpszVariable = (LPTSTR)lpvEnv;
   while (*lpszVariable) {
      i++;
      lpszVariable += lstrlen(lpszVariable) + 1;
   }
   return i;
#else
   int i;

   for (i = 0; environ[i]; i++) /* */;

   return i;
#endif
}

/*! get pointer to null terminated string, env variable n */
const char *GetArgEnvV(
   int n)		/*!< environment variable number */
{
#if defined(_WIN32) || defined(MINGW)
   LPTSTR lpszVariable;
   LPVOID lpvEnv;

   /* Get a pointer to the environment block. */
   lpvEnv = GetEnvironmentStrings();

   if (lpvEnv == NULL)
      return 0;
   /* Variable strings are separated by NULL byte, and the block is terminated by a NULL byte. */
   int i = 0;
   lpszVariable = (LPTSTR)lpvEnv;
   while (*lpszVariable) {
      if (i == n) {
         return lpszVariable;
      }
      i++;
      lpszVariable += lstrlen(lpszVariable) + 1;
   }
   return NULL; 
#else
   int i;

   for (i = 0; i < n && environ[i]; i++) /* */;

   if (environ[i]) return environ[i];
   return NULL;
#endif
}
