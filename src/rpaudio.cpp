#include "../includes/rpaudio.h"
#include "../includes/commontools.h"
#define MINIMP3_IMPLEMENTATION
#include "../includes/minimp3/minimp3_ex.h"
#define sourceCount 5


using namespace rp;
RosenoernAudio::RosenoernAudio(bool _debug, int buffers)
{
    m_logger = new Logger(_debug,LoggerTarget::TERMINAL); //terminal is just default
    if(_debug)
    {
        debug = 1;
    }
    else
    {
        debug = 0;
    }
    bufferCounter = buffers;
}
void RosenoernAudio::SetDebugMode(LoggerTarget target)
{
    m_logger->SetTarget(target);
}
void RosenoernAudio::SetDebugState(bool state)
{
    debug = state;
    m_logger->SetState(state);
}

void RosenoernAudio::SetDebugPrefix(std::string _prefix)
{
    m_debugPrefix = _prefix;
}

void RosenoernAudio::init()
{
    playingQueue = std::vector<AudioFile*>();
    //RosenoernAudio::mp3d = mp3dec_t();
    //Init 3'rd party mp3 lib
    //mp3dec_init(&RosenoernAudio::mp3d);
    queue = std::vector<AudioFile*>();
    //Get default audio device
    int err = 0;
    freeBuffers = std::vector<int>();
    for (int i = 0;i < bufferCounter;i++)
    {
        freeBuffers.push_back(i);
    }
    audioDevice = alcOpenDevice(NULL);  // <--- Usually works :)
    err = alcGetError(audioDevice);
    if(err != 0 || debug)
    {
     m_logger->Log(ErrorStrHandler(err));
     if(err == 40965)
     {
         m_logger->Log("This Usually happen when testing the lib with travis. Testing decode capabilities - If this is a test on a system with no soundcard, then ignore all the AL errors");
     }
    }
    alGetError();
    context = alcCreateContext(audioDevice,NULL);
    if(alcMakeContextCurrent(context) == ALC_TRUE)
    {
     m_logger->Log("Created Context");   
    }
    else
    {
     m_logger->Log("Failed to create context");   
    }
    CheckErrors();
    

    buffers = new ALuint[bufferCounter];
    sources = new ALuint[sourceCount];
    CheckErrors();
    alGenBuffers(bufferCounter,buffers); // <-- This Though... Pops an error every fucking time - June 23, Finnaly fixed it :D - It was a context issue...
    CheckErrors();
    alGenSources(sourceCount,sources);
    CheckErrors();
    
    
}


