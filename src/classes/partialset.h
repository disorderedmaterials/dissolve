/*
	*** Set of Partials
	*** src/classes/partialset.h
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

#ifndef DUQ_PARTIALSET_H
#define DUQ_PARTIALSET_H

#include "classes/histogram.h"
#include "classes/atomtypelist.h"
#include "base/xydata.h"
#include "templates/list.h"
#include "templates/array2d.h"

// Forward Declarations
class Configuration;

// Set of Partials
class PartialSet : public ListItem<PartialSet>
{
	public:
	// Constructor
	PartialSet();
	// Destructor
	~PartialSet();


	/*
	 * Partials Data
	 */
	private:
	// AtomTypes used to generate matrices
	AtomTypeList atomTypes_;
	// Fingerprint for these partials (e.g. reflecting Configuration indices at which they were calculated)
	CharString fingerprint_;
	// Abscissa units for contained data
	CharString abscissaUnits_;
	// Histograms used for calculating full atom-atom partials in r
	Array2D<Histogram> fullHistograms_;
	// Histograms used for calculating bound atom-atom partials in r
	Array2D<Histogram> boundHistograms_;
	// Histograms used for deriving unbound atom-atom partials in r
	Array2D<Histogram> unboundHistograms_;
	// Pair matrix, containing full atom-atom partial
	Array2D<XYData> partials_;
	// Unbound matrix, containing atom-atom partial of pairs not joined by bonds or angles
	Array2D<XYData> unboundPartials_;
	// Bound matrix, containing atom-atom partial of pairs joined by bonds or angles
	Array2D<XYData> boundPartials_;
	// Bragg matrix, derived from summation of HKL terms
	Array2D<XYData> braggPartials_;
	// Total function
	XYData total_;

	public:
	// Set up using supplied Configuration
	bool setUp(Configuration* cfg, const char* prefix, const char* tag, const char* suffix, const char* abscissaUnits);
	// Set up PartialSet, initialising arrays for g(r) use
	bool setUp(const AtomTypeList& atomTypes, double rdfRange, double binWidth, const char* prefix, const char* tag, const char* suffix, const char* abscissaUnits);
	// Set up PartialSet without initialising arrays
	bool setUp(const AtomTypeList& atomTypes, const char* prefix, const char* tag, const char* suffix, const char* abscissaUnits);
	// Reset partial arrays
	void reset();
	// Return number of AtomTypes used to generate matrices
	int nAtomTypes() const;
	// Return atom types list
	const AtomTypeList& atomTypes() const;
	// Set new fingerprint
	void setFingerprint(const char* fingerprint);
	// Return fingerprint of partials
	const char* fingerprint() const;
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
	// Return atom-atom Bragg partial for pairs joined by bonds or angles
	XYData& braggPartial(int i, int j);
	// Sum partials into total
	void formTotal(bool applyConcentrationWeights);
	// Return total function
	XYData& total();
	// Save all partials and total
	bool save();
	// Name all resources based on the supplied prefix
	void setResourceNames(const char* prefix, const char* suffix = NULL);
	// Set resources name suffix, retaining original prefix
	void setResourceNameSuffixes(const char* suffix);


	/*
	 * Manipulation
	 */
	public:
	// Form partials from stored Histogram data
	void formPartials(double boxVolume, XYData& boxNormalisation);
	// Add in partials from source PartialSet to our own
	bool addPartials(PartialSet& source, double weighting);
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
