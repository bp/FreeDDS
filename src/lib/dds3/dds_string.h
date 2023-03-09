#ifndef __DDS_STRING_H
#define __DDS_STRING_H
 
/*
RCS-t=DDS, string "utility" declarations
 
$Id: dds_string.h 16 2009-01-06 21:52:47Z user $
*/

#define NULL_STRING {0, NULL}
 
#define NULL_TOKEN {{0, NULL}, NULL}
 
/*********************************/
/* macro front end to ddsToken() */
/*********************************/

/********************************************************/
/* macro to convert Fortran string into a C string      */
/* trailing blanks are stripped from the Fortran string */
/********************************************************/
 
#define FTN_STR_TO_STR(C_STR, STR_FTN, STR_LEN) \
   ddsStrCpyNB(&(C_STR), (STR_FTN), (STR_LEN))
 
/******************************************************/
/* macro to convert C string into a Fortran string    */
/* trailing blanks are padded onto the Fortran string */
/******************************************************/
 
#define STR_TO_FTN_STR(STR_FTN, STR_LEN, C_STR)  \
   ddsFtnCpy((STR_FTN), (STR_LEN), (C_STR))

 
#define TOKEN_INIT(SAVE, LIST)  \
   ((0 <= ddsToken(&(SAVE), (LIST))) ? (SAVE).str.ptr : NULL)
 
#define TOKEN_NEXT(SAVE)  \
   ((0 <= ddsToken(&(SAVE), NULL)) ? (SAVE).str.ptr : NULL)
 
#define TOKEN_COUNT(LIST)  (ddsToken(NULL, (LIST)))

/*********************************/
/* typedef and enumerated values */
/*********************************/


typedef struct DDS_STRING
{  /* string descriptor */
   size_t max;          /* malloc maximum, buffer[max] */
   char *ptr;           /* pointer to string buffer */
} DDS_STRING;


 
typedef struct DDS_TOKEN
{  /* token descriptor, item within a string list */
   DDS_STRING str;      /* string descriptor */
   const char *next;    /* char after current item */
} DDS_TOKEN;


/*****************************************/
/* extern global variables and constants */
/*****************************************/

extern const DDS_STRING DDS_NULL_STRING;

extern const DDS_TOKEN DDS_NULL_TOKEN;


/***********************/
/* function prototypes */
/***********************/

const char * ddsString(
   const char *old_str)
;
const char * ddsStringN(
   const char *old_str,
   size_t old_size)
;


DDS_STRING * ddsStrFree(
   DDS_STRING *str)
;
DDS_STRING * ddsStrNull(
   DDS_STRING *str)
;
DDS_STRING * ddsStrSet(
   DDS_STRING *str,
   const char *from)
;
DDS_STRING * ddsStrSetMin(
   DDS_STRING *str,
   const char *from,
   size_t min)
;
DDS_STRING * ddsStrSetN(
   DDS_STRING *str,
   const char *from,
   size_t size)
;
 

DDS_STRING * ddsStrCat(
   DDS_STRING *str,
   const char *from)
;
DDS_STRING * ddsStrCat2(
   DDS_STRING *str,
   const char *from,
   const char *from2)
;
DDS_STRING * ddsStrCat3(
   DDS_STRING *str,
   const char *from,
   const char *from2,
   const char *from3)
;
DDS_STRING * ddsStrCat4(
   DDS_STRING *str,
   const char *from,
   const char *from2,
   const char *from3,
   const char *from4)
;
DDS_STRING * ddsStrCat5(
   DDS_STRING *str,
   const char *from,
   const char *from2,
   const char *from3,
   const char *from4,
   const char *from5)
;
DDS_STRING * ddsStrCatN(
   DDS_STRING *str,
   const char *from,
   size_t size)
;
 

DDS_STRING * ddsStrCpy(
   DDS_STRING *str,
   const char *from)
;
DDS_STRING * ddsStrCpyF(
   DDS_STRING *str,
   const char *from)
;
DDS_STRING * ddsStrCpyMin(
   DDS_STRING *str,
   const char *from,
   size_t min)
;
DDS_STRING * ddsStrCpyN(
   DDS_STRING *str,
   const char *from,
   size_t size)
;
DDS_STRING * ddsStrCpyNB(
   DDS_STRING *str,
   const char *from,
   size_t size)
;


DDS_STRING * ddsStrCpyCat2(
   DDS_STRING *str,
   const char *from,
   const char *from2)
;
DDS_STRING * ddsStrCpyCat3(
   DDS_STRING *str,
   const char *from,
   const char *from2,
   const char *from3)
;
DDS_STRING * ddsStrCpyCat4(
   DDS_STRING *str,
   const char *from,
   const char *from2,
   const char *from3,
   const char *from4)
;
DDS_STRING * ddsStrCpyCat5(
   DDS_STRING *str,
   const char *from,
   const char *from2,
   const char *from3,
   const char *from4,
   const char *from5)
;
DDS_STRING * ddsStrCpyCat6(
   DDS_STRING *str,
   const char *from,
   const char *from2,
   const char *from3,
   const char *from4,
   const char *from5,
   const char *from6)
;
DDS_STRING * ddsStrCpyCat7(
   DDS_STRING *str,
   const char *from,
   const char *from2,
   const char *from3,
   const char *from4,
   const char *from5,
   const char *from6,
   const char *from7)
;
DDS_STRING * ddsStrCpyCat8(
   DDS_STRING *str,
   const char *from,
   const char *from2,
   const char *from3,
   const char *from4,
   const char *from5,
   const char *from6,
   const char *from7,
   const char *from8)
;


int ddsToken(          /* token count, or EOF if error */
   DDS_TOKEN *save,    /* state pointer, count only if NULL */
   const char *list)   /* initialize, save continue if NULL */
;
DDS_TOKEN * ddsTokenNull(
   DDS_TOKEN *token)
;


#endif /* __DDS_STRING_H */
