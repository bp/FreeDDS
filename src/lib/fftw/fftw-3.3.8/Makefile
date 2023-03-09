########################################################################
#                      copyright 2008, BP America                      #
#                          All Rights Reserved                         #
########################################################################
#
COMPILER:=$(if $(COMPILER),$(COMPILER),$(compiler))

#-----------------------------------------------------------------------
#  Setup Host information
#-----------------------------------------------------------------------
#
All:= $(shell LNDMRK=${LNDMRK};export LNDMRK;${DDSROOT}/src/lib/host/HostDir -a)
All:= $(subst ^,^ ,$(shell echo "${All}"|sed -e 's/ /SPACE/g'))
TArchBinDir:=$(subst ^,,$(subst SPACE, ,$(word  2,${All})))
ArchName   :=$(subst ^,,$(subst SPACE, ,$(word  1,${All})))

#
#-----------------------------------------------------------------------
#  Setup the Compiler settings
#-----------------------------------------------------------------------
#
Args:=$(if ${LNDMRK},LNDMRK) $(if ${EXTEND},EXTEND) $(if ${FFT},FFT) \
      $(if ${MP},MP) $(if ${MPI}${MPIX},MPI) $(if ${COMPILER},COMPILER=${COMPILER})
All:= $(shell ${DDSROOT}/bin/HostComp -a ${Args})
All:= $(subst ^,^ ,$(shell echo "${All}"|sed -e 's/ /SPACE/g'))
CompExt:=$(subst ^,,$(subst SPACE, ,$(word  3,${All})))
CompVer:=$(subst ^,,$(subst SPACE, ,$(word  4,${All})))
CC     :=$(subst ^,,$(subst SPACE, ,$(word  5,${All})))
FC     :=$(subst ^,,$(subst SPACE, ,$(word  7,${All})))
FCFlags :=$(subst ^,,$(subst SPACE, ,$(word 10,${All})))
FPICFlg :=$(shell echo ${FCFlags} | grep '[-]fPIC' > /dev/null 2>&1 && echo "--with-pic" )

TArchDir:=${TArchBinDir}${CompVer}
VARCH   :=${ArchName}${CompExt}
VARCH   :=${VARCH}$(shell if [ "${VARCH}" = "IRIX64" ]; then echo "-`uname -m`"; fi)
SINGLE  :=${VARCH}-single
DOUBLE  :=${VARCH}-double

CONFIGEXTRA :=

ifneq (,$(findstring w64-mingw32,$(CC)))
   CONFIGEXTRA := "--host=x86_64-w64-mingw32 --disable-alloca --with-our-malloc --with-windows-f77-mangling --disable-shared --enable-static"
else
endif

.PHONY: single double check install allinstall

target: single double

single:
	@if [ ! -d ${SINGLE} ]; then mkdir ${SINGLE}; fi
	@if [ ! -f ${SINGLE}/Makefile ]; then \
      cd ${SINGLE}; \
      ../configure --enable-single --prefix=${PWD}/${SINGLE} "${CONFIGEXTRA}" \
         CC="${CC}" F77="${FC}" ${FPICFlg}; \
   fi
	@cd ${SINGLE}; make; 

double:
	@if [ ! -d ${DOUBLE} ]; then mkdir ${DOUBLE}; fi
	@if [ ! -f ${DOUBLE}/Makefile ]; then \
      cd ${DOUBLE}; \
      ../configure --prefix=${PWD}/${DOUBLE} "${CONFIGEXTRA}" \
         CC="${CC}" F77="${FC}" ${FPICFlg}; \
   fi
	@cd ${DOUBLE}; make; 

check:
	@cd ${SINGLE}; make check; 
	@cd ${DOUBLE}; make check; 

install:
	@cd ${SINGLE}; make install
	@cd ${DOUBLE}; make install
	@cp ${SINGLE}/.libs/libfftw*.a ${DOUBLE}/.libs/libfftw*.a ${DDSROOT}/lib/${TArchDir}
	@cp api/fftw3.* ${DDSROOT}/include

all:
	@if [ "${REMOTE_HOSTS}" ]; then\
           REMOTE_HOSTS="${REMOTE_HOSTS}";export REMOTE_HOSTS;\
        fi;\
        ${DDSROOT}/bin/makeremotes make
	@find . -name 'libfftw*.a' -exec ls -tl {} \;

allinstall:
	@if [ "${REMOTE_HOSTS}" ]; then\
           REMOTE_HOSTS="${REMOTE_HOSTS}";export REMOTE_HOSTS;\
        fi;\
        ${DDSROOT}/bin/makeremotes make install
	@ls -Fatl ${DDSROOT}/lib/*/libfftw*.a
	
clean:
	@cd ${SINGLE}; make clean
	@cd ${DOUBLE}; make clean

distclean:
	@rm -rf ${SINGLE}
	@rm -rf ${DOUBLE}




