#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <fstream>

#ifdef WIN32

#include <winsock2.h> /* socket */
#include <ws2tcpip.h> /* getaddrinfo */

#else

#include <sys/socket.h> /* socket */
#include <sys/types.h>
#include <netinet/in.h> /* socket */
#include <arpa/inet.h>  /* socket */
#include <unistd.h>
#include <netdb.h> /* getaddrinfo */
#include <poll.h>  /* poll */
#include <signal.h>
#include <string.h> /* memset */
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#endif

namespace slib
{
#define READ_WAIT_MS 50
    namespace fs = std::filesystem;

    // Распарсенный запрос клиента
    class Request
    {
        std::string m_method;
        std::string m_url;

    public:
        Request(const std::string &data)
        {
            std::istringstream recv(data);
            std::string line;
            std::getline(recv, line);

            std::istringstream line_s(line);
            std::getline(line_s, m_method, ' ');
            
            std::istringstream url_s(line);
            std::string url;
            std::getline(line_s, url, ' ');

            std::istringstream url_s_(url);
            std::getline(url_s_, m_url, '?');
            std::getline(url_s_, m_url, '?');
        }

        std::string GetMethod() const
        {
            return m_method;
        }

        std::string GetURL() const
        {
            return m_url;
        }
    };

    std::string GetAnswer(const std::string &body)
    {
        std::stringstream answer;
        answer << "HTTP/1.1 200 OK\r\n"
                << "Content-Type: text/plain\r\n"
                << "Content-Length: " << body.length()
                << "\r\n\r\n"
                << body;
        return answer.str();
    }

    std::string ReadFile(const std::string &file_path)
    {
        std::ifstream file(file_path, std::ios::binary);
        std::stringstream str;
        str << file.rdbuf() << "\n";
        return str.str();
    }

    class SocketBase
    {
    public:
        SocketBase() : m_socket(INVALID_SOCKET)
        {
#ifdef WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
            // Игнорируем SIGPIPE сигнал
            // чтобы программа не терминировалась при попытке записи в закрытый сокет
            signal(SIGPIPE, SIG_IGN);
#endif
        }
        ~SocketBase()
        {
            CloseSocket();
#if defined(WIN32)
            WSACleanup();
#endif
        }

        static int ErrorCode()
        {
#ifdef WIN32
            return WSAGetLastError();
#else
            return errno;
#endif
        }

        bool IsValid()
        {
            return m_socket != INVALID_SOCKET;
        }

    protected:
        void CloseSocket()
        {
            CloseSocket(m_socket);
            m_socket = INVALID_SOCKET;
        }

        static void CloseSocket(SOCKET sock)
        {
            if (sock == INVALID_SOCKET)
                return;
#ifdef WIN32
            shutdown(sock, SD_SEND);
            closesocket(sock);
#else
            shutdown(sock, SHUT_WR);
            close(sock);
#endif
        }

        // Если < 0, то сокет не готов к общению
        static int Poll(const SOCKET &socket, int timeout_ms = READ_WAIT_MS)
        {
            struct pollfd polstr;
            memset(&polstr, 0, sizeof(polstr));
            polstr.fd = socket;
            polstr.events |= POLLIN;
#ifdef WIN32
            return WSAPoll(&polstr, 1, timeout_ms);
#else
            return poll(&polstr, 1, timeout_ms);
#endif
        }

        SOCKET m_socket;
    };

    class HTTPServer : public SocketBase
    {
    public:
        HTTPServer(const std::string &interface_ip, const short int port)
        {
            Listen(interface_ip, port);
        }

        void Listen(const std::string &interface_ip, const short int port)
        {
            if (m_socket != INVALID_SOCKET)
            {
                CloseSocket();
            }

            struct addrinfo hints;
            memset(&hints, 0, sizeof(hints));

            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
            hints.ai_flags = AI_PASSIVE;

            struct addrinfo *addr = NULL;
            int res = getaddrinfo(interface_ip.c_str(), std::to_string(port).c_str(), &hints, &addr);

            if (res != 0)
            {
                std::cerr << "Failed getaddrinfo: " << res << std::endl;
                freeaddrinfo(addr);
                return;
            }

            m_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
            if (m_socket == INVALID_SOCKET)
            {
                std::cerr << "Cant open socket: " << ErrorCode() << std::endl;
                freeaddrinfo(addr);
            }

            if (bind(m_socket, addr->ai_addr, addr->ai_addrlen) == SOCKET_ERROR)
            {
                std::cerr << "Failed to bind: " << ErrorCode() << std::endl;
                freeaddrinfo(addr);
                CloseSocket();
            }
            freeaddrinfo(addr);

            if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
            {
                std::cerr << "Failed to start listen: " << ErrorCode() << std::endl;
                CloseSocket();
            }
        }

        void ProcessClient()
        {
            if (!IsValid())
            {
                std::cerr << "Server (listening) socket is invalid!" << std::endl;
                return;
            }

            SOCKET client_socket = accept(m_socket, NULL, NULL);
            if (client_socket == INVALID_SOCKET)
            {
                std::cerr << "Error accepting client: " << ErrorCode() << std::endl;
                CloseSocket(client_socket);
                return;
            }

            if (Poll(client_socket) <= 0)
            {
                CloseSocket(client_socket);
                return;
            }

            std::stringstream recv_str;

            int buf_size = sizeof(m_input_buf) - 1;

            int result = -1;
            do
            {
                result = recv(client_socket, m_input_buf, buf_size, 0);
                if (result < 0)
                {
                    break;
                }
                m_input_buf[result] = '\0';
                recv_str << m_input_buf;
            } while (result >= buf_size);

            if (result == SOCKET_ERROR || result < 0)
            {
                std::cerr << "Error on client receive: " << ErrorCode() << std::endl;
                CloseSocket(client_socket);
                return;
            }
            else if (result == 0)
            {
                std::cerr << "Client closed connection before getting any data!" << std::endl;
                CloseSocket(client_socket);
                return;
            }

            auto request = Request(recv_str.str());
            std::cout << recv_str.str() << "\n\n";

            std::cout << request.GetMethod() << " " << request.GetURL() << std::endl;
            
            std::string data;
            if (request.GetURL() == "/sec")
            {
                data = ReadFile("logs/sec.log");
            }
            else if (request.GetURL() == "/hour")
            {
                data = ReadFile("logs/hour.log");
            }
            else if (request.GetURL() == "/day")
            {
                data = ReadFile("logs/day.log");
            }

            auto response = GetAnswer(data);

            result = send(client_socket, response.c_str(), (int)response.length(), 0);
            if (result == SOCKET_ERROR)
            {

                std::cerr << "Failed to send response to client: " << ErrorCode() << std::endl;
            }
            
            CloseSocket(client_socket);
            std::cout << "Answered to client!" << std::endl;
        }

    private:
        char m_input_buf[1024];
    };

}