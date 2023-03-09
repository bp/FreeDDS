static char rcsid[]="$Id: c_print.c 306 2017-01-20 20:55:43Z user $";
/*
  RCS-t=DDS, create and control printing to a unique print file 

*/

/**********************************************/
/***   Copyright (C) 2006                   ***/
/***   BP America, Inc, Houston, Texas      ***/
/***   Jerry Ehlers, September 2006         ***/
/**********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#if defined(_WIN32) || defined(MINGW)
#include <io.h>
#else
#include <unistd.h>
#endif
#include <stdarg.h>
#include <sys/types.h>
#include "dds_sys.h"
#include "dds_time.h"

/* Use "SId: ...$" from svn_version.c for each application */
/* J. Ehlers - Jan 2009                                 */
extern const char SVN_ID[];

#if defined(_WIN32) || defined(MINGW)
#if (defined(stderr) && !defined(HOST_Linux))
/* in stdio.h: #define stderr &__iobuf[2] */
static FILE	*file = NULL;
#define	INIT_FILE() {if(file==NULL)file=stderr;}
#else
/* in stdio.h: extern FILE *stderr; */
static FILE	*file = NULL;
#define	INIT_FILE() {if(file==NULL)file=stderr;}
#endif
#else
#if (defined(stderr) && !defined(HOST_Linux) && !defined(HOST_Macos))
/* in stdio.h: #define stderr &__iobuf[2] */
static FILE	*file = stderr;
#define	INIT_FILE()
#else
/* in stdio.h: extern FILE *stderr; */
static FILE	*file = NULL;
#define	INIT_FILE() {if(file==NULL)file=stderr;}
#endif
#endif

#define hlen 256
static DDS_STRING str = NULL_STRING;
static int errors = 0;
static int return_status;
static int closing = 0;
static int slice_level = 0;
static char slice_mode[2];
static int InfoEnable = DDS_FALSE;
static int fd_stderr;
static char *prog = NULL;
static char Host[hlen+1];
static FILE *file_stderr;
static size_t bsize = 256;
static char filebuf[256], stdoutbuf[256], stderrbuf[256], filestderrbuf[256];
static time_t calendar_time;
#if !defined(_WIN32) && !defined(MINGW)
static struct sigaction		action;
static struct sigaction		int_action;
static struct sigaction		quit_action;
static struct sigaction		ill_action;
static struct sigaction		abrt_action;
static struct sigaction		fpe_action;
static struct sigaction		bus_action;
static struct sigaction		segv_action;
static struct sigaction		term_action;
#endif
static char *codewarn = (char*)0;

static char	BORDER[]=
"***************************************************************************\n";
static char	EMPTY[]=
"*                                                                         *\n";
static char	AMOCO[]=
"*                           B P     A M E R I C A                         *\n";
static char	PROG[]="* Program : %-61s *\n";
static char	WARN[]="* WARNING : %-61s *\n*               ------------ USE AT YOUR OWN RISK! -------------          *\n";
static char	WRN2[]="* WARNING : %s\n*               ------------ USE AT YOUR OWN RISK! -------------\n";
static char	REV[] ="* Revision: %-61s *\n";
static char	DREV[]="* DDS Rev : %-61s *\n";
static char	APP[] ="* Binary  : %-61s *\n";
static char	PID[] ="* Prc Id  : %-61i *\n";
static char	DAT[] ="* Date    : %-61s *\n";
static char	AUT[] ="* Author  : %-61s *\n";
static char	USER[]="* User: %-65s *\n";
static char	HOST[]="* Host: %-65s *\n";
static char	CWD[] ="* Cwd : %-65s *\n";
#if !defined(_WIN32) && !defined(MINGW)
static char     DATE[]="* Date: %-65s *\n";
#endif
static char	BLANK[]=
"*                                                                          \n";
static char	XNORMAL[]= "NORMAL Termination";
static char	ABNORMAL[]= "!!!ABNORMAL TERMINATION!!!";
static char	OUTOFDATE[]=
"This code is OUT-OF-DATE";
static char	MODIFIED[]=
"This code is NOT under Revision Control";
static char	TSTAPPS[]=
"This is test code under '/tstapps/global'";


/***********************************************************************
 *
 *	timestamp
 *
 **********************************************************************/
static void timeprint(FILE *file, char *msg)
{
   char *env, *date;
   time_t newtime;
   static int  EOL = 1;
   static int  stamp = -2;
   static time_t oldtime;

   if (stamp == -2) {
      stamp = -1; 
      env = getenv("DDS_TIMESTAMP");
      if (env) {
         stamp = 0; 
         sscanf(env, "%d", &stamp); 
         oldtime = time(NULL) - stamp;
      }
   } 

   if (stamp == 0 && EOL) {
      newtime = time(NULL);
      date = (char*)ddsString(ctime(&newtime));
      date += 4;
      date[strlen(date)-6] = '\0';
      fprintf(file, "[%s] ", date);
      date -= 4;
      ddsFree(date);
   } else if (stamp > 0) {
      newtime = time(NULL);
      if (newtime - oldtime >= stamp) {
         oldtime = newtime;
         date = ctime(&newtime);
         fprintf(file, date);
      }
   }

   EOL = (msg[strlen(msg)-1] == '\n');
   fprintf(file, "%s", msg);
}


/***********************************************************************
 *
 *	PrintCenter
 *
 **********************************************************************/
static void PrintCenter(const char *line, const char *msg)
{
   int l, n, m;
   char *text;

   l = strlen(line);
   m = strlen(msg);
   if (m > l) m = l;
   n = (l - m)/2;
   text = (char*)ddsMalloc(1+l+m);

   strcpy(text, line);
   strncpy(&text[n], msg, m);
   
   fprintf(file, text);
   fflush(file);

   ddsFree(text);

   return;
}