RosenoernAudio::~RosenoernAudio()
{
    for(unsigned int i = 0; i < queue.size();i++)
    {
        delete(queue.at(i));
    }
    delete(m_logger);
    alDeleteBuffers(bufferCounter,buffers);
    alDeleteSources(sourceCount,sources);
    context = alcGetCurrentContext();
    alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(audioDevice);
    delete[] sources;
    delete[] buffers;
}
AudioFile::AudioFile(std::string _path, Logger& logger, bool debug)
{
  path =  _path;
  if(path.find(".ogg") != std::string::npos)
  {
      logger.Log("File type is ogg");
      
      ft = FileType::ogg;
      
      C_FILE* fp = fopen(path.c_str(),"rb");
      ov_open_callbacks(fp,&vf,NULL,0,OV_CALLBACKS_NOCLOSE);
      vi = ov_info(&vf,-1);
      size_t data_len = ov_pcm_total(&vf, -1) * vi->channels * 2;
      bufferSize = data_len;
      data = new uint8_t[bufferSize];
      char* tmpbuf = new char[bufferSize];
      for (size_t size = 0, offset = 0, sel=0; (size = ov_read(&vf, tmpbuf + offset, 4096, 0, 2, 1, (int*) &sel)) != 0; offset += size) {
          if(size < 0)
          {
           logger.Log("Ogg audio stream is invalid or corrupted");   
          }
	}
	fclose(fp);
        for(unsigned int i = 0; i < bufferSize; i++)
        {
          data[i] = tmpbuf[i];
        }
    delete[] tmpbuf;
    //delete(fp);
      
  }
      else if(path.find(".mp3") != std::string::npos)
    {
        ft = FileType::mp3;
        logger.Log("Filetype is MP3");
        //This may only work for decoding old mp3's
        file = std::ifstream(path,std::ios::binary | std::ios::ate);
        std::streamsize size = file.tellg();
        bufferSize = size;
        file.seekg(0, std::ios::beg);
        data = new uint8_t[bufferSize];
        char* tmpbuf = new char[bufferSize];
        file.read(tmpbuf,size);
        file.close();
        for(unsigned int i = 0; i < bufferSize; i++)
        {
          data[i] = tmpbuf[i];
        }
        delete[] tmpbuf;
        
    }
  else
  {
      file = std::ifstream(path,std::ios::binary | std::ios::ate);
      std::streamsize size = file.tellg();
      bufferSize = size;
      file.seekg(0, std::ios::beg);
      logger.Log("File type is wav");
      ft = FileType::wav;
      data = new uint8_t[bufferSize];
      char* tmpbuf = new char[bufferSize];
      file.read(tmpbuf,size);
      file.close();
      for(unsigned int i = 0; i < bufferSize; i++)
      {
         data[i] = tmpbuf[i];
      }
      delete[] tmpbuf;
      vi = nullptr;
      vf = OggVorbis_File();
  }
  
  
  //Debug
    logger.Log("Creating buffer with a size of: " + std::to_string(bufferSize) + " Byte(s)");
  

  source = 0;
  buffer = 0;
  
};
AudioFile::~AudioFile()
{
   if(data != nullptr)
   {
       delete[] data;
   }
   ov_clear(&vf);
   
   //free(reinterpret_cast<mp3dec_file_info_t*>(mp3Info)->buffer);
   //delete(vi);
};





AudioFile* RosenoernAudio::GetAudioBase(std::string _path)
{
    AudioFile* af = new AudioFile(_path,*m_logger,debug);
    ALuint buffer = buffers[FindFreeBuffer()];
    af->buffer = buffer;
    CheckErrors();
//     alBufferData(buffers[0],AL_FORMAT_STEREO16,&af->data,sizeof(af->data),af->vi->rate);
    m_logger->Log("Reading " + std::to_string(af->bufferSize) + " byte(s) from buffer");
    if(af->ft == FileType::ogg)
    {
            m_logger->Log("Clip freq: " + std::to_string(af->vi->rate) + " hz");
            alBufferData(buffer,AL_FORMAT_STEREO16,af->data,af->bufferSize,af->vi->rate);
            CheckErrors();
    }
    else if (af->ft == FileType::wav)
    {
        uint8_t sampleRateData[4];
        sampleRateData[0] = af->data[24];
        sampleRateData[1] = af->data[25];
        sampleRateData[2] = af->data[26];
        sampleRateData[3] = af->data[27];
        
        uint8_t channelsData[2];
        channelsData[0] = af->data[22];
        channelsData[1] = af->data[23];
        
        unsigned short channels = 0;
        uint32_t sampleRate = 0;
        memcpy(&sampleRate, sampleRateData,sizeof(sampleRateData));
        memcpy(&channels,channelsData, sizeof(channelsData));
        m_logger->Log("Clip freq: " + std::to_string(sampleRate) + "hz and is using " + std::to_string(channels) + " channel(s)");
        
        
        if(channels > 1)
        {
            alBufferData(buffer,AL_FORMAT_STEREO16,af->data,af->bufferSize,sampleRate);
        }
        else
        {
            //This warning is important - So it's enabled regardless of debug state
            m_logger->Log("Warning: This clip is in MONO - This can lead to problems if a stero track is being added to the queue",1);
            alBufferData(buffer,AL_FORMAT_MONO16,af->data,af->bufferSize,sampleRate);
        }
        CheckErrors();
           
        
    }
    else if(af->ft == FileType::mp3) //<-- Seems to have a bug that only play just about 1/2 of the audio file if its 3 min long (suspects the buffer can only hold about 1:30 stereo) - Please refer to GitHub issue #4
    {
        static mp3dec_t dec;
        mp3dec_init(&dec);
        mp3dec_file_info_t mp3Info;
        mp3dec_load(&dec,af->path.c_str(),&mp3Info, NULL, NULL);
        m_logger->Log("Freq: " + std::to_string(mp3Info.hz) + "Hz. - AVG. kbit/s rate: " + std::to_string(mp3Info.avg_bitrate_kbps) + " Kbits/s");
        
        //std::cout << "Freq: " << std::to_string(decoder.info.hz) << "Hz. - AVG. kbit/s rate: " << std::to_string(decoder.info.bitrate_kbps) << " Kbits/s"<< std::endl;
        int channels = 0;
        if(mp3Info.channels == 2)
        {
            channels = AL_FORMAT_STEREO16;
        }
        else
        {
            channels = AL_FORMAT_MONO16;
        }
        
        alBufferData(buffer,channels,mp3Info.buffer,mp3Info.samples*sizeof(mp3d_sample_t),mp3Info.hz);
        
    }
    else
    {
        //Should also be enabled regardless of debug state
     m_logger->Log("Unsupported fileformat - ignoring - No buffer will be created",1);
     af->data = nullptr;
    }
    return af;
}

