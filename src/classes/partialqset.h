/*
	*** Set of Partials in Q (S(Q))
	*** src/classes/partialqset.h
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

#ifndef DUQ_PARTIALQSET_H
#define DUQ_PARTIALQSET_H

#include "classes/atomtypelist.h"
#include "base/xydata.h"
#include "templates/list.h"
#include "templates/array.h"

// Forward Declarations
class Configuration;

// Set of Partials in Q (S(Q))
class PartialQSet : public ListItem<PartialQSet>
{
	public:
	// Constructor
	PartialQSet();
	// Destructor
	~PartialQSet();


	/*
	 * Partials Data
	 */
	private:
	// AtomTypes used to create matrices
	AtomTypeList atomTypes_;
	// Index (e.g. in related Configuration) at which these partials were last calculated
	int index_;
	// Pair matrix, containing full atom-atom partial
	Array2D<XYData> partials_;
	// Unbound matrix, containing atom-atom partial of pairs not joined by bonds or angles
	Array2D<XYData> unboundPartials_;
	// Bound matrix, containing atom-atom partial of pairs joined by bonds or angles
	Array2D<XYData> boundPartials_;
	// Bragg S(Q) matrix, derived from summation of HKL terms
// 	Array2D<XYData> braggSQMatrix_;
	// Total function
	XYData total_;

	public:
	// Setup array storage based on supplied Configuration
	bool setup(Configuration* cfg, const char* tag, const char* suffix);
	// Reset partial arrays
	void reset();
	// Return number of AtomTypes used to generate matrices
	int nTypes();
	// Return index of partials
	int index() const;
	// Set new index
	void setIndex(int index);
	// Return full atom-atom partial specified
	XYData& partial(int i, int j);
	// Return atom-atom partial for pairs not joined by bonds or angles
	XYData& unboundPartial(int i, int j);
	// Return atom-atom partial for pairs joined by bonds or angles
	XYData& boundPartial(int i, int j);
	// Sum partials into total
	void formTotal();
	// Return total function
	XYData& total();
	// Save partials information to disk
	bool save();


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, int rootRank);
};

#endif
