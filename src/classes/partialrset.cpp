/*
	*** Set of Partials in r (g(r))
	*** src/classes/partialrset.cpp
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
#include "classes/partialrset.h"
#include "classes/configuration.h"
#include "classes/box.h"

// Constructor
PartialRSet::PartialRSet() : ListItem<PartialRSet>()
{
	index_ = -1;
}

// Destructor
PartialRSet::~PartialRSet()
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
bool PartialRSet::setup(Configuration* cfg, const char* prefix, const char* tag, const char* suffix)
{
	return setup(cfg->usedAtomTypesList(), cfg->rdfRange(), cfg->rdfBinWidth(), prefix, tag, suffix);
}

// Setup PartialRSet
bool PartialRSet::setup(const AtomTypeList& atomTypes, double rdfRange, double binWidth, const char* prefix, const char* tag, const char* suffix)
{
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

	CharString title;
	AtomTypeData* at1 = atomTypes_.first(), *at2;
	Messenger::printVerbose("  --> Creating lists of partials and linking into matrices...\n");
	for (n=0; n<nTypes; ++n, at1 = at1->next)
	{
		at2 = at1;
		for (m=n; m<nTypes; ++m, at2 = at2->next)
		{
			// Working arrays
			fullHistograms_.ref(n,m).initialise(0.0, rdfRange, binWidth);
			boundHistograms_.ref(n,m).initialise(0.0, rdfRange, binWidth);
			unboundHistograms_.ref(n,m).initialise(0.0, rdfRange, binWidth);

			// Partial g(r)
			title.sprintf("%s-%s-%s-%s.%s", prefix, tag, at1->name(), at2->name(), suffix);
			partials_.ref(n,m).setName(title.get());
			boundPartials_.ref(n,m).setName(title.get());
			unboundPartials_.ref(n,m).setName(title.get());
		}
	}

	// Total g(r)
	int nBins = fullHistograms_.ref(0,0).nBins();
	total_.initialise(nBins);
	for (n=0; n<nBins; ++n) total_.setX(n, n*binWidth);
	title.sprintf("%s-%s-total.%s", prefix, tag, suffix);
	total_.setName(title);

	index_ = -1;

	return true;
}

// Reset partial arrays
void PartialRSet::reset()
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
		}
	}
	total_.arrayY() = 0.0;

	index_ = -1;
}

// Return number of AtomTypes used to generate matrices
int PartialRSet::nAtomTypes() const
{
	return atomTypes_.nItems();
}

// Return atom types array
AtomTypeList PartialRSet::atomTypes() const
{
	return atomTypes_;
}

// Return index of partials
int PartialRSet::index() const
{
	return index_;
}

// Set new index
void PartialRSet::setIndex(int index)
{
	index_ = index;
}

// Return full histogram specified
Histogram& PartialRSet::fullHistogram(int i, int j)
{
	return fullHistograms_.ref(i, j);
}

// Return bound histogram specified
Histogram& PartialRSet::boundHistogram(int i, int j)
{
	return boundHistograms_.ref(i, j);
}

// Return unbound histogram specified
Histogram& PartialRSet::unboundHistogram(int i, int j)
{
	return unboundHistograms_.ref(i, j);
}

// Return full atom-atom partial specified
XYData& PartialRSet::partial(int i, int j)
{
	return partials_.ref(i, j);
}

// Return atom-atom partial for pairs not joined by bonds or angles
XYData& PartialRSet::unboundPartial(int i, int j)
{
	return unboundPartials_.ref(i, j);
}

// Return atom-atom partial for pairs joined by bonds or angles
XYData& PartialRSet::boundPartial(int i, int j)
{
	return boundPartials_.ref(i, j);
}

// Sum partials into total
void PartialRSet::formTotal()
{
	int nTypes = atomTypes_.nItems();
	if (nTypes == 0)
	{
		total_.clear();
		return;
	}

	// Copy x and y arrays from one of the partials, and zero the latter
	total_.templateFrom(partials_.ref(0,0));
	total_.arrayY() = partials_.ref(0,0).arrayY();
	total_.arrayY() = 0.0;

	int typeI, typeJ;
	for (typeI=0; typeI<nTypes; ++typeI)
	{
		for (typeJ=typeI; typeJ<nTypes; ++typeJ)
		{
			// Calculate weighting factor
			double ci = atomTypes_[typeI]->fraction();
			double cj = atomTypes_[typeJ]->fraction();
			double factor = ci * cj * (typeI == typeJ ? 1.0 : 2.0);

			// Add contribution from partial (bound + unbound)
			total_.addY(partials_.ref(typeI,typeJ).arrayY(), factor);
			// TODO Does not include contributions from Bragg partials
		}
	}
}

// Return total function
XYData& PartialRSet::total()
{
	return total_;
}

// Save all partials and total
bool PartialRSet::save()
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
			Messenger::print("--> Writing RDF file '%s'...\n", filename);

			parser.openOutput(filename, true);
			if (!parser.isFileGoodForWriting())
			{
				Messenger::error("Couldn't open file '%s' for writing.\n", filename);
				return false;
			}
			
			XYData& rdf = partials_.ref(typeI,typeJ);
			XYData& bound = boundPartials_.ref(typeI,typeJ);
			XYData& unbound = unboundPartials_.ref(typeI,typeJ);
			parser.writeLineF("# %-14s  %-16s  %-16s  %-16s\n", "r, Angstroms", "g(r)", "bound(r)", "unbound(r)"); 
			for (n = 0; n<rdf.nPoints(); ++n) parser.writeLineF("%16.10e  %16.10e  %16.10e  %16.10e\n", rdf.x(n), rdf.y(n), bound.y(n), unbound.y(n));
			parser.closeFiles();
		}
	}

	Messenger::print("--> Writing RDF file '%s'...\n", total_.name());
	return total_.save(total_.name());
}

/*
 * Manipulation
 */

// Form partials from stored Histogram data
void PartialRSet::formPartials(double boxVolume, XYData& boxNormalisation)
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

// Add in partials from source PartialRSet to our own
bool PartialRSet::addPartials(PartialRSet& source, double weighting)
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
			Messenger::error("AtomType '%s' not present in this PartialRSet, so can't add in the associated data.\n", atI->name());
			return false;
		}

		for (typeJ=typeI; typeJ<sourceNTypes; ++typeJ)
		{
			AtomType* atJ = source.atomTypes_.atomType(typeJ);
			localJ = atomTypes_.indexOf(atJ);
			if (localJ == -1)
			{
				Messenger::error("AtomType '%s' not present in this PartialRSet, so can't add in the associated data.\n", atJ->name());
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
void PartialRSet::reweightPartials(double factor)
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
void PartialRSet::calculateRDF(XYData& destination, Histogram& histogram, double boxVolume, int nCentres, int nSurrounding, double multiplier, XYData& boxNormalisation)
{
	int nBins = histogram.nBins();
	double delta = histogram.delta();
	Array<int>& histo = histogram.histogram();

	destination.clear();

	double shellVolume, factor, r = 0.0, numberDensity = nSurrounding / boxVolume, normalisation;
	for (int n=0; n<nBins; ++n)
	{
		shellVolume = (4.0/3.0)*PI*(pow(r+delta,3.0) - pow(r,3.0));
		factor = nCentres * (shellVolume * numberDensity);
		normalisation = (multiplier / factor) * boxNormalisation.interpolated(r+delta*0.5);

		destination.addPoint(r, histo[n]*normalisation);

		r += delta;
	}
}

/*
 * Parallel Comms
 */

// Broadcast data from root to all other processes
bool PartialRSet::broadcast(ProcessPool& procPool, int rootRank)
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
