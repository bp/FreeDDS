#ifndef __DDS_STATE_H
#define __DDS_STATE_H

/*
RCS-t=DDS, private "types" (nucleus)

$Id: dds_state.h 42 2009-01-19 18:31:06Z user $
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

/*
   All global and macro symbolic names are prefixed with "DDS" or "dds",
   unless they are actually part of the Application Program Interface.
   This reduces name space polution and name clashes.
*/


/*********************************/
/* typedef and enumerated values */
/*********************************/

typedef enum DDS_STATE
{  /* bit flags defining overall DDS state */
   DDS_CLEANUP          = 1 <<  0,      /* exit() processing */
   DDS_WARN_ABORT       = 1 <<  1,      /* abort, if warnings */
   DDS_WARN_PRINT       = 1 <<  2,      /* print, if warnings */
   DDS_WARN_LOG         = 1 <<  3,      /* log,   if warnings */
   DDS_ERROR_ABORT      = 1 <<  4,      /* abort, if errors */
   DDS_ERROR_PRINT      = 1 <<  5,      /* print, if errors */
   DDS_ERROR_LOG        = 1 <<  6,      /* log,   if errors */
   DDS_DEBUG_ABORT      = 1 <<  7,      /* abort, if api bugs */
   DDS_DEBUG_PRINT      = 1 <<  8,      /* print, if api bugs */
   DDS_DEBUG_LOG        = 1 <<  9,      /* log,   if api bugs */
   DDS_OPERATOR         = 1 << 10,      /* print operator messages */
   DDS_OPERATOR_LOG     = 1 << 11,      /* log   operator messages */
   DDS_VERBOSE          = 1 << 12,      /* print verbose messages */
   DDS_VERBOSE_LOG      = 1 << 13,      /* log   verbose messages */

   DDS_SAVE_TMP         = 1 << 15,      /* save temporary files */
                                        /*    Cadaver for autopsy. */
   DDS_DUMP_FMTS        = 1 << 16,      /* dump format tables */
                                        /*    Deferred... */
   DDS_DUMP_MAPS        = 1 << 17,      /* dump mapping tables */
                                        /*    Deferred... */
   DDS_DUMP_READ        = 1 << 18,      /* dump data read  buffers */
                                        /*    Set iff "read" dumps'em. */
   DDS_DUMP_WRITE       = 1 << 19,      /* dump data write buffers */
                                        /*    Set iff "write" dumps'em. */
   DDS_DUMP_SEEK        = 1 << 20,      /* dump data seek  arguments */
                                        /*    Set iff "seek" are noted. */
   DDS_DUMP_DEFAULT     = 1 << 21,      /* dump data default type. */
                                        /*    Set iff dumping the default */
                                        /*    type (circumvents DIFF). */
   DDS_DUMP_DIFF0       = 1 << 22,      /* dump data, all values */
   DDS_DUMP_DIFF1       = 1 << 23,      /* dump data 1st differences */
   DDS_DUMP_DIFF2       = 1 << 24,      /* dump data 2nd differences */
                                        /*    Any combination of DIFF */
                                        /*    flags may be set, assuming */
                                        /*    LIST is reset. */
   DDS_DUMP_LIST        = 1 << 25,      /* dump data in tabular columns */
                                        /*    May be set, iff all DIFF */
                                        /*    flags are reset. */

   DDS_DUMP_DIFF = (DDS_DUMP_DIFF0 |
                    DDS_DUMP_DIFF1 |
                    DDS_DUMP_DIFF2),
 
   DDS_DUMP_DATA = (DDS_DUMP_DIFF0 |
                    DDS_DUMP_DIFF1 |
                    DDS_DUMP_DIFF2 |
                    DDS_DUMP_LIST),

   DDS_YACC_CREATE      = 1 << 27,      /* yacc CREATE output format.     */
                                        /*    Set iff creating new data.  */
                                        /*    Format is created via       */
                                        /*    "fmt:*:*.*=" definitions    */
                                        /*    and defaulted via "fmt:*=". */
                                        /*    If reset, the format is     */
                                        /*    predetermined via "fmt:*=". */

   DDS_YACC_MAP         = 1 << 28,      /* yacc MAP output binary.          */
                                        /*    Set iff compiling expressions */
                                        /*    for binary data mapping.      */
   DDS_YACC_IO          = 1 << 29       /* yacc READ/WRITE binary.          */
                                        /*    Set iff compiling expressions */
                                        /*    for binary data i/o.          */
} DDS_STATE;


/*****************************************/
/* extern global variables and constants */
/*****************************************/

extern DDS_BOOLEAN dds_initialized;  /* true iff cdds_init has been called */
extern DDS_STATE dds_state;          /* global state flags */

extern DDS_DEBUG dds_debug_user;     /* user debug flags */
extern DDS_DEBUG dds_debug_api;      /* API debug flags */
extern int dds_yy_flex_debug;        /* scanner trace flag */

#if defined(_WIN32) || defined(MINGW)
extern size_t dds_cmd_pid;            /* process id */
#else
extern pid_t dds_cmd_pid;            /* process id */
#endif
extern const char *dds_cmd_name;     /* command name, argv[0] */
extern const char *dds_cmd_rev;      /* command revision, from openpr */
extern const char *dds_cmd_ddsrev;   /* dds revision */
extern const char *dds_cmd_ddsdat;   /* dds date */
extern const char *dds_cmd_user;     /* user name */
extern const char *dds_cmd_date;     /* starting date */
extern const char *dds_cmd_host;     /* host name */
extern const char *dds_cmd_cwd;      /* current working directory */
extern const char *dds_cmd_home;     /* user home directory */

extern const char *dds_project_dict; /* "$CWD/project.dds" or NULL */
extern const char *dds_ddsrc_dict;   /* "$HOME/.ddsrc" or NULL */
extern const char *dds_base_dict;    /* "par:  env:  ./project  ~/.ddsrc" */
                                     /*    deprecate dds_base_dict */

extern DDS_STRING dds_path;          /* last DDS_PATH3 search path */

extern DDS_BOOLEAN *dds_sislgbuf;    /* kludge for usp large buffer */
                                     /*    NULL unless emulating usp. */
                                     /*    TRUE == on, FALSE == off. */

extern DDS_TABLE dds_prime_table;    /* prime type symbol table */

extern DDS_STACK dds_prime_tag;      /* array of (DDS_SYM*) values. */
                                     /*  Subscripts are SYM_TAG values */
                                     /*    for symbols exported to API. */

extern DDS_STRING dds_data_path;     /* data path   definition alias */
extern DDS_STRING dds_data_suffix;   /* data suffix definition alias */

extern DDS_BOOLEAN dds_openpr;       /* flag indicating print file open */

extern DDS_STACK dds_tmp;            /* stack for temporary data */

#endif /* __DDS_STATE_H */
