#!/bin/bash
#Windows - LIB
Comp=x86_64-w64-mingw32-g++
x86_64-w64-mingw32-g++ --version &> /dev/null
if [ $? -eq 0 ]
then
    echo "x86_64-w64-mingw32-g++ Works!!"
    Comp=g++-9
else
    echo "x86_64-w64-mingw32-g++ does not work - please install it before compiling"
    exit 1
    
fi
ldd --version &> /dev/null
if [ $? -ne 0 ]
then
    echo "ldd could not be found"
    exit 1
fi
make lib CXX=$Comp OS=Windows LIB=1
if [ $? -ne 0 ]
then
    echo "Make failed..."
    exit 1
fi
ldd ./RPAudio.dll

