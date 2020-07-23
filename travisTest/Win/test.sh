#!/bin/bash
#Windows
make test CXX=x86_64-w64-mingw32-g++ EX=.exe
mkdir audio
cd audio
curl "https://file-examples-com.github.io/uploads/2017/11/file_example_MP3_5MG.mp3" --output mp3.mp3
curl "https://file-examples-com.github.io/uploads/2017/11/file_example_WAV_10MG.wav" --output wav.wav
curl "https://file-examples-com.github.io/uploads/2017/11/file_example_OOG_5MG.ogg" --output ogg.ogg
cd ..
wine rptest.exe --async --mp3
wine rptest.exe --mp3
wine rptest.exe --wav
wine rptest.exe --ogg

