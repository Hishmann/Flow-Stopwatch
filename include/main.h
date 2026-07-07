#pragma once

#include <ncurses.h>

#include <chrono>
#include <thread>

#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>


using Clock = std::chrono::steady_clock;


class Timer
{
    private:
        bool focusing = false;
        bool onBreak = false;

        Clock::time_point focusStart;
        Clock::time_point breakStart;

        long long totalFocusSeconds = 0;
        long long breakBankSeconds = 0;

    public:

        bool get_focusing() {return focusing;}
        bool get_onbreak() {return onBreak;}
        long long CurrentFocusTime();
        long long CurrentBreakBank();
        void StartFocus();
        void StopFocus();
        void StartBreak();
        void StopBreak();
        void Reset();
};


std::string FormatTime(long long seconds)
{
    if (seconds < 0)
        seconds = 0;

    long long h = seconds / 3600;
    seconds %= 3600;

    long long m = seconds / 60;
    seconds %= 60;

    std::stringstream ss;

    ss << std::setfill('0')
       << std::setw(2) << h << ":"
       << std::setw(2) << m << ":"
       << std::setw(2) << seconds;

    return ss.str();
}

