/* program to test GIO cached file descriptors */
/* Terry Griffin (Axian) - April 1997 */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#if defined(_WIN32) || defined(MINGW)
#include <io.h>
#else
#include <unistd.h>
#endif

#include "gio.h"



#define	TEST_STRING "This is a test."
#define	TEST_FMT    "%03d:%03d %s\n"

static int test_len;
static GFILE   **fp;

/* stress factors */
#define MAX_FD_OPEN  1
#define	NUM_FILES    1000	   /* MAX_FD_OPEN < NUM_FILES <= 1000 */
#define	NUM_LINES    20		   /* NUM_LINES <= 1000 */

void writetest( void )
{
    char    filename[_POSIX_PATH_MAX], line[256];
    int	    fi, li, status;

    sprintf( line, TEST_FMT, 0, 0, TEST_STRING );
    test_len = strlen(line);

    for( fi = 0; fi < NUM_FILES; fi++ )
    {
	sprintf( filename, "cfdtest%03d.txt", fi );

	fp[fi] = gopen( filename, "cw" );

	if( fp[fi] == NULL )
	{
	    perror( filename );
	    exit(1);
	}
    }

    for( li = 0; li < NUM_LINES; li++ )
    {
	printf(
	    "Writing Line %d -------------------------------\n", li );

	for( fi = 0; fi < NUM_FILES; fi++ )
	{
	    sprintf( line, TEST_FMT, fi, li, TEST_STRING );
	    status = gwritec( fp[fi], line, test_len);
	    if( status < test_len )
		printf( "Error writing to file %d\n", fi );
	}
    }

    for( fi = 0; fi < NUM_FILES; fi++ )
    {
	status = gclose( fp[fi] );
	if( status != 0 )
	    printf( "Error %d closing file %d\n", status, fi );
    }
}

void readtest( void )
{
    char    filename[_POSIX_PATH_MAX], buf[256];
    int	    fi, li, status;

    for( fi = 0; fi < NUM_FILES; fi++ )
    {
	sprintf( filename, "cfdtest%03d.txt", fi );

	fp[fi] = gopen( filename, "cr" );

	if( fp[fi] == NULL )
	{
	    perror( filename );
	    exit(1);
	}
    }

    for( li = 0; li < NUM_LINES; li++ )
    {
	printf( "Reading Line %d -------------------------------\n", li );

	for( fi = 0; fi < NUM_FILES; fi++ )
	{
	    status = greadc( fp[fi], buf, test_len );
	    if( status > 0 )
		printf( "%*.*s", test_len, test_len, buf );
	    else
		printf( "status=%d\n", status );
	}
    }

    for( fi = 0; fi < NUM_FILES; fi++ )
    {
	status = gclose( fp[fi] );
	if( status != 0 )
	    printf( "Error %d closing file %d\n", status, fi );
    }
}

void seektest( void )
{
    char    filename[_POSIX_PATH_MAX], buf[256];
    int	    fi, li, status;
    long    seekto;

    for( fi = 0; fi < NUM_FILES; fi++ )
    {
	sprintf( filename, "cfdtest%03d.txt", fi );

	fp[fi] = gopen( filename, "cr" );

	if( fp[fi] == NULL )
	{
	    perror( filename );
	    exit(1);
	}
    }

    printf( "\nSEEK_SET to 0 and read\n" );
    seekto = 0;

    for( fi = 0; fi < NUM_FILES; fi++ )
	gseek( fp[fi], seekto, SEEK_SET );

    for( fi = 0; fi < NUM_FILES; fi++ )
    {
	status = greadc( fp[fi], buf, test_len );
	if( status > 0 )
	    printf( "%*.*s", test_len, test_len, buf );
	else
	    printf( "status=%d\n", status );
    }

    printf( "\nSEEK_SET to 2*%d and read\n", test_len );
    seekto = 2*test_len;

    for( fi = 0; fi < NUM_FILES; fi++ )
	gseek( fp[fi], seekto, SEEK_SET );

    for( fi = 0; fi < NUM_FILES; fi++ )
    {
	status = greadc( fp[fi], buf, test_len );
	if( status > 0 )
	    printf( "%*.*s", test_len, test_len, buf );
	else
	    printf( "status=%d\n", status );
    }

    printf( "\nSEEK_CUR to %d and read\n", test_len );
    seekto = test_len;

    for( fi = 0; fi < NUM_FILES; fi++ )
	gseek( fp[fi], seekto, SEEK_CUR );

    for( fi = 0; fi < NUM_FILES; fi++ )
    {
	status = greadc( fp[fi], buf, test_len );
	if( status > 0 )
	    printf( "%*.*s", test_len, test_len, buf );
	else
	    printf( "status=%d\n", status );
    }

    printf( "\nSEEK_END to -%d and read\n", test_len );
    seekto = -test_len;

    for( fi = 0; fi < NUM_FILES; fi++ )
	gseek( fp[fi], seekto, SEEK_END );

    for( fi = 0; fi < NUM_FILES; fi++ )
    {
	status = greadc( fp[fi], buf, test_len );
	if( status > 0 )
	    printf( "%*.*s", test_len, test_len, buf );
	else
	    printf( "status=%d\n", status );
    }

    for( fi = 0; fi < NUM_FILES; fi++ )
    {
	status = gclose( fp[fi] );
	if( status != 0 )
	    printf( "Error %d closing file %d\n", status, fi );
    }
}

int main( int argc, char *argv[] )
{
   int	 open_max;

   open_max = gopenmax(GIO_OPEN_MAX_GET);
   printf( "initial open_max=%d\n", open_max );

   open_max = gopenmax(GIO_OPEN_MAX_SET,40);
   printf( "set open_max=%d\n", open_max );

   open_max = gopenmax(GIO_OPEN_MAX_RESET);
   printf( "reset open_max=%d\n", open_max );

   open_max = gopenmax(GIO_OPEN_MAX_INCR,-10);
   printf( "incr open_max=%d\n", open_max );

   open_max = gopenmax(GIO_OPEN_MAX_SET,MAX_FD_OPEN);
   printf( "set open_max=%d\n", open_max );

   fp = (GFILE**)malloc(NUM_FILES * sizeof(GFILE*));

   writetest();
   readtest();
   seektest();
}
