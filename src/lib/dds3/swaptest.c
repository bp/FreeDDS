static char rcsid[]="$Id: swaptest.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS main, test byte swap formats.

*/


/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>

#define CONVERT2(word) \
	(short)(((long)word<<8)&0x0000ff00)|(((long)word>>8)&0x000000ff) 
#define CONVERT4(word) \
	(((word<<24)&0xff000000)|((word<<8)&0x00ff0000) \
	|((word>>8)&0x0000ff00)|((word>>24)&0x000000ff))                                  
#define DMY(x) (x)
#define PACK(B1,B2,B3,B4) (B1) << 24 | (B2) << 16 | (B3) << 8 | (B4)

main(int argc, char **argv)
{
   int i, j, n;
   unsigned b1 = 0xdc, b2=0xc6, b3=0x00, b4=0xa8;
   int in[24];

   n = 1;
   if (argc > 1) sscanf(argv[1], "%d", &n);

   in[0] = PACK(b1,b2,b3,b4);
   in[1] = PACK(b1,b2,b4,b3);

   in[2] = PACK(b1,b3,b2,b4);
   in[3] = PACK(b1,b3,b4,b2);

   in[4] = PACK(b1,b4,b2,b3);
   in[5] = PACK(b1,b4,b3,b2);

   in[6] = PACK(b2,b1,b3,b4);
   in[7] = PACK(b2,b1,b4,b3);

   in[8] = PACK(b2,b3,b1,b4);
   in[9] = PACK(b2,b3,b4,b1);

   in[10] = PACK(b2,b4,b1,b3);
   in[11] = PACK(b2,b4,b3,b1);

   in[12] = PACK(b3,b1,b2,b4);
   in[13] = PACK(b3,b1,b4,b2);

   in[14] = PACK(b3,b2,b1,b4);
   in[15] = PACK(b3,b2,b4,b1);

   in[16] = PACK(b3,b4,b1,b4);
   in[17] = PACK(b3,b4,b4,b1);

   in[18] = PACK(b4,b1,b2,b3);
   in[19] = PACK(b4,b1,b3,b2);

   in[20] = PACK(b4,b2,b1,b3);
   in[21] = PACK(b4,b2,b3,b1);

   in[22] = PACK(b4,b3,b1,b2);
   in[23] = PACK(b4,b3,b2,b1);
   for(i=0;i<24;i++) printf("in[%2d]=%08x CONVERT4=%08x htonl=%08x\n",i,in[i],
		CONVERT4(in[i]),htonl(in[i]));

	printf("\nrunning CONVERT4...\n");
	system("date");
   for(j=0;j<n;j++) for(i=0;i<24;i++) {
	   CONVERT4(in[i]);
	}
	system("date");

	printf("\nrunning hton...\n");
	system("date");
   for(j=0;j<n;j++) for(i=0;i<24;i++) {
	   htonl(in[i]);
	}
	system("date");

	printf("\nrunning null...\n");
	system("date");
   for(j=0;j<n;j++) for(i=0;i<24;i++) {
	   DMY(in[i]);
	}
	system("date");

}
