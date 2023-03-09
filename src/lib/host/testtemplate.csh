#!/bin/TSTSHELL

#=
#= testNNN
#= DESCRIPTION:
#=

set EXE      = `ddsarg arg=exe      default=none $*`
set INPUTDIR = `ddsarg arg=inputdir default=none $*`
set TESTDIR  = `ddsarg arg=testdir  default=none $*`
if ( "$EXE" == "none" || "$INPUTDIR" == "none" || "$TESTDIR" == "none" ) then
    echo "testNNN: need to specify exe= inputdir= and testdir=" 
    exit -1
endif

#
# Define the directory where we may have some input data stored.
# The INPUTDIR variable is defined by the Makefile.
# You can create subdirectories with test data specific for your code.
#
# NOTE: always access this data through the INPUTDIR variable,
#       as the physical disk location may be changed in the future.
# NOTE: in some cases you can also create your own synthetic data in the test script.
#
#set INPUTDIR = $INPUTDIR/MY_TEST_DATA
echo "INPUTDIR=$INPUTDIR" 

#
# Go to the location where the test script will be run.
# Usually this is defined by the Makefile to a local disk, but it can be overridden by:
# gmake tests DDS_TEST_DIR=/some/other/test/dir
#
cd $TESTDIR

#
# Set the time stamp to 0 so that every dds_prtmsg, dds_prtcon and dds_prterr message
# in the print file is preceded by a time stamp.
#
setenv DDS_TIMESTAMP 0

#
# Create a parfile for the test.
# This is preferable to putting "command-line" parameters,
# as it makes it easier to restart the test in a debugger.
#
cat << EOF >! parfile
in= $INPUTDIR/input
out=output
EOF

#
# Run the test
#
$EXE par=parfile  >&! messages
if ( $status != 0 ) then
   echo "TESTNNN: FATAL PROBLEM." 
   exit -1
endif

#
# It can be useful to directly create a QC plot.
# You can usually use ddsplot or dds2fig.
# If you do this, then modify the FINISHED OK line below.
#
#mergeND in= $INPUTDIR/input output merge_axis=2 |\
#transpose order=321 |\
#ddsplot mode=image hw1=Samples reversey= title= \"Input VS. Output\" landscape=yes |\
#convert - results.jpg
#if ( ! -f results.jpg ) then
#   echo "TESTNNN: FATAL PROBLEM: Unable to plot results"
#   exit -1
#endif


echo " " 
echo "TESTNNN: FINISHED OK." 
#echo "TESTNNN: FINISHED OK. Look at `pwd`/results.jpg" 
echo " " 

