/*
	*** MasterIntra Definition
	*** src/classes/masterintra.cpp
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

#include "classes/masterintra.h"
#include "classes/atom.h"
#include "base/messenger.h"

// Constructor
MasterIntra::MasterIntra()
{
	parent_ = NULL;
	masterParameters_ = NULL;
	for (int n=0; n<MAXINTRAPARAMS; ++n) parameters_[n] = 0.0;
}

// Destructor
MasterIntra::~MasterIntra()
{
}

/*
 * Basic Data
 */

// Set name for interaction (if relevant)
void MasterIntra::setName(const char* name)
{
	name_ = name;
}

// Return name for interaction
const char* MasterIntra::name()
{
	return name_.get();
}

/*
 * Usage Counting
 */

// Initialise usage count array
void MasterIntra::initialiseUsageArray(int nAtomTypes)
{
	usageCounts_.initialise(nAtomTypes, nAtomTypes, true);
	usageCounts_ = 0;
}

// Return usage between specified pair of AtomType indices
int MasterIntra::usageCount(int idI, int idJ)
{
	return usageCounts_.value(idI, idJ);
}

// Register single usage of this term by the specified AtomType pair
void MasterIntra::registerUsage(int idI, int idJ)
{
	++usageCounts_.ref(idI, idJ);
}

// Unregister single usage of this term by the specified AtomType pair
void MasterIntra::unregisterUsage(int idI, int idJ)
{
	--usageCounts_.ref(idI, idJ);
}
