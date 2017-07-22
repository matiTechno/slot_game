#include <asio.hpp>
#include <iostream>
#include <random>
#include "common.hpp"

using asio::ip::tcp;

int main(int argc, char** argv)
{
    try
    {
        if(argc != 3)
        {
            std::cout << "usage: client <host> <port>" << std::endl;
            return 0;
        }
        asio::io_service ioService;
        tcp::resolver resolver(ioService);
        tcp::socket socket(ioService);
        auto endpointIt = resolver.resolve({argv[1], argv[2]});
        asio::connect(socket, endpointIt);
        char readBuffer[BUFF_SIZE];
        char writeBuffer[BUFF_SIZE];

        const int pool[] = {1, 2, 3};
        constexpr int poolSize = getSize(pool);
        std::random_device rd;
        std::mt19937 engine(rd());
        std::uniform_int_distribution<int> dist(0, poolSize - 1);
        int winCount = 0;

        for(;;)
        {
            asio::error_code ec;
            socket.read_some(asio::buffer(readBuffer), ec);
            if(ec == asio::error::eof)
                break;
            else if(ec)
                throw asio::system_error(ec);

            auto args = split(readBuffer);

            if(args.empty())
                continue;

            auto& arg0 = args.front();
            makeLower(arg0);
            bool exists = false;
            for(auto cmd: commands)
            {
                if(arg0 == cmd.second)
                {
                    exists = true;
                    break;
                }
            }
            if(exists == 0)
            {
                std::cout << readBuffer << std::endl;
                continue;
            }

            printArgs(args);

            if(arg0 == commands[Cmd::exit])
            {
                std::string winCountStr = "winCount " + std::to_string(winCount);
                writeStrToBuffer(writeBuffer, winCountStr.c_str());
                socket.write_some(asio::buffer(writeBuffer));
                socket.write_some(asio::buffer("exiting"));
                break;
            }
            else if(arg0 == commands[Cmd::draw])
            {
                std::cout << "drawn numbers: ";
                int drawnNums[poolSize];
                for(int i = 0; i < poolSize; ++i)
                {
                    drawnNums[i] = pool[dist(engine)];
                    std::cout << drawnNums[i] << ' ';
                }
                std::cout << std::endl;

                bool win = true;
                int num = drawnNums[0];
                for(int i = 1; i < poolSize; ++i)
                {
                    if(drawnNums[i] != num)
                    {
                        win = false;
                        break;
                    }
                }
                if(win)
                {
                    ++winCount;
                    std::string writeStr = "win " + std::to_string(num);
                    socket.write_some(asio::buffer(writeStr.c_str(), writeStr.size() + 1));
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
