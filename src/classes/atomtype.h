/*
	*** AtomType Definition
	*** src/classes/atomtype.h
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

#ifndef DUQ_ATOMTYPE_H
#define DUQ_ATOMTYPE_H

#include "base/charstring.h"
#include "templates/mpilistitem.h"

// Forward Declarations
class Parameters;
class Isotope;

/*
 * AtomType Definition
 */
class AtomType : public MPIListItem<AtomType>
{
	public:
	// Constructor
	AtomType();
	// Destructor
	~AtomType();


	/*
	 * Properties
	 */
	private:
	// Name
	CharString name_;
	// Associated Element
	int element_;
	// Associated Parameters
	Parameters* parameters_;
	// Index of this type in the master type index
	int index_;

	public:
	// Set name of AtomType
	void setName(const char* name);
	// Return name of AtomType
	const char* name() const;
	// Set atomic element
	void setElement(int el);
	// Return atomic element
	int element() const;
	// Set associated Parameters
	void setParameters(Parameters* params);
	// Return associated Parameters
	Parameters* parameters() const;
	// Set index of this type in the master type index
	void setIndex(int id);
	// Return index of this type in the master type index
	int index() const;


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, int root = 0);
};

#endif
