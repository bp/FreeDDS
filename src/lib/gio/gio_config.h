#ifndef __GIO_CONFIG_H
#define __GIO_CONFIG_H

/*
GIO host and site dependent configuration.

$Id: gio_config.h 13 2009-01-06 16:56:43Z user $
*/

/* sgi malloc alignment is 4 for 32 bit mode, 8 for 64 (see malloc(3x)) */

/* GIO magic number, revision, release, and author */
#define GIO_REVISION  1
#define GIO_RELEASE   "Rev 1.0, 18 July 1995"
#define GIO_MAGIC     (0x9e352a00 | GIO_REVISION)

#define GIO_DEBUG     /* required, if any DEBUG is defined */

#define GIO_DEBUG_SGI_DIRECT_IO

/*** NOTE: special host dependencies... (SEE ALSO gsetvbuf.c)

   GIO_BIG type must be a SIGNED integer (normally ptrdiff_t).

   GIO_MMAP_IS_SUPPORTED is defined, if host supports mmap io,
   in addition to traditional read and write.

   GIO_RAWIO_IS_SUPPORTED is defined, if host supports direct io,
   not buffered by the kernel (SGI's O_DIRECT).

   GIO_RAWIO_HONORS_KERNEL_EOF is defined, if raw i/o honors EOF.

   GIO_IO_COPY_READ is a GIO extension for the gsetvbuf mode flags.
   If set, copy semantics are used for read io (kernel "read" call).

   GIO_IO_MMAP_READ is a GIO extension for the gsetvbuf mode flags.
   If set, memory map semantics are used for read io (kernel "mmap" call),
   assuming the kernel supports this interface and GIO implements it.
   By default, the fastest available method is used (copy or memory map).

   GIO_IO_RAW_READ is a GIO extension for the gsetvbuf mode flags.
   If set, direct i/o semantics are used for read io (raw "read" call).
   These flags are mutually exclusive.

   GIO_IO_COPY_WRITE, GIO_IO_MMAP_WRITE, GIO_IO_RAW_WRITE
   are comparable gsetvbuf flags for write operations.

   GIO_MASK_SUPPORTED_MODES is a mask for supported setvbuf mode advisory flags.
   Hosts may support a limited combination of kernel i/o semantics.

   GIO_DEFAULT_SPEED_MODE is a setvbuf mode advisory flag, optimized for speed.
   
   GIO_IO_NON_TUNED is a GIO extension for the gsetvbuf mode flags.
   It is intended for development only, i.e. NOT production usage.
   If set, cache size is NOT dynamically tuned (constant, and > zero).
   This allows performance to be tuned, using known block/page sizes.

   GIO_IO_NON_CACHE is a GIO extension for the gsetvbuf mode flags.
   It is intended for development only, i.e. NOT production usage.
   If set, cache size is NOT dynamically tuned (constant, and == zero).
   This allows performance to be measured, using zero block/page sizes.
   This should degenerate to non-buffered speed, plus overhead for cache
   management (5% typical)...   Reality check on implementation merits.

   GIO_MALLOC_COPY is a macro to allocate cache buffers.
   Kernel copy i/o semantics are honored and efficiency is maximized.

   GIO_MALLOC_RAWIO is a macro to allocate cache buffers.
   Kernel raw i/o semantics are honored and efficiency is maximized.

***/

/* default minimum modulo for cache buffer size (non-block and non-page) */
#define GIODEF_MALLOC_MIN 512

#ifdef HOST_Sun
   #define GIO_MMAP_IS_SUPPORTED
   #define GIO_MMAP_PROT              (PROT_READ)
   #define GIO_MMAP_FLAGS             (MAP_PRIVATE)
   #define GIO_MADVISE_WILLNEED       /* good or bad ??? */
   #define GIODEF_MASK_SUPPORTED_MODES \
              (GIO_IO_COPY_READ | GIO_IO_COPY_WRITE | \
              GIO_IO_MMAP_READ)
   #define GIODEF_DEFAULT_SPEED_MODE \
              (GIO_IO_COPY_READ | GIO_IO_COPY_WRITE)
   #define GIO_MALLOC_COPY(fp, size) \
      memalign((unsigned)gio.page_size, (unsigned)(size))
   #define GIO_MALLOC_RAWIO(fp, size) \
      memalign((unsigned)fp->align_p, (unsigned)(size))
#elif HOST_SGI
   #define GIO_RAWIO_IS_SUPPORTED
   #define GIO_RAWIO_HONORS_KERNEL_EOF
   #define GIO_MMAP_IS_SUPPORTED
   #define GIO_MMAP_PROT              (PROT_READ)
   #define GIO_MMAP_FLAGS             (MAP_PRIVATE)
   #define GIO_MADVISE_WILLNEED       /* good or bad ??? */
   #define GIODEF_MASK_SUPPORTED_MODES \
              (GIO_IO_COPY_READ | GIO_IO_COPY_WRITE | \
              GIO_IO_MMAP_READ | \
              GIO_IO_RAW_READ)
   #define GIODEF_DEFAULT_SPEED_MODE \
              (GIO_IO_COPY_READ | GIO_IO_COPY_WRITE)
   #define GIO_MALLOC_COPY(fp, size) \
      memalign(gio.page_size, (size))
   #define GIO_MALLOC_RAWIO(fp, size) \
      memalign(fp->align_p, (size))
#else
   #define GIODEF_MASK_SUPPORTED_MODES \
              (GIO_IO_COPY_READ | GIO_IO_COPY_WRITE)
   #define GIODEF_DEFAULT_SPEED_MODE \
              (GIO_IO_COPY_READ | GIO_IO_COPY_WRITE)
   #define GIO_MALLOC_COPY(fp, size) malloc((size))
   #define GIO_MALLOC_RAWIO(fp, size) malloc((size))
#endif

/* Define the maximum number of cache file descriptor that may
   be open simultaneously.   If exceeded, old file descriptor are
   closed, but only if they were cached (used a regular file).
   The least recently used CFD is closed when the limit is exceeded.
   This is not a hard limit, in the unlikely event that ALL CFDs are
   used for non-regular files (not closeable).

   This value is used to initialize gio.open_max at compile time. In
   most cases gio.open_max will be reinitialized at runtime (see gio_init)
   with a better value. */

#define GIO_OPEN_MAX_PAD 10

#ifdef _POSIX_OPEN_MAX
#define GIO_OPEN_MAX_BASE _POSIX_OPEN_MAX
#else
#define	GIO_OPEN_MAX_BASE 64
#endif

#define GIO_OPEN_MAX (GIO_OPEN_MAX_BASE-GIO_OPEN_MAX_PAD)


#if defined(_SC_OPEN_MAX) || defined(sysconf)
#define	 GIO_SYSCONF_AVAILABLE
#endif

/* On some systems _SC_OPEN_MAX is an enumeration value instead of a
   macro, so the above test won't work. For these cases we need to use
   brute force. */

#ifndef GIO_SYSCONF_AVAILABLE

#ifdef __linux__
#define	 GIO_SYSCONF_AVAILABLE
#endif

#endif

/* Define the maximum length of a file name */

#if defined(_POSIX_PATH_MAX)
#define GIO_FILENAME_MAX _POSIX_PATH_MAX

#elif defined(FILENAME_MAX)
#define GIO_FILENAME_MAX FILENAME_MAX

#elif defined(NAME_MAX)
#define GIO_FILENAME_MAX NAME_MAX

#elif defined(MAXNAMLEN)
#define GIO_FILENAME_MAX MAXNAMLEN

#else /* take a guess */
#define GIO_FILENAME_MAX 256

#endif

#endif /* __GIO_CONFIG_H */
