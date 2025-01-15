#include "shmem.hpp"
#include "processes.hpp"
#include "util.hpp"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <format>

bool is_working(shlib::SharedMem<Data> &shared_data)
{
    shared_data.Lock();
    bool is_working = shared_data.Data()->count_programs > 0;
    shared_data.Unlock();
    return is_working;
}

bool is_this_stopped(shlib::SharedMem<Data> &shared_data)
{
    int this_pid = plib::get_current_pid();

    shared_data.Lock();
    int ended_process = shared_data.Data()->ended_process;
    bool is_w = !is_working(shared_data) || this_pid == ended_process;
    shared_data.Unlock();
    return is_w;
}

void wait_other()
{
    auto shared_data = GetSharedMem();
    auto this_pid = plib::get_current_pid();
    auto time_sleep = std::chrono::milliseconds(10);

    int log_process = 0;
    do
    {
        shared_data.Lock();
        log_process = shared_data.Data()->log_process;
        if (log_process == -1)
        {
            shared_data.Data()->log_process = this_pid;
        }
        shared_data.Unlock();
        if (log_process == this_pid)
        {
            break;
        }
        std::this_thread::sleep_for(time_sleep);
    } while (!is_this_stopped(shared_data));
}

void counter_thread()
{
    auto time_sleep = std::chrono::milliseconds(300);
    auto shared_data = GetSharedMem();

    auto this_pid = plib::get_current_pid();

    while (shared_data.IsValid() && !is_this_stopped(shared_data))
    {
        shared_data.Lock();
        shared_data.Data()->counter += 1;
        shared_data.Unlock();
        std::this_thread::sleep_for(time_sleep);
    }
}

void copies_thread()
{
    wait_other();

    auto shared_data = GetSharedMem();
    auto this_pid = plib::get_current_pid();
    auto time_sleep = std::chrono::seconds(3);

    while (shared_data.IsValid() && !is_this_stopped(shared_data))
    {
        shared_data.Lock();
        if (shared_data.Data()->copy_num > 0)
        {
            std::string time_pid = std::format("[{} | {}] Failed starting copies: {} more copies exist.",
                                                get_time_str(),
                                                get_pid_str(),
                                                shared_data.Data()->count_programs);
            write_file(shared_data, time_pid);
        }
        else
        {
#ifdef WIN32
            const char *c1 = "./copy1.exe";
            const char *c2 = "./copy2.exe";
#else
            const char *c1 = "./copy1";
            const char *c2 = "./copy2";
#endif

            int status{};
            plib::start_process(c1, status);
            plib::start_process(c2, status);
        }
        shared_data.Unlock();
        std::this_thread::sleep_for(time_sleep);
    }
}

void log_thread()
{
    wait_other();

    auto shared_data = GetSharedMem();
    auto this_pid = plib::get_current_pid();
    auto time_sleep = std::chrono::seconds(1);

    while (shared_data.IsValid() && !is_this_stopped(shared_data))
    {
        shared_data.Lock();
        auto counter = shared_data.Data()->counter;
        std::string time_pid = std::format("[{} | {}] {}", get_time_str(), get_pid_str(), counter);
        shared_data.Unlock();
        write_file(shared_data, time_pid);
        std::this_thread::sleep_for(time_sleep);
    }

    shared_data.Lock();
    auto log_process = shared_data.Data()->log_process;
    if (log_process == this_pid)
    {
        shared_data.Data()->log_process = -1;
    }
    shared_data.Unlock();
}

int main(int argc, char **argv)
{
    auto shared_data = GetSharedMem();
    auto this_pid = plib::get_current_pid();
    if (!shared_data.IsValid())
    {
        std::cout << "Failed to create shared memory block!" << std::endl;
        return -1;
    }

    if (!is_working(shared_data))
    {
        std::ofstream log(LOG_FILE_NAME);
        log.clear();
    }

    std::string tp = std::format("[{} | {}] Started main.", get_time_str(), get_pid_str());
    write_file(shared_data, tp);

    std::cout << std::format("Started {} : pid={}. Log file: {}\n", argv[0], get_pid_str(), LOG_FILE_NAME);
    std::vector<std::thread> threads;

    shared_data.Lock();
    shared_data.Data()->count_programs++;
    threads.emplace_back(counter_thread);
    threads.emplace_back(log_thread);
    threads.emplace_back(copies_thread);
    shared_data.Unlock();

    std::string param;
    int set_counter = 0;
    while (true)
    {
        std::cin >> param;
        if (param == "e" || param == "exit")
        {
            break;
        }
        else if (param == "c" || param == "change")
        {
            std::cout << "Enter int to set counter\n";
            std::cin >> set_counter;
            shared_data.Lock();
            shared_data.Data()->counter = set_counter;
            shared_data.Unlock();
        }
        else if (param == "s" || param == "show")
        {
            shared_data.Lock();
            auto counter = shared_data.Data()->counter;
            shared_data.Unlock();
            std::cout << "Counter value: " << counter << "\n";
        }
    }
    std::cout << "Exiting\n";

    shared_data.Lock();
    shared_data.Data()->count_programs--;
    shared_data.Data()->ended_process = this_pid;
    shared_data.Unlock();

    std::cout << "Joining threads\n";

    for (auto &t : threads)
    {
        t.join();
    }
    std::cout << "Goodbye\n";
    

    tp = std::format("[{} | {}] Finished main", get_time_str(), get_pid_str());
    write_file(shared_data, tp);
    
    shared_data.Lock();
    shared_data.Data()->ended_process = -1;
    shared_data.Unlock();

    return 0;
}