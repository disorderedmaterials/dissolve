/*
	*** MasterIntra Definition
	*** src/classes/masterintra.h
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

#ifndef DUQ_MASTERINTRA_H
#define DUQ_MASTERINTRA_H

#include "base/charstring.h"
#include "classes/speciesintra.h"
#include "templates/array2d.h"
#include "templates/listitem.h"

// Forward Declarations
/* none */

/*
 * MasterIntra Definition
 */
class MasterIntra : public SpeciesIntra, public ListItem<MasterIntra>
{
	public:
	// Constructor
	MasterIntra();
	// Destructor
	~MasterIntra();


	/*
	 * Basic Data
	 */
	protected:
	// Name for interaction
	CharString name_;

	public:
	// Set name for interaction (if relevant)
	void setName(const char* name);
	// Return name for interaction
	const char* name();


	/*
	 * Usage Counting
	 */
	private:
	// Use counts by specific AtomType pairs
	Array2D<int> usageCounts_;

	public:
	// Initialise usage count array
	void initialiseUsageArray(int nAtomTypes);
	// Return usage between specified pair of AtomType indices
	int usageCount(int idI, int idJ);
	// Register single usage of this term by the specified pair of AtomType indices
	void registerUsage(int idI, int idJ);
	// Unregister single usage of this term by the specified pair of AtomType indices
	void unregisterUsage(int idI, int idJ);
};

#endif
