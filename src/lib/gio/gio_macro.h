#ifndef __GIO_MAC_H
#define __GIO_MAC_H

/*
GIO non-trivial macros used internally.

$Id: gio_macro.h 38 2009-01-12 18:39:42Z user $
*/

#undef GIO_SEEK

/************************************************************************/
/* GIO_SEEK(fp, offset, whence) is a macro to perform a low level seek. */
/*    return:  HOST_INT6_T, file offset, minimum of 6 byte precision.   */
/*    fp:      GFILE *, GIO file pointer.                               */
/*    offset:  HOST_INT6_T, file offset, minimum of 6 byte precision.   */
/*    whence:  int, relative position (SEEK_SET, SEEK_CUR, SEEK_END).   */
/*                                                                      */
/*    Note: the HOST_INT6_T may actually be an 8 byte float,            */
/*       if 8 byte integers are not supported.                          */
/************************************************************************/
#if defined(_WIN32) || defined(MINGW)
   #define GIO_STAT __stat64
#else
   #define GIO_STAT stat
#endif
#if defined(_WIN32) || defined(MINGW)
   #define GIO_SEEK(fp, offset, whence) \
      ((long)lseek((fp)->fd, (long)(offset), (int)(whence)))
#elif defined(HOST_LONG_P) && (HOST_LONG_P == 8)
   /*****************************************************************/
   /* Good, host long provides 8 byte precision,                    */
   /* i.e. HOST_INT6_T == HOST_INT8_T == HOST_LONG_T == lseek type. */
   /*****************************************************************/
   #define GIO_SEEK(fp, offset, whence) \
      ((HOST_INT6_T)lseek((fp)->fd, (HOST_INT6_T)(offset), (int)(whence)))
#elif defined(HOST_lseek64)
   /**************************************************/
   /* Good, host provides a 64 bit version of lseek. */
   /* HOST_INT6_T == HOST_INT8_T == lseek64 type.    */
   /**************************************************/
   #define GIO_SEEK(fp, offset, whence) \
      ((HOST_INT6_T)lseek64((fp)->fd, (HOST_INT6_T)(offset), (int)(whence)))
#else
   /****************************************************************/
   /* Punt, use the standard lseek with only 4 bytes of precision. */
   /****************************************************************/
   #define GIO_SEEK(fp, offset, whence) \
      ((HOST_INT6_T)lseek((fp)->fd, (long)(offset), (int)(whence)))
#endif


#define GIO_PTR_ALIGNED(buf, align) \
   (0 ==      ((((char*)(buf) - (char*)0)         ) & (align) - 1))

#define GIO_PTR_BIASED(buf, align, bias) \
   (0 ==      ((((char*)(buf) - (char*)0) + (bias)) & (align) - 1))

#define GIO_PTR_SKIP(skip, buf, align, bias) \
   (skip) = - ((((char*)(buf) - (char*)0) + (bias)) & (align) - 1); \
   if((skip)) (skip) += (align);

/***********************************************/
/* Masking macros for cached file descriptor.  */
/* This avoids the function call overhead,     */
/* when entering various API functions.        */
/***********************************************/

#define GIO_cfd_check(fp) \
   (((fp)->cfd.last_access_count = gio.access_age++),  \
      (((fp)->cfd.really_open) ? 0 : ((GIO_open((fp)) < 0) ? -1 : 0)))

/********************************************************************/
/* minimum percentage, associated with small waste and cache tuning */
/********************************************************************/
#define GIO_MIN_READ  80
#define GIO_MIN_WRITE 80

#endif /* __GIO_MAC_H */

#define	XOPEN(path, flags, mode) \
	open((path), (flags), (mode))
  
#define	XCLOSE(fd) \
	close((fd))
   
#define	XREAD(fd, buf, nbyte) \
   	read((fd), (buf), (int)(nbyte))
   
#define	XWRITE(fd, buf, nbyte) \
   	write((fd), (buf), (int)(nbyte))
   
#define	XFCNTL(fd, cmd, arg) \
   	fcntl((fd), (cmd), (arg))

#if defined(_WIN32) || defined(MINGW)
#define	XFSTAT(fd, st) \
   	_stat64((fd), (st))
#else
#define	XFSTAT(fd, st) \
   	fstat((fd), (st))
#endif