void RosenoernAudio::LoadBGM(std::string _path, bool async)
{
    AudioFile* ab = GetAudioBase(_path);
    
    if(async)
    {
        //its already async
    }
    int c = 0;
    int loop = 0;
    alGetSourcei(sources[0],AL_LOOPING,&loop);
    CheckErrors();
    
    if(loop != 0)
    {
        //We can't have looping when we use more than one buffer
        m_logger->Log("Looping of BGM disabled");
        alSourcei(sources[0],AL_LOOPING,0); 
        CheckErrors();
    }
    alGetSourcei(sources[0],AL_BUFFERS_QUEUED,&c);
    CheckErrors();
    m_logger->Log("Current queue is " + std::to_string(c+1) + " long");
    alSourceQueueBuffers(sources[0],1,&ab->buffer);
    //alSourcei(sources[0],AL_BUFFER,buffers[0]);
    CheckErrors();
    //alSourcePlay(sources[0]);
    ab->source = sources[0];
    //return af;
    if(ab->ft == FileType::mp3)
    {
         ab->data = 0;   
    }
    playingQueue.push_back(ab);
}
void RosenoernAudio::AddToQueue(std::string _path)
{
   LoadBGM(_path);
}
int RosenoernAudio::GetQueueLength()
{
  int c = 0;
  alGetSourcei(sources[0],AL_BUFFERS_QUEUED,&c);
  return c;
}

std::string RosenoernAudio::ErrorStrHandler(int err)
{
    switch(err)
    {
        
        case AL_INVALID_VALUE:
            return "AL_INVALID_VALUE";
        break;
        
        case 40961:
            return "AL_INVALID_NAME OR ALC_INVALID_DEVICE";
        break;
        
        case AL_INVALID_OPERATION:
                return "AL_INVALID_OPERATION";
        break;
        
        case AL_NO_ERROR: 
            return "AL_NO_ERROR";
        break;
        
        case 40965:
            return "COULD NOT FIND AUDIO DEVICE";
        break;
        
        default:
            std::string error = std::string();
            error += "UNKNOWN OR UNIMPLEMENTED ERROR - CODE: ";
            error += std::to_string(err);
            return error;
        break;
    }
}

