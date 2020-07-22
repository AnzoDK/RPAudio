//HEADER_START
    struct PhysicalFrame
    {
      PhysicalFrame(char* FrameData, int _id);
      char* frameMainData;
      unsigned long int id;
    };
    
    class FrameManager
    {
        public:
            FrameManager();
            void SortFramesByID();
            int RemoveFrameByID(int id); //Returns 0 on found and 1 on failure 
            void RemoveFrameByIndex(int index);
            void AddFrame(PhysicalFrame frame);
        private:
            std::vector<PhysicalFrame> currentFrames;
    };
    
class RosenoernAudio
    {
    public:
       void DecodeMp3Frames(int frameOffset, AudioFile* af_ptr);
    private:
       uint8_t* ConstructLogicalFrame(uint8_t* physFrames);
    };




//CPP
PhysicalFrame::PhysicalFrame(char* FrameData, int _id)
{
    frameMainData = FrameData;
    id = _id;
}
FrameManager::FrameManager()
{
 currentFrames = std::vector<PhysicalFrame>();   
}
void FrameManager::SortFramesByID()
{
    //Just a bubble sort, as that is easy to implement
    bool sorted = 0;
    int c = 0;
    int changes = 0;
    while(!sorted)
    {
        if(c != currentFrames.size()-2)
        {
            if(currentFrames.at(c).id > currentFrames.at(c+1).id)
            {
               PhysicalFrame tmp = currentFrames.at(c+1);
               currentFrames.at(c+1) = currentFrames.at(c);
               currentFrames.at(c) = tmp;
               changes++;
               c++;
            }
            else
            {
             c++;  
            }
        }
        else if(changes == 0)
        {
            sorted = 1;
        }
        else
        {
         c = 0;
         changes = 0;
        }
        
    }
}
void FrameManager::AddFrame(PhysicalFrame frame)
{
    currentFrames.push_back(frame);
}
void FrameManager::RemoveFrameByIndex(int index)
{
    currentFrames.erase(currentFrames.begin()+index-1);
}
int FrameManager::RemoveFrameByID(int id)
{
    bool found = 0;
    int result = 0;
    for(unsigned int i = 0; i < currentFrames.size();i++)
    {
        if(currentFrames.at(i).id == id)
        {
            currentFrames.erase(currentFrames.begin()+i-1);
            found = 1;
        }
    }
    if(found)
    {
      result = 0;   
    }
    else
    {
        result = 1;
    }
    return result;
}
void RosenoernAudio::DecodeMp3Frames(int frameOffset, AudioFile* af_ptr)
{
        //Frame Decoding
        
        int currentOffset = 0;
        int headerSize = 4; //in bytes
        char* header = new char[headerSize];
        for(int i = 0; i < headerSize;i++)
        {
            header[i] = af_ptr->data[i+frameOffset];
        }
        unsigned char bitAndFreqByte = header[2]; /* First 4 bit contains bit rate - next 2 bit contains frequency. next bit is pad. bit followed by priv. bit(we dont need the priv. bit for anything) (if read from MSB)*/
        unsigned int syncword = 0; /*We ignore the lower 4 bit as they are not a part of the sync word*/
        syncword = header[0] << 8 | header[1];
        if((syncword & 0xFFF0) >> 4 != 0xFFF)
        {
            std::cout << "Invalid SyncWord - Assuming broken mp3, reading error or bad frame" << std::endl;
        }
        else
        {
            std::cout << "Valid SyncWord - 12 bits long" << std::endl;   
        }
        std::cout << "Mp3 file is " << std::to_string((int)(bitAndFreqByte & 0xF0)) << " kbit/s" << std::endl;
        bool padding = (bitAndFreqByte & 0x02) >> 1;
        if(padding)
        {
            std::cout << "Frame is padded" << std::endl;
        }
        unsigned char dataByte = header[3];
        //TODO Add support for these extensions
        bool extensionIntensity = (dataByte & 0x20) >> 5;
        bool extensionMSstereo = (dataByte & 0x30) >> 4;
        
        int CRCoffset = 0;
        if((syncword & 0x01) == 1)
        {
            CRCoffset = 16; //from bit 16 to 31 in both header and sideInfo
        }
        int sideInfoSize = 0;
        if(RPtools::mp3ModeTable((dataByte & 0xC0) >> 6) == AL_FORMAT_STEREO16)
        {
            sideInfoSize = 32;
        }
        else
        {
            sideInfoSize = 17;
        }
        int mainDataOffset = headerSize+sideInfoSize+currentOffset+CRCoffset;
        int sideInfoOffset = headerSize+currentOffset+CRCoffset;
        int privateBitsSize = 0;int scfsiSize = 0;int par2_3_lengthSize = 0; int bigValuesSize = 0;int globalGainSize = 0;int scaleafac_compressSize = 0;int windowsSwitchingFlagSize = 0;int blockTypeSize = 0;int mixedBlockflagSize=0;int tableSelectSize=0;int subBlockGainSize=0;int region0CountSize=0;int region1CountSize=0; int preflagSize=0;int scalfacScaleSize=0;int count1tableSelectSize=0;
        
        if(RPtools::mp3ModeTable((dataByte & 0xC0) >> 6) == AL_FORMAT_STEREO16)
        {
          //every size is in bits NOT bytes
          privateBitsSize = 3;
          scfsiSize = 8;
          par2_3_lengthSize = 24;
          bigValuesSize = 18;
          globalGainSize = 18;
          scaleafac_compressSize = 8;
          windowsSwitchingFlagSize = 2;
          blockTypeSize = 4;
          mixedBlockflagSize = 2;
          tableSelectSize = 15; // or 30
          subBlockGainSize = 18;
          region0CountSize = 8;
          region1CountSize = 6;
          preflagSize = 2;
          scalfacScaleSize = 2;
          count1tableSelectSize = 2;
        }
        else
        {
            privateBitsSize = 5;
            scfsiSize = 4;
            par2_3_lengthSize = 12;
            bigValuesSize = 9;
            globalGainSize = 8;
            scaleafac_compressSize = 4;
            windowsSwitchingFlagSize = 1;
            blockTypeSize = 2;
            mixedBlockflagSize = 1;
            tableSelectSize = 10; // or 20
            subBlockGainSize = 9;
            region0CountSize = 4;
            region1CountSize = 3;
            preflagSize = 1;
            scalfacScaleSize = 1;
            count1tableSelectSize = 1;
        }
        /*is a negative offset*/ int main_data_begin = af_ptr->data[sideInfoOffset]; //points negativly due to logical frames being contructed from previous physical frames. the offset does not count the header *First side data instance 0x384
        main_data_begin = main_data_begin << 1 | (af_ptr->data[sideInfoOffset+1] & 0x80) >> 7;
        
        //alBufferData(buffer,RPtools::mp3ModeTable((dataByte & 0xC0) >> 6),af->data,af->bufferSize,RPtools::Mp3FrequencyTable((bitAndFreqByte & 0x0C) >> 2));
        delete[] header;
        FrameManager fm = FrameManager();
        
        
}
uint8_t* RosenoernAudio::ConstructLogicalFrame(uint8_t* physFrames)
{
    
    return 0;
}

//Clip from rp::RosenoernAudio::GetAudioBase - else if statement for filetype mp3
else if(af->ft == FileType::mp3)
    {
        //WARNING THIS CODE ONLY WORK ON CONSTANT BITRATE, LESS-THAN-320KBPS MP3 FILES
        
        
        //check for ID3 header
        bool id3 = 0;
        //Create a temp header to check against
        char* ThreeByteCheck = new char[3];
        memcpy(ThreeByteCheck,af->data,3);
        std::string tmp = std::string(ThreeByteCheck);
        if(tmp.find("ID") != std::string::npos)
        {
            std::cout << "Mp3 file has ID3 header" << std::endl;
            id3 = 1;
        }
        else
        {
            std::cout << "Mp3 file does not contain a ID3 header" << std::endl;
        }
        delete[] ThreeByteCheck;
        
        
        int mp3framesize = 0;
        int ID3headerSize = 0;
        int headerTotalSize = 0;
        bool usync = 0; bool extendedHeader = 0; bool withFooter = 0;
        int tagSize = 0;
        if(id3)
        {
            ID3headerSize = 10;
            char* header = new char[10];
            memcpy(header, af->data, 10);
            usync = (header[5]/*byte 6*/& 0x80) >> 7;
            extendedHeader = (header[5]/*byte 6*/& 0x40) >> 6;
            withFooter = (header[5]/*byte 6*/& 0x20) >> 5;
            
            if(extendedHeader)
            {
              uint32_t exheaderSize = 0;
              //TODO Decode length of extended header and add it to the firstFrameOffset
              int extendedOffset = 0xA;
              //Read size in format 4 * %0xxxxxxx
              unsigned char* exheaderSizeBytes = new unsigned char[4];
              for(int i = 0; i < 4;i++)
              {
                  exheaderSizeBytes[i] = af->data[i+extendedOffset];
              }
              if(debug)
              {
                  std::vector<char> exheaderSizeBytesDebugVector = std::vector<char>();
                  for(int i = 0;i < 4;i++)
                  {
                      exheaderSizeBytesDebugVector.push_back(exheaderSizeBytes[i]);
                  }
                  std::cout << "For more info use GBD on exheaderSizeBytesDebugVector" << std::endl;
              }
              unsigned char exheaderOptionsByte = af->data[5+extendedOffset];
              if((exheaderOptionsByte & 0x20) >> 5)
              {
                  //uses text (CRC) in the tag
                  //That has to be decoded as well
                  unsigned char* CRCLengthBytes = new unsigned char[5];
                  uint32_t CRClength = 0;
                  for(int i = 0; i < 5;i++)
                  {
                      CRCLengthBytes[i] = af->data[i+0x11];
                     
                  }
                  
                   /*CRClength = (CRCLengthBytes[0] << 26) | CRCLengthBytes[1] << 4 | CRCLengthBytes[2] << 4 | CRCLengthBytes[3] << 4 | CRCLengthBytes[4] << 4;*/
                   //TODO decode CRC-32 as I give up for now CRClength = RPtools::Syncsafe_to_int(CRCLengthBytes,5);
                   /*This overflows due to int being 32 bits and the CRCLength being 35 bits - however this doesn't matter because the 4 first bits are always zeroes anyway*/
                  delete[] CRCLengthBytes;
                  //firstFrameOffset += CRClength;
                  
              }
              
              //memcpy(&exheaderSize, exheaderSizeBytes,4);
              exheaderSize = RPtools::Syncsafe_to_int(exheaderSizeBytes);
              delete[] exheaderSizeBytes;
              headerTotalSize = ID3headerSize + exheaderSize;
              
              
            }
        }
        //Frame locating
        /*
         * We are searching for a pattern: (0xFF 0xFx) this is USUALLY (not always) 0xFF 0xFB
         */
        int firstFrameOffset = 0;
        int c = 0;
        while(firstFrameOffset == 0)
        {
            if(af->data[c+headerTotalSize] == -1) /*Due to the file reading being done in singed mode (and hard to work around) i have to search for -1 instead of 0xFF*/
            {
             if((af->data[c+headerTotalSize+1] & 0xF0) >> 4 == 0x0F)
             {
                 std::cout << "Found offset of first frame!!" << std::endl;
                 firstFrameOffset = c+headerTotalSize;
             }
            }
            c++;
        }
    }
