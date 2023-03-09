#ifndef __DDS_OPCODE_H
#define __DDS_OPCODE_H

/*
RCS-t=DDS, DDS_OPCODE enumeration and DDS_OPNOTE declaration/initialization

$Id: dds_opcode.h 16 2009-01-06 21:52:47Z user $
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#undef  DDS_OPDEF
#define DDS_OPDEF(NAME, FLAG, ARGS, PREC, PREFIX, INFIX, SUFFIX)  DDS_##NAME,

typedef enum DDS_OPCODE
{  /* DDS_EXPR opcodes (operation codes) */

   #include "dds_oplist.h"

   DDS_OPCODE_MAX
} DDS_OPCODE_ENUM;


#endif /* __DDS_OPCODE_H */

