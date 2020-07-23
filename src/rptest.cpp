#include <iostream>
#include "../includes/rpaudio.h"
#include <chrono>
int main(int arcs, char* argv[])
{
    std::vector<rp::FileType> tests = std::vector<rp::FileType>();
    rp::RosenoernAudio* ra = new rp::RosenoernAudio(1,10);
    bool async = 0;
    for(int i = 1; i < arcs;i++)
    {
        std::string arg = argv[i];
        if(arg == "--mp3")
        {
            tests.push_back(rp::FileType::mp3);
        }
        if(arg == "--wav")
        {
            tests.push_back(rp::FileType::wav);
        }
        if(arg == "--ogg")
        {
            tests.push_back(rp::FileType::ogg);
        }
        if(arg == "--async")
        {
            async = 1;
        }
    }
    ra->init();
    for(unsigned int i = 0; i < tests.size();i++)
    {
        switch(static_cast<int>(tests.at(i)))
        {
            case 0:
                ra->AddToQueue("./audio/ogg.ogg");
            break;
            case 1:
                ra->AddToQueue("./audio/wav.wav");
            break;
            case 2:
                ra->AddToQueue("./audio/mp3.mp3");
            break;
        }
        
    }
    ra->PlayFromQueue();
    sleep(180*tests.size());
    return 0;
}
