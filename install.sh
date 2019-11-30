#!/bin/bash

shellPath="$(dirname "${BASH_SOURCE[0]}")"
cd $shellPath
echo $PWD
cd "base"
echo $PWD

if [ !  -e "build" ]
then
	mkdir "build"
fi

cd "build"
echo $PWD
cmake ..
make


cd .. && cd ..
echo $PWD
cp base/build/libasynclogStaticLib.a .
if [ ! -e "build" ]
then
	mkdir "build"
fi

cd "build"
cmake ..
make
