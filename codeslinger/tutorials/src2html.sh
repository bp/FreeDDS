#!/bin/sh

#
# Define the programs to convert
#
SOURCES="\
avg \
scale  \
tttn   \
transp \
taper  \
rcfft  \
xyrange"

#
# Also define source file extensions to consider
#
EXTENTS="c F f F90 f90"

APILIST="\
fdds_axis_prod \
fdds_close \
fdds_closepr \
fdds_dict \
fdds_errors \
fdds_free \
fdds_get \
fdds_index \
fdds_initmpi \
fdds_initopenmp \
fdds_lseek8 \
fdds_malloc8 \
fdds_member \
fdds_openpr \
fdds_printf \
fdds_prtcon \
fdds_prterr \
fdds_prtmsg \
fdds_put \
fdds_scanf \
fdds_scank \
fddx_dict \
fddx_in \
fddx_in2 \
fddx_out \
fddx_read \
fddx_write \
cdds_axis_prod \
cdds_close \
cdds_closepr \
cdds_dict \
cdds_errors \
cdds_free \
cdds_get \
cdds_index \
cdds_initmpi \
cdds_initopenmp \
cdds_lseek8 \
cdds_malloc \
cdds_member \
cdds_openpr \
cdds_printf \
cdds_prtcon \
cdds_prterr \
cdds_prtmsg \
cdds_put \
cdds_scanf \
cdds_scank \
cdds_scant \
cddx_dict \
cddx_in \
cddx_in2 \
cddx_out \
cddx_read \
cddx_write"


#
# Now go to work
#
for SRC in $SOURCES; do

  echo
  echo Conversions for $SRC:

  FILES=""
  for EXT in $EXTENTS; do
    FILES="$FILES `ls -1 ../$SRC/*.$EXT 2>/dev/null`"
  done

  for SRC_FILE in $FILES; do

    HTML_FILE=`basename $SRC_FILE | awk -F. '{print $1}'`.html

    echo $SRC_FILE $HTML_FILE |\
    awk '{printf"  %-27s -> %-20s\n",$1,$2}'

    sed -e "s/&/\&amp;/g ;
            s/</\&lt;/g  ;
            s/>/\&gt;/g"   < $SRC_FILE > $HTML_FILE

    APIDIR=fapishort
    EXT=`echo $SRC_FILE | awk -F. '{print $NF}'`
    if [ "$EXT" == "c" ]; then APIDIR=capishort; fi

    for API in $APILIST; do
      # handle API functions with name collisions first
      if [ "$API" == "fdds_close" ] ||
         [ "$API" == "cdds_close" ] ||
         [ "$API" == "fddx_in"    ] ||
         [ "$API" == "cddx_in"    ]; then
        sed -i -e "s/${API}(/<a href=\"..\/$APIDIR\/$API.html\">$API<\/a>(/g" $HTML_FILE
      else
        sed -i -e "s/$API/<a href=\"..\/$APIDIR\/$API.html\">$API<\/a>/g" $HTML_FILE
      fi
    done

    INCDIR="..\\/resources\\/"

    # Fix includes that already have <,> changed to &lt; and &gt;
    sed -i -e "s/;fdds.h/;<a href=\"${INCDIR}fdds.h.html\">fdds.h<\/a>/g" $HTML_FILE
    sed -i -e "s/;f90dds.h/;<a href=\"${INCDIR}f90dds.h.html\">f90dds.h<\/a>/g" $HTML_FILE
    sed -i -e "s/;cdds.h/;<a href=\"${INCDIR}cdds.h.html\">cdds.h<\/a>/g" $HTML_FILE

    sed -i -e "s/;fft.h/;<a href=\"${INCDIR}fft.h.html\">fft.h<\/a>/g" $HTML_FILE
    sed -i -e "s/;f90fft.h/;<a href=\"${INCDIR}f90fft.h.html\">f90fft.h<\/a>/g" $HTML_FILE
    sed -i -e "s/;cfft.h/;<a href=\"${INCDIR}cfft.h.html\">cfft.h<\/a>/g" $HTML_FILE

  done

done