/***********************************************************************
 *
 *	CheckParm
 *
 **********************************************************************/
static int CheckParm()
{
   int count=0;
   char *start, *text, *ptr;
   const char 	*defn;

   count = 0;
   ddsParCheck(0);

   fprintf(file, "*\n");
   cdds_dict("par:", "scan");
   while(defn = cdds_newer("")) {
      if (strncmp(defn, "dds_par",7) && strcmp(defn, "cmd_args") && 
          strcmp(defn, "par")  && strcmp(defn, "_mpi_dummy")) { 
         if (dds_scan_defn->status!=0) continue;
         if (!count) fprintf(file, "*\n*      Unused Parameters:\n");
         count++;
         fprintf(file, "*         %s=", defn);
         if (cdds_scanv("", &defn) <= 0) {
            fprintf(file, "\n");
            continue;
         }
         start = text = (char*)ddsString(defn);
         while (ptr = strchr(text, '\n')) {
            *ptr = '\0';
            fprintf(file, "%s\n", text);
            text = ptr + 1;
            if (*text) fprintf(file, "*            ");
         }
         if (*text) fprintf(file, "%s\n", text);
         ddsFree(start);
      }
   }
   fflush(file);
   
   ddsParCheck(1);

   return(count);
}


/***********************************************************************
 *
 *	PrintParm
 *
 **********************************************************************/
static void PrintParm()
{
   char		*text, *start, *ptr, *par;
   const char 	*defn;

   par = NULL;
   ddsParCheck(0);

   PrintCenter(BORDER, " PARAMETERS ");
   fprintf(file, "*\n");
   cdds_dict("par:", "scan");
   while(defn = cdds_newer("")) {
      if (!strcmp(defn, "dds_par")) {
         cdds_scant("", &defn);
         par = (char*)defn;
         fprintf(file, "*\n*   --- par(%s) ---\n", par);
      } else if (!strcmp(defn, "dds_par_end")) {
         fprintf(file, "*   --- end par(%s) ---\n*\n", par);
         par = NULL;
      } else if (!strcmp(defn, "cmd_args") && !par) {
         if (cdds_scank("", "") <= 0) continue;
         cdds_scanv("", &defn);
         fprintf(file, "*\n*   --- non-DDS arguments ---\n");
         start = text = (char*)ddsString(defn);
         while (ptr = strchr(text, '\n')) {
            *ptr = '\0';
            fprintf(file, "%s\n", text);
            text = ptr + 1;
            if (*text) fprintf(file, "*      ");
         }
         if (*text) fprintf(file, "%s\n", text);
         ddsFree(start);
      } else {
         if (par) fprintf(file, "*      %s= ", defn);
         else fprintf(file, "*   %s= ", defn);
         if (cdds_scanv("", &defn) <= 0) {
            fprintf(file, "\n");
            continue;
         }
         start = text = (char*)ddsString(defn);
         while (ptr = strchr(text, '\n')) {
            *ptr = '\0';
            fprintf(file, "%s\n", text);
            text = ptr + 1;
            if (*text) {
               if (par) fprintf(file, "*         ");
               else fprintf(file, "*      ");
            }
         }
         if (*text) fprintf(file, "%s\n", text);
         ddsFree(start);
      }
   }
   fprintf(file, "*\n%s", BORDER);

   ddsParCheck(1);

   return;
}

/***********************************************************************
 *
 *	Monitor
 *
 **********************************************************************/
