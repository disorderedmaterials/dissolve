/*
	*** Parameters Definition
	*** src/base/parameters.cpp
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

#include "base/parameters.h"
#include "base/processpool.h"

// Constructor
Parameters::Parameters() : MPIListItem<Parameters>()
{
	for (int n=0; n<MAXSRPARAMETERS; ++n) parameters_[n] = 0.0;
	charge_ = 0.0;
}

// Destructor
Parameters::~Parameters()
{
}

/*
 * Name / Description
 */

// Set name of Parameters
void Parameters::setName(const char* name)
{
	name_ = name;
}

// Return short name of Parameters
const char* Parameters::name() const
{
	return name_.get();
}

// Set description of Parameters
void Parameters::setDescription(const char* desc)
{
	description_ = desc;
}

// Return description of element
const char* Parameters::description() const
{
	return description_.get();
}

/*
 * Potential Parameters
 */

// Set parameter with index specified
void Parameters::setParameter(int index, double value)
{
#ifdef CHECKS
	if ((index < 0) || (index >= MAXSRPARAMETERS))
	{
		Messenger::error("OUT_OF_RANGE - Parameter index %i is out of range (MAXSRPARAMETERS = %i) so it cannot be set.\n", index, MAXSRPARAMETERS);
		return;
	}
#endif
	 parameters_[index] = value;
}

// Return parameter with index specified
double Parameters::parameter(int index)
{
#ifdef CHECKS
	if ((index < 0) || (index >= MAXSRPARAMETERS))
	{
		Messenger::error("OUT_OF_RANGE - Parameter index %i is out of range (MAXSRPARAMETERS = %i) so it cannot be returned.\n", index, MAXSRPARAMETERS);
		return 0.0;
	}
#endif
	 return parameters_[index];
}

// Set atomic charge
void Parameters::setCharge(double charge)
{
	charge_ = charge;
}

// Return atomic charge
double Parameters::charge()
{
	return charge_;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool Parameters::broadcast(ProcessPool& procPool, int root)
{
#ifdef PARALLEL
	if (!procPool.broadcast(name_, root)) return false;
	if (!procPool.broadcast(description_, root)) return false;
	if (!procPool.broadcast(&sigma_, 1, root)) return false;
	if (!procPool.broadcast(&epsilon_, 1, root)) return false;
	if (!procPool.broadcast(&charge_, 1, root)) return false;
#endif
	return true;
}
