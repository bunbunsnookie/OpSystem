#include "background_mode.hpp"
#include <stdexcept>
#include <sstream>
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
    #include <string>
#else
    #include <spawn.h>
    #include <wait.h>
    #include <string.h>
#endif

std::pair<int,int> runs_in_background(const char *path){

    #ifdef _WIN32

        STARTUPINFO si{};
        PROCESS_INFORMATION pi;
        int exit_code{};

        if( !CreateProcess(
            path,       // Module name
            NULL,       // Command line
            NULL ,      // Process handle not inheritable
            NULL,       // Thread handle not inheritable
            FALSE,      // Set handle inheritance to FALSE
            0,          // No creation flags
            NULL,       // Use parent's environment block
            NULL,       // Use parent's starting directory 
            &si,        // Pointer to STARTUPINFO structure
            &pi         // Pointer to PROCESS_INFORMATION structure
        )) {
            exit_code = GetLastError(); // Process startup error
        }

        return std::pair<int,int>(pi.dwProcessId,exit_code);

    #else

        pid_t pid;
        char *const argv[] = {NULL};
        status = posix_spawnp(
            &pid,
            program_path,
            NULL,
            NULL,
            argv,
            NULL);

        return pid;

    #endif
}

// int start_background_mode(const char *path) {
//     int status{};
//     return start_background_mode(path, status);
// }   

int wait_program(const int pid){

    #ifdef _WIN32

        HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        int exit_code = WaitForSingleObject(handle, INFINITE);
        CloseHandle(handle);
        return exit_code;

    #else

        int exit_code;
        waitpid(pid, &exit_code, 0);
        return exit_code;

    #endif
}

std::pair<int,int> wait_program_with_options(const char *path, bool is_wait){

    auto [pid,exit_code] = runs_in_background(path);

    if (exit_code != 0) { return std::pair<int,int>(pid,exit_code); }

    if (is_wait){ return std::pair<int,int>(pid,wait_program(pid)); }

    return std::pair<int,int>(0,0);

}