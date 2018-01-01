/*
	*** Parameters Definition
	*** src/base/parameters.h
	Copyright T. Youngs 2012-2018

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

#ifndef DUQ_PARAMETERS_H
#define DUQ_PARAMETERS_H

#define MAXSRPARAMETERS 4

#include "templates/mpilistitem.h"
#include "base/charstring.h"

// Short-Range Interaction Prameters Definition
class Parameters : public MPIListItem<Parameters>
{
	public:
	// Constructor
	Parameters();
	// Destructor
	~Parameters();


	/*
	 * Name / Description
	 */
	private:
	// Short Name
	CharString name_;
	// Description
	CharString description_;

	public:
	// Set name of Parameters
	void setName(const char* name);
	// Return name of Parameters
	const char* name() const;
	// Set description of Parameters
	void setDescription(const char* desc);
	// Return description of element
	const char* description() const;


	/*
	 * Potential Parameters
	 */
	private:
	// Parameter array
	double parameters_[MAXSRPARAMETERS];
	// Atomic charge
	double charge_;

	public:
	// Set parameter with index specified
	void setParameter(int index, double value);
	// Return parameter with index specified
	double parameter(int index);
	// Set atomic charge
	void setCharge(double charge);
	// Return atomic charge
	double charge();


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, int root = 0);
};

#endif
