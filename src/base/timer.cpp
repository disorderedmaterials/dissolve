/*
	*** Timer
	*** src/base/timer.cpp
	Copyright T. Youngs 2012-2019

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

#include "base/charstring.h"
#include "base/timer.h"

// Constructor
Timer::Timer()
{
	totalTime_ = 0;
	running_ = false;
	start();
}

/*
 * Timing Routines
 */

// Return time string based on provided tick count
const char* Timer::timeString(clock_t ticks)
{
	int n = ticks / CLOCKS_PER_SEC;
	int hours = n / 3600;
	n %= 3600;
	int minutes = n / 60;
	n %= 60;
	double seconds = ticks /  double(CLOCKS_PER_SEC) - hours*3600 - minutes*60;
	if (hours != 0) timeString_.sprintf("%i hours, %i minutes, and %0.2f seconds", hours, minutes, seconds);
	else if (minutes != 0) timeString_.sprintf("%i minutes and %0.2f seconds", minutes, seconds);
	else timeString_.sprintf("%0.2f seconds", seconds);
	return timeString_.get();
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
void Timer::accumulate()
{
	totalTime_ += clock() - startTime_;
}

// Zero total time
void Timer::zero()
{
	totalTime_ = 0;
}

// Return current elapsed time as a time string
const char* Timer::elapsedTimeString()
{
	if (running_) return timeString(clock() - startTime_);
	else return timeString(totalTime_);
}

// Return total time (after stop()) as a time string
const char* Timer::totalTimeString()
{
	return timeString(totalTime_);
}

// Return number of seconds elapsed
double Timer::secondsElapsed() const
{
	if (running_) return (clock() - startTime_) / double(CLOCKS_PER_SEC);
	else return totalTime_ / double(CLOCKS_PER_SEC);
}

// Return time string for number of seconds provided
const char* Timer::timeString(double seconds)
{
	static CharString result;

	int hours = int(seconds) / 3600;
	seconds -= hours * 3600;
	int minutes = int(seconds) / 60;
	seconds -= minutes * 60;
	
	if (hours != 0) result.sprintf("%i hours, %i minutes, and %0.1f seconds", hours, minutes, seconds);
	else if (minutes != 0) result.sprintf("%i minutes and %0.1f seconds", minutes, seconds);
	else result.sprintf("%0.1f seconds", seconds);
	return result.get();
}

// Return ETA string for number of seconds provided
const char* Timer::etaString(double seconds)
{
	static CharString result;

	int hours = int(seconds) / 3600;
	seconds -= hours * 3600;
	int minutes = int(seconds) / 60;
	seconds -= minutes * 60;
	
	result.sprintf("%02i:%02i:%02i", hours, minutes, int(seconds));

	return result.get();
}
