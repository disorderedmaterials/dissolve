/*
	*** Set of Partials
	*** src/classes/partialset.cpp
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

#include "classes/atomtype.h"
#include "classes/partialset.h"
#include "classes/configuration.h"
#include "classes/box.h"

// Constructor
PartialSet::PartialSet() : ListItem<PartialSet>()
{
	index_ = -1;
}

// Destructor
PartialSet::~PartialSet()
{
	fullHistograms_.clear();
	boundHistograms_.clear();
	unboundHistograms_.clear();

	partials_.clear();
	boundPartials_.clear();
	unboundPartials_.clear();
}

/*
 * Set of Partials
 */

// Setup using supplied Configuration
bool PartialSet::setup(Configuration* cfg, const char* prefix, const char* tag, const char* suffix, const char* abscissaUnits)
{
	return setup(cfg->usedAtomTypesList(), cfg->rdfRange(), cfg->rdfBinWidth(), prefix, tag, suffix, abscissaUnits);
}

// Setup PartialSet
bool PartialSet::setup(const AtomTypeList& atomTypes, double rdfRange, double binWidth, const char* prefix, const char* tag, const char* suffix, const char* abscissaUnits)
{
	abscissaUnits_ = abscissaUnits;

	// Construct a matrix based on the usedAtomTypes_ list of the Configuration, since this reflects all our possible partials
	int n, m;
	atomTypes_ = atomTypes;
	int nTypes = atomTypes_.nItems();

	Messenger::printVerbose("  --> Creating matrices (%ix%i)...\n", nTypes, nTypes);

	fullHistograms_.initialise(nTypes, nTypes, true);
	boundHistograms_.initialise(nTypes, nTypes, true);
	unboundHistograms_.initialise(nTypes, nTypes, true);

	partials_.initialise(nTypes, nTypes, true);
	boundPartials_.initialise(nTypes, nTypes, true);
	unboundPartials_.initialise(nTypes, nTypes, true);
	braggPartials_.initialise(nTypes, nTypes, true);

	// Setup array matrices for partials
	CharString title;
	AtomTypeData* at1 = atomTypes_.first(), *at2;
	Messenger::printVerbose("  --> Creating lists of partials and linking into matrices...\n");
	for (n=0; n<nTypes; ++n, at1 = at1->next)
	{
		at2 = at1;
		for (m=n; m<nTypes; ++m, at2 = at2->next)
		{
			// Histogram arrays for g(r)
			fullHistograms_.ref(n,m).initialise(0.0, rdfRange, binWidth);
			boundHistograms_.ref(n,m).initialise(0.0, rdfRange, binWidth);
			unboundHistograms_.ref(n,m).initialise(0.0, rdfRange, binWidth);

			// Partials
			title.sprintf("%s-%s-%s-%s.%s", prefix, tag, at1->atomTypeName(), at2->atomTypeName(), suffix);
			partials_.ref(n,m).setName(title.get());
			boundPartials_.ref(n,m).setName(title.get());
			unboundPartials_.ref(n,m).setName(title.get());
			braggPartials_.ref(n,m).setName(title.get());
		}
	}

	// Setup array for total
	int nBins = fullHistograms_.ref(0,0).nBins();
	total_.initialise(nBins);
	for (n=0; n<nBins; ++n) total_.setX(n, (n+0.5)*binWidth);
	title.sprintf("%s-%s-total.%s", prefix, tag, suffix);
	total_.setName(title);

	index_ = -1;

	return true;
}

// Setup PartialSet without initialising arrays
bool PartialSet::setup(const AtomTypeList& atomTypes, const char* prefix, const char* tag, const char* suffix, const char* abscissaUnits)
{
	abscissaUnits_ = abscissaUnits;

	// Copy type array
	atomTypes_ = atomTypes;
	int nTypes = atomTypes_.nItems();

	partials_.initialise(nTypes, nTypes, true);
	boundPartials_.initialise(nTypes, nTypes, true);
	unboundPartials_.initialise(nTypes, nTypes, true);
	braggPartials_.initialise(nTypes, nTypes, true);

	// Setup array matrices for partials
	CharString title;
	AtomTypeData* at1 = atomTypes_.first(), *at2;
	for (int n=0; n<nTypes; ++n, at1 = at1->next)
	{
		at2 = at1;
		for (int m=n; m<nTypes; ++m, at2 = at2->next)
		{
			title.sprintf("%s-%s-%s-%s.%s", prefix, tag, at1->atomTypeName(), at2->atomTypeName(), suffix);
			partials_.ref(n,m).setName(title.get());
			boundPartials_.ref(n,m).setName(title.get());
			unboundPartials_.ref(n,m).setName(title.get());
			braggPartials_.ref(n,m).setName(title.get());
		}
	}

	// Setup array for total
	title.sprintf("%s-%s-total.%s", prefix, tag, suffix);
	total_.setName(title);
	total_.clear();

	index_ = -1;

	return true;
}

