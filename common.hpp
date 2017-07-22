#pragma once

#include <string>
#include <vector>
#include <map>

enum class Cmd
{
    exit,
    draw
};

extern std::map<Cmd, const char*> commands;

template<typename T, int N>
constexpr int getSize(T(&)[N])
{
    return N;
}

std::vector<std::string> split(const std::string& str);

void makeLower(std::string& str);

void printArgs(const std::vector<std::string>& args);

void writeStrToBuffer(char* buffer, const char* str);
