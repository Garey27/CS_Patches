#!/bin/bash

cd $( dirname $0 )
mkdir -p build/ao

# .c
cp libao/src/config.c    build/
cp libao/src/ao_wmm.c    build/
cp libao/src/ao_null.c   build/
cp libao/src/audio_out.c build/
cp windows/dllmain.cpp   build/

# .h
cp windows/targetver.h           build/
cp libao/include/ao/ao.h         build/ao/
cp libao/include/ao/ao_private.h build/ao/
cp windows/os_types.h            build/ao/

# patch
cd build
while read line
do
	patch -u -p0 < $line
done < <(find ../patch | sort | grep -E '\.patch$')
