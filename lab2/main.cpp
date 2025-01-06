#include "background_mode.hpp"
#include <iostream>

int main(int argc, char **argv){

    #ifdef _WIN32
        const char *path = "..\\program.exe";
    #else
        const char *path = "../program";
    #endif

    bool wait_for_completion = true;
    if (argc > 1){
        if (std::string(argv[1]) == "true"){
            wait_for_completion = true;
        } 
        if (std::string(argv[1]) == "false"){
            wait_for_completion = false;
        }   
    }
    
    unsigned long int response_code;
    auto status_process = runs_in_background(path, wait_for_completion, &response_code);
    std::cout << "Status process " << status_process << " Response code " << response_code << "\n";

}