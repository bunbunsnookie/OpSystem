#pragma once

#ifdef WIN32

#include <windows.h>

#else

#include <spawn.h>
#include <wait.h>
#include <string.h>
#include <time.h>

#endif

#include <format>
#include <string>
#include <chrono>

namespace plib
{
    int start_process(const char *path, int &status)
    {

#ifdef WIN32

        STARTUPINFOA si{};
        PROCESS_INFORMATION pi;
        int success = CreateProcessA(path,
                                     NULL,
                                     NULL,
                                     NULL,
                                     FALSE,
                                     0,
                                     NULL,
                                     NULL,
                                     &si,                          
                                     &pi
        );

        if (success == 0)
        {
            status = GetLastError();
        }
        else
        {
            status = 0;
        }

        return pi.dwProcessId;

#else
        char *const argv[] = {NULL};
        pid_t pid;
        status = posix_spawnp(
            &pid,
            path,
            NULL,
            NULL,
            argv,
            NULL);

        return pid;
#endif
    }

    int get_current_pid()
    {
#ifdef WIN32
        return GetCurrentProcessId();
#else
        return getpid();
#endif
    }

    std::string get_current_time_str()
    {
        auto now = std::chrono::system_clock::now();

        time_t t = std::chrono::system_clock::to_time_t(now);
        tm *st = localtime(&t);

        return std::format("{:0>4}-{:0>2}-{:0>2} {:0>2}:{:0>2}:{:0>2}.{:0>3}",
                           st->tm_year + 1900,
                           st->tm_mon + 1,
                           st->tm_mday,
                           st->tm_hour,
                           st->tm_min,
                           st->tm_sec,
                           now.time_since_epoch().count() % 1000);
    }

    int wait_program(const int pid)
    {
#ifdef WIN32

        HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        signed int status = WaitForSingleObject(handle, INFINITE);
        CloseHandle(handle);

        return status;

#else

        int status;
        waitpid(pid, &status, 0);

        return status;

#endif
    }
}