#include "Utility.h"

#include "libs/sdl/include/SDL.h"

std::string GetFileDirectory()
{
    return SDL_GetBasePath();
}

CommandLineArgs GetCLArgs(int argc, char* args[])
{
    CommandLineArgs str_args;

    for (int i = 0; i < argc; i++)
    {
        std::string str;
        for (int ci = 0; args[i][ci] != '\0'; ci++)
        {
            str += args[i][ci];
        }
        str_args.push_back(str);
    }

    return str_args;
}

std::string CStrToString(const char* c)
{
    return c;
}