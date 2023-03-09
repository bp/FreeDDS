########################################################################
#                      copyright 2005, BP America                      #
#                          All Rights Reserved                         #
########################################################################
# $Id: Makefile 20 2009-01-07 16:38:59Z user $

### The application Makefile should be patterned after "Makefile.*".
### This Makefile is different because make is making itself,
### and *must* avoid the installed/production files.

include ./MakeMacro
include ./MakeSrcMacro

MAKE_ARCH= \
   ./MakeMacro  \
   ./MakeArchMacro  \
   Makefile2 \
   ./MakeArchRules

include ./MakeSrcRules
