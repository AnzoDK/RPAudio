#!/bin/bash
#Linux
Comp=0
x86_64-w64-mingw32-g++ --version
if [ $? -eq 0 ]
then
    echo "x86_64-w64-mingw32-g++ works!"
    Comp=x86_64-w64-mingw32-g++
else
    echo "x86_64-w64-mingw32-g++ does not work! - No other Windows compiler available... Exiting"
    exit 1
    
fi
ldd --version
if [ $? -ne 0 ]
then
    echo "ldd could not be found"
    exit 1
fi
make CXX=$Comp EX=.exe OS=Windows
chmod +x ./RPtest.exe
mkdir -p audio
ldd ./RPtest.exe


