#!/bin/bash
# Run this from within a bash shell
cmake -G "Unix Makefiles" \
	-DCMAKE_CXX_FLAGS_RELEASE=-O3 \
	-DCMAKE_INSTALL_PREFIX=`pwd`/../../.. \
	../../source
