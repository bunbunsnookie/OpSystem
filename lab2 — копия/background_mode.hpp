#include <utility>

#pragma once

/**
 * @brief   Runs a program in the background.
 *
 * @param path  Absolute or relative path to program.
 * 
 * @return  Pair of Identifier running process and exit code equal to 0 if successful, otherwise last error.
 */
std::pair<int,int> runs_in_background(const char *path);

// /**
//  * @brief Runs a program in the background.
//  *
//  * @param path Absolute or relative path to program.
//  * @param status Returns code of error or 0 if successful.
//  * @return process id (pid) of started process.
//  */
// int start_background_mode(const char *path, int &status);

/**
 * @brief   Waits for the program to finish.
 *
 * @param pid   Process id of program.
 * 
 * @return  Error code or 0 if successful.
 */
int wait_program(const int pid);

/**
 * @brief Runs a program in the background with the option to wait for it to finish.
 *
 * @param path Absolute or relative path to program.
 * @param wait Boolean. If true, the program is waiting for completion, otherwise not.
 * 
 * @return Pair of Identifier running process and error code equal to 0 if successful, otherwise last error.
 */
std::pair<int,int> wait_program_with_options(const char *path, bool is_wait = true);