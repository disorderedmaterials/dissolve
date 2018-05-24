/*
	*** Timer
	*** src/base/timer.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_TIMER_H
#define DISSOLVE_TIMER_H

#include "base/charstring.h"
#include <ctime>

// Timer
class Timer
{
	/*
	 * Simple class to store and manipulate time information in order to provide timings for routines etc.
	 */
	public:
	// Constructor
	Timer();


	/*
	 * Timing Routines
	 */
	private:
	// Start time
	clock_t startTime_;
	// Total time
	clock_t totalTime_;
	// Local string storage
	CharString timeString_;
	// Whether the timer is running or not
	bool running_;

	private:
	// Return time string based on provided tick count
	const char* timeString(clock_t ticks);

	public:
	// Start timer
	void start();
	// Stop timer and set total time
	void stop();
	// Accumulate time since last start
	void accumulate();
	// Zero total time
	void zero();
	// Return current elapsed time as a time string
	const char* elapsedTimeString();
	// Return total time (after stop()) as a time string
	const char* totalTimeString();
	// Return number of seconds elapsed
	double secondsElapsed();
	// Return time string for number of seconds provided
	static const char* timeString(double seconds);
};

#endif
