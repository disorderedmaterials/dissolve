/*
	*** Timer
	*** src/base/timer.cpp
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/charstring.h"
#include "base/timer.h"

// Constructor
Timer::Timer()
{
	totalTime_ = 0;
	start();
}

/*
 * Timing Routines
 */

// Start timer
void Timer::start()
{
	startTime_ = clock();
}

// Set total time
void Timer::stop()
{
	totalTime_ = clock() - startTime_;
}

// Zero total time
void Timer::zero()
{
	totalTime_ = 0;
}

// Accumulate partial time since last start
void Timer::accumulate()
{
	totalTime_ += clock() - startTime_;
}

// Return time string
const char* Timer::timeString()
{
	int n = totalTime_ / CLOCKS_PER_SEC;
	int hours = n / 3600;
	n %= 3600;
	int minutes = n / 60;
	n %= 60;
	double seconds = totalTime_ /  double(CLOCKS_PER_SEC) - hours*3600 - minutes*60;
	if (hours != 0) timeString_.sprintf("%i hours, %i minutes, and %0.1f seconds", hours, minutes, seconds);
	else if (minutes != 0) timeString_.sprintf("%i minutes and %0.1f seconds", minutes, seconds);
	else timeString_.sprintf("%0.1f seconds", seconds);
	return timeString_.get();
}
