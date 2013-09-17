#!/bin/sh

install_name_tool -change /opt/local/lib/libstdc++.6.dylib /Library/Application\ Support/BEDOPS/libstdc++.6.dylib /usr/local/bin/wig2bed_bin
install_name_tool -change /opt/local/lib/gcc47/libgcc_s.1.dylib /Library/Application\ Support/BEDOPS/libgcc_s.1.dylib /usr/local/bin/wig2bed_bin

exit 0


