#include "background_mode.hpp"

int runs_in_background(const char *path, bool wait_completion, long unsigned int *response_code){

    #ifdef _WIN32

        // Создание нового процесса

        STARTUPINFO si{}; 
        PROCESS_INFORMATION pi;

        if (!CreateProcess(
            path,       
            NULL,       
            NULL, 
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi
        ))
        {
            auto status_process = GetLastError();
            std::cout << "Error CreateProcess!" << std::endl;
            return status_process;
        }

        auto pid = pi.dwProcessId; // Получаем идентификатор созданного процесса

        // Ожидание завершения процесса
        if (wait_completion){

            HANDLE handle = OpenProcess(
                PROCESS_ALL_ACCESS, 
                FALSE, 
                pid
            );

            auto status_process = WaitForSingleObject(
                handle,
                INFINITE
            );
            // Получаем код завершения процесса
            GetExitCodeProcess(handle, (unsigned long*)response_code);
            CloseHandle(handle);

            return status_process;    
        } else {
            return 0;
        }

    #else

        pid_t pid;
        char *const argv[] = {NULL};

        // Создание нового процесса
        auto status_process = posix_spawnp(
            &pid,
            path,
            NULL,
            NULL,
            argv,
            NULL
        );

        // Ожидание завершения процесса
        if (wait_completion){

            waitpid(
                pid, 
                &status_process, 
                0
            );
  
        } 

        // Получаем код завершения процесса
        *response_code = WEXITSTATUS(status_process);
        return WTERMSIG(status_process);


    #endif
}