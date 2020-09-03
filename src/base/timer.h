/*
    *** Timer
    *** src/base/timer.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
