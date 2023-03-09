########################################################################
#                 copyright 2009, BP America Inc.                      #
#                        All Rights Reserved                           #
########################################################################
#
# gmake Makefile for dds local make from DDSROOT
#
# This make will make all libraries within dds
#

targets: 
	(cd src/lib/host;  ${MAKE})
	(cd src/lib/host;  ${MAKE} install)
	(cd src/lib/gio;   ${MAKE})
	(cd src/lib/gio;   ${MAKE} install)
	(cd src/lib/dds3;  ${MAKE})
	(cd src/lib/dds3;  ${MAKE} install)
	(cd src/lib/fftw;  ${MAKE})
	(cd src/lib/fftw;  ${MAKE} install)
	(cd src/lib/fft;   ${MAKE})
	(cd src/lib/fft;   ${MAKE} install)
	(if [ -f src/scripts ]; then cd src/scripts; ${MAKE}; fi)
	find ${DDSROOT}/lib -noleaf -name 'lib*.a'
	./CheckLibs

clean:
	(cd src/lib/host;  ${MAKE} clean)
	(cd src/lib/gio;   ${MAKE} clean)
	(cd src/lib/dds3;  ${MAKE} clean)
	(cd src/lib/fftw;  ${MAKE} clean)
	(cd src/lib/fft;   ${MAKE} clean)

apps:
	./MakeApps
	./CheckApps

appsclean:
	./MakeAppsClean

