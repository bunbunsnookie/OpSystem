#include "background_mode.hpp"
#include <iostream>

int main(int argc, char **argv){

    #ifdef _WIN32
        const char *path = "..\\wait.exe";
    #else
        const char *path = "../wait";
    #endif

    bool wait = true;
    if (argc > 1){
        if (std::string(argv[1]) == "true"){
            wait = true;
        } 
        if (std::string(argv[1]) == "false"){
            wait = false;
        }   
    }

    auto [pid, code] = wait_program_with_options(path, wait);
    std::cout << "Process id " << pid << " Code status " << code << "\n";

}