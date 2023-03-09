#ifndef __DDS_CON4_H
#define __DDS_CON4_H

/*
RCS-t=DDS, macros convert data between binary representations
 
$Id: dds_con4.h 16 2009-01-06 21:52:47Z user $
*/

/****************************************************/
/* These macros convert data between types that are */
/* ARE implicitily supported by the host.           */
/****************************************************/

/******************************************************/
/* Macros to convert data using "C" assignment.       */
/* Lvalue and rvalue types must be supported by host. */
/******************************************************/

#define CON4_SS_ASSN(LT3, RT3, LP3, RP3, MAX3)  \
      *(DDS_##LT3##_T*)LP3 = *(DDS_##RT3##_T*)RP3; 

#define CON4_VV_ASSN(LT4, RT4, LP4, RP4, MAX4)  \
   {size_t n;  \
      DDS_##LT4##_T *lptr = (DDS_##LT4##_T*)(LP4);  \
      DDS_##RT4##_T *rptr = (DDS_##RT4##_T*)(RP4);  \
      for(n = 0; n < MAX4; n++) lptr[n] = rptr[n];  \
   }

#define CON4_VS_ASSN(LT4, RT4, LP4, RP4, MAX4)  \
   {size_t n;  \
      DDS_##LT4##_T *lptr =  (DDS_##LT4##_T*)(LP4);  \
      DDS_##RT4##_T  rval = *(DDS_##RT4##_T*)(RP4);  \
      for(n = 0; n < MAX4; n++) lptr[n] = rval;  \
   } 

/************************************************/
/* Macros to convert data using "C" assignment. */
/* Lvalue is a "complex" host type.             */
/* Rvalue is a float or integral host type.     */
/************************************************/

#define CON4_SS_ASSR(LT4, RT4, LP4, RP4, MAX4)  \
      ((DDS_##LT4##_T*)(LP4))->real = *(DDS_##RT4##_T*)(RP4),  \
      ((DDS_##LT4##_T*)(LP4))->imag = 0.0;  \

#define CON4_VV_ASSR(LT4, RT4, LP4, RP4, MAX4)  \
   {size_t n;  \
      DDS_##LT4##_T *lptr = (DDS_##LT4##_T*)(LP4);  \
      DDS_##RT4##_T *rptr = (DDS_##RT4##_T*)(RP4);  \
      for(n = 0; n < MAX4; n++)  \
         lptr[n].real = rptr[n],  \
         lptr[n].imag = 0.0;  \
   }

#define CON4_VS_ASSR(LT4, RT4, LP4, RP4, MAX4)  \
   {size_t n;  \
      DDS_##LT4##_T *lptr =  (DDS_##LT4##_T*)(LP4);  \
      DDS_##RT4##_T  rval = *(DDS_##RT4##_T*)(RP4);  \
      for(n = 0; n < MAX4; n++)  \
         lptr[n].real = rval, lptr[n].imag = 0.0;  \
   }

/************************************************/
/* Macros to convert data using "C" assignment. */
/* Lvalue and rvalue are  "complex" host types. */
/************************************************/

#define CON4_SS_ASSX(LT4, RT4, LP4, RP4, MAX4)  \
      (*(DDS_##LT4##_T*)(LP4)).real = (*(DDS_##RT4##_T*)(RP4)).real,  \
      (*(DDS_##LT4##_T*)(LP4)).imag = (*(DDS_##RT4##_T*)(RP4)).imag;

#define CON4_VV_ASSX(LT4, RT4, LP4, RP4, MAX4)  \
   {size_t n;  \
      DDS_##LT4##_T *lptr = (DDS_##LT4##_T*)(LP4);  \
      DDS_##RT4##_T *rptr = (DDS_##RT4##_T*)(RP4);  \
      for(n = 0; n < MAX4; n++)  \
         lptr[n].real = rptr[n].real,  \
         lptr[n].imag = rptr[n].imag;  \
   }

#define CON4_VS_ASSX(LT4, RT4, LP4, RP4, MAX4)  \
   {size_t n;  \
      DDS_##LT4##_T *lptr =  (DDS_##LT4##_T*)(LP4);  \
      DDS_##RT4##_T  rval = *(DDS_##RT4##_T*)(RP4);  \
      for(n = 0; n < MAX4; n++)  \
         lptr[n].real = rval.real,  \
         lptr[n].imag = rval.imag;  \
   }

/***************************************************/
/* Macros to execute special conversion functions. */
/***************************************************/

#define CON4_SS_FUNC(LT4, RT4, LP4, RP4, MAX4)  \
   CON5_##LT4##_##RT4(LP4, RP4, 1)

#define CON4_VV_FUNC(LT4, RT4, LP4, RP4, MAX4)  \
   CON5_##LT4##_##RT4(LP4, RP4 ,MAX4)

#define CON4_VS_FUNC(LT4, RT4, LP4, RP4, MAX4)  \
   {size_t n;  \
      char *lptr = (char*)(LP4);  \
      CON5_##LT4##_##RT4(LP4, RP4, 1)  \
      for(n = 1; n < MAX4; n++)  \
         memcpy(lptr + n * DDS_##LT4##_P, lptr, DDS_##LT4##_P);  \
   }

/***************************************************************************/
/*** NOTE: to support byte swapped formats, like PC's, Joe Wade suggests ***/
/*** ntohs, htons, ntohl, htonl (seem to be Unix/Berkley standards)      ***/
/*** which should be optimized.   otherwise, Joe has some macros...      ***/
/*** #define CONVERT2(word)                                              ***/
/*** (short)(((long)word<<8)&0x0000ff00)|(((long)word>>8)&0x000000ff)    ***/
/***                                                                     ***/
/*** #define CONVERT4(word)                                              ***/
/*** (((word<<24)&0xff000000)|((word<<8)&0x00ff0000)|((word>>8)&0x       ***/
/*** 0000ff00)|((word>>24)&0x000000ff))                                  ***/
/***************************************************************************/

#endif /* __DDS_CON4_H */
