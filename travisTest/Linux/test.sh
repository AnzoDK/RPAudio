#!/bin/bash
#Linux
$Comp=g++
g++ --version
g++-9 --version
if [ $? -eq 0 ]
then
    echo "g++-9 works!"
    $Comp=g++9
else
    echo "g++-9 does not work! - defaulting to g++"
fi
$old=$(pwd)
cd /
tree -f | grep "ogg/ogg.h"
make test CXX=$Comp
mkdir -p audio
cd audio
curl "https://file-examples-com.github.io/uploads/2017/11/file_example_MP3_5MG.mp3" --output mp3.mp3
curl "https://file-examples-com.github.io/uploads/2017/11/file_example_WAV_10MG.wav" --output wav.wav
curl "https://file-examples-com.github.io/uploads/2017/11/file_example_OOG_5MG.ogg" --output ogg.ogg
cd ..
rptest --async --mp3
rptest --mp3
rptest --wav
rptest --ogg
