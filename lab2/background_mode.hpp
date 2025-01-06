
#include <iostream>
#ifdef _WIN32
    #include <windows.h>
    #include <string>
#else
    #include <spawn.h>
    #include <wait.h>
    #include <string.h>
#endif

#pragma once

/**
 * @brief Runs a program in the background with the ability to wait for the child program to complete and receive a response code.
 * 
 * @param path  Absolute or relative path to program.
 * @param wait_completion  Boolean. If true, the program is waiting for completion, otherwise not.
 * @param exit_code Pointer to response code.
 * 
 *  @return Response code.
 */
int runs_in_background(const char *path, bool wait_completion, long unsigned int *response_code);