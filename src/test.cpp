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
    
    rp::RosenoernAudio* ra = new rp::RosenoernAudio(debug,10);
    ra->init();
    
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
     if(input == "exit")
     {
       run = 0;
     }
     else if(input != "")
     {
       ra->AddToQueue(input);
     }
     input = "";
    }
    delete(ra);
    std::cout << "Cleaning Complete" << std::endl;
    //std::terminate();
    return 0;
    
    
}

