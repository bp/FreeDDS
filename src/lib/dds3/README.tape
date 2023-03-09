########################################################################
#                 copyright 2003, Amoco Production Company             #
#                             All Rights Reserved                      #
#                     an affiliate of BP America Inc.                  #
########################################################################

 File: README.tape

test cases and things to observe

sun4u-SC3/bridge

***************************************************************************

This sequence was done 28 Aug 1998 using gsul16, 8mm low density drive.

cram < classic.usp
   create tape using usp's cram and verify default blocking
      tcopy /dev/rmt/0l
      file 1: records 1 to 9: size 32768
      file 1: record 10: size 5184
      file 1: eof after 10 records: 300096 bytes
      eot
      total length: 300096 bytes
   verify that bridge can read it
      bridge in_data=/dev/rmt/0l in_format=usp out=junk

in=classic.usp dds_debug= dbg_call out_data=/dev/rmt/0l
   default physical blocking on usp tape is 32k blocks
      tcopy /dev/rmt/0l
      file 1: records 1 to 9: size 32768
      file 1: record 10: size 5395
      file 1: eof after 10 records: 300307 bytes
      eot
      total length: 300307 bytes
   verify that usp's cram can read it
      cram -r | sis_verify

in=classic64 dds_debug= dbg_call out_format=usp out_data=/dev/rmt/0l /dev/rmt/0l= capacity 800k
   verify and prompt for multiple output tapes in usp format.
   verify "capacity 800k" device attribute override.
   One foot of media at nominal capacity will probably override this.
      estimated media= 3.4 tapes, at capacity= 0.005 Gbytes per tape
      tcopy /dev/rmt/0l
      file 1: records 1 to 168: size 32768
      file 1: eof after 168 records: 5505024 bytes
      eot
      total length: 5505024 bytes
      same for tape2, tape3, then tape4...
      tcopy /dev/rmt/0l
      file 1: records 1 to 77: size 32768
      file 1: record 78: size 19112
      file 1: eof after 78 records: 2542248 bytes
      eot
      total length: 2542248 bytes
   verify that usp's cram can read multiple tape written by dds
      cram -r | sis_verify

in_data=/dev/rmt/0l /dev/rmt/0l= label '"t1 t2 t3 t4"' in_format=usp out=junk dds_debug= dbg_call
   verify that dds can read multiple tapes in usp's format

in=classic.usp dds_debug= dbg_call out_format=segy out_data=/dev/rmt/0l
   verify default media capacity for 8mm low density
      estimated media= 0.0 tapes, at capacity= 1.849 Gbytes per tape
   default physical blocking on segy tape is unblocked
      tcopy /dev/rmt/0l
      file 1: record 1: size 3200
      file 1: record 2: size 400
      file 1: records 3 to 404: size 724
      file 1: eof after 404 records: 294648 bytes
      eot
      total length: 294648 bytes

in=classic64 dds_debug= dbg_call out_format=segy out_data=/dev/rmt/0l /dev/rmt/0l= capacity 800k
   verify logic and prompts for multiple output tapes in segy format.
   verify "capacity 800k" device attribute override
      estimated media= 3.4 tapes, at capacity= 0.005 Gbytes per tape

in_data=/dev/rmt/0l in_format=segy out=junk dds_debug= dbg_call
   verify basic segy tape input without any label specification
      mount read tape /dev/rmt/0l (1) <no label>

in_data=/dev/rmt/0l in_format=segy out=junk dds_debug= dbg_call /dev/rmt/0l= label '"t1 t2"'
   verify segy tape input, with two tapes, each with their own line headers,
   by simply reading the same tape twice.
      mount read tape /dev/rmt/0l (1) t1
      prefetch= t2

*****************************************************************************
end of basic test, now try broader variations.

still testing on gsul16 with 8mm tapes
   uname -a
      SunOS gsul16 5.5.1 Generic_103640-01 sun4u sparc SUNW,Ultra-2

in=classic.usp dds_debug= dbg_call out_data=/dev/rmt/0l
   verify automagic detection of tape capacity, based upon device name.
      estimated media= 0.0 tapes, at capacity= 1.849 Gbytes per tape

in=classic.usp dds_debug= dbg_call out_data=/dev/rmt/0m
   verify automagic detection of tape capacity, based upon device name.
      estimated media= 0.0 tapes, at capacity= 3.622 Gbytes per tape

testing on Mike O'Brien's WS with DLT tapes
   Note: sun's device status doesn't seem to distinguish DLT3 and DLT4.
   Both "say" DLT4.
   uname -a
      SunOS ttsmjo04 5.5.1 Generic_103640-19 sun4u sparc SUNW,Ultra-2

in=classic.usp dds_debug= dbg_call out_data=/dev/rmt/1l
   verify automagic detection of tape capacity, based upon device name.
      estimated media= 0.0 tapes, at capacity= 19.512 Gbytes per tape

in=classic64 dds_debug= dbg_call out_format=usp out_data=/dev/rmt/1l /dev/rmt/1l= capacity 800k
   verify and prompt for multiple output tapes in usp format.
   verify "capacity 800k" device attribute override.
   One foot of media at nominal capacity will probably override this.

in=classic64 dds_debug= dbg_call out_format=segy out_data=/dev/rmt/1l /dev/rmt/1l= capacity 800k
   verify and prompt for multiple output tapes in segy format.
   verify "capacity 800k" device attribute override.
   One foot of media at nominal capacity will probably override this.

in_data=/dev/rmt/1l in_format=segy out=junk dds_debug= dbg_call /dev/rmt/1l= label '"t1 t2"'
   verify segy tape input, with two tapes, each with their own line headers,
   by simply reading the same tape twice.
      mount read tape /dev/rmt/0l (1) t1
      prefetch= t2

=============================================================
testing on gssgi2
   uname -a
      IRIX64 gssgi2 6.2 03131016 IP25
   /dev/rmt/tps90d2vns

tps90d2nsv

in=classic.usp dds_debug= dbg_call out_data=/dev/rmt/tps90d2nsv
   verify automagic detection of tape capacity, based upon device name.
      estimated media= 0.0 tapes, at capacity= 19.512 Gbytes per tape

in=classic64 dds_debug= dbg_call out_format=usp out_data=/dev/rmt/tps90d2nsv /dev/rmt/tps90d2nsv= capacity 800k
   verify and prompt for multiple output tapes in usp format.
   verify "capacity 800k" device attribute override.
   One foot of media at nominal capacity will probably override this.

in=classic64 dds_debug= dbg_call out_format=segy out_data=/dev/rmt/tps90d2nsv /dev/rmt/tps90d2nsv= capacity 800k
   verify and prompt for multiple output tapes in segy format.
   verify "capacity 800k" device attribute override.
   One foot of media at nominal capacity will probably override this.

in_data=/dev/rmt/tps90d2nsv in_format=segy out=junk dds_debug= dbg_call /dev/rmt/tps90d2nsv= label '"t1 t2"'
   verify segy tape input, with two tapes, each with their own line headers,
   by simply reading the same tape twice.
      mount read tape /dev/rmt/0l (1) t1
      prefetch= t2

