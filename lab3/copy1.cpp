#include "util.hpp"
#include <format>

int main()
{
    auto shared_data = GetSharedMem();

    auto str = std::format("[{} | {}] Started copy 1", get_time_str(), get_pid_str());
    write_file(shared_data, str);
    
    shared_data.Lock();
    shared_data.Data()->counter += 10;
    shared_data.Unlock();

    str = std::format("[{} | {}] Finished copy 1", get_time_str(), get_pid_str());
    write_file(shared_data, str);
}