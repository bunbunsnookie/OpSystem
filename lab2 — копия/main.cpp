#include "background_mode.hpp"
#include <iostream>

int main(int argc, char **argv){

    #ifdef _WIN32
        const char *path = "..\\wait.exe";
    #else
        const char *path = "../wait";
    #endif

    int pid{}, exit_code;
    // auto [pid, error_code] = runs_in_background(path);
    // if (error_code == 0){
    //     auto exit_code = wait_program(pid);
    // } else {
    //     auto exit_code = error_code;
    // }

    auto exit_code = wait_program_with_options(path, false);
    std::cout << "Process id" << pid << "Exit code " << exit_code << "\n";
    

}