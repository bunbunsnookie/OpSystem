#include "background_mode.hpp"
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
    #include <string>
#else
    #include <spawn.h>
    #include <wait.h>
    #include <string.h>
#endif

int runs_in_background(const char *path){

    #ifdef _WIN32

        STARTUPINFO si{};
        PROCESS_INFORMATION pi;

        CreateProcess(
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
        );

        return pi.dwProcessId;

    #else

        pid_t pid;
        char *const argv[] = {NULL};
        code = posix_spawnp(
            &pid,
            program_path,
            NULL,
            NULL,
            argv,
            NULL);

        return pid;

    #endif
}

int wait_program(const int pid){

    #ifdef _WIN32

        HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        int code = WaitForSingleObject(handle, INFINITE);
        CloseHandle(handle);
        return code;

    #else

        int code;
        waitpid(pid, &code, 0);
        return code;

    #endif
}

std::pair<int,int> wait_program_with_options(const char *path, bool wait){

    auto pid = runs_in_background(path);

    if (wait){ return std::pair<int,int>(pid,wait_program(pid)); }

    return std::pair<int,int>(pid,0);

}