
Files := asciidump dds_bu dds_rs dds_mv dds_rm dds_files dds_axis \
         dds_what dds_when dds_where dds_who dds_www makedict \
         dds_prtdiff mbs-list mbs-show mbs-which

Mans:= $(shell ls *.1)

DDS_DIST:=$(if ${DDS_DIST},${DDS_DIST},$(shell if [ -d /tstapps ]; then echo /tstapps; else echo /tstapps/global; fi))
ASI:=$(shell if [ -d ${DDS_DIST}/asi ]; then echo "/asi"; fi)

CHPC_HOST:=""
CAMPUS_HOST:=""
DIST_ID:=""

CHPC:=$(shell if [ `ssh -q -o PubkeyAuthentication=yes -o PasswordAuthentication=no -o ConnectTimeout=3 ${DIST_ID}@${CHPC_HOST} hostname 2>&1` ]; then echo OK; fi)
CAMPUS:=$(shell if [ `ssh -q -o PubkeyAuthentication=yes -o PasswordAuthentication=no -o ConnectTimeout=3 ${DIST_ID}@${CAMPUS_HOST} hostname 2>&1` ]; then echo OK; fi)

target: $(patsubst %, ${DDS_DIST}${ASI}/bin/%, ${Files}) $(patsubst %, ${DDS_DIST}${ASI}/man/mbs/man1/%, ${Mans})

ifeq "${CHPC}" "OK"
${DDS_DIST}${ASI}/bin/%: %
	@ssh -l ${DIST_ID} ${CHPC_HOST} test -d ${DDS_DIST}${ASI}/bin '||' mkdir -p ${DDS_DIST}${ASI}/bin
	-@ssh -l ${DIST_ID} ${CHPC_HOST} test -f $@ '&&' unlink $@
	scp $< ${DIST_ID}@${CHPC_HOST}:$@
	@ssh -l ${DIST_ID} ${CHPC_HOST} chmod 755 $@
ifeq "${CAMPUS}" "OK"
	@ssh -l ${DIST_ID} ${CAMPUS_HOST} test -d ${DDS_DIST}${ASI}/bin '||' mkdir -p ${DDS_DIST}${ASI}/bin
	-@ssh -l ${DIST_ID} ${CAMPUS_HOST} test -f $@ '&&' unlink $@
	scp $< ${DIST_ID}@${CAMPUS_HOST}:$@
	@ssh -l ${DIST_ID} ${CAMPUS_HOST} chmod 755 $@
endif
else
${DDS_DIST}${ASI}/bin/%: %
	@mkdir -p ${DDS_DIST}${ASI}/bin
	@if [ -f $@ ]; then unlink $@; fi
	cp $< $@
	@chmod ug+w $@
endif

ifeq "${CHPC}" "OK"
${DDS_DIST}${ASI}/man/mbs/man1/%: %
	@ssh -l ${DIST_ID} ${CHPC_HOST} test -d ${DDS_DIST}${ASI}/man/mbs/man1 '||' mkdir -p ${DDS_DIST}${ASI}/man/mbs/man1
	-@ssh -l ${DIST_ID} ${CHPC_HOST} test -f $@ '&&' unlink $@
	scp $< ${DIST_ID}@${CHPC_HOST}:$@
	@ssh -l ${DIST_ID} ${CHPC_HOST} chmod 755 $@
ifeq "${CAMPUS}" "OK"
	@ssh -l ${DIST_ID} ${CAMPUS_HOST} test -d ${DDS_DIST}${ASI}/man/mbs/man1 '||' mkdir -p ${DDS_DIST}${ASI}/man/mbs/man1
	-@ssh -l ${DIST_ID} ${CAMPUS_HOST} test -f $@ '&&' unlink $@
	scp $< ${DIST_ID}@${CAMPUS_HOST}:$@
	@ssh -l ${DIST_ID} ${CAMPUS_HOST} chmod 755 $@
endif
else
${DDS_DIST}${ASI}/man/mbs/man1/%: %
	@mkdir -p ${DDS_DIST}${ASI}/man/mbs/man1
	@if [ -f $@ ]; then unlink $@; fi
	cp $< $@
	@chmod ug+w $@
endif
