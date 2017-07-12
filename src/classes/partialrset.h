/*
	*** Set of Partials in r (g(r))
	*** src/classes/partialrset.h
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

#ifndef DUQ_PARTIALRSET_H
#define DUQ_PARTIALRSET_H

#include "classes/histogram.h"
#include "classes/atomtypelist.h"
#include "base/xydata.h"
#include "templates/list.h"
#include "templates/array2d.h"

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
	// AtomTypes used to generate matrices
	AtomTypeList atomTypes_;
	// Index (e.g. in related Configuration) at which these partials were last calculated
	int index_;
	// Histograms used for calculating full atom-atom partials
	Array2D<Histogram> fullHistograms_;
	// Histograms used for calculating bound atom-atom partials
	Array2D<Histogram> boundHistograms_;
	// Histograms used for deriving unbound atom-atom partials
	Array2D<Histogram> unboundHistograms_;
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
	// Setup using supplied Configuration
	bool setup(Configuration* cfg, const char* prefix, const char* tag, const char* suffix);
	// Setup PartialRSet
	bool setup(const AtomTypeList& atomTypes, double rdfRange, double binWidth, const char* prefix, const char* tag, const char* suffix);
	// Reset partial arrays
	void reset();
	// Return number of AtomTypes used to generate matrices
	int nAtomTypes() const;
	// Return atom types array
	AtomTypeList atomTypes() const;
	// Return index of partials
	int index() const;
	// Set new index
	void setIndex(int index);
	// Return full histogram specified
	Histogram& fullHistogram(int i, int j);
	// Return bound histogram specified
	Histogram& boundHistogram(int i, int j);
	// Return unbound histogram specified
	Histogram& unboundHistogram(int i, int j);
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
	// Save all partials and total
	bool save();


	/*
	 * Manipulation
	 */
	public:
	// Form partials from stored Histogram data
	void formPartials(double boxVolume, XYData& boxNormalisation);
	// Add in partials from source PartialRSet to our own
	bool addPartials(PartialRSet& source, double weighting);
	// Re-weight partials (including total) with supplied weighting factor
	void reweightPartials(double factor);
	// Calculate RDF from supplied Histogram and normalisation data
	static void calculateRDF(XYData& destination, Histogram& histogram, double boxVolume, int nCentres, int nSurrounding, double multiplier, XYData& boxNormalisation);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from root to all other processes
	bool broadcast(ProcessPool& procPool, int rootRank);
};

#endif
