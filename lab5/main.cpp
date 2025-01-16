#include "server.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <thread>

namespace fs = std::filesystem;

std::string LOG_DIR = "logs";
std::string LOG_SEC_NAME = LOG_DIR + "/second.log";
std::string LOG_HOUR_NAME = LOG_DIR + "/hour.log";
std::string LOG_DAY_NAME = LOG_DIR + "/day.log";

std::string DEFAULT_SERVER_HOST = "127.0.0.1";
short int DEFAULT_SERVER_PORT = 8080;

void Server(const std::string &host_ip, const short int port)
{
    slib::HTTPServer server(host_ip, port);
    if (!server.IsValid())
    {
        std::cerr << "Server error" << std::endl;
        return;
    }

    std::cout << "http://" << host_ip << ":" << port << std::endl;

    while (true)
    {
        if (!server.IsValid())
        {
            std::cerr << "Server error" << std::endl;
            break;
        }
        server.ProcessClient();
    }
}

int main(int argc, char **argv)
{
    std::string host_ip(argc > 1 ? argv[1] : DEFAULT_SERVER_HOST);
    signed int server_port = argc > 2 ? std::stoi(argv[2]) : DEFAULT_SERVER_PORT;

    Server(host_ip, server_port);
}