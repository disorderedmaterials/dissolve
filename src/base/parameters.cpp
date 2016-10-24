/*
	*** Parameters Definition
	*** src/base/parameters.cpp
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

#include "base/parameters.h"
#include "base/comms.h"

// Constructor
Parameters::Parameters() : MPIListItem<Parameters>()
{
	sigma_ = 0.0;
	epsilon_ = 0.0;
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

// Set all parameters simultaneously
void Parameters::set(double sigma, double epsilon, double charge)
{
	sigma_ = sigma;
	epsilon_ = epsilon;
	charge_ = charge;
}

// Set Lennard-Jones Sigma
void Parameters::setSigma(double value)
{
	sigma_ = value;
}

// Return Lennard-Jones Sigma
double Parameters::sigma()
{
	return sigma_;
}

// Set Lennard-Jones Epsilon
void Parameters::setEpsilon(double epsilon)
{
	epsilon_ = epsilon;
}

// Return Lennard-Jones Epsilon
double Parameters::epsilon()
{
	return epsilon_;
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
bool Parameters::broadcast()
{
#ifdef PARALLEL
	if (!Comm.broadcast(name_)) return false;
	if (!Comm.broadcast(description_)) return false;
	if (!Comm.broadcast(&sigma_, 1)) return false;
	if (!Comm.broadcast(&epsilon_, 1)) return false;
	if (!Comm.broadcast(&charge_, 1)) return false;
#endif
	return true;
}
