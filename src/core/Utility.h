#pragma once

#include <vector>
#include <string>

#define CommandLineArgs std::vector<std::string>

std::string GetFileDirectory();
CommandLineArgs GetCLArgs(int argc, char* args[]);
std::string CStrToString(const char* c);