void Monitor()
{
   int fd, i, n, m, ss, sv, tmin;
   int BSIZE=1024;
   int ndefn=0;
   time_t cal_time;
#if !defined(_WIN32) && !defined(MINGW)
   struct flock flk;
#endif
   char *date, *dat, *user, *monpth, *app, **defns, **values;
   char *defaults, *r, *s, *monlog;
   const char *defn;

   /* if any error skip any monitor logging */
   if (errors) return;

   /* check for monitor path; else skip */
   monpth = getenv("DDS_MONITOR");
   if (!monpth) return;

   /* check for a application; else skip */
   app = getenv("APP");
   if (!app) app = (char*)dds_cmd_name;
   if (!app) return;

   /* check for a monitor log file; else skip */
   s = strrchr(app,'/');
   if (s) s++;
   else s = app;
   r = strstr(s,".rev");
   if (r) *r='\0';
   monlog = (char*)ddsMalloc(6+strlen(monpth)+strlen(s));
   sprintf(monlog,"%s/%s.log", monpth, s);
   fd = open(monlog, O_RDONLY);
   if (fd < 0) {
      ddsFree(monlog);
      return;
   }
     
   /* get the needed parameters with defaults */
   defaults = (char*)ddsMalloc(BSIZE);
   m = 0;             
   n = BSIZE;
   while(n == BSIZE) {
      defaults = (char*)ddsRealloc(defaults, n+m+1);
      n = read(fd, &defaults[m], n);
      defaults[m+n] = '\0';
      for(i=0;i<n;i++) if (defaults[i+m] == '\n') {
         n = i;
         break;
      }
      m += n;
   }
   close(fd);

   /* digest the tmin & parms with defaults */
   ndefn = 0;
   defns = (char**)ddsMalloc(sizeof(char*));
   values = (char**)ddsMalloc(sizeof(char*));
   tmin = -1;
   ss = -1;
   for (i=0;i<m;i++) {
      if (defaults[i] == '=') {
         if (ss > 0) defaults[ss-1] = '\0';
         if (ss >= 0) {
            if (ndefn && ss > 0) {
               values[ndefn-1] = (char*)ddsMalloc(1+strlen(&defaults[sv]));
               sscanf(&defaults[sv], "%s", values[ndefn-1]);
            }
            sv = i+1;
            ndefn++;
            defns = (char**)ddsRealloc(defns,ndefn*sizeof(char*));
            values = (char**)ddsRealloc(values,ndefn*sizeof(char*));
            defaults[i] = '\0';
            defns[ndefn-1] = (char*)ddsMalloc(1+strlen(&defaults[ss]));
            strcpy(defns[ndefn-1],&defaults[ss]);
            values[ndefn-1] = '\0';
         }
         ss = -1;
      } else if (isspace(defaults[i])) {
         if (tmin < 0) {
            tmin = 30*60;
            if (ss > 0) sscanf(defaults, "%d", &tmin);
         }
         ss = -1;
      } else if (ss < 0) {
         ss = i;
      }
   }
   if (ndefn && ss > 0) {
      defaults[ss-1] = '\0';
      values[ndefn-1] = (char*)ddsMalloc(1+strlen(&defaults[sv]));
      sscanf(&defaults[sv], "%s", values[ndefn-1]);
   }

   /* Determine the used time; skip if < tmin */
   cal_time = time(NULL);
   date = (char*)ddsString(ctime(&cal_time));
   if (s = strchr(date, '\n')) *s = '\0';
   cal_time -= calendar_time;
   if (cal_time < tmin) {
      for (i=0;i<ndefn;i++) {
         ddsFree(defns[i]);
         if (values[i]) ddsFree(values[i]);
      }
      ddsFree(date);
      ddsFree(monlog);
      ddsFree(defaults);
      ddsFree(defns);
      ddsFree(values);
      return;
   }

   /* get user */
#if defined(_WIN32) || defined(MINGW)
   user = getenv("LOGNAME");
#else
   user = getlogin();
#endif
   if (user == NULL) user = getenv("USER");
   if (user == NULL) user = getenv("LOGNAME");
   if (user == NULL) {
      user = (char*)ddsMalloc(2);
      strcpy(user, "?");
   }

#if defined(_WIN32) || defined(MINGW)
   if (fd >= 0) {
#else
   /* add log entry of parameters */
   /* scan defns: if scan:=<file>, scan par: for <file> 
      and then scan <file> */
   flk.l_type = F_WRLCK;
   flk.l_whence = SEEK_END;
   flk.l_start = 0;
   flk.l_len = 0;
   fd = open(monlog, O_WRONLY);
   if (fd >= 0 && fcntl(fd, F_SETLK, &flk) != -1) {
#endif
      defaults = (char*)ddsRealloc(defaults,64+strlen(app)+strlen(date)+strlen(user));
      sprintf(defaults,"%s %s user: %s time(sec): %d mem(Mb): %d in(MSmples): %d out(MSmples): %d",
              app,date,user,(int)cal_time,(int)(dds_mem_max/(1024*1024)),
              (int)(dds_samples_in/(1024*1024)), (int)(dds_samples_out/(1024*1024)));
      lseek(fd, 0, SEEK_END);
      write(fd, defaults, strlen(defaults));

      if (ndefn > 0) write(fd, " parms: ", 8);

      cdds_dict("par:","scan");
      for(i=0;i<ndefn;i++) {
         if (!strcmp(defns[i],"scan:")) {
            cdds_dict("par:","scan");
            cdds_scant(defns[i], &defn);
            defns[i] = (char*)ddsRealloc(defns[i],1+strlen(defn));
            strcpy(defns[i], defn);
            cdds_dict(defns[i], "scan");
         } else if (cdds_scant(defns[i], &defn)>0) {
            defaults = (char*)ddsRealloc(defaults,2+strlen(defn));
            sprintf(defaults,"%s ",defn);
            write(fd, defaults, strlen(defaults));
         } else if(values[i]) {
            defaults = (char*)ddsRealloc(defaults,2+strlen(values[i]));
            sprintf(defaults,"%s ",values[i]);
            write(fd, defaults, strlen(defaults));
         }
      }
      write(fd, "\n", 1);
   }

   if (fd >= 0) close(fd);
   for(i=0;i<ndefn;i++) {
      ddsFree(defns[i]);
      if (values[i]) ddsFree(values[i]);
   }
   ddsFree(date);
   ddsFree(monlog);
   ddsFree(defaults);
   ddsFree(defns);
   ddsFree(values);
}


/***********************************************************************
 *
 *	Closepr
 *
 **********************************************************************/
static void Closepr()
{
   int unused;
   time_t	cal_time;
   char		*date, *msg;

   closing = 1;
   InfoEnable = DDS_FALSE;

   /* Determine the time & status */

   cal_time = time(NULL);
   date = (char*)ddsString(ctime(&cal_time));
   if (msg = strchr(date, '\n')) *msg = '\0';

   /* Print status */

   if (errors) {
      msg = ABNORMAL;
   } else {
      msg = XNORMAL;
   }
   PrintCenter(BORDER, " SUMMARY ");
   fprintf(file, "*\n");
   PrintCenter(BLANK, msg);

   /* Print any code warnings */
   if (codewarn) {
      fprintf(file, WRN2, codewarn);
      ddsFree(codewarn);
   }

   /* Print any unused par: definitions */
   unused = CheckParm();

   /* Print # of errors/warnings */

   if (errors || dds_warn_count || dds_error_count || dds_debug_count) {
      fprintf(file, "*\n");
   }
   if (errors) {
      fprintf(file, "*      API Errors  : %d\n", errors);
   }
   if (dds_warn_count) {
      fprintf(file, "*      DDS Warnings: %d\n", dds_warn_count);
   }
   if (dds_error_count) {
      fprintf(file, "*      DDS Errors  : %d\n", dds_error_count);
   }
   if (dds_debug_count) {
      fprintf(file, "*      DDS Debugs  : %d\n", dds_debug_count);
   }

   /* Print time */

   fprintf(file, "*\n*      %s\n*\n%s\n", date, BORDER);

   /* Print on command line if print file opened	*/

   if (dds_openpr && file_stderr != file) {
      if (!errors && unused) {
         msg = (char*)ddsMalloc(32+strlen(XNORMAL));
         sprintf(msg, "%s (%d unused parameters)", XNORMAL, unused);
      }
      else {
         msg = (char*)ddsString(msg);
      }
      if (prog) {
         fprintf(file_stderr, "%s: %s %s\n", prog, date, msg);
      } else {
         fprintf(file_stderr, "%s %s\n", date, msg);
      }
      ddsFree(msg);
   }

   /*	restore files	*/

   fflush(file);
   if (dds_openpr && fd_stderr >= 0) {
#if !defined(_WIN32) && !defined(MINGW)
      /* can not close stderr twice */
      dup2(fd_stderr, 2);
      close(fd_stderr);
#endif
      fclose(file);
      file = file_stderr = stderr;
   }
   dds_openpr = DDS_FALSE;

   /* update any monitor logs (turn off debug messages) */
   dds_state &= (DDS_STATE) ~ (DDS_VERBOSE | DDS_DEBUG_ABORT |
                                DDS_DEBUG_PRINT | DDS_DEBUG_LOG);
   Monitor();

   /* The exit argument is implementation dependent, i.e. */
   /* it may return only the low order byte (zero if 256, etc.) */
   /* POSIX recommends this... */
   exit(return_status ? EXIT_FAILURE : EXIT_SUCCESS);
}


/***********************************************************************
 *
 *	signal_handler
 *
 **********************************************************************/
static void signal_handler(int sig)
{
   char *msg;

   /* skip error messages if already closing down */
   if (closing) return;

   /* turn off any more heapchecks */

   dds_debug_api = (DDS_DEBUG)(dds_debug_api 
      & ~ (DDS_DBG_CALL | DDS_DBG_FREE | DDS_DBG_ALLOC));

#if defined(_WIN32) || defined(MINGW)
   if (sig == SIGINT) msg = "Signal Interrupt...\n";
/*   else if (sig == SIGQUIT) msg = "'openpr' not called...\n";*/
   else if (sig == SIGILL) msg = "Illegal instruction...\n";
   else if (sig == SIGABRT) msg = "Aborting...\n";
   else if (sig == SIGFPE) msg = "Arithmetic exception...\n";
//   else if (sig == SIGBUS) msg = "Bus Error...\n";
   else if (sig == SIGSEGV) msg = "Segmentation Violation...\n";
   else if (sig == SIGTERM) msg = "Software Termination...\n";
   else {
      msg = malloc(32);
      sprintf(msg, "Unknown signal, %d?!\n", sig);
   }
#else
   /*	retore signal actions	*/

   sigaction(SIGINT, &int_action, NULL);
   sigaction(SIGQUIT, &quit_action, NULL);
   sigaction(SIGILL, &ill_action, NULL);
   sigaction(SIGABRT, &abrt_action, NULL);
   sigaction(SIGFPE, &fpe_action, NULL);
   sigaction(SIGBUS, &bus_action, NULL);
   sigaction(SIGSEGV, &segv_action, NULL);
   sigaction(SIGTERM, &term_action, NULL);

   /* determine type of signal */

   if (sig == SIGINT) msg = "Signal Interrupt...\n";
   else if (sig == SIGQUIT) msg = "'openpr' not called...\n";
   else if (sig == SIGILL) msg = "Illegal instruction...\n";
   else if (sig == SIGABRT) msg = "Aborting...\n";
   else if (sig == SIGFPE) msg = "Arithmetic exception...\n";
   else if (sig == SIGBUS) msg = "Bus Error...\n";
   else if (sig == SIGSEGV) msg = "Segmentation Violation...\n";
   else if (sig == SIGTERM) msg = "Software Termination...\n";
   else {
      msg = malloc(32);
      sprintf(msg, "Unknown signal, %d?!\n", sig);
   }
#endif
   /* print the error message */

   errors++;
   if (dds_openpr) {
      fprintf(file, "ERROR: %s\n", msg);
      if (fd_stderr >= 0 && file_stderr != file) {
         if (prog) fprintf(file_stderr, "ERROR(%s): %s", prog, msg);
         else fprintf(file_stderr, "ERROR: %s", msg);
      }
   } else {
      if (prog) fprintf(stderr, "ERROR(%s) %s", prog, msg);
      else fprintf(stderr, "ERROR: %s", msg);
   }

   Closepr();

   return;
}


/***********************************************************************
 *
 *	cdds_openpr
 *
 **********************************************************************/
int cdds_openpr(const char *pname, const char *id)
{
   int ier, i, n;
   char	*filename, cmd[5];
   char	*value;
   STRUCTSTAT statbuf;

   DDS_API_ENTRY("openpr");

   INIT_FILE();

   setvbuf(stderr, stderrbuf, _IOLBF, bsize);

   if (dds_openpr) {
      cdds_prtcon("BUG: 'openpr' can only be called once!\n");
      DDS_API_EXIT((int)-1);
   }

   /*	Check if help requested	*/

   cdds_dict("par:", "scan");
   if (cdds_switch("help",0) != 0) {
      cdds_prthdr(pname, id);
      DDS_API_EXIT((int)1);
   }

   /*	Treat as help if any cmd_args	*/

   if(0 < cdds_scank("cmd_args", DDS_HELP_KEY)) {
      cdds_prthdr(pname, id);
      DDS_API_EXIT((int)2);
   }

   /*	Check if already open	*/

   if (file != stderr) {
      ddsWarn("openpr: printfile already open\n");
      DDS_API_EXIT((int)-2);
   }

   /* get the host name in caps */
#if defined(_WIN32) || defined(MINGW)
   value = getenv("COMPUTERNAME");
   if (value) {
      strcpy(Host, value);
      for (i = 0; i<strlen(Host); i++) Host[i] = toupper(Host[i]);
   } else {
      ddsWarn("can not gethostname\n");
   }
#else
   errno = 0;
   Host[0] = '\0';
   ier = gethostname(Host, hlen);
   if (ier) ddsWarn("gethostname=%d len=%d errno=%d\n", ier, strlen(Host), errno);   	
   for (i=0;i<strlen(Host);i++) Host[i] = toupper(Host[i]); 
#endif
   /*	Save program name in upper case	*/

   n = strlen(pname);
   prog = (char*)ddsMalloc(1+n);
   strcpy(prog, pname);
   for (i=0;i<n;i++) if (islower(prog[i])) prog[i] = toupper(prog[i]);

   /*	Create filename & open for printout	*/

   cdds_dict(dds_base_dict, "scan");
   value = (char*)0;
   if (cdds_scant("PRINT_PATH print_path", (const char**)&value) > 0) {
      if (strcmp(value,"/dev/null")==0) {
         filename = (char*)ddsMalloc(10);
         sprintf(filename, "%s", value);
      } else {
#if defined(_WIN32) || defined(MINGW)
         if (value[strlen(value)-1] == '/' ||
             value[strlen(value)-1] == '\\') {
#else
         if (value[strlen(value)-1] == '/') {
#endif
           *(value+strlen(value)-1) = '\0';
           ier = ddsSTAT(value, &statbuf);
           if (ier == -1 || !(S_ISDIR(statbuf.st_mode))) {
#if defined(_WIN32) || defined(MINGW)
             ier = CreateDirectory(value, NULL);
             if (ier == 0 && ERROR_ALREADY_EXISTS != GetLastError()) {
                cdds_prtcon("Directory creation error: %d",GetLastError());
             }
#else
             ier = mkdir(value,0755);
             if (ier == -1) {
                cdds_prtcon("%s",(char *)strerror(errno));
             }
#endif
           }
         }
         ier = ddsSTAT(value, &statbuf);
         if (ier != -1 && (S_ISDIR(statbuf.st_mode))) {
           filename = (char*)ddsMalloc(strlen(value)+1+strlen(prog)
                                     +strlen(Host)+16);
           sprintf(filename, "%s/%s_%s.%d", value, prog, Host, 
                 getpid());
         } else {
           filename = (char*)ddsMalloc(strlen(value)+1);
           sprintf(filename, "%s", value);
         }
      }
   } else if (cdds_scanf("PRINT_PATH print_path", "") == 0) {
      filename = (char*)0;
      file_stderr = stderr;
   } else {
      filename = (char*)ddsMalloc(strlen(prog)+strlen(Host)+16);
      sprintf(filename, "%s_%s.%d", prog, Host, getpid());
   }
   
   if (filename) {
      file = fopen(filename, "w");
      if (! file && value) {
         file = fopen(value, "w");
         if (! file) {
            file = stderr;
            ddsWarn("openpr: unable to open printfile %s or %s\n", 
                    filename, value);
            ddsFree(filename);
            DDS_API_EXIT((int)-1);
         }
         filename = (char*)ddsRealloc(filename, strlen(value)+1);
         strcpy(filename, value);
      }
      if (! file) {
         file = stderr;
         ddsWarn("openpr: unable to open printfile %s\n", filename);
         ddsFree(filename);
         DDS_API_EXIT((int)-1);
      }
      fprintf(stderr, "Printout file: %s\n", filename);
      ddsFree(filename);
      setvbuf(file, filebuf, _IOLBF, bsize);

      /*	Initialize open print file	*/
   
      if ((fd_stderr = dup(2)) == -1) {
         fclose(file);
         file = stderr;
         DDS_API_EXIT((int)-1);
      }

#if defined(_WIN32) || defined(MINGW)
      if (dup(fileno(file)) <0) {
#else
      if (dup2(fileno(file), 2) != 2) {
		  close(fd_stderr);
		  fclose(file);
#endif
         file = stderr;
         DDS_API_EXIT((int)-1);
      }

      if (!(file_stderr = fdopen(fd_stderr, "w"))) {
#if !defined(_WIN32) && !defined(MINGW)
         dup2(fd_stderr, 2);
         close(fd_stderr);
#endif
         fclose(file);
         file = stderr;
         DDS_API_EXIT((int)-1);
      }
      setvbuf(file_stderr, filestderrbuf, _IOLBF, bsize);
   }

   /*	Catch any terminations	*/

   dds_openpr = DDS_TRUE;
   errors = 0;
#if !defined(_WIN32) && !defined(MINGW)
   action.sa_handler = signal_handler;
   sigfillset(&action.sa_mask);
   sigaction(SIGINT, &action, &int_action);
   sigaction(SIGQUIT, &action, &quit_action);
   sigaction(SIGILL, &action, &ill_action);
   sigaction(SIGABRT, &action, &abrt_action);
   sigaction(SIGFPE, &action, &fpe_action);
   sigaction(SIGBUS, &action, &bus_action);
   sigaction(SIGSEGV, &action, &segv_action);
   sigaction(SIGTERM, &action, &term_action);
#endif
   /*	Print header	*/

   cdds_prthdr(pname, id);

   /*	Print par/cmd parameters	*/

   PrintParm();

   InfoEnable = DDS_TRUE;

   if (cdds_scanf("cmd_args", "%4s", cmd) > 0) {
      DDS_API_EXIT((int)2);
   }
   DDS_API_EXIT((int)0);
}


/***********************************************************************
 *
 *	cdds_savepr
 *
 **********************************************************************/
int cdds_savepr()
{
   DDS_API_ENTRY("savepr");

   INIT_FILE();

   if (dds_openpr && fd_stderr >= 0) {
      setvbuf(stdout, stdoutbuf, _IOLBF, bsize);
      dup2(fileno(file), 1);
      
      fd_stderr = -1;
   }

   DDS_API_EXIT((int)0);
}


/***********************************************************************
 *
 *	cdds_prthdr
 *
 **********************************************************************/
int cdds_prthdr(const char *prg, const char *id)
{
   int ier;
   char *tmp, *ID, *user, *date, *rev, *dat, *aut, *app;
   char host[hlen+1], blnk[]="";
   struct stat stat_buf;

   DDS_API_ENTRY("prthdr");

   INIT_FILE();

   ID = rev = dat = aut = (char*)0;
   
   /* Now use SVN_ID from svn_version.c from each app - Jan 2009 */
   if (SVN_ID && *SVN_ID) id = SVN_ID; 
   if (id) ID = (char*)ddsString(id);

   /*  Fetch the program revision # */
   if (ID) {
      rev = strchr(ID, ' ');
      if (rev) rev = strchr(rev+1, ' ');
      if (rev) rev++;
      
   }

   /*  Fetch the program date */
   if (rev) {
      dat = strchr(rev, ' ');
      if (dat) {
         *dat = 0;
         dat++;
      }
   }

   /*  Fetch the program author */

   if (dat) {
      aut = strchr(dat, ' ');
      if (aut) aut = strchr(aut+1, ' ');
      if (aut) {
         *aut = 0;
         aut++;
         tmp = strchr(aut, ' ');
         if (tmp) *tmp = 0;
      }
   }

   /*	Fetch the host */

#if defined(_WIN32) || defined(MINGW)
   tmp = getenv("COMPUTERNAME");
   if (tmp) {
      strcpy(host, tmp);
   } else {
      ddsWarn("can not gethostname\n");
   }
#else
   errno = 0;
   host[0] = '\0';
   ier = gethostname(host, hlen);
   if (ier) ddsWarn("gethostname=%d len=%d errno=%d\n", ier, strlen(host), errno);   	
#endif
   /* Fetch the date */
   calendar_time = time(NULL);
   tmp = ctime(&calendar_time);
   date = (char*)ddsString(tmp);
   if (tmp = strchr(date,'\n')) *tmp = '\0';

   /* Fetch the user name */
#if defined(_WIN32) || defined(MINGW)
   tmp = getenv("USERNAME");
#else
   tmp = getlogin();
#endif
   if (!tmp) tmp = getenv("USER");
   if (!tmp) tmp = getenv("LOGNAME");
   if (tmp) {
      user = (char*)ddsString(tmp);
   } else {
      user = (char*)ddsString("?");
   }

   /* Fetch the executable */
   tmp = getenv("APP");
   if (!tmp) tmp = (char*)dds_cmd_name;
   if (tmp) {
      app = (char*)ddsString(tmp);
   } else {
      app = (char*)ddsString("UNKNOWN");
   }

   if (dat) {
      dat = (char*)ddsString(dat);
   } else if (!stat(app, &stat_buf)) {
      tmp = ctime(&stat_buf.st_mtime);
      dat = (char*)ddsString(tmp);
      if (tmp = strchr(dat,'\n')) *tmp = '\0';
   }

   /* Save revision & code warning globally for Title */
#if !defined(_WIN32) && !defined(MINGW)
   if (*rev) {
      dds_cmd_rev = (char*)ddsString(rev);
      if (strchr(rev,'M') && SVN_ID && *SVN_ID) {
         codewarn = (char*)ddsString(OUTOFDATE);
      } else if (strstr(rev,"exported") && SVN_ID && *SVN_ID) {
         codewarn = (char*)ddsString(MODIFIED);
      }
   }
#endif

   /* Check tstapps for any code warning */
   
   if (!codewarn && app && !strncmp(app,"/tstapps/global/",16)) {
      codewarn = (char*)ddsString(TSTAPPS);
   }  

   /* print header info */

   fprintf(file, "\n");
   fprintf(file, BORDER);
   fprintf(file, EMPTY);
   fprintf(file, AMOCO);
   fprintf(file, EMPTY);
   fprintf(file, PROG, prg?prg:"?");
   fprintf(file, REV,  rev?rev:"?");
   fprintf(file, DREV, dds_cmd_ddsrev);
   fprintf(file, APP,  app);
   if (codewarn) fprintf(file, WARN, codewarn);
   fprintf(file, PID,  dds_cmd_pid);
   fprintf(file, DAT,  dat?dat:"?");
   fprintf(file, AUT,  aut?aut:"?");
   fprintf(file, BORDER);
   fprintf(file, USER, user);
   fprintf(file, HOST, host);
   fprintf(file, CWD,  dds_cmd_cwd);
#if !defined(_WIN32) && !defined(MINGW)
   fprintf(file, DATE, date);
#endif
   fprintf(file, BORDER);

   if (dds_openpr && file_stderr != file) {
      if (prog) {
         fprintf(file_stderr, "%s: %s START...\n", prog, date);
      } else {
         fprintf(file_stderr, "%s START...\n", date);
      }
   }

   fflush(file);

   ddsFree(app);
   ddsFree(date);
   ddsFree(user);
   if (ID) ddsFree(ID);
   if (dat) ddsFree(dat);

   DDS_API_EXIT((int)0);
}


/***********************************************************************
 *
 *	cdds_prtmsg
 *
 **********************************************************************/
int cdds_prtmsg(const char *fmt, ...)
{
   char* msg;
   va_list args;

   DDS_API_ENTRY("prtmsg");

   INIT_FILE();

   /*	Get the message	*/

   va_start(args, fmt);
   msg = ddsVsmprintf(fmt, args);
   va_end(args);

   timeprint(file, msg);

   fflush(file);

   DDS_API_EXIT((int)0);
}


/***********************************************************************
 *
 *	cdds_prtcon
 *
 **********************************************************************/
int cdds_prtcon(const char *fmt, ...)
{
   char *msg;
   va_list args;

   DDS_API_ENTRY("prtcon");

   INIT_FILE();

   /*	Get the message	*/

   va_start(args, fmt);
   msg = ddsVsmprintf(fmt, args);
   va_end(args);

   /*	Print each line	*/

   if (dds_openpr) {
      timeprint(file, msg);
      if (fd_stderr >= 0 && file_stderr != file) {
         if (prog) fprintf(file_stderr, "%s: %s", prog, msg);
         else fprintf(file_stderr, "%s", msg);
      }
   } else {
      if (prog) ddsStrCpyCat3(&str, prog, ": ", msg);
      else ddsStrCpy(&str, msg);
      timeprint(stderr, str.ptr);
   }

   fflush(file);

   DDS_API_EXIT((int)0);
}


/***********************************************************************
 *
 *	cdds_prterr
 *
 **********************************************************************/
int cdds_prterr(const char *fmt, ...)
{
   char *msg;
   va_list args;

   DDS_API_ENTRY("prterr");

   INIT_FILE();

   errors++;

   /*	Get the message	*/

   va_start(args, fmt);
   msg = ddsVsmprintf(fmt, args);
   va_end(args);

   /*	Print each line	*/

   if (dds_openpr) {
      ddsStrCpyCat3(&str, "ERROR: ", msg, "\n");
      timeprint(file, str.ptr);
      if (fd_stderr >= 0 && file_stderr != file) {
         if (prog) fprintf(file_stderr, "ERROR(%s): %s", prog, msg);
         else fprintf(file_stderr, "ERROR: %s", msg);
      }
   } else {
      if (prog) ddsStrCpyCat4(&str, "ERROR(", prog, ") ", msg);
      else ddsStrCpyCat2(&str, "ERROR: ", msg);
      timeprint(stderr, str.ptr);
   }

   fflush(file);

   DDS_API_EXIT((int)0);
}


/***********************************************************************
 *
 *	cdds_info
 *
 **********************************************************************/
int cdds_info(int enable)
{
   DDS_API_ENTRY("info");

   INIT_FILE();

   InfoEnable = enable;

   DDS_API_EXIT((int)0);
}


/***********************************************************************
 *
 *	cdds_errors
 *
 **********************************************************************/
int cdds_errors()
{
   DDS_API_ENTRY("errors");

   INIT_FILE();

   DDS_API_EXIT((int)errors);
}


/***********************************************************************
 *
 *	cdds_closepr
 *
 **********************************************************************/
int cdds_closepr()
{
   DDS_API_ENTRY("closepr");

   INIT_FILE();

   return_status = errors;

   Closepr();

   DDS_API_EXIT((int)0);
}


/***********************************************************************
 *
 *	cdds_closepr2
 *
 **********************************************************************/
int cdds_closepr2(int status)
{
   DDS_API_ENTRY("closepr2");

   INIT_FILE();

   return_status = status;

   Closepr();

   DDS_API_EXIT((int)0);
}


/***********************************************************************
 * 
 *	ddsSetSliceLevel
 *
 * Maintain slice level so to enable print info only for main binary
 *
 **********************************************************************/
void ddsSetSliceLevel(const char *mode, int adjust)
{
   if (!slice_level) {
      slice_mode[0] = mode[0];
      slice_mode[1] = mode[1];
   }
   slice_level += adjust;
}


/***********************************************************************
 * 
 *	ddsPrintCloseInfo
 *
 **********************************************************************/
void ddsPrintCloseInfo(DDS_BIN *bin)
{
   const char *mode;

   /*	Do not print info on buffers, 'm'	*/
   if (!InfoEnable || slice_level) return;
   if (bin->mode[0] == 'm' && bin->slice_max == 1) return;
   if (bin->slice_max > 1) {
      mode = slice_mode;
   } else {
      mode = bin->mode;
   }

   /*	Print cache size	*/
   if (bin->media && bin->media->fp) {
      if (mode[0] == 'a' || mode[1] == '+') {
         ddsVerbose("* final %s (fd=%d) read/write cache sizes=%d/%d\n", 
            bin->alias_name, bin->media->fp->fd,
            (int)bin->media->fp->cache_r,
            (int)bin->media->fp->cache_w);
      } else if (mode[0] == 'r') {
         ddsVerbose("* final %s (fd=%d) cache size=%d\n", 
            bin->alias_name, bin->media->fp->fd,
            (int)bin->media->fp->cache_r);
      } else {
         ddsVerbose("* final %s (fd=%d) cache size=%d\n", 
            bin->alias_name, bin->media->fp->fd,
            bin->media->fp->cache_w);
      }
   } 

	return;
}


/***********************************************************************
 * 
 *	ddsPrintInfo
 *
 **********************************************************************/
void ddsPrintInfo(DDS_BIN *bin)
{
   int i, rank;
   char text[256], prev[256], modify[8], *chr;
   SYM_TAG tag;
   const char *mode, *sort, *defn, *dict, *type;

   /*	Do not print info on buffers, 'm'	*/
   if (!InfoEnable || slice_level) return;
   if (bin->mode[0] == 'm' && bin->slice_max == 1) return;
   if (bin->slice_max > 1) {
      mode = slice_mode;
   } else {
      mode = bin->mode;
   }

   /*	Print ident header	*/
   if (mode[0] == 'a' || mode[1] == '+') {
      sprintf(text, " UPDATE: %s ", bin->alias_name);
   } else if (mode[0] == 'r') {
      sprintf(text, " INPUT: %s ", bin->alias_name);
   } else {
      sprintf(text, " OUTPUT: %s ", bin->alias_name);
   } 
        
   PrintCenter(BORDER, text);
   fprintf(file, "*\n");

   /*	Print dictionary, binary & format	*/
   dict = bin->dict_open;
   for (i=strlen(dict)-1;i>0;i--) if (!isspace(dict[i])) break;
   for (;i>=0;i--) if (isspace(dict[i])) break;
   i++;
   chr = (char*)ddsMalloc(1+strlen(&dict[i]));
   strcpy(chr, &dict[i]);
   for (i=strlen(chr)-1;i>0;i--) if (!isspace(chr[i])) break;
   i++;
   chr[i] = '\0';
   fprintf(file, "* dictionary=%s\n", chr);
   ddsFree(chr);
   if (strlen(bin->name) < 80) {
      fprintf(file, "* data=%s\n", bin->name);
   } else {
      chr = (char*)ddsMalloc(81);
      strncpy(chr, bin->name, 80);
      chr[80] = '\0';
      fprintf(file, "* data=%s...\n", chr);
      ddsFree(chr);
   }
   tag = cdds_member(bin->tag, 0, "Samples");
   if (tag >= 0) {
      tag = cdds_prime(bin->tag, tag);
      type = cdds_name(bin->tag, tag);
      fprintf(file, "* format=%s %s\n", bin->fmt_name, type);
   } else {
      fprintf(file, "* format=%s\n", bin->fmt_name);
   }

   /*	Get axis	 */
   rank = bin->cube.rank;

   fprintf(file, "*   axis  : ");
   for(i=0;i<rank;i++) fprintf(file, "%12s ", bin->cube.axis[i+1].name);
   fprintf(file, "\n");

   /*	Get axis size	*/
   fprintf(file, "*   size  : ");
   for(i=0;i<rank;i++) fprintf(file, "%12i ", (int)bin->cube.axis[i+1].size);
   fprintf(file, "\n");

   /*	Get axis delta	 */
   fprintf(file, "*   delta : ");
   for(i=0;i<rank;i++) fprintf(file, "%12g ", bin->cube.axis[i+1].delta);
   fprintf(file, "\n");

   /*	Get axis origin	 */
   fprintf(file, "*   origin: ");
   for(i=0;i<rank;i++) fprintf(file, "%12g ", bin->cube.axis[i+1].origin);
   fprintf(file, "\n");

   /*	Get axis units	*/
   fprintf(file, "*   units : ");
   for(i=0;i<rank;i++) fprintf(file, "%12s ", bin->cube.axis[i+1].units);
   fprintf(file, "\n");

   /*	Get axis sort	*/
   fprintf(file, "*   sort  : ");
   for(i=0;i<rank;i++) {
      sort = bin->cube.axis[i+1].sort_name;
      if (!sort || strlen(sort) < 1) fprintf(file, "           ? ");
      else fprintf(file, "%12s ", sort);
   }
   fprintf(file, "\n");

   /*	Get axis step	 */
   fprintf(file, "*   step  : ");
   for(i=0;i<rank;i++) fprintf(file, "%12g ", bin->cube.axis[i+1].step);
   fprintf(file, "\n");

   /*	Get axis base	 */
   fprintf(file, "*   base  : ");
   for(i=0;i<rank;i++) fprintf(file, "%12g ", bin->cube.axis[i+1].base);
   fprintf(file, "\n");

   /*	Print history for input files	*/
   if (mode[0] == 'r' && (dds_state & (DDS_VERBOSE | DDS_VERBOSE_LOG))) {
      fprintf(file, "*\n");
      fprintf(file, "*         --- HISTORY ---\n");

      prev[0] = '\0';
      cdds_dict(bin->dict_open, "scan");
      while(defn = cdds_newer("previous_history")) {
         if (cdds_scanv("", &defn) > 0) {
            if (*defn == '*') {
               if (defn[1]=='*') continue;
               if (!strstr(defn, "*CALL") && !strstr(defn, "*call")) 
                  continue;
            }
            strncpy(text, defn, 256);
            text[255] = '\0';
            chr = strchr(text, '\n');
            if (chr) *chr = '\0';
            if (strcmp(text, prev)) {
               strcpy(prev, text);
               fprintf(file, "*         %s\n", text);
            }
         }
      }

      cdds_dict(bin->dict_open, "scan");
      defn = cdds_newer("cmd_title");
      while(defn) {
         if (cdds_scant("", &defn) > 0) {
            strncpy(text, defn, 256);
            text[255] = '\0';
            modify[0] = '\0';
            if (cdds_scant("", &defn) > 0) {
               strncpy(modify, defn, 8);
               modify[7] = '\0';
            }
            defn = cdds_newer("cmd_title");
            if (defn && strcmp(modify, "modify") && strcmp(text, prev)) {
               strcpy(prev, text);
               fprintf(file, "*         %s\n", text);
            }
         } else defn = cdds_newer("cmd_title");
      }
   }
   fprintf(file, "*\n");

   /*	Print cache size	*/
   if (bin->media && bin->media->fp) {
      if (mode[0] == 'a' || mode[1] == '+') {
         ddsVerbose("* initial %s (fd=%d) read/write cache sizes=%d/%d min=%d/%d max=%d/%d\n", 
            bin->alias_name, bin->media->fp->fd,
            (int)bin->media->fp->cache_r,
            (int)bin->media->fp->cache_w,
            (int)bin->media->fp->min_r,
            (int)bin->media->fp->min_w,
            (int)bin->media->fp->max_r,
            (int)bin->media->fp->max_w);
      } else if (mode[0] == 'r') {
         ddsVerbose("* initial %s (fd=%d) cache size=%d min=%d max=%d\n", 
            bin->alias_name, bin->media->fp->fd,
            (int)bin->media->fp->cache_r,
            (int)bin->media->fp->min_r,
            (int)bin->media->fp->max_r);
      } else {
         ddsVerbose("* initial %s (fd=%d) cache size=%d min=%d max=%d\n", 
            bin->alias_name, bin->media->fp->fd,
            (int)bin->media->fp->cache_w,
            (int)bin->media->fp->min_w,
            (int)bin->media->fp->max_w);
      }
   } 

   fprintf(file, BORDER);

   return;
}
