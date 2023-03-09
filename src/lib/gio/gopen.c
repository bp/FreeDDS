static char rcsid[]="$Id: gopen.c 199 2010-11-04 17:14:10Z user $";
/*
OPEN, gio stream

MAM 260809 added gopen_tempfile and GIO_open_tempfile
that create temp files using mkstemp

*/

#include "gio_sys.h"
#if defined(_WIN32) || defined(MINGW)
#include <windows.h>
#endif

/* initialized cached file descriptor (cfd) data */
void GIO_cfd_init(
   GFILE *fp)
{
   memset((void*)&fp->cfd, 0, sizeof(fp->cfd) );
}

   
/* close fd, but GFILE remains logically open */
static void GIO_tmp_close(
   GFILE *fp)
{
   /* save current position */
   GIO_getpos(fp,&fp->cfd.last_open_pos);

   /* save current mode */
#if defined(_WIN32) || defined(MINGW)
   fp->cfd.open_mode = dup(fp->fd);
#else
   fp->cfd.open_mode = XFCNTL(fp->fd, F_GETFL, 0);
#endif

   /* close file descriptor */
   fp->cfd.last_close_status = GIO_close(fp);

   /* clear open flags that we don't want to use on reopening */
   fp->state &= ~GIO_STATE_OPENA;
   fp->cfd.open_flags &= ~(O_CREAT | O_APPEND | O_TRUNC);

   return;
}

 
/* try to close one file descriptor */
static void GIO_tmp_close_one( void )
{
   GFILE *fp, *oldest_fp = NULL;
   unsigned long oldest_count = ULONG_MAX;

   /* look for fd with oldest access of those we're allowed to close */
   for(fp = gio.open_chain; fp; fp = fp->chain) {
      if( fp->cfd.really_open && fp->cfd.tmp_close_ok) {
         if(fp->cfd.last_access_count < oldest_count) {
            oldest_fp = fp;
            oldest_count = fp->cfd.last_access_count;
         }
      }
   }

   /* close it */
   if( oldest_fp ) {
      GIO_tmp_close(oldest_fp);
   }
}


