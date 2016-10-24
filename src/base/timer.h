/*
	*** Timer
	*** src/base/timer.h
	Copyright T. Youngs 2012-2016

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

#ifndef DUQ_TIMER_H
#define DUQ_TIMER_H

#include "base/dnchar.h"
#include <ctime>

/*
 * \brief Timer
 * \details Simple class to store and manipulate time information in order to provide timings for routines etc.
 */
class Timer
{
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
	Dnchar timeString_;
	
	public:
	// Start timer
	void start();
	// Stop timer and set total time
	void stop();
	// Zero total time
	void zero();
	// Accumulate partial time since last start
	void accumulate();
	// Return time string
	const char* timeString();
};

#endif