// Reset partial arrays
void PartialSet::reset()
{
	int nTypes = atomTypes_.nItems();
	for (int n=0; n<nTypes; ++n)
	{
		for (int m=n; m<nTypes; ++m)
		{
			fullHistograms_.ref(n,m).reset();
			boundHistograms_.ref(n,m).reset();
			unboundHistograms_.ref(n,m).reset();

			partials_.ref(n,m).arrayY() = 0.0;
			boundPartials_.ref(n,m).arrayY() = 0.0;
			unboundPartials_.ref(n,m).arrayY() = 0.0;
			braggPartials_.ref(n,m).arrayY() = 0.0;
		}
	}
	total_.arrayY() = 0.0;

	index_ = -1;
}

// Return number of AtomTypes used to generate matrices
int PartialSet::nAtomTypes() const
{
	return atomTypes_.nItems();
}

// Return atom types array
AtomTypeList PartialSet::atomTypes() const
{
	return atomTypes_;
}

// Return index of partials
int PartialSet::index() const
{
	return index_;
}

// Set new index
void PartialSet::setIndex(int index)
{
	index_ = index;
}

// Return full histogram specified
Histogram& PartialSet::fullHistogram(int i, int j)
{
	return fullHistograms_.ref(i, j);
}

// Return bound histogram specified
Histogram& PartialSet::boundHistogram(int i, int j)
{
	return boundHistograms_.ref(i, j);
}

// Return unbound histogram specified
Histogram& PartialSet::unboundHistogram(int i, int j)
{
	return unboundHistograms_.ref(i, j);
}

// Return full atom-atom partial specified
XYData& PartialSet::partial(int i, int j)
{
	return partials_.ref(i, j);
}

// Return atom-atom partial for pairs not joined by bonds or angles
XYData& PartialSet::unboundPartial(int i, int j)
{
	return unboundPartials_.ref(i, j);
}

// Return atom-atom partial for pairs joined by bonds or angles
XYData& PartialSet::boundPartial(int i, int j)
{
	return boundPartials_.ref(i, j);
}

// Return atom-atom Bragg partial for pairs joined by bonds or angles
XYData& PartialSet::braggPartial(int i, int j)
{
	return braggPartials_.ref(i, j);
}

// Sum partials into total
void PartialSet::formTotal(bool applyConcentrationWeights)
{
	int nTypes = atomTypes_.nItems();
	if (nTypes == 0)
	{
		total_.clear();
		return;
	}

	// Copy x and y arrays from one of the partials, and zero the latter
	total_.templateFrom(partials_.ref(0,0));
	total_.arrayY() = 0.0;

	int typeI, typeJ;
	for (typeI=0; typeI<nTypes; ++typeI)
	{
		for (typeJ=typeI; typeJ<nTypes; ++typeJ)
		{
			// Calculate weighting factor if requested
			double factor = 1.0;
			if (applyConcentrationWeights)
			{
				double ci = atomTypes_[typeI]->fraction();
				double cj = atomTypes_[typeJ]->fraction();
				factor *= ci * cj * (typeI == typeJ ? 1.0 : 2.0);
			}

			// Add contribution from partial (bound + unbound)
			total_.addY(partials_.ref(typeI,typeJ).arrayY(), factor);
			// TODO Does not include contributions from Bragg partials
		}
	}

	total_.interpolate(XYData::SplineInterpolation);
}

// Return total function
XYData& PartialSet::total()
{
	return total_;
}

// Save all partials and total
bool PartialSet::save()
{
	LineParser parser;
	int typeI, typeJ, n;

	int nTypes = atomTypes_.nItems();
	for (typeI=0; typeI<nTypes; ++typeI)
	{
		for (typeJ=typeI; typeJ<nTypes; ++typeJ)
		{
			// Open file and check that we're OK to proceed writing to it
			const char* filename = partials_.ref(typeI, typeJ).name();
			Messenger::printVerbose("--> Writing partial file '%s'...\n", filename);

			parser.openOutput(filename, true);
			if (!parser.isFileGoodForWriting())
			{
				Messenger::error("Couldn't open file '%s' for writing.\n", filename);
				return false;
			}
			
			XYData& full = partials_.ref(typeI,typeJ);
			XYData& bound = boundPartials_.ref(typeI,typeJ);
			XYData& unbound = unboundPartials_.ref(typeI,typeJ);
			XYData& bragg = braggPartials_.ref(typeI,typeJ);
			parser.writeLineF("# %-14s  %-16s  %-16s  %-16s  %-16s\n", abscissaUnits_.get(), "Full", "Bound", "Unbound", "Bragg"); 
			for (n=0; n<full.nPoints(); ++n) parser.writeLineF("%16.9e  %16.9e  %16.9e  %16.9e  %16.9e\n", full.x(n), full.y(n), bound.y(n), unbound.y(n), n < bragg.nPoints() ? bragg.y(n) : 0.0);
			parser.closeFiles();
		}
	}

	Messenger::printVerbose("--> Writing total file '%s'...\n", total_.name());
	return total_.save(total_.name());
}

/*
 * Manipulation
 */

// Form partials from stored Histogram data
void PartialSet::formPartials(double boxVolume, XYData& boxNormalisation)
{
	int n, m;
	int nTypes = atomTypes_.nItems();

	AtomTypeData* at1 = atomTypes_.first(), *at2;
	for (n=0; n<nTypes; ++n, at1 = at1->next)
	{
		at2 = at1;
		for (m=n; m<nTypes; ++m, at2 = at2->next)
		{
			calculateRDF(partials_.ref(n, m), fullHistograms_.ref(n, m), boxVolume, at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation);
			calculateRDF(boundPartials_.ref(n, m), boundHistograms_.ref(n, m), boxVolume, at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation);
			calculateRDF(unboundPartials_.ref(n, m), unboundHistograms_.ref(n, m), boxVolume, at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation);
		}
	}
}

// Add in partials from source PartialSet to our own
bool PartialSet::addPartials(PartialSet& source, double weighting)
{
	// Loop over partials in source set
	int typeI, typeJ, localI, localJ;

	int sourceNTypes = source.atomTypes_.nItems();
	for (typeI=0; typeI<sourceNTypes; ++typeI)
	{
		AtomType* atI = source.atomTypes_.atomType(typeI);
		localI = atomTypes_.indexOf(atI);
		if (localI == -1)
		{
			Messenger::error("AtomType '%s' not present in this PartialSet, so can't add in the associated data.\n", atI->name());
			return false;
		}

		for (typeJ=typeI; typeJ<sourceNTypes; ++typeJ)
		{
			AtomType* atJ = source.atomTypes_.atomType(typeJ);
			localJ = atomTypes_.indexOf(atJ);
			if (localJ == -1)
			{
				Messenger::error("AtomType '%s' not present in this PartialSet, so can't add in the associated data.\n", atJ->name());
				return false;
			}

			// Grab source partials
			partials_.ref(localI, localJ).addInterpolated(source.partial(typeI, typeJ), weighting);
			boundPartials_.ref(localI, localJ).addInterpolated(source.boundPartial(typeI, typeJ), weighting);
			unboundPartials_.ref(localI, localJ).addInterpolated(source.unboundPartial(typeI, typeJ), weighting);
		}
	}

	// Add total function
	total_.addInterpolated(source.total(), weighting);

	return true;
}

// Re-weight partials (including total) with supplied weighting factor
void PartialSet::reweightPartials(double factor)
{
	int n, m;
	int nTypes = atomTypes_.nItems();

	AtomTypeData* at1 = atomTypes_.first(), *at2;
	for (n=0; n<nTypes; ++n, at1 = at1->next)
	{
		at2 = at1;
		for (m=n; m<nTypes; ++m, at2 = at2->next)
		{
			partials_.ref(n, m).arrayY() *= factor;
			boundPartials_.ref(n, m).arrayY() *= factor;
			unboundPartials_.ref(n, m).arrayY() *= factor;
		}
	}

	total_.arrayY() *= factor;
}

// Calculate and return RDF from supplied Histogram and normalisation data
void PartialSet::calculateRDF(XYData& destination, Histogram& histogram, double boxVolume, int nCentres, int nSurrounding, double multiplier, XYData& boxNormalisation)
{
	int nBins = histogram.nBins();
	double delta = histogram.delta();
	Array<int>& histo = histogram.histogram();

	destination.clear();

	double shellVolume, factor, r = 0.5*delta, lowerShellLimit = 0.0, numberDensity = nSurrounding / boxVolume, normalisation;
	for (int n=0; n<nBins; ++n)
	{
		shellVolume = (4.0/3.0)*PI*(pow(lowerShellLimit+delta,3.0) - pow(lowerShellLimit,3.0));
		factor = nCentres * (shellVolume * numberDensity);
		normalisation = (multiplier / factor) * boxNormalisation.interpolated(r+delta*0.5);

		destination.addPoint(r, histo[n]*normalisation);

		r += delta;
		lowerShellLimit += delta;
	}
}

/*
 * Parallel Comms
 */

// Broadcast data from root to all other processes
bool PartialSet::broadcast(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
	// The structure should have already been setup(), so arrays should be ready to copy
	int nTypes = atomTypes_.nItems();
	for (int typeI=0; typeI<nTypes; ++typeI)
	{
		for (int typeJ=typeI; typeJ<nTypes; ++typeJ)
		{
			partials_.ref(typeI, typeJ).broadcast(procPool, rootRank);
			boundPartials_.ref(typeI, typeJ).broadcast(procPool, rootRank);
			unboundPartials_.ref(typeI, typeJ).broadcast(procPool, rootRank);
		}
	}
	total_.broadcast(procPool, rootRank);
#endif
	return true;
}
