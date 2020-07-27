# RPAudio 
#### Build Status: 
- Dev branch [![Build Status](https://travis-ci.com/AnzoDK/RPAudio.svg?branch=dev)](https://travis-ci.com/AnzoDK/RPAudio)
- Master branch [![Build Status](https://travis-ci.com/AnzoDK/RPAudio.svg?branch=master)](https://travis-ci.com/AnzoDK/RPAudio)
### What is this?
- This is a small audio library that as of now supports OggVorbis through the Vorbisfile library, wav through a simple selfmade wav reader and mp3 through the MiniMp3 lib. AudioDevice handling is done through OpenAL so it should be both Windows and Linux compatible.

### Why bother?
- Good question. The core idea is to provide a simple wrapper for OpenAL with a little extra functionallity and at the same time, I intend to use it for a small and simple game engine (mainly for 2D)

### How to use
- No clue - But for now you can test its capabilities with the included test program. Build the test program with the BuildTest.sh script. Now run RPtest. A terminal window will now popup and all you have to do is type the path of a either .wav .mp3 or .ogg file and it will play it back for you. Type exit to exit it
