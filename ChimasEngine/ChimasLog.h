#pragma once

#include <cstdarg>
#include <cstdio>

// Tiny engine-owned logging helper so game code doesn't call SDL_Log directly.
namespace ChimasLog
{
    inline void Info(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        std::vfprintf(stdout, fmt, args);
        std::fprintf(stdout, "\n");
        va_end(args);
    }
}

