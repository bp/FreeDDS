#ifndef __DDS_STACK_H
#define __DDS_STACK_H
 
/*
RCS-t=DDS, stack "utility" declarations
 
$Id: dds_stack.h 16 2009-01-06 21:52:47Z user $
*/

/*****************************************/
/* masking macros for stack manipulation */
/*****************************************/
 
/* woe be unto you, if STACK has side effects */
#define ddsStackPushChar(STACK, C) \
   ((((STACK)->top >= (STACK)->max) ? \
      ((STACK)->max *= 2, \
         (STACK)->ptr = (char*)ddsRealloc((STACK)->ptr, (STACK)->max)) : \
      (STACK)->ptr) [(STACK)->top] = (C), (STACK)->top++)
 
#define ddsStackPtr(STACK, INDEX)  ((void*)((STACK)->ptr + (INDEX)))
 
#define ddsStackPop(STACK, INDEX) ((STACK)->top -= (DDS_IDX)(INDEX))
 
#define ddsStackSet(STACK, INDEX) ((STACK)->top = (INDEX))
 
#define ddsStackTop(STACK) ((STACK)->top)

#define ddsStackTmp(COUNT, PREC) \
   ddsStackPtr(&dds_tmp, ddsStackPush(&dds_tmp, (COUNT) * (PREC), (PREC)))

#define ddsPushTmp(BYTES) \
   ddsStackPtr(&dds_tmp, ddsStackPush(&dds_tmp, (BYTES), HOST_MALLOC_A))

/*********************************/
/* typedef and enumerated values */
/*********************************/

typedef struct DDS_STACK
{  /* stack descriptor (relocatable memory block) */
   /**********************************************************************/
   /* DDS_STACK structures describe resizeable memory blocks.            */
   /* The byte "size" and "alignment" of pushed items may be specified.  */
   /* This allows any type to be pushed onto the stack and accessed.     */
   /*                                                                    */
   /* The stack's base address may change when data is pushed onto it    */
   /* because the memory may be re-allocated and the buffer moved.       */
   /*                                                                    */
   /*       ***************************************************          */
   /*       ***  WARNING    WARNING     WARNING    WARNING  ***          */
   /*       ***************************************************          */
   /*       ***  All pointers into the stack are invalid    ***          */
   /*       ***  after any data is pushed on the the stack. ***          */
   /*       ***  INVALID POINTER BUGS ARE DATA DEPENDENT.   ***          */
   /*       ***************************************************          */
   /*                                                                    */
   /* The DDS_IDX (char index) to existing items within the stack is     */
   /* always preserved, however the absolute address may change.         */
   /* Links between items should use a DDS_IDX index (char index).       */
   /*                                                                    */
   /* Support functions are named "ddsStack*.c" (Init, Term, Push, Pop...*/
   /**********************************************************************/
   size_t top;               /* index of next available character */
   size_t max;               /* malloc maximum, i.e. buf[max] */
   char *ptr;                /* pointer to start of buf */
} DDS_STACK;

/***********************/
/* function prototypes */
/***********************/

size_t ddsStackInit(
   DDS_STACK *stack,
   size_t min)
;
size_t (ddsStackPop)(
   DDS_STACK *stack,
   size_t size)
;
void * (ddsStackPtr)(
   DDS_STACK *stack,
   size_t index)
;
size_t (ddsStackPush)(
   DDS_STACK *stack,
   size_t size,
   size_t align)
;
size_t (ddsStackPushChar)(
   DDS_STACK *stack,
   int c)
;
const void ** (ddsStackPushPtr)(
   DDS_STACK *stack,
   const void *ptr)
;
size_t (ddsStackSet)(
   DDS_STACK *stack,
   size_t index)
;
size_t (ddsStackStr)(
   DDS_STACK *stack,
   const char *str)
;
size_t (ddsStackChar)(
   DDS_STACK *stack,
   const char *array,
   size_t size)
;
void ddsStackTerm(
   DDS_STACK *stack)
;

#endif /* __DDS_STACK_H */
