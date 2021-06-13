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
 * 
 * Datasheet located at http://mpgedit.org/mpgedit/mpeg_format/MP3Format.html for mp3 format
 */



#pragma once
#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define DIV 1048576 
#define WIDTH 7
#define usleep Sleep
#define TIMECONVERT 100
#endif

#ifndef _WIN32
#define TIMECONVERT 1000
#include <unistd.h>
#include <string>
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
    typedef _IO_FILE C_FILE;
    enum FileType{ogg,wav,mp3};
    enum LoggerTarget{TERMINAL,FILE};
    
    class Logger
    {
    public:
        Logger(bool printState,LoggerTarget target)
        {
            m_printState = printState;
            m_target = target;
        }
        Logger(bool printState, LoggerTarget target, std::string logPath)
        {
            m_printState = printState;
            m_logPath = logPath;
            m_target = target;
        }
        void Log(std::string str, bool ignoreState=0)
        {
            if(m_target == LoggerTarget::TERMINAL)
            {
                if(m_printState || ignoreState)
                {
                    std::cout << m_prefix << str << std::endl;
                }
            }
            else if(m_target == LoggerTarget::FILE)
            {
                //We don't care about printState if we are printing to a file
                std::ofstream out = std::ofstream(m_logPath, std::ios::out | std::ios::ate);
                out << m_prefix << str << std::endl;
                out.close();
            }
        }
        void SetPrefix(std::string prefix)
        {
            m_prefix = prefix;
        }
        void SetState(bool state)
        {
            m_printState = state;
        }
        void SetTarget(LoggerTarget target)
        {
            m_target = target;
        }
    private:
        bool m_printState;
        std::string m_logPath = "./RPAudioLog.log";
        std::string m_prefix = "";
        LoggerTarget m_target;
    };
    
    struct AudioFile
    {
        AudioFile(std::string path, Logger& logger, bool debug=0);
        ~AudioFile();
        std::string path;
        std::ifstream file;
        uint8_t* data;
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
        RosenoernAudio(bool debug=0, int buffers=3);
        void init();
        void SetDebugState(bool state);
        void SetDebugMode(LoggerTarget target);
        void LoadBGM(std::string path, bool async=0);
        void AddToQueue(std::string _path);
        int GetQueueLength();
        void PlayFromQueue();
        void PlaySound(std::string _path);
        void PauseAudio();
        void SetDebugPrefix(std::string _prefix);
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
        int FindFreeSource();
        std::vector<int> GetPlayingSources();
        void ClearBuffer(ALuint* bufPtr,int amount);
        AudioFile* GetAudioBase(std::string _path);
        std::vector<AudioFile*> playingQueue;
        std::string m_debugPrefix = "";
        Logger* m_logger;
    };
    
}
