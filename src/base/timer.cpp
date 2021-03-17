// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/timer.h"
#include <fmt/core.h>

Timer::Timer() : totalTime_(0), running_(false) { start(); }

/*
 * Timing Routines
 */

// Return time string based on provided tick count
std::string Timer::timeString(clock_t ticks)
{
    auto n = ticks / CLOCKS_PER_SEC;
    auto hours = n / 3600;
    n %= 3600;
    auto minutes = n / 60;
    n %= 60;
    double seconds = ticks / double(CLOCKS_PER_SEC) - hours * 3600 - minutes * 60;
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
    startTime_ = clock();
    splitTime_ = startTime_;
}

// Set total time
void Timer::stop()
{
    totalTime_ = clock() - startTime_;
    running_ = false;
}

// Return split time
double Timer::split()
{
    clock_t newSplit = clock();
    double splitTime = (newSplit - splitTime_) / double(CLOCKS_PER_SEC);

    splitTime_ = newSplit;

    return splitTime;
}

// Accumulate time since last start
void Timer::accumulate() { totalTime_ += clock() - startTime_; }

// Zero total time
void Timer::zero() { totalTime_ = 0; }

// Return current elapsed time as a time string
std::string Timer::elapsedTimeString()
{
    if (running_)
        return timeString(clock() - startTime_);
    else
        return timeString(totalTime_);
}

// Return total time (after stop()) as a time string
std::string Timer::totalTimeString() { return timeString(totalTime_); }

// Return number of seconds elapsed
double Timer::secondsElapsed() const
{
    if (running_)
        return (clock() - startTime_) / double(CLOCKS_PER_SEC);
    else
        return totalTime_ / double(CLOCKS_PER_SEC);
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
