/* program to test cached file descriptors */
/* Terry Griffin (Axian) - January 1997 */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>

#include "gio.h"

#define	NUM_LINES   20
#define	TEST_STRING "This is a test."
#define	TEST_FMT    "%02d:%02d %s\n"

static int test_len;
static int num_files;
static GFILE   **fp;

void writetest( void )
{
    char    filename[_POSIX_PATH_MAX], line[256];
    int	    fi, li, status;

    sprintf( line, TEST_FMT, 0, 0, TEST_STRING );
    test_len = strlen(line);

    for( fi = 0; fi < num_files; fi++ )
    {
	sprintf( filename, "giotest%02d.txt", fi );

	fp[fi] = gopen( filename, "cw" );

	if( fp[fi] == NULL )
	{
	    perror( filename );
	    exit(1);
	}
    }

    for( li = 0; li < NUM_LINES; li++ )
    {
	fprintf( stderr,
	    "Writing Line %d -------------------------------\n", li );

	for( fi = 0; fi < num_files; fi++ )
	{
	    sprintf( line, TEST_FMT, fi, li, TEST_STRING );
	    status = gwritec( fp[fi], line, test_len);
	    if( status < test_len )
		fprintf( stderr, "Error writing to file %d\n", fi );
	    fprintf( stderr, "\n" );
	}
    }

    for( fi = 0; fi < num_files; fi++ )
    {
	status = gclose( fp[fi] );
	if( status != 0 )
	    fprintf( stderr, "Error %d closing file %d\n", status, fi );
    }
}

void readtest( void )
{
    char    filename[_POSIX_PATH_MAX], buf[256];
    int	    fi, li, status;

    for( fi = 0; fi < num_files; fi++ )
    {
	sprintf( filename, "giotest%02d.txt", fi );

	fp[fi] = gopen( filename, "cr" );

	if( fp[fi] == NULL )
	{
	    perror( filename );
	    exit(1);
	}
    }

    for( li = 0; li < NUM_LINES; li++ )
    {
	fprintf( stderr, "Reading Line %d -------------------------------\n", li );

	for( fi = 0; fi < num_files; fi++ )
	{
	    status = greadc( fp[fi], buf, test_len );
	    if( status > 0 )
		fprintf( stderr, "%*.*s", test_len, test_len, buf );
	    else
		fprintf( stderr, "status=%d\n", status );
	    fprintf( stderr, "\n" );
	}
    }

    for( fi = 0; fi < num_files; fi++ )
    {
	status = gclose( fp[fi] );
	if( status != 0 )
	    fprintf( stderr, "Error %d closing file %d\n", status, fi );
    }
}

void seektest( void )
{
    char    filename[_POSIX_PATH_MAX], buf[256];
    int	    fi, li, status;
    long    seekto;

    for( fi = 0; fi < num_files; fi++ )
    {
	sprintf( filename, "giotest%02d.txt", fi );

	fp[fi] = gopen( filename, "cr" );

	if( fp[fi] == NULL )
	{
	    perror( filename );
	    exit(1);
	}
    }

    fprintf( stderr, "\nSEEK_SET to 0 and read\n" );
    seekto = 0;

    for( fi = 0; fi < num_files; fi++ )
	gseek( fp[fi], seekto, SEEK_SET );

    for( fi = 0; fi < num_files; fi++ )
    {
	status = greadc( fp[fi], buf, test_len );
	if( status > 0 )
	    fprintf( stderr, "%*.*s", test_len, test_len, buf );
	else
	    fprintf( stderr, "status=%d\n", status );
	fprintf( stderr, "\n" );
    }

    fprintf( stderr, "\nSEEK_SET to 2*%d and read\n", test_len );
    seekto = 2*test_len;

    for( fi = 0; fi < num_files; fi++ )
	gseek( fp[fi], seekto, SEEK_SET );

    for( fi = 0; fi < num_files; fi++ )
    {
	status = greadc( fp[fi], buf, test_len );
	if( status > 0 )
	    fprintf( stderr, "%*.*s", test_len, test_len, buf );
	else
	    fprintf( stderr, "status=%d\n", status );
	fprintf( stderr, "\n" );
    }

    fprintf( stderr, "\nSEEK_CUR to %d and read\n", test_len );
    seekto = test_len;

    for( fi = 0; fi < num_files; fi++ )
	gseek( fp[fi], seekto, SEEK_CUR );

    for( fi = 0; fi < num_files; fi++ )
    {
	status = greadc( fp[fi], buf, test_len );
	if( status > 0 )
	    fprintf( stderr, "%*.*s", test_len, test_len, buf );
	else
	    fprintf( stderr, "status=%d\n", status );
	fprintf( stderr, "\n" );
    }

    fprintf( stderr, "\nSEEK_END to -%d and read\n", test_len );
    seekto = -test_len;

    for( fi = 0; fi < num_files; fi++ )
	gseek( fp[fi], seekto, SEEK_END );

    for( fi = 0; fi < num_files; fi++ )
    {
	status = greadc( fp[fi], buf, test_len );
	if( status > 0 )
	    fprintf( stderr, "%*.*s", test_len, test_len, buf );
	else
	    fprintf( stderr, "status=%d\n", status );
	fprintf( stderr, "\n" );
    }

    for( fi = 0; fi < num_files; fi++ )
    {
	status = gclose( fp[fi] );
	if( status != 0 )
	    fprintf( stderr, "Error %d closing file %d\n", status, fi );
    }
}

int main( int argc, char *argv[] )
{
   int	 open_max;

   open_max = gopenmax(GIO_OPEN_MAX_GET);
   fprintf( stderr, "initial open_max=%d\n", open_max );

   open_max = gopenmax(GIO_OPEN_MAX_SET,40);
   fprintf( stderr, "set open_max=%d\n", open_max );

   open_max = gopenmax(GIO_OPEN_MAX_RESET);
   fprintf( stderr, "reset open_max=%d\n", open_max );

   open_max = gopenmax(GIO_OPEN_MAX_INCR,-10);
   fprintf( stderr, "incr open_max=%d\n", open_max );

   open_max = gopenmax(GIO_OPEN_MAX_SET,10);
   fprintf( stderr, "set open_max=%d\n", open_max );

   num_files = open_max + 20;
   fp = (GFILE**)malloc(num_files * sizeof(GFILE*));

   writetest();
   readtest();
   seektest();
}
