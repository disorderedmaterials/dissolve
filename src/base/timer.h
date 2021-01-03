// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <ctime>
#include <string>

// Timer
class Timer
{
    /*
     * Simple class to store and manipulate tick information in order to provide timings for routines etc.
     */
    public:
    Timer();

    /*
     * Timing Routines
     */
    private:
    // Start time
    clock_t startTime_;
    // Split time
    clock_t splitTime_;
    // Total time
    clock_t totalTime_;
    // Whether the timer is running or not
    bool running_;

    private:
    // Return time string based on provided tick count
    std::string timeString(clock_t ticks);

    public:
    // Start timer
    void start();
    // Stop timer and set total time
    void stop();
    // Return split time
    double split();
    // Accumulate time since last start
    void accumulate();
    // Zero total time
    void zero();
    // Return current elapsed time as a time string
    std::string elapsedTimeString();
    // Return total time (after stop()) as a time string
    std::string totalTimeString();
    // Return number of seconds elapsed
    double secondsElapsed() const;
    // Return time string for number of seconds provided
    static std::string timeString(double seconds);
    // Return ETA string for number of seconds provided
    static std::string etaString(double seconds);
};
