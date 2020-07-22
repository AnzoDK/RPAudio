#include "../includes/rpaudio.h"
#include "../includes/commontools.h"
#define MINIMP3_IMPLEMENTATION
#include "../includes/minimp3/minimp3_ex.h"




using namespace rp;
RosenoernAudio::RosenoernAudio(bool _debug, int buffers)
{
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
     std::cout  << ErrorStrHandler(err) << std::endl;
    }
    alGetError();
    context = alcCreateContext(audioDevice,NULL);
    if(alcMakeContextCurrent(context) == ALC_TRUE)
    {
     std::cout << "Created Context" << std::endl;   
    }
    else
    {
     std::cout << "Failed to create context" << std::endl;   
    }
    CheckErrors();
    

    buffers = new ALuint[bufferCounter];
    sources = new ALuint[5];
    CheckErrors();
    alGenBuffers(bufferCounter,buffers); // <-- This Though... Pops an error every fucking time - June 23, Finnaly fixed it :D - It was a context issue...
    CheckErrors();
    alGenSources(5,sources);
    CheckErrors();
    
    
}


RosenoernAudio::~RosenoernAudio()
{
    for(uint i = 0; i < queue.size();i++)
    {
        delete(queue.at(i));
    }
    alDeleteBuffers(bufferCounter,buffers);
    alDeleteSources(5,sources);
    context = alcGetCurrentContext();
    alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(audioDevice);
    delete[] sources;
    delete[] buffers;
}
AudioFile::AudioFile(std::string _path)
{
  path =  _path;
  if(path.find(".ogg") != std::string::npos)
  {
      //Debug
      std::cout << "File type is ogg" << std::endl;
      
      ft = FileType::ogg;
      
      FILE* fp = fopen(path.c_str(),"rb");
      ov_open_callbacks(fp,&vf,NULL,0,OV_CALLBACKS_NOCLOSE);
      vi = ov_info(&vf,-1);
      size_t data_len = ov_pcm_total(&vf, -1) * vi->channels * 2;
      bufferSize = data_len;
      data = new uint8_t[bufferSize];
      char* tmpbuf = new char[bufferSize];
      for (size_t size = 0, offset = 0, sel=0; (size = ov_read(&vf, tmpbuf + offset, 4096, 0, 2, 1, (int*) &sel)) != 0; offset += size) {
          if(size < 0)
          {
           std::cout << "Ogg audio stream is invalid or corrupted" << std::endl;   
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
        std::cout << "Filetype is MP3" << std::endl;
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
      //Debug
      std::cout << "File type is wav" << std::endl;
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
  std::cout << "Creating buffer with a size of: " + std::to_string(bufferSize) + " Byte(s)" << std::endl;

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
    AudioFile* af = new AudioFile(_path);
    ALuint buffer = buffers[FindFreeBuffer()];
    af->buffer = buffer;
    CheckErrors();
//     alBufferData(buffers[0],AL_FORMAT_STEREO16,&af->data,sizeof(af->data),af->vi->rate);
    if(debug)
    {
        std::cout << "Reading " + std::to_string(af->bufferSize) + " byte(s) from buffer" << std::endl;
    }
    if(af->ft == FileType::ogg)
    {
            std::cout << "Clip freq: " << std::to_string(af->vi->rate) << " hz" << std::endl;
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
        std::cout << "Clip freq: " << std::to_string(sampleRate) << "hz and is using " << std::to_string(channels) << " channel(s)" << std::endl;
        
        if(channels > 1)
        {
            alBufferData(buffer,AL_FORMAT_STEREO16,af->data,af->bufferSize,sampleRate);
        }
        else
        {
            std::cout << "Warning: This clip is in MONO - This can lead to problems if a stero track is being added to the queue`" << std::endl;
            alBufferData(buffer,AL_FORMAT_MONO16,af->data,af->bufferSize,sampleRate);
        }
        CheckErrors();
           
        
    }
    else if(af->ft == FileType::mp3) //<-- Seems to have a bug that only play just about 1/2 of the audio file if its 3 min long (suspects the buffer can only hold about 1:30 stereo) - Please refer to GitHub issue 2
    {
        static mp3dec_t dec;
        mp3dec_init(&dec);
        mp3dec_file_info_t mp3Info;
        mp3dec_load(&dec,af->path.c_str(),&mp3Info, NULL, NULL);
        
        std::cout << "Freq: " << std::to_string(mp3Info.hz) << "Hz. - AVG. kbit/s rate: " << std::to_string(mp3Info.avg_bitrate_kbps) << " Kbits/s (if 192 then it might really be a 320kbit)"<< std::endl;
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
        
        alBufferData(buffer,channels,mp3Info.buffer,mp3Info.samples,mp3Info.hz);
        
    }
    else
    {
     std::cout << "Unsupported fileformat - ignoring - No buffer will be created" << std::endl;
     af->data = nullptr;
    }
    return af;
}

void RosenoernAudio::LoadBGM(std::string _path, bool async)
{
    AudioFile* ab = GetAudioBase(_path);
    
    if(async)
    {
        //do something async here...
    }
    int c = 0;
    int loop = 0;
    alGetSourcei(sources[0],AL_LOOPING,&loop);
    CheckErrors();
    
    if(loop != 0)
    {
        //We can't have looping when we use more than one buffer
        std::cout << "Looping of BGM disabled" << std::endl;
        alSourcei(sources[0],AL_LOOPING,0); 
        CheckErrors();
    }
    alGetSourcei(sources[0],AL_BUFFERS_QUEUED,&c);
    CheckErrors();
    std::cout << "Current queue is " + std::to_string(c+1) + " long" << std::endl;
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
     std::cout  << ErrorStrHandler(err) << std::endl;
    }
}
void RosenoernAudio::PlayFromQueue()
{
    alSourcePlay(sources[0]);
    
}
void RosenoernAudio::PlaySound(std::string _path)
{
    AudioFile* ab = GetAudioBase(_path);
    ab->source = sources[1];
    alSourcePlay(ab->source);
}
//Only work for queue
void RosenoernAudio::PauseAudio()
{
    alSourcePause(sources[0]);
}
void RosenoernAudio::ClearBuffer(ALuint* bufPtr, int amount)
{
    std::cout << "Entered ClearBuffer()" << std::endl;
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
                 std::cout << "Set buffer id: " + std::to_string(u) + " to empty" << std::endl;
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
    std::cout << "Buffer could not be cleared..." << std::endl;
}
int RosenoernAudio::FindFreeBuffer()
{
    int buf = 0;
    if(freeBuffers.empty())
    {
        std::cout << "Ran out of buffers, Attempting to free an existing buffer" << std::endl;
        ALint toBeFreed = 0;
        ALuint freed = 0;
        CheckErrors();
        if(debug)
        {
            std::cout << "Pauses and then contunies music to avoid RPAuio bug 1" << std::endl;
        }
        alSourcePause(sources[0]); // <-- Refer to issue 1 for explanation
        alGetSourcei(sources[0],AL_BUFFERS_PROCESSED,&toBeFreed);
        
        CheckErrors();
    
        if(toBeFreed > 0)
        {
            CheckErrors();
            alSourceUnqueueBuffers(sources[0],toBeFreed,&freed);
            CheckErrors();
            std::cout << "Buffer marked for potential freeing found !!" << std::endl;
            std::cout << "Attempting to clear buffer!" << std::endl;
            ClearBuffer(&freed,toBeFreed);
            alSourcePlay(sources[0]);
        }
    }

    try{
    buf = freeBuffers.at(0);
    }
    catch(std::out_of_range ex)
    {
          std::cout << "Could not find or clear buffer - CRITICAL ERROR - more info: "; std::cout << ex.what() << std::endl;
          exit(-1);
          
    }
    freeBuffers.erase(freeBuffers.begin());
    std::cout << "There is now " << std::to_string(freeBuffers.size()) << " buffers left" << std::endl;
    return buf;
}







