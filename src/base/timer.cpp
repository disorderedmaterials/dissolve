// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/timer.h"
#include "base/sysFunc.h"
#include <fmt/format.h>

Timer::Timer(bool immediateStart)
{
    if (immediateStart)
        start();
}

/*
 * Timing Routines
 */

// Start timer
void Timer::start()
{
    running_ = true;

    startTime_ = std::chrono::high_resolution_clock::now();
    splitTime_ = startTime_;
}

// Set total time
void Timer::stop()
{
    if (running_)
    {
        totalTime_ = std::chrono::high_resolution_clock::now() - startTime_;
        running_ = false;
    }
}

// Return split time
double Timer::split()
{
    auto newSplit = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> splitTime = newSplit - splitTime_;

    splitTime_ = newSplit;

    return splitTime.count();
}

// Accumulate time since last start
void Timer::accumulate() { totalTime_ += std::chrono::high_resolution_clock::now() - startTime_; }

// Zero total time
void Timer::zero() { totalTime_ = std::chrono::duration<double>(); }

// Return current elapsed time as a time string
std::string Timer::elapsedTimeString(bool truncate) { return timeString(secondsElapsed(), truncate); }

// Return total time (after stop()) as a time string
std::string Timer::totalTimeString() { return timeString(totalTime_.count()); }

// Return number of seconds elapsed
double Timer::secondsElapsed() const
{
    if (running_)
        return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime_).count();
    else
        return totalTime_.count();
}

// Return time string based on provided duration in seconds
std::string Timer::timeString(double duration, bool truncate)
{
    int n = duration;
    auto hours = n / 3600;
    n %= 3600;
    auto minutes = n / 60;
    n %= 60;
    auto seconds = duration - hours * 3600 - minutes * 60;

    if (hours != 0)
        return fmt::format("{} hour{}, {} minute{}, and {:0.0f} seconds", hours, DissolveSys::plural(hours), minutes,
                           DissolveSys::plural(minutes), seconds);
    else if (minutes != 0)
        return fmt::format("{} minute{} and {:0.0f} seconds", minutes, DissolveSys::plural(minutes), seconds);
    else if (truncate)
        return fmt::format("{:0.0f} seconds", seconds);
    else
        return fmt::format("{:0.1f} seconds", seconds);
}