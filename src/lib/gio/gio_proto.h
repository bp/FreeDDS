#ifndef __GIO_PROTO_H
#define __GIO_PROTO_H

/*
RCS-t=GIO, internal function prototypes.

$Id: gio_proto.h 113 2009-09-03 22:53:32Z user $
*/

/* READ stream Buffer Malloc pointer, buffered */
int GIO_greadm_buf(     /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void **buf,    /* read buffer pointer to pointer */
   GIO_BIG size);       /* requested byte count */

/* READ stream Buffer pointer, memory mapped */
int GIO_greadm_map(     /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void **buf,    /* read buffer pointer to pointer */
   GIO_BIG size);       /* requested byte count */

/* READ stream Buffer pointer, raw (direct) i/o */
int GIO_greadm_raw(     /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void **buf,    /* read buffer pointer to pointer */
   GIO_BIG size);       /* requested byte count */

/* READ stream Buffer pointer, non buffered */
int GIO_greadm_non(     /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void **buf,    /* read buffer pointer to pointer */
   GIO_BIG size);       /* requested byte count */

/* READ stream Buffer Biased pointer, buffered */
int GIO_greadb_buf(     /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void **buf,    /* read buffer pointer to pointer */
   GIO_BIG size,        /* requested byte count */
   GIO_ALIGN align,     /* buffer alignment */
   size_t bias);        /* alignment bias */

/* READ stream Buffer Biased pointer, memory mapped */
int GIO_greadb_map(     /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void **buf,    /* read buffer pointer to pointer */
   GIO_BIG size,        /* requested byte count */
   GIO_ALIGN align,     /* buffer alignment */
   size_t bias);        /* alignment bias */

/* READ stream Buffer Biased pointer, raw (direct) i/o */
int GIO_greadb_raw(     /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void **buf,    /* read buffer pointer to pointer */
   GIO_BIG size,        /* requested byte count */
   GIO_ALIGN align,     /* buffer alignment */
   size_t bias);        /* alignment bias */

/* READ stream Buffer Biased pointer, non buffered */
int GIO_greadb_non(     /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void **buf,    /* read buffer pointer to pointer */
   GIO_BIG size,        /* requested byte count */
   GIO_ALIGN align,     /* buffer alignment */
   size_t bias);        /* alignment bias */

/* READ stream Buffer Copy data, buffered */
int GIO_greadc_buf(     /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   void *buf,           /* read buffer pointer */
   GIO_BIG size);       /* requested byte count */

/* READ stream Buffer Copy data, memory mapped */
int GIO_greadc_map(     /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   void *buf,           /* read buffer pointer */
   GIO_BIG size);       /* requested byte count */

/* READ stream Buffer Copy data, raw (direct) i/o */
int GIO_greadc_raw(     /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   void *buf,           /* read buffer pointer */
   GIO_BIG size);       /* requested byte count */

/* READ stream Buffer Copy data, non buffered */
int GIO_greadc_non(     /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   void *buf,           /* read buffer pointer */
   GIO_BIG size);       /* requested byte count */

/* WRITE stream Buffer Malloc pointer, buffered */
int GIO_gwritem_buf(    /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   GIO_BIG size);       /* requested byte count */

/* WRITE stream Buffer Malloc pointer, memory mapped */
int GIO_gwritem_map(    /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   GIO_BIG size);       /* requested byte count */

/* WRITE stream Buffer Malloc pointer, raw (direct) i/o */
int GIO_gwritem_raw(    /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   GIO_BIG size);       /* requested byte count */

/* WRITE stream Buffer Malloc pointer, non buffered */
int GIO_gwritem_non(    /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   GIO_BIG size);       /* requested byte count */

/* WRITE stream Buffer Biased pointer, buffered */
int GIO_gwriteb_buf(    /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   GIO_BIG size,        /* requested byte count */
   GIO_ALIGN align,     /* buffer alignment */
   size_t bias);        /* alignment bias */

/* WRITE stream Buffer Biased pointer, memory mapped */
int GIO_gwriteb_map(    /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   GIO_BIG size,        /* requested byte count */
   GIO_ALIGN align,     /* buffer alignment */
   size_t bias);        /* alignment bias */

/* WRITE stream Buffer Biased pointer, raw (direct) i/o */
int GIO_gwriteb_raw(    /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   GIO_BIG size,        /* requested byte count */
   GIO_ALIGN align,     /* buffer alignment */
   size_t bias);        /* alignment bias */

/* WRITE stream Buffer Biased pointer, non buffered */
int GIO_gwriteb_non(    /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   GIO_BIG size,        /* requested byte count */
   GIO_ALIGN align,     /* buffer alignment */
   size_t bias);        /* alignment bias */

/* WRITE stream Buffer Copy data, buffered */
int GIO_gwritec_buf(    /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   const void *buf,     /* write buffer pointer */
   GIO_BIG size);       /* requested byte count */

/* WRITE stream Buffer Copy data, memory mapped */
int GIO_gwritec_map(    /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   const void *buf,     /* write buffer pointer */
   GIO_BIG size);       /* requested byte count */

/* WRITE stream Buffer Copy data, raw (direct) i/o */
int GIO_gwritec_raw(    /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   const void *buf,     /* write buffer pointer */
   GIO_BIG size);       /* requested byte count */

/* WRITE stream Buffer Copy data, non buffered */
int GIO_gwritec_non(    /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   const void *buf,     /* write buffer pointer */
   GIO_BIG size);       /* requested byte count */

HOST_INT8_T xGIO_seek(
   GFILE *fp,
   HOST_INT8_T n_seek,
   int whence);

GIO_STATE GIO_gopen_mode(
   int *open_flags,
   int *open_mode,
   const char *mode);

int GIO_setvbuf_mode(
   GFILE *fp,           /* i/o stream */
   int *mode_buffer,    /* mode, restricted to standard flags. */
   int *mode_read,      /* mode, restricted to valid read  flags. */
   int *mode_write,     /* mode, restricted to valid write flags. */
   int mode);           /* mode provided by API */

int GIO_setvbuf_stat(
   GFILE *fp);

GFILE * GIO_gopen_init(	/* i/o stream (error == NULL) */
   GFILE *fp);		/* i/o stream */

/* FLUSH READ stream */
int GIO_gflushr_buf(    /* return 0 (error < 0) */
   GFILE *fp);          /* i/o stream */

/* FLUSH READ stream */
int GIO_gflushr_map(    /* return 0 (error < 0) */
   GFILE *fp);          /* i/o stream */

/* FLUSH READ stream */
int GIO_gflushr_non(    /* return 0 (error < 0) */
   GFILE *fp);          /* i/o stream */

/* FLUSH READ stream */
int GIO_gflushr_raw(    /* return 0 (error < 0) */
   GFILE *fp);          /* i/o stream */

/* FLUSH WRITE stream */
int GIO_gflushw_buf(    /* return 0 (error < 0) */
   GFILE *fp);          /* i/o stream */

/* FLUSH WRITE stream */
int GIO_gflushw_map(    /* return 0 (error < 0) */
   GFILE *fp);          /* i/o stream */

/* FLUSH WRITE stream */
int GIO_gflushw_non(    /* return 0 (error < 0) */
   GFILE *fp);          /* i/o stream */

/* FLUSH WRITE stream */
int GIO_gflushw_raw(    /* return 0 (error < 0) */
   GFILE *fp);          /* i/o stream */

/* TUNE READ cache */
void GIO_tune_read(
   GFILE *fp,           /* i/o stream */
   GIO_BIG n_flush);    /* number of bytes actually read by the API */

/* TUNE WRITE cache */
void GIO_tune_write(
   GFILE *fp,           /* i/o stream */
   GIO_BIG n_flush);    /* number of bytes actually written by the API */

int GIO_debug(          /* status, non-zero = error */
   GFILE *fp,		/* i/o stream */
   const char *msg);	/* debug message */

int GIO_int_defer();

HOST_INT8_T xGIO_offset_defer();

GFILE * xGIO_fileb_defer();

/* deferred open, for when !fp->really_open */
int GIO_open(		/* return fp->fd (error < 0) */
   GFILE *fp);

/* Open temp file */
int GIO_open_tempfile(
   GFILE *fp);

/* convenience routine for gopen, gdopen, greopen */
void GIO_cfd_init(
   GFILE *fp);

/* open/reopen if necessary */
int (GIO_cfd_check)(	/* return status (error < 0) */
   GFILE *fp);

/* add open fp to the open chain */
void GIO_chain(
   GFILE *fp);

/* identical to ggetpos, but without cfd checking */
int GIO_getpos(         /* -1 = Error */
   GFILE *fp,           /* file pointer */
   gpos_t *offset);     /* current offset */

/* identical to gsetpos, but without cfd checking */
int GIO_setpos(         /* -1 = Error */
   GFILE *fp,           /* file pointer */
   const gpos_t *offset);/* current offset */

/* like gclose, but fp remains logically open */
int GIO_close(		/* EOF = Error */
   GFILE *fp);

#endif /* __GIO_PROTO_H */
