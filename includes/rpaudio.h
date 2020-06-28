/* Created by AnzoDK@Rosenørn-Productions
 * This lib may be freely distrubuted and modified as long as a link to the original repository, and the licence is bundled with the library
 * 
 * If to be modified:
 * 
 * 1. It is important to note that the lib is built to reserve Sources[0] for background music
 * 2. That the remaining 4 sources are reserved for soundeffects
 * 
 * It will be easier to modify if those two facts are to be unchanged.
 * 
 */



#pragma once
#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define DIV 1048576 
#define WIDTH 7
#endif

#ifndef _WIN32_
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#endif
#include <iostream>
#include <vector>
#include <fstream>
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
#include <AL/al.h>
#include <AL/alc.h> 
namespace rp
{
    enum FileType{ogg,wav};
    
    struct AudioFile
    {
        AudioFile(std::string path);
        ~AudioFile();
        std::string path;
        std::ifstream file;
        char* data;
        vorbis_info* vi;
        OggVorbis_File vf;
        long unsigned int bufferSize;
        FileType ft;
        ALuint source;
        ALuint buffer;
    };
    
    class RosenoernAudio
    {
    public:
        RosenoernAudio(bool debug=0);
        void init();
        void LoadBGM(std::string path, bool async=0);
        void AddToQueue(std::string _path);
        void PlayFromQueue();
        void PlaySound(std::string _path);
        void PauseAudio();
        ~RosenoernAudio();
    private:
        std::vector<AudioFile*> queue;
        ALCdevice* audioDevice;
        ALuint* buffers;
        ALsizei bufferCounter;
        ALuint* sources;
        ALCcontext* context;
        std::vector<int>freeBuffers;
        bool debug;
        std::string ErrorStrHandler(int err);
        void CheckErrors();
        int FindFreeBuffer();
        void ClearBuffer(ALuint* bufPtr,int amount);
        AudioFile* GetAudioBase(std::string _path);
    };
    
}
