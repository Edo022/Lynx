#pragma once
#include "LuxEngine/System/SystemMacros.hpp"



#ifdef __linux__
#   include <unistd.h>
#elif defined(_WIN64)
#   include <io.h>
#   include <windows.h>
#endif
#include <iostream>
#include <cstdio>



namespace __tmp_output_color{
    namespace __pvt{
        static int colorize_index = std::ios_base::xalloc();

        inline FILE* get_standard_stream(const std::ostream& stream);
        inline bool is_atty(const std::ostream& stream);

        #if defined(_WIN64)
        inline void win_change_attributes(std::ostream& stream, int foreground, int background = -1);
        #endif
    }








    inline auto& red(std::ostream& stream) {
        linux(stream << "\033[31m");
        win10(__pvt::win_change_attributes(stream, FOREGROUND_RED));
        return stream;
    }

    inline auto& green(std::ostream& stream) {
        linux(stream << "\033[32m");
        win10(__pvt::win_change_attributes(stream, FOREGROUND_GREEN));
        return stream;
    }

    inline auto& yellow(std::ostream& stream) {
        linux(stream << "\033[33m");
        win10(__pvt::win_change_attributes(stream, FOREGROUND_GREEN | FOREGROUND_RED));
        return stream;
    }

    inline auto& blue(std::ostream& stream) {
        linux(stream << "\033[34m");
        win10(__pvt::win_change_attributes(stream, FOREGROUND_BLUE));
        return stream;
    }

    inline auto& magenta(std::ostream& stream) {
        linux(stream << "\033[35m");
        win10(__pvt::win_change_attributes(stream, FOREGROUND_BLUE | FOREGROUND_RED));
        return stream;
    }


    inline auto& white(std::ostream& stream) {
        linux(stream << "\033[37m");
        win10(__pvt::win_change_attributes(stream, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED));
        return stream;
    }





    namespace __pvt{
        inline FILE* get_standard_stream(const std::ostream& stream) {
            if (&stream == &std::cout) return stdout;
            else if ((&stream == &std::cerr) || (&stream == &std::clog)) return stderr;
            else return 0;
        }

        //checks if ostream is a terminal
        inline bool is_atty(const std::ostream& stream) {
            FILE* std_stream = get_standard_stream(stream);

            if (!std_stream) return false;

            linux(return ::isatty(fileno(std_stream)));
            win10(return ::_isatty(_fileno(std_stream)));
        }

        #ifdef _WIN64
        inline void win_change_attributes(std::ostream& stream, int foreground, int background) {
            static WORD defaultAttributes = 0;

            if (!__pvt::is_atty(stream)) return;

            //Get terminal handle
            HANDLE hTerminal = INVALID_HANDLE_VALUE;
            if (&stream == &std::cout) hTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
            else if (&stream == &std::cerr) hTerminal = GetStdHandle(STD_ERROR_HANDLE);

            //Save default terminal attributes
            if (!defaultAttributes) {
                CONSOLE_SCREEN_BUFFER_INFO info;
                if (!GetConsoleScreenBufferInfo(hTerminal, &info))
                    return;
                defaultAttributes = info.wAttributes;
            }

            if (foreground == -1 && background == -1) {
                SetConsoleTextAttribute(hTerminal, defaultAttributes);
                return;
            }

            CONSOLE_SCREEN_BUFFER_INFO info;
            if (!GetConsoleScreenBufferInfo(hTerminal, &info)) return;

            if (foreground != -1) {
                info.wAttributes &= ~(info.wAttributes & 0x0F);
                info.wAttributes |= static_cast<WORD>(foreground);
            }

            if (background != -1) {
                info.wAttributes &= ~(info.wAttributes & 0xF0);
                info.wAttributes |= static_cast<WORD>(background);
            }

            SetConsoleTextAttribute(hTerminal, info.wAttributes);
        }
        #endif
    }
}