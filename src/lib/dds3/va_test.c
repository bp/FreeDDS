/***
#include <stdio.h>
#include <stdarg.h>
***/

/*#include "dds_sys.h" */


#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#if defined(_WIN32) || defined(MINGW)
#include <io.h>
#else
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <stdarg.h>

#define FUNCPROTO 1

#include "gio.h"
#include "cdds.h"
#include "dds_config.h"
#include "dds_api.h"
#include "dds_stack.h"
#include "dds_util.h"
#include "dds_string.h"
#include "dds_table.h"
#include "dds_state.h"
#include "dds_message.h"
#include "dds_ptype.h"
#include "dds_media.h"
#include "dds_dict.h"
/***
#include <stdarg.h>
#include "dds_symbol.h"
#include "dds_hdr.h"
#include "dds_binary.h"
***/

int va_test(
   const char *fmt,
   ...)
{
   char msg[1000];
   va_list args;
 
   /*   Get the message */
 
   va_start(args, fmt);
   vsprintf(msg, fmt, args);
   va_end(args);
 
   fprintf(stdout, msg);
}
