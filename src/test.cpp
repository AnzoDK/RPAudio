#include "../includes/rpaudio.h"
#include <future>
#include <chrono>
rp::RosenoernAudio* ra;
std::string readCin()
{
    std::string line;
    std::getline(std::cin, line);
    return line;
}


int main(int arcs, char* argv[])
{
    
    
    bool debug = 0;
    for(int i = 1; i < arcs;i++)
    {
        std::string arg = argv[i];
        if(arg == "--debug")
        {
            debug = 1;
        }
    }
    
    rp::RosenoernAudio* ra = new rp::RosenoernAudio(debug);
    ra->init();
    //std::thread inv1(&rp::RosenoernAudio::LoadBGM,*ra,"./Sound2.wav");
    ra->PlayFromQueue();
    ra->AddToQueue("./sound2.wav");
    ra->AddToQueue("./sound.ogg");
    std::cout << "Playing Audio from BGM queue" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(230));
    ra->AddToQueue("./sound3.wav");
    bool run = 1;
    std::string input = "";
    auto future = std::async(std::launch::async,readCin);
    while(run)
    {
     if(future.wait_for(std::chrono::milliseconds(5)) == std::future_status::ready)
     {
         input = future.get();
         future = std::async(std::launch::async,readCin);
         
     }
     if(input == "e")
     {
       run = 0;
     }
     
    }
    delete(ra);
    std::cout << "Cleaning Complete" << std::endl;
    return 0;
    
    
}