void RosenoernAudio::CheckErrors()
{
    int err = 0;
    err = alGetError();
    if(err != 0 || debug)
    {
     m_logger->Log(ErrorStrHandler(err));
    }
}
void RosenoernAudio::PlayFromQueue()
{
    alSourcePlay(sources[0]);
    
}
void RosenoernAudio::PlaySound(std::string _path)
{
    int buffNr = 0;
    buffNr = FindFreeSource();
    if(buffNr == -1)
    {
        m_logger->Log("No Free Source could be found - Adding to first soundeffect players queue");
        buffNr = 1;
    }
    AudioFile* ab = GetAudioBase(_path);
    ab->source = sources[buffNr];
    alSourceQueueBuffers(sources[buffNr],1,&ab->buffer);
    alSourcePlay(ab->source);
}
//Only work for queue
void RosenoernAudio::PauseAudio()
{
    alSourcePause(sources[0]);
}
void RosenoernAudio::ClearBuffer(ALuint* bufPtr, int amount)
{
    m_logger->Log("Entered ClearBuffer()");
   for(int a = 0; a < amount;a++)
   {
    for(int i = 0; i < bufferCounter;i++)
    {
        for(int u = 0; u < bufferCounter; u++)
        {
            if(bufPtr[a] == buffers[u])
            {
                freeBuffers.push_back(u);
                if(debug)
                {
                 m_logger->Log("Set buffer id: " + std::to_string(u) + " to empty");
                 for(unsigned int c = 0; c < playingQueue.size();c++)
                 {
                     if(playingQueue.at(c)->buffer == buffers[u])
                     {
                         playingQueue.erase(playingQueue.begin()+c-1);
                     }
                 }
                }
                return;
            }
        }
    }
   }
    m_logger->Log("Buffer could not be cleared...");
}
int RosenoernAudio::FindFreeBuffer()
{
    int buf = 0;
    if(freeBuffers.empty())
    {
        m_logger->Log("Ran out of buffers, Attempting to free an existing buffer");
        ALint toBeFreed = 0;
        ALuint freed = 0;
        CheckErrors();

        m_logger->Log("Pauses and then contunies music to avoid RPAuio bug 1");
        
        std::vector<int> playing = GetPlayingSources();
        for(int i = 0; i < playing.size();i++)// <-- Refer to issue 1 for explanation
        {
            alSourcePause(sources[playing.at(i)]);
        }
        for(int i = 0; i < bufferCounter;i++)
        {

            alGetSourcei(sources[i],AL_BUFFERS_PROCESSED,&toBeFreed);
            CheckErrors();
            if(toBeFreed > 0)
            {
                CheckErrors();
                alSourceUnqueueBuffers(sources[i],toBeFreed,&freed);
                CheckErrors();
                m_logger->Log("Buffer marked for potential freeing found !!");
                m_logger->Log("Attempting to clear buffer!");
                ClearBuffer(&freed,toBeFreed);
                alSourcePlay(sources[i]);
            }
        }
        for(int i = 0; i < playing.size();i++)
        {
            alSourcePlay(sources[playing.at(i)]);
        }
    }

    try{
    buf = freeBuffers.at(0);
    }
    catch(std::out_of_range ex)
    {
          m_logger->Log("Could not find or clear buffer - CRITICAL ERROR - more info: " + std::string(ex.what()));
          exit(-1);
          
    }
    freeBuffers.erase(freeBuffers.begin());
    m_logger->Log("There is now " + std::to_string(freeBuffers.size()) + " buffers left");
    return buf;
}
int RosenoernAudio::FindFreeSource()
{
    ALint state = 0;
    int source = 0;
    bool resolved = false;
    for(int i = 0; i < sourceCount;i++)
    {
        
        alGetSourcei(sources[i],AL_SOURCE_STATE,&state);
        if(state == AL_STOPPED || state == AL_INITIAL)
        {
            source = i;
            resolved = true;
            break;
        }
    }
    if(resolved)
    {
        return source;
    }
    else
    {
      return -1;  
    }
}
std::vector<int> RosenoernAudio::GetPlayingSources()
{
    std::vector<int> playing = std::vector<int>();
    int state = 0;
    for(int i = 0; i < sourceCount;i++)
    {
        
        alGetSourcei(sources[i],AL_SOURCE_STATE,&state);
        if(state == AL_PLAYING)
        {
            playing.push_back(i);
        }
    }
    return playing;
}







