/*
	*** Set of Partials in Q (S(Q))
	*** src/classes/partialqset.cpp
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

#include "classes/partialqset.h"
#include "classes/configuration.h"
#include "classes/partialrset.h"
#include "classes/atomtype.h"

// Constructor
PartialQSet::PartialQSet() : ListItem<PartialQSet>()
{
	index_ = -1;
}

// Destructor
PartialQSet::~PartialQSet()
{
}

/*
 * Partials Data
 */

// Setup arrays
bool PartialQSet::setup(const AtomTypeList& types, const char* prefix, const char* tag, const char* suffix)
{
	// Copy type array
	atomTypes_ = types;
	int nTypes = atomTypes_.nItems();

	partials_.initialise(nTypes, nTypes, true);
	boundPartials_.initialise(nTypes, nTypes, true);
	unboundPartials_.initialise(nTypes, nTypes, true);
	braggPartials_.initialise(nTypes, nTypes, true);

	CharString title;
	AtomTypeData* at1 = atomTypes_.first(), *at2;
	for (int n=0; n<nTypes; ++n, at1 = at1->next)
	{
		at2 = at1;
		for (int m=n; m<nTypes; ++m, at2 = at2->next)
		{
			// Partial S(Q)
			title.sprintf("%s-%s-%s-%s.%s", prefix, tag, at1->name(), at2->name(), suffix);
			partials_.ref(n,m).setName(title.get());
			boundPartials_.ref(n,m).setName(title.get());
			unboundPartials_.ref(n,m).setName(title.get());
			braggPartials_.ref(n,m).setName(title.get());
		}
	}

	// Total S(Q)
	title.sprintf("%s-%s-total.%s", prefix, tag, suffix);
	total_.setName(title);
	total_.clear();

	index_ = -1;

	return true;
}

// Reset partial arrays
void PartialQSet::reset()
{
	int nTypes = atomTypes_.nItems();
	for (int n=0; n<nTypes; ++n)
	{
		for (int m=n; m<nTypes; ++m)
		{
			partials_.ref(n,m).arrayY() = 0.0;
			boundPartials_.ref(n,m).arrayY() = 0.0;
			unboundPartials_.ref(n,m).arrayY() = 0.0;
		}
	}
	total_.arrayY() = 0.0;

	index_ = -1;
}

// Return number of AtomTypes used to generate matrices
int PartialQSet::nAtomTypes() const
{
	return atomTypes_.nItems();
}

// Return index of partials
int PartialQSet::index() const
{
	return index_;
}

// Set new index
void PartialQSet::setIndex(int index)
{
	index_ = index;
}

// Return full atom-atom partial specified
XYData& PartialQSet::partial(int i, int j)
{
	return partials_.ref(i, j);
}

// Return atom-atom partial for pairs not joined by bonds or angles
XYData& PartialQSet::unboundPartial(int i, int j)
{
	return unboundPartials_.ref(i, j);
}

// Return atom-atom partial for pairs joined by bonds or angles
XYData& PartialQSet::boundPartial(int i, int j)
{
	return boundPartials_.ref(i, j);
}

// Sum partials into total
void PartialQSet::formTotal()
{
	int nTypes = atomTypes_.nItems();
	if (nTypes == 0)
	{
		total_.clear();
		return;
	}

	// Copy x and y arrays from one of the partials, and zero the latter
	total_.arrayX() = partials_.ref(0,0).arrayX();
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
XYData& PartialQSet::total()
{
	return total_;
}

// Save all unweighted RDFs
bool PartialQSet::save()
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
			Messenger::print("Writing S(Q) file '%s'...\n", filename);

			parser.openOutput(filename, true);
			if (!parser.isFileGoodForWriting())
			{
				Messenger::error("Couldn't open file '%s' for writing.\n", filename);
				return false;
			}
			
			XYData& sq = partials_.ref(typeI,typeJ);
			XYData& bound = boundPartials_.ref(typeI,typeJ);
			XYData& unbound = unboundPartials_.ref(typeI,typeJ);
			parser.writeLineF("# %-14s  %-16s  %-16s  %-16s\n", "Q, 1/Angstroms", "S(Q)", "bound(Q)", "unbound(Q)"); 
			for (n = 0; n< sq.nPoints(); ++n) parser.writeLineF("%16.10e  %16.10e  %16.10e  %16.10e\n", sq.x(n), sq.y(n), bound.y(n), unbound.y(n));
			parser.closeFiles();
		}
	}

	Messenger::print("Writing RDF file '%s'...\n", total_.name());
	return total_.save(total_.name());
}

/*
 * Manipulation
 */

// Add in partials from source PartialQSet to our own
bool PartialQSet::addPartials(PartialQSet& source, double weighting)
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
			Messenger::error("AtomType '%s' not present in this PartialQSet, so can't add in the associated data.\n", atI->name());
			return false;
		}

		for (typeJ=typeI; typeJ<sourceNTypes; ++typeJ)
		{
			AtomType* atJ = source.atomTypes_.atomType(typeJ);
			localJ = atomTypes_.indexOf(atJ);
			if (localJ == -1)
			{
				Messenger::error("AtomType '%s' not present in this PartialQSet, so can't add in the associated data.\n", atJ->name());
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
void PartialQSet::reweightPartials(double factor)
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

/*
 * Parallel Comms
 */

// Broadcast data from root to all other processes
bool PartialQSet::broadcast(ProcessPool& procPool, int rootRank)
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

