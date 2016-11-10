/*
	*** Set of Partials
	*** src/classes/partialset.cpp
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

#include "classes/partialset.h"
#include "configuration.h"
#include "box.h"

// Constructor
PartialSet::PartialSet() : MPIListItem<PartialSet>()
{
	partialsIndex_ = -1;
	nTypes_ = 0;
}

// Destructor
PartialSet::~PartialSet()
{
}

/*
 * Set of Partials
 */

// Setup array storage based on supplied Configuration
bool PartialSet::setup(Configuration* cfg, const char* tag, const char* suffix)
{
	// Construct a matrix based on the usedAtomTypes_ list of the Configuration, since this reflects all our possible partials
	int n, m;
	nTypes_ = cfg->nUsedAtomTypes();
	targetConfiguration_ = cfg;
	const double rdfRange = targetConfiguration_->rdfRange();
	const double binWidth = targetConfiguration_->rdfBinWidth();
	const double boxVolume = targetConfiguration_->box()->volume();
	Data2D boxNormalisation = targetConfiguration_->boxNormalisation();

	Messenger::print("--> Creating matrices (%ix%i)...\n", nTypes_, nTypes_);

	partials_.initialise(nTypes_, nTypes_, true);
	boundPartials_.initialise(nTypes_, nTypes_, true);
	unboundPartials_.initialise(nTypes_, nTypes_, true);

	Dnchar title;
	AtomTypeData* at1 = targetConfiguration_->usedAtomTypes(), *at2;
	Messenger::print("--> Creating Lists of partials and linking into matrices...\n");
	Messenger::printVerbose("Range/binWidth/Volume = %f/%f/%f\n", rdfRange, binWidth, boxVolume);
	for (n=0; n<nTypes_; ++n, at1 = at1->next)
	{
		at2 = at1;
		for (m=n; m<nTypes_; ++m, at2 = at2->next)
		{
			// Partial g(r)
			title.sprintf("%s-%s-%s-%s.%s", cfg->niceName(), tag, at1->name(), at2->name(), suffix);
			partials_.ref(n,m).initialise(0.0, rdfRange, binWidth);
			partials_.ref(n,m).normalisedData().setName(title.get());
			boundPartials_.ref(n,m).initialise(0.0, rdfRange, binWidth);
			boundPartials_.ref(n,m).normalisedData().setName(title.get());
			unboundPartials_.ref(n,m).initialise(0.0, rdfRange, binWidth);
			unboundPartials_.ref(n,m).normalisedData().setName(title.get());

			// -- For normalisation, self-terms must be multiplied by 2.0
			partials_.ref(n,m).setRadialNumberDensityNormalisation(boxVolume, at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation);
			boundPartials_.ref(n,m).setRadialNumberDensityNormalisation(boxVolume, at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation);
			unboundPartials_.ref(n,m).setRadialNumberDensityNormalisation(boxVolume, at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation);
		}
	}

	// Total g(r)
	int nBins = partials_.ref(0,0).nBins();
	total_.initialise(nBins);
	for (n=0; n<nBins; ++n) total_.setX(n, (n+0.5)*binWidth);
	title.sprintf("%s-%s-total.%s", cfg->name(), tag, suffix);
	total_.setName(title);

	partialsIndex_ = -1;

	return true;
}

// Reset partial arrays
void PartialSet::reset()
{
	for (int n=0; n<nTypes_; ++n)
	{
		for (int m=n; m<nTypes_; ++m)
		{
			partials_.ref(n,m).reset();
			boundPartials_.ref(n,m).reset();
			unboundPartials_.ref(n,m).reset();
		}
	}
	total_.arrayY() = 0.0;

	partialsIndex_ = -1;
}

// Return number of AtomTypes used to generate matrices
int PartialSet::nTypes()
{
	return nTypes_;
}

// Return target Configuration
Configuration* PartialSet::targetConfiguration()
{
	return targetConfiguration_;
}

// Return whether these partials are up-to-date?usedAtomTypes_.nItems
bool PartialSet::upToDate()
{
	return (partialsIndex_ == targetConfiguration_->coordinateIndex());
}

// Flag that these partials are up-to-date
void PartialSet::setUpToDate()
{
	partialsIndex_ = targetConfiguration_->coordinateIndex();
}

// Return full atom-atom partial specified
Histogram& PartialSet::partial(int i, int j)
{
	return partials_.ref(i, j);
}

// Return atom-atom partial for pairs not joined by bonds or angles
Histogram& PartialSet::unboundPartial(int i, int j)
{
	return unboundPartials_.ref(i, j);
}

// Return atom-atom partial for pairs joined by bonds or angles
Histogram& PartialSet::boundPartial(int i, int j)
{
	return boundPartials_.ref(i, j);
}


// Save all unweighted RDFs
bool PartialSet::save()
{
	LineParser parser;
	int typeI, typeJ, n;

	for (typeI=0; typeI<nTypes_; ++typeI)
	{
		for (typeJ=typeI; typeJ<nTypes_; ++typeJ)
		{
			// Open file and check that we're OK to proceed writing to it
			const char* filename = partials_.ref(typeI, typeJ).normalisedData().name();
			Messenger::print("Writing RDF file '%s'...\n", filename);

			parser.openOutput(filename, true);
			if (!parser.isFileGoodForWriting())
			{
				Messenger::error("Couldn't open file '%s' for writing.\n", filename);
				return false;
			}
			
			Data2D& rdf = partials_.ref(typeI,typeJ).normalisedData();
			Data2D& bound = boundPartials_.ref(typeI,typeJ).normalisedData();
			Data2D& unbound = unboundPartials_.ref(typeI,typeJ).normalisedData();
			parser.writeLineF("# %-14s  %-16s  %-16s  %-16s\n", "r, Angstroms", "g(r)", "bound(r)", "unbound(r)"); 
			for (n = 0; n<rdf.nPoints(); ++n) parser.writeLineF("%16.10e  %16.10e  %16.10e  %16.10e\n", rdf.x(n), rdf.y(n), bound.y(n), unbound.y(n));
			parser.closeFiles();
		}
	}

	Messenger::print("Writing RDF file '%s'...\n", total_.name());
	return total_.save(total_.name());
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool PartialSet::broadcast(ProcessPool& procPool)
{
#ifdef PARALLEL
	Messenger::print("BROADCAST OF PartialSet NOT IMPLEMENTED.\n");
	return false;
#endif
	return true;
}
