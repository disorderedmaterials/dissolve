// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <chrono>
#include <string>

// Timer
class Timer
{
    public:
    explicit Timer(bool immediateStart = true);

    private:
    // Start time
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
    // Split time
    std::chrono::time_point<std::chrono::high_resolution_clock> splitTime_;
    // Total time
    std::chrono::duration<double> totalTime_;
    // Whether the timer is running or not
    bool running_{false};
    // Whether the timer has started before
    bool started_{false};

    public:
    // Returns whether timer has started before
    bool hasStarted();
    // Start timer
    void start(bool resetTimer = true);
    // Stop timer and set total time
    void stop();
    // Return split time
    double split();
    // Accumulate time since last start
    void accumulate();
    // Zero total time
    void zero();
    // Return current elapsed time as a time string
    std::string elapsedTimeString(bool truncate = false);
    // Return total time (after stop()) as a time string
    std::string totalTimeString();
    // Return number of seconds elapsed
    double secondsElapsed() const;
    // Return time string based on provided duration in seconds
    static std::string timeString(std::chrono::duration<double> duration = std::chrono::duration<double>(),
                                  bool truncate = false);
    // Return time string for number of seconds provided
    static std::string timeString(double seconds);
};
