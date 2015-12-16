#!/bin/bash
# Run this from within a bash shell
if ! [ -z "$1" ] && [ $1 = "reset" ]
then
    echo "rm -rf CMake* ...\n"
    rm -rf CMake*
else
    echo "Warning: some changes may not be updated this time.\n"
fi
cmake -G "Unix Makefiles" \
	-DCMAKE_CXX_FLAGS_RELEASE=-O3 \
	-DCMAKE_INSTALL_PREFIX=`pwd`/../../.. \
	../../source
