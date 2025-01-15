#pragma once

#include "shmem.hpp"
#include "processes.hpp"
#include <fstream>
#include <chrono>

#define LOG_FILE_NAME "./log.txt"

struct Data
{
    int counter = 0;

    int copy_num = 0;
    int count_programs = 0;

    int log_process = -1;
    int ended_process = -1;
};

shlib::SharedMem<Data> GetSharedMem()
{
    return shlib::SharedMem<Data>("shared_memory");
}

template <typename T>
void write_file(shlib::SharedMem<Data> &shared_data, T &str)
{
    shared_data.Lock();
    std::ofstream log(LOG_FILE_NAME, std::ios::app);
    log << str << "\n";
    shared_data.Unlock();
}

std::string get_time_str()
{
    return plib::get_current_time_str();
}

std::string get_pid_str()
{
    return std::to_string(plib::get_current_pid());
}