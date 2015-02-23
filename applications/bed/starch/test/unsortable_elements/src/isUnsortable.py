#!/usr/bin/env python

import os, sys, subprocess

starch = sys.argv[1]
flag = sys.argv[2]
bed = sys.argv[3]

chrAll_proc = subprocess.Popen([starch, flag, bed], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
chrAll_proc_out, chrAll_proc_err = chrAll_proc.communicate()
chrAll_proc_out.strip('\n')
if int(chrAll_proc.returncode) == 0:
    sys.stderr.write("[STARCH] ERROR: Was able to make an archive, but this process should have exited with an error: " + str(chrAll_proc.returncode) + "\n")
    sys.exit(os.EX_USAGE)
else:
    sys.stderr.write("[STARCH] Successful test (i.e., starch exited with an error): " + str(chrAll_proc.returncode) + "\n")

sys.exit(os.EX_OK)
