#!/bin/bash
#Windows
mkdir build
cd build
make ../../.. test CXX=x86_64-w64-mingw32-g++
mkdir audio
cd audio
curl "https://file-examples-com.github.io/uploads/2017/11/file_example_MP3_5MG.mp3" --output mp3.mp3
curl "https://file-examples-com.github.io/uploads/2017/11/file_example_WAV_10MG.wav" --output wav.wav
curl "https://file-examples-com.github.io/uploads/2017/11/file_example_OOG_5MG.ogg" -- output ogg.ogg
cd ..
rptest --async --mp3
rptest --mp3
rptest --wav
rptest --ogg

