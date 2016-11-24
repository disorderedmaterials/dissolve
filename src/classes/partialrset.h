/*
	*** Set of Partials in r (g(r))
	*** src/classes/partialrset.h
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

#ifndef DUQ_PARTIALRSET_H
#define DUQ_PARTIALRSET_H

#include "classes/histogram.h"
#include "templates/list.h"
#include "templates/array.h"

// Forward Declarations
class Configuration;

// Set of Partials in r (g(r))
class PartialRSet : public ListItem<PartialRSet>
{
	public:
	// Constructor
	PartialRSet();
	// Destructor
	~PartialRSet();


	/*
	 * Partials Data
	 */
	private:
	// Number of AtomTypes used to generate matrices
	int nTypes_;
	// Target Configuration
	Configuration* targetConfiguration_;
	// Configuration index at which these partials were last calculated
	int partialsIndex_;
	// Pair matrix, containing full atom-atom partial
	Array2D<Histogram> partials_;
	// Unbound matrix, containing atom-atom partial of pairs not joined by bonds or angles
	Array2D<Histogram> unboundPartials_;
	// Bound matrix, containing atom-atom partial of pairs joined by bonds or angles
	Array2D<Histogram> boundPartials_;
	// Bragg S(Q) matrix, derived from summation of HKL terms
// 	Array2D<Data2D> braggSQMatrix_;
	// Total function
	Data2D total_;

	public:
	// Setup array storage based on supplied Configuration
	bool setup(Configuration* cfg, const char* tag, const char* suffix);
	// Reset partial arrays
	void reset();
	// Return number of AtomTypes used to generate matrices
	int nTypes();
	// Return target Configuration
	Configuration* targetConfiguration();
	// Return whether these partials are up-to-date?
	bool upToDate();
	// Flag that these partials are up-to-date
	void setUpToDate();
	// Return full atom-atom partial specified
	Histogram& partial(int i, int j);
	// Return atom-atom partial for pairs not joined by bonds or angles
	Histogram& unboundPartial(int i, int j);
	// Return atom-atom partial for pairs joined by bonds or angles
	Histogram& boundPartial(int i, int j);
	// Sum partials into total
	void formTotal();
	// Return total function
	Data2D& total();
	// Save partials information to disk
	bool save();


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from root to all other processes
	bool broadcast(ProcessPool& procPool, int rootRank);
};

#endif