/* open file descriptor for logically opened GFILE */
int GIO_open(
   GFILE *fp)
{
   int fd = -1;
   int open_priv = 0666;

   if (gio.verbose) {
      fprintf(gio.std_debug, "GIO_open: fd=%d (max=%d)\n",
         gio.num_fd_open, gio.open_max);
   }

   if(gio.num_fd_open >= gio.open_max) {
      GIO_tmp_close_one();
   }

   fd = XOPEN(fp->cfd.name, fp->cfd.open_mode | fp->cfd.open_flags, open_priv);
#if defined(_WIN32) || defined(MINGW)
   /* check file is text or binary */
   if ( (fd >=0) && (fp->cfd.open_flags == 0) && ((fp->cfd.open_mode & O_BINARY) == 0) &&
      ((fp->cfd.open_mode == O_RDONLY) || (fp->cfd.open_mode & O_RDWR) != 0)) {
      /* Mode is READ and not BINARY. */
      static char NUL = (char)0; /* Null char */
      static char BS = (char)8; /* Back Space */
      static char CR = (char)13; /* Carriage Return */
      static char SUB = (char)26; /* Substitute */
      struct GIO_STAT filestat;
      XFSTAT(fp->cfd.name, &filestat);
      size_t fsize = filestat.st_size;
      size_t ii = 0;
      char fin[1];
      int nn;
      for (; ii < fsize; ii++) {
         nn = XREAD(fd, fin, 1);
         if (nn < 0) {
            break;
         } else if ( (fin[0] > NUL && fin[0] < BS)
                    || (fin[0] > CR && fin[0] < SUB)
                    || (fin[0] > 127)) {
            break; /* Binary File */
         }
      }
      if (ii != fsize) {
         /* binary file */
         XCLOSE(fd);
         fd = XOPEN(fp->cfd.name, fp->cfd.open_mode | fp->cfd.open_flags | O_BINARY, open_priv);
      } else {
         lseek(fd, 0, SEEK_SET);
      }
   }
#endif

   if (gio.verbose) {
      char *cwd = (char*)malloc(2048);
      getcwd(cwd, (size_t)2048);
      fprintf(gio.std_debug, "GIO_open: fd=%d errno=%d(%s) cwd=%s name=%s\n",
         fd, errno, strerror(errno), cwd, fp->cfd.name);
      free(cwd);
   }
   if (fd < 0 && errno == EMFILE) {
      /* sometimes open_max is not getting the correct max; so trial-and-error */
      fprintf(gio.std_debug, "GIO_open: gio.open_max=%d -> %d\n",
         gio.open_max, gio.num_fd_open);
      gio.open_max = gio.num_fd_open;
      GIO_tmp_close_one();
      fd = XOPEN(fp->cfd.name, fp->cfd.open_mode | fp->cfd.open_flags, open_priv);
   } 

   if(gio.trc_file) {
      fprintf(gio.trc_file, "GIO_open: fd=%d filename=%s access_age=%lu\n", 
              fd, fp->cfd.name, (long)gio.access_age+1);
   } 
   if (fd < 0) return fd; /* errno set by open */

   fp->fd = fd;
   fp->cfd.really_open = GIO_TRUE;
   fp->cfd.last_access_count = gio.access_age++;

   if(fp->state & GIO_STATE_OPENA) {
      /* seek to end of file for append mode */
      fp->offset = GIO_SEEK(fp, 0, SEEK_END);
      if(fp->offset < 0 && errno != ESPIPE) {
         XCLOSE(fp->fd);
         if(gio.trc_file) {
            fprintf(gio.trc_file, "CLOSE: fd=%d\n", fp->fd);
         }
         fp->cfd.really_open = GIO_FALSE;
         return -1;
      }
   } else if(fp->cfd.last_open_pos) {
      /* restore offset from last temporary closing */
      fp->offset = GIO_SEEK(fp, fp->cfd.last_open_pos, SEEK_SET);
      if(fp->offset < 0 && errno != ESPIPE) {
         XCLOSE(fp->fd);
         if(gio.trc_file) {
            fprintf(gio.trc_file, "CLOSE: fd=%d\n", fp->fd);
         }
         fp->cfd.really_open = GIO_FALSE;
         return -1;
      }
   }

   gio.num_fd_open++;

   if(!fp->cfd.initialized) {
      GIO_gopen_init(fp);
   }

   return fd;
}

 
/* i/o stream (error == NULL) */
GFILE * gopen(
   const char *name,    /* path and file name */
   const char *mode)    /* mode string; leading 'c' for cfd */
{
   int open_flags, open_mode, saved_errno;
   GIO_STATE state;
   GFILE *fp = NULL;
   GIO_BOOLEAN cached_fd = GIO_FALSE;
   struct GIO_STAT filestat;

   gio_init();

   if( *mode == 'c' ) {
      cached_fd = GIO_TRUE;
      mode++;
   }

   state =  GIO_gopen_mode(&open_flags, &open_mode, mode);
   if(! state) return NULL;

   if(strlen(name) > GIO_FILENAME_MAX) {
       errno = ENAMETOOLONG;
       return NULL;
   }

   saved_errno = errno;
   fp = (GFILE*)malloc(sizeof(GFILE));
   if(! fp) {errno = ENOMEM; return NULL;}
   errno = saved_errno;

   fp->state = state;

   GIO_cfd_init(fp);
   strcpy( fp->cfd.name, name );
   fp->cfd.open_flags = open_flags;
   fp->cfd.open_mode = open_mode;
   fp->cfd.tmp_close_ok = cached_fd;

   if(GIO_open(fp) < 0) {
      free(fp);
      return NULL;
   }
   
   if(fp->cfd.tmp_close_ok) {
#if defined(_WIN32) || defined(MINGW)
      XFSTAT(fp->cfd.name, &filestat);
#else
      XFSTAT(fp->fd, &filestat);
#endif
      if(!S_ISREG(filestat.st_mode)) {
         fp->cfd.tmp_close_ok = GIO_FALSE;
      }
   }
   
   GIO_chain(fp);

   return fp;
}



/* MAM 260809 -- open (create) a unique temporary file */
GFILE * gopen_tempfile(
   const char **name,    /* pointer to address of returned path 
                            and file name for the temp file */
   const char *mode)     /* mode string; leading 'c' for cfd */
{
   int open_flags, open_mode, saved_errno;
   GIO_STATE state;
   GFILE *fp = NULL;
   GIO_BOOLEAN cached_fd = GIO_FALSE;
   struct GIO_STAT filestat;

   gio_init();

   if( *mode == 'c' ) {
      cached_fd = GIO_TRUE;
      mode++;
   }

   state =  GIO_gopen_mode(&open_flags, &open_mode, mode);
   if(! state) return NULL;

   saved_errno = errno;
   fp = (GFILE*)malloc(sizeof(GFILE));
   if(! fp) {errno = ENOMEM; return NULL;}
   errno = saved_errno;

   fp->state = state;

   GIO_cfd_init(fp);

   fp->cfd.open_flags = open_flags;
   fp->cfd.open_mode = open_mode;
   fp->cfd.tmp_close_ok = cached_fd;

   if(GIO_open_tempfile(fp) < 0) {
      free(fp);
      return NULL;
   }
   /* return temp file name on success --
      cfd.name is a dynamically allocated char buffer */
   *name = fp->cfd.name;
   if(fp->cfd.tmp_close_ok) {
#if defined(_WIN32) || defined(MINGW)
      XFSTAT(fp->cfd.name, &filestat);
#else
      XFSTAT(fp->fd, &filestat);
#endif
      if(!S_ISREG(filestat.st_mode)) {
         fp->cfd.tmp_close_ok = GIO_FALSE;
      }
   }
   
   GIO_chain(fp);

   return fp;
}

