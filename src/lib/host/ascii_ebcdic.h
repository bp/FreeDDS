/*
RCS-t=HOST, conversion among ASCII and EBCDIC characters

*/

#ifndef _ASCII_EBCDIC
#define _ASCII_EBCDIC

/***********************/
/* function prototypes */
/***********************/

int (ascii_to_ebcdic)(int c);

int (ebcdic_to_ascii)(int c);

int (ascii_to_ebcdic_tilde)(int c);

int (ebcdic_to_ascii_tilde)(int c);

int (to_ebcdic_tilde)(int c);

int (to_ascii_tilde)(int c);


/********************************/
/* masking macros for functions */
/********************************/

#define ascii_to_ebcdic(c) ((unsigned char)toEBCDIC_table[0377 & c])

#define ebcdic_to_ascii(c) ((unsigned char)toASCII_table[0377 & c])

#define ascii_to_ebcdic_tilde(c) ((unsigned char)toEBCDIC_tilde[0377 & c])

#define ebcdic_to_ascii_tilde(c) ((unsigned char)toASCII_tilde[0377 & c])

#define to_ebcdic_tilde(c) ((unsigned char)EBCDIC_tilde[0377 & c])

#define to_ascii_tilde(c) ((unsigned char)ASCII_tilde[0377 & c])

extern const char toASCII_table[256];
extern const char toEBCDIC_table[256];
extern const char toASCII_tilde[256];
extern const char toEBCDIC_tilde[256];
extern const char ASCII_tilde[256];
extern const char EBCDIC_tilde[256];

#endif
