#!/usr/bin/env python

import os, sys, subprocess

unstarch = sys.argv[1]
archive = sys.argv[2]
flag = "--has-duplicate"

chr1_proc = subprocess.Popen([unstarch, 'chr1', flag, archive], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
chr1_proc_out, chr1_proc_err = chr1_proc.communicate()
chr1_proc_out.strip('\n')
if int(chr1_proc_out) == 0:
    print "[STARCH] ERROR: chr1 is reporting it doesn't have a duplicate element, but it does!"
    sys.exit(os.EX_USAGE)

chr2_proc = subprocess.Popen([unstarch, 'chr2', flag, archive], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
chr2_proc_out, chr2_proc_err = chr2_proc.communicate()
chr2_proc_out.strip('\n')
if int(chr2_proc_out) == 1:
    print "[STARCH] ERROR: chr2 is reporting it has a duplicate element, but it doesn't!"
    sys.exit(os.EX_USAGE)

chr3_proc = subprocess.Popen([unstarch, 'chr3', flag, archive], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
chr3_proc_out, chr3_proc_err = chr3_proc.communicate()
chr3_proc_out.strip('\n')
if int(chr3_proc_out) == 1:
    print "[STARCH] ERROR: chr3 is reporting it has a duplicate element, but it doesn't!"
    sys.exit(os.EX_USAGE)

chrAll_proc = subprocess.Popen([unstarch, 'all', flag, archive], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
chrAll_proc_out, chrAll_proc_err = chrAll_proc.communicate()
chrAll_proc_out.strip('\n')
if int(chrAll_proc_out) == 0:
    print "[STARCH] ERROR: 'all' chromosome usage is reporting the archive doesn't have a duplicate element, but there is a duplicate element in chr1!"
    sys.exit(os.EX_USAGE)

chrOmit_proc = subprocess.Popen([unstarch, flag, archive], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
chrOmit_proc_out, chrOmit_proc_err = chrOmit_proc.communicate()
chrOmit_proc_out.strip('\n')
if int(chrOmit_proc_out) == 0:
    print "[STARCH] ERROR: omitted chromosome usage is reporting the archive doesn't have a duplicate element, but there is a duplicate element in chr1!"
    sys.exit(os.EX_USAGE)

sys.exit(os.EX_OK)