/* temp file template -- must end with 6 X's */
static const char * tempfile_name_template = "/tmp/__ddstmp__XXXXXX";

/* MAM 260809 -- open file descriptor for logically opened temporary file */
int GIO_open_tempfile(
   GFILE *fp)
{
   int fd = -1;
   int open_priv = 0666;
   int flags; /* used in fcntl() */

   if (gio.verbose) {
      fprintf(gio.std_debug, "GIO_open_tempfile: fd=%d (max=%d)\n",
         gio.num_fd_open, gio.open_max);
   }

   if(gio.num_fd_open >= gio.open_max) {
      GIO_tmp_close_one();
   }

   /* create a temp file with a unique name based on above template */
#if defined(_WIN32) || defined(MINGW)
   HANDLE hTempFile = INVALID_HANDLE_VALUE;
   DWORD dwRetVal = 0;
   UINT uRetVal = 0;
   TCHAR szTempFileName[MAX_PATH];
   TCHAR lpTempPathBuffer[MAX_PATH];

   /* Gets the temp path env string (no guarantee it's a valid path). */
   dwRetVal = GetTempPath(MAX_PATH, /* length of the buffer */
                          lpTempPathBuffer); /* buffer for path */
   if (dwRetVal > MAX_PATH || (dwRetVal == 0)) {
      return (-1);
   }

   /* Generates a temporary file name. */
   uRetVal = GetTempFileName(lpTempPathBuffer, /* directory for tmp files */
           TEXT("__ddstmp__XXXXXX"),     /* temp file name prefix */
           0,                /* create unique name */ 
           szTempFileName);  /* buffer for name */
   if (uRetVal == 0) {
      return (-1);
   }
   strcpy(fp->cfd.name, szTempFileName);

   /* Creates the new file to write to for the upper-case version. */
   hTempFile = CreateFile(fp->cfd.name, /* file name */
           GENERIC_WRITE| GENERIC_READ, /* open for write */
           0,                    /* do not share */
           NULL,                 /* default security */
           CREATE_ALWAYS,        /* overwrite existing */
           FILE_ATTRIBUTE_NORMAL,/* normal file */
           NULL);                /* no template */
   if (hTempFile == INVALID_HANDLE_VALUE) {
      return (-1);
   }
   fd = _open_osfhandle((intptr_t)hTempFile, _O_APPEND | _O_RDONLY);
#else
   strcpy(fp->cfd.name, tempfile_name_template);
   fd = mkstemp(fp->cfd.name);
#endif
   if (fd>=0) {
     /* set the flags if other than the default 
        O_EXCL | O_CREAT | O_TRUNC | O_RDWR */
#if defined(_WIN32) || defined(MINGW)
     if (dup(fd) >= 0)
#else
     if (fcntl(fd,F_SETFL,fp->cfd.open_mode | fp->cfd.open_flags) >=0)
#endif
     /* Now grant read/write access to everyone --
        the default mode is 0600 (owner read/write only);
        No conflict will occur until after the "chmod" call
        as the temp file was created with permission 0600
        for glib > 2.06  */
        chmod(fp->cfd.name,open_priv);
   }

   if (gio.verbose) {
      char *cwd = (char*)malloc(2048);
      getcwd(cwd, (size_t)2048);
      fprintf(gio.std_debug, "GIO_open_tempfile: fd=%d errno=%d(%s) cwd=%s name=%s\n",
         fd, errno, strerror(errno), cwd, fp->cfd.name);
      free(cwd);
   }

   if(gio.trc_file) {
      fprintf(gio.trc_file, "GIO_open_tempfile: fd=%d filename=%s access_age=%lu\n", 
              fd, fp->cfd.name, (long)gio.access_age+1);
   } 
   if (fd < 0) return fd; /* errno set by mkstemp */

   fp->fd = fd;
   fp->cfd.really_open = GIO_TRUE;
   fp->cfd.last_access_count = gio.access_age++;

   if(fp->state & GIO_STATE_OPENA) {
      /* seek to end of file for append mode */
      fp->offset = GIO_SEEK(fp, 0, SEEK_END);
      if(fp->offset < 0 && errno != ESPIPE) {
         XCLOSE(fp->fd);
         if(gio.trc_file) {
            fprintf(gio.trc_file, "CLOSE: fd=%d\n", fp->fd);
         }
         fp->cfd.really_open = GIO_FALSE;
         return -1;
      }
   } else if(fp->cfd.last_open_pos) {
      /* restore offset from last temporary closing */
      fp->offset = GIO_SEEK(fp, fp->cfd.last_open_pos, SEEK_SET);
      if(fp->offset < 0 && errno != ESPIPE) {
         XCLOSE(fp->fd);
         if(gio.trc_file) {
            fprintf(gio.trc_file, "CLOSE: fd=%d\n", fp->fd);
         }
         fp->cfd.really_open = GIO_FALSE;
         return -1;
      }
   }

   gio.num_fd_open++;

   if(!fp->cfd.initialized) {
      GIO_gopen_init(fp);
   }

   return fd;
}


