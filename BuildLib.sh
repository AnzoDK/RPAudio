#!/bin/bash
#Linux - LIB
DebugLevel="DEBUG=-g3"

Comp=g++
g++ --version &> /dev/null
g++-9 --version &> /dev/null
if [ $? -eq 0 ]
then
    echo "g++-9 works!"
    Comp=g++-9
else
    echo "g++-9 does not work! - defaulting to g++"
    g++ --version  &> /dev/null
    if [ $? -ne 0 ]
    then
        echo "No g++ version found"
        exit 1
    fi
    
fi
ldd --version  &> /dev/null
if [ $? -ne 0 ]
then
    echo "ldd could not be found"
    exit 1
fi
make lib $DebugLevel CXX=$Comp OS=Linux LIB=1
ldd ./rpaudio.so

