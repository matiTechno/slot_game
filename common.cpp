#include "common.hpp"
#include <algorithm>
#include <iostream>

std::map<Cmd, const char*> commands = {{Cmd::exit, "exit"}, {Cmd::draw, "draw"}};

bool isWhite(char c)
{
    return c == ' ' || c == '\t';
}

std::vector<std::string> split(const std::string& str)
{
    std::vector<std::string> vec;
    bool ret = true;
    for(auto c: str)
    {
        if(isWhite(c) == 0)
        {
            ret = false;
            break;
        }
    }
    if(ret)
        return {};

    vec.emplace_back();

    for(auto c: str)
    {
        auto& top = vec.back();
        if(isWhite(c) == 0)
            top.push_back(c);
        
        else if(top.size() && isWhite(top.back()) == 0)
            vec.emplace_back();
    }

    if(vec.back().empty())
        vec.pop_back();

    return vec;
}

void makeLower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void printArgs(const std::vector<std::string>& args)
{
    if(args.empty())
        return;
    std::cout << "received command: " << args[0] << '\n';
    for(std::size_t i = 1; i < args.size(); ++i)
        std::cout << "arg " << std::to_string(i) << ": " << args[i] << '\n';
    std::cout.flush();
}

void writeStrToBuffer(char* buffer, const char* str)
{
    for(;;)
    {
        *buffer = *str;
        if(*str == '\0')
            break;
        ++buffer;
        ++str;
    }
}
