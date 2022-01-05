// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/timer.h"
#include <fmt/core.h>

Timer::Timer() { start(); }

/*
 * Timing Routines
 */

// Return time string based on provided duration in seconds
std::string Timer::timeString(std::chrono::duration<double> duration)
{
    int n = duration.count();
    auto hours = n / 3600;
    n %= 3600;
    auto minutes = n / 60;
    n %= 60;
    double seconds = duration.count() - hours * 3600 - minutes * 60;
    if (hours != 0)
        return fmt::format("{} hours, {} minutes, and {:0.2f} seconds", hours, minutes, seconds);
    else if (minutes != 0)
        return fmt::format("{} minutes and {:0.2f} seconds", minutes, seconds);
    else
        return fmt::format("{:0.2f} seconds", seconds);
}

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
    totalTime_ = std::chrono::high_resolution_clock::now() - startTime_;
    running_ = false;
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
std::string Timer::elapsedTimeString()
{
    if (running_)
        return timeString(std::chrono::high_resolution_clock::now() - startTime_);
    else
        return timeString(totalTime_);
}

// Return total time (after stop()) as a time string
std::string Timer::totalTimeString() { return timeString(totalTime_); }

// Return number of seconds elapsed
double Timer::secondsElapsed() const
{
    if (running_)
        return (std::chrono::high_resolution_clock::now() - startTime_).count();
    else
        return totalTime_.count();
}

// Return time string for number of seconds provided
std::string Timer::timeString(double seconds)
{
    auto hours = int(seconds) / 3600;
    seconds -= hours * 3600;
    auto minutes = int(seconds) / 60;
    seconds -= minutes * 60;

    if (hours != 0)
        return fmt::format("{} hours, {} minutes, and {:0.1f} seconds", hours, minutes, seconds);
    else if (minutes != 0)
        return fmt::format("{} minutes and {:0.1f} seconds", minutes, seconds);
    else
        return fmt::format("{:0.1f} seconds", seconds);
}

// Return ETA string for number of seconds provided
std::string Timer::etaString(double seconds)
{
    auto hours = int(seconds) / 3600;
    seconds -= hours * 3600;
    auto minutes = int(seconds) / 60;
    seconds -= minutes * 60;

    return fmt::format("{:02d}:{:02d}:{:02d}", hours, minutes, int(seconds));
}
