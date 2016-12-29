/*
	*** Set of Partials in Q (S(Q))
	*** src/classes/partialqset.cpp
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

#include "classes/partialqset.h"
#include "classes/configuration.h"

// Constructor
PartialQSet::PartialQSet() : ListItem<PartialQSet>()
{
	partialsIndex_ = -1;
	nTypes_ = 0;
}

// Destructor
PartialQSet::~PartialQSet()
{
}

/*
 * Set of Partials
 */

// Setup array storage based on supplied Configuration
bool PartialQSet::setup(Configuration* cfg, const char* tag, const char* suffix)
{
	// Construct a matrix based on the usedAtomTypes_ list of the Configuration, since this reflects all our possible partials
	int n, m;
	nTypes_ = cfg->nUsedAtomTypes();
	targetConfiguration_ = cfg;

	Messenger::print("--> Creating S(Q) matrices (%ix%i)...\n", nTypes_, nTypes_);

	partials_.initialise(nTypes_, nTypes_, true);
	boundPartials_.initialise(nTypes_, nTypes_, true);
	unboundPartials_.initialise(nTypes_, nTypes_, true);

	CharString title;
	AtomTypeData* at1 = targetConfiguration_->usedAtomTypes(), *at2;
	Messenger::print("--> Creating lists of partials and linking into matrices...\n");
	for (n=0; n<nTypes_; ++n, at1 = at1->next)
	{
		at2 = at1;
		for (m=n; m<nTypes_; ++m, at2 = at2->next)
		{
			// Partial S(Q)
			title.sprintf("%s-%s-%s-%s.%s", cfg->niceName(), tag, at1->name(), at2->name(), suffix);
			partials_.ref(n,m).setName(title.get());
			boundPartials_.ref(n,m).setName(title.get());
			unboundPartials_.ref(n,m).setName(title.get());
		}
	}

	// Total F(Q)
	title.sprintf("%s-%s-total.%s", cfg->name(), tag, suffix);
	total_.setName(title);

	partialsIndex_ = -1;

	return true;
}

// Reset partial arrays
void PartialQSet::reset()
{
	for (int n=0; n<nTypes_; ++n)
	{
		for (int m=n; m<nTypes_; ++m)
		{
			partials_.ref(n,m).arrayY() = 0.0;
			boundPartials_.ref(n,m).arrayY() = 0.0;
			unboundPartials_.ref(n,m).arrayY() = 0.0;
		}
	}
	total_.arrayY() = 0.0;

	partialsIndex_ = -1;
}

// Return number of AtomTypes used to generate matrices
int PartialQSet::nTypes()
{
	return nTypes_;
}

// Return target Configuration
Configuration* PartialQSet::targetConfiguration()
{
	return targetConfiguration_;
}

// Return whether these partials are up-to-date?usedAtomTypes_.nItems
bool PartialQSet::upToDate()
{
	return (partialsIndex_ == targetConfiguration_->coordinateIndex());
}

// Flag that these partials are up-to-date
void PartialQSet::setUpToDate()
{
	partialsIndex_ = targetConfiguration_->coordinateIndex();
}

// Return full atom-atom partial specified
Data2D& PartialQSet::partial(int i, int j)
{
	return partials_.ref(i, j);
}

// Return atom-atom partial for pairs not joined by bonds or angles
Data2D& PartialQSet::unboundPartial(int i, int j)
{
	return unboundPartials_.ref(i, j);
}

// Return atom-atom partial for pairs joined by bonds or angles
Data2D& PartialQSet::boundPartial(int i, int j)
{
	return boundPartials_.ref(i, j);
}

// Sum partials into total
void PartialQSet::formTotal()
{
	if (nTypes_ == 0)
	{
		total_.clear();
		return;
	}

	// Copy x and y arrays from one of the partials, and zero the latter
	total_.arrayX() = partials_.ref(0,0).arrayX();
	total_.arrayY() = partials_.ref(0,0).arrayY();
	total_.arrayY() = 0.0;

	int typeI, typeJ;
	for (typeI=0; typeI<nTypes_; ++typeI)
	{
		for (typeJ=typeI; typeJ<nTypes_; ++typeJ)
		{
			
			// Calculate weighting factor
			double ci = targetConfiguration_->usedAtomTypeFraction(typeI);
			double cj = targetConfiguration_->usedAtomTypeFraction(typeJ);
			double factor = ci * cj * (typeI == typeJ ? 1.0 : 2.0);

			// Add contribution from partial (bound + unbound)
			total_.addY(partials_.ref(typeI,typeJ).arrayY(), factor);
			// TODO Does not include contributions from Bragg partials
		}
	}
}

// Return total function
Data2D& PartialQSet::total()
{
	return total_;
}

// Save all unweighted RDFs
bool PartialQSet::save()
{
	LineParser parser;
	int typeI, typeJ, n;

	for (typeI=0; typeI<nTypes_; ++typeI)
	{
		for (typeJ=typeI; typeJ<nTypes_; ++typeJ)
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
			
			Data2D& sq = partials_.ref(typeI,typeJ);
			Data2D& bound = boundPartials_.ref(typeI,typeJ);
			Data2D& unbound = unboundPartials_.ref(typeI,typeJ);
			parser.writeLineF("# %-14s  %-16s  %-16s  %-16s\n", "Q, 1/Angstroms", "S(Q)", "bound(Q)", "unbound(Q)"); 
			for (n = 0; n< sq.nPoints(); ++n) parser.writeLineF("%16.10e  %16.10e  %16.10e  %16.10e\n", sq.x(n), sq.y(n), bound.y(n), unbound.y(n));
			parser.closeFiles();
		}
	}

	Messenger::print("Writing RDF file '%s'...\n", total_.name());
	return total_.save(total_.name());
}

/*
 * Parallel Comms
 */

// Broadcast data from root to all other processes
bool PartialQSet::broadcast(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
	// The structure should have already been setup(), so arrays should be ready to copy
	for (int typeI=0; typeI<nTypes_; ++typeI)
	{
		for (int typeJ=typeI; typeJ<nTypes_; ++typeJ)
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

