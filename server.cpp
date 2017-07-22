#include <asio.hpp>
#include <iostream>
#include "common.hpp"
#include <stdlib.h>
#include <future>
#include <atomic>

using asio::ip::tcp;

void read(tcp::socket& socket);
void shutdown(tcp::socket& socket);

static std::atomic_bool isConnection;

int main(int argc, char** argv)
{
    try
    {
        int port = DEFAULT_PORT;
        if(argc == 2)
            port = atoi(argv[1]);
        std::cout << "commands:";
        for(auto cmd: commands)
            std::cout << ' ' << cmd.second;
        std::cout << std::endl;

        asio::io_service ioService;
        tcp::endpoint endpoint(tcp::v4(), port);
        tcp::acceptor acceptor(ioService, endpoint);
        tcp::socket socket(ioService);

        std::string shStr;
#ifdef __unix__
        shStr = "terminator -e './client localhost " + std::to_string(port) + "' &";
#elif _WIN32
        shStr = "start ./client localhost " + std::to_string(port);
#endif
        if(system(shStr.c_str()) == -1)
        {
            std::cout << "system() failed to start client" << std::endl;
            return 0;
        }
        acceptor.accept(socket);
        isConnection = true;
        read(socket);
        auto future = std::async(std::launch::async, [&ioService](){ioService.run();});

        std::string input;
        while(std::getline(std::cin, input))
        {
            if(!isConnection)
                break;
            ioService.post([input, &socket]
            {
                socket.async_write_some(asio::buffer(input.c_str(), input.size() + 1),
                                        [&socket](asio::error_code ec, std::size_t)
                {
                    if(ec)
                        shutdown(socket);
                });
            });
        }
        future.get();
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}

void read(tcp::socket& socket)
{
    static char buffer[BUFF_SIZE];
    socket.async_read_some(asio::buffer(buffer),
                           [&socket](asio::error_code ec, std::size_t)
    {
        if(!ec)
        {
            auto args = split(buffer);
            printArgs(args);
            read(socket);
        }
        else
            shutdown(socket);
    });
}

void shutdown(tcp::socket& socket)
{
    asio::error_code ec;
    socket.shutdown(tcp::socket::shutdown_both, ec);
    if(ec)
        std::cout << "socket::shutdown failed" << std::endl;
    isConnection = false;
    std::cout << "connection with client lost, press ENTER to exit" << std::endl;
}
