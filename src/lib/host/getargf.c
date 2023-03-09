static char rcsid[]="$Id: getargf.c 50 2009-02-10 21:29:01Z user $";
/*
RCS-t=HOST, Fortran API, Command arguments and environment variable information.

*/
 
#include <stdio.h>
#include "chost.h"

#define MAX_ARG     8192
#if defined(_WIN32) || defined(MINGW)
#include <windows.h>
#else
#define HOST_ENV    environ
#endif
#undef  HOST_ARGC
#undef  HOST_ARGV

#if defined(HOST_SGI)
   #define HOST_ARGC     __Argc
   #define HOST_ARGV     __Argv
#elif defined(sun4SC1)
   #define HOST_ARGC     xargc
   #define HOST_ARGV     xargv
#elif defined(HOST_Sun)
   #define HOST_ARGC     __xargc
   #define HOST_ARGV     __xargv
#elif defined(HOST_freebsd) || (defined(__ia64__) && defined(CompLevel7))
   #define HOST_ARGC     xargc
   #define HOST_ARGV     xargv
#endif

#if defined(GNU) || defined(MINGW)
   #define iargc  _gfortran_iargc
   #define getarg _gfortran_getarg_i4
#elif (defined(HOST_freebsd) || defined(HOST_Linux))
   #define iargc   iargc_
   #define getarg  getarg_
#else
   #define iargc   iargc
   #define getarg  getarg
#endif



/*! get command line argument count */
int GetArgCmdC(void)
{
   #if defined(HOST_ARGC)
      extern int HOST_ARGC;
      return HOST_ARGC; 
   #else
      extern HOST_FTN_INT  iargc(void);
      return iargc();
   #endif
}


/*! get pointer to null terminated string, cmd line arg n */
const char *GetArgCmdV(int n)
{
   #if (defined(HOST_ARGC) && defined(HOST_ARGV))
      extern int HOST_ARGC;
      extern char **HOST_ARGV;

      if(0 <= n && n < HOST_ARGC) {
         return HOST_ARGV[n];
      }
   #else
      extern HOST_FTN_INT  iargc(void);
      extern HOST_FTN_INT  getarg(int *, char *, int);
      int i;
      static char arg[MAX_ARG+1];
      static char null_str[] = "";

      if(0 <= n && n <= iargc()) {
         getarg(&n, arg, MAX_ARG);
         for(i = MAX_ARG; 0 < i && ' ' == arg[i - 1]; i--) /* */ ;
         arg[i] = '\0';
         return arg;
      } else if(0 == n) {
         return null_str;
      }
   #endif
   return NULL;
}


/*! get environment variable count */
int GetArgEnvC(void)
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
    extern char **HOST_ENV;
    int i;

    for(i=0;HOST_ENV[i];i++) /* */ ;

    return i;
#endif
}

/*! get pointer to null terminated string, env variable n */
const char *GetArgEnvV(int n)	
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
   extern char **HOST_ENV;
   int i;

   for(i=0;i<n && HOST_ENV[i];i++) /* */ ;

   if(HOST_ENV[i]) return HOST_ENV[i];
       return NULL;
#endif
}

/*! Test routine used by Ftest1.F */
HOST_FTN_INT  HOST_FTN_FUNC(host_linkf, HOST_LINKF)(HOST_FTN_INT  *j)
{
   int i, n;
   const char *str;

   n = GetArgCmdC();
   fprintf(stderr,"argc=%d\n",n);
   for(i=1;;i++) {   
      str = GetArgCmdV(i);
      if (!str) break;
      fprintf(stderr,"arg[%d]=%s\n",i,str);
   }

#ifdef CRAZY_HORSE
   /* Big chief Crazy Horse will try anything once. */
   extern char **HOST_ENV;

   extern int argc;
   extern char **argv;
   extern int _argc;
   extern char **_argv;
   extern int __argc;
   extern char **__argv;

   extern int xargc;
   extern char **xargv;
   extern int _xargc;
   extern char **_xargv;
   extern int __xargc;
   extern char **__xargv;

   extern int Argc;
   extern char **Argv;
   extern int _Argc;
   extern char **_Argv;
   extern int __Argc;
   extern char **__Argv;

   if( argc || _argc || __argc ||
      xargc || _xargc || __xargc ||
      Argc || _Argc || __Argc)
   fprintf(stderr, "host_linkf: test\n");

#endif /* CRAZY_HORSE */
   return 0;
}
