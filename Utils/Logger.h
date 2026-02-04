#pragma once
#include <windows.h>
#include <string>
#include <cstdio>

namespace Utils {
    class Logger {
    public:
        static void Log(const wchar_t* format, ...) {
            wchar_t buffer[1024];
            va_list args;
            va_start(args, format);
            vswprintf_s(buffer, 1024, format, args);
            va_end(args);
            OutputDebugStringW(buffer);
            OutputDebugStringW(L"\n");
        }
    };
}
