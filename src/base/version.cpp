/*
	*** Version Counter
	*** src/base/version.h
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

#include "base/version.h"
#include "base/processpool.h"

// Constructor
VersionCounter::VersionCounter()
{
	version_ = 0;
}

// Destructor
VersionCounter::~VersionCounter()
{
}

/*
 * Version Information
 */

// Reset version counter to zero
void VersionCounter::zero()
{
	version_ = 0;
}

/*
 * Operators
 */

// Automatic conversion to integer
VersionCounter::operator int() const
{
	return version_;
}

// Prefix increment
int VersionCounter::operator++()
{
	return (++version_);
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool VersionCounter::broadcast(ProcessPool& procPool, const int root)
{
#ifdef PARALLEL
	if (!procPool.broadcast(version_, root)) return false;
#endif
	return true;
}
