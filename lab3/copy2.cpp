#include "util.hpp"
#include <thread>
#include <format>

int main()
{
    auto shared_data = GetSharedMem();

    auto str = std::format("[{} | {}] Started copy 2", get_time_str(), get_pid_str());
    write_file(shared_data, str);
    

    shared_data.Lock();
    shared_data.Data()->counter *= 2;
    shared_data.Data()->copy_num++;
    shared_data.Unlock();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    shared_data.Lock();
    shared_data.Data()->counter /= 2;
    shared_data.Data()->copy_num--;
    shared_data.Unlock();

    str = std::format("[{} | {}] Finished copy 2", get_time_str(), get_pid_str());
    write_file(shared_data, str);

}