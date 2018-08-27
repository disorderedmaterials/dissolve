/*
	*** Set of Partials
	*** src/classes/partialset.cpp
	Copyright T. Youngs 2012-2018

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/lineparser.h"
#include "classes/atomtype.h"
#include "classes/partialset.h"
#include "classes/configuration.h"
#include "classes/box.h"

// Constructor
PartialSet::PartialSet() : ListItem<PartialSet>()
{
	fingerprint_.clear();
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

// Set up using supplied Configuration
bool PartialSet::setUp(Configuration* cfg, const char* prefix, const char* tag, const char* suffix, const char* abscissaUnits)
{
	return setUp(cfg->usedAtomTypesList(), cfg->rdfRange(), cfg->rdfBinWidth(), prefix, tag, suffix, abscissaUnits);
}

// Set up PartialSet
bool PartialSet::setUp(const AtomTypeList& atomTypes, double rdfRange, double binWidth, const char* prefix, const char* tag, const char* suffix, const char* abscissaUnits)
{
	// Set up partial arrays
	if (!setUpPartials(atomTypes, prefix, tag, suffix, abscissaUnits)) return false;

	// Initialise histograms for g(r) calcultion
	setUpHistograms(rdfRange, binWidth);

	fingerprint_.clear();

	return true;
}

// Set up PartialSet without initialising histogram arrays
bool PartialSet::setUpPartials(const AtomTypeList& atomTypes, const char* prefix, const char* tag, const char* suffix, const char* abscissaUnits)
{
	abscissaUnits_ = abscissaUnits;

	// Copy type array
	atomTypes_ = atomTypes;
	int nTypes = atomTypes_.nItems();

	partials_.initialise(nTypes, nTypes, true);
	boundPartials_.initialise(nTypes, nTypes, true);
	unboundPartials_.initialise(nTypes, nTypes, true);
	braggPartials_.initialise(nTypes, nTypes, true);

	// Set up array matrices for partials
	CharString title;
	AtomTypeData* at1 = atomTypes_.first(), *at2;
	for (int n=0; n<nTypes; ++n, at1 = at1->next)
	{
		at2 = at1;
		for (int m=n; m<nTypes; ++m, at2 = at2->next)
		{
			title.sprintf("%s-%s-%s-%s.%s", prefix, tag, at1->atomTypeName(), at2->atomTypeName(), suffix);
			partials_.at(n,m).setName(title.get());
			boundPartials_.at(n,m).setName(title.get());
			unboundPartials_.at(n,m).setName(title.get());
			braggPartials_.at(n,m).setName(title.get());
		}
	}

	// Set up array for total
	title.sprintf("%s-%s-total.%s", prefix, tag, suffix);
	total_.setName(title);
	total_.clear();

	fingerprint_.clear();

	return true;
}

// Set up histogram arrays for g(r) calculation
void PartialSet::setUpHistograms(double rdfRange, double binWidth)
{
	int nTypes = atomTypes_.nItems();

	fullHistograms_.initialise(nTypes, nTypes, true);
	boundHistograms_.initialise(nTypes, nTypes, true);
	unboundHistograms_.initialise(nTypes, nTypes, true);

	for (int n=0; n<nTypes; ++n)
	{
		for (int m=n; m<nTypes; ++m)
		{
			// Histogram arrays for g(r)
			fullHistograms_.at(n,m).initialise(0.0, rdfRange, binWidth);
			boundHistograms_.at(n,m).initialise(0.0, rdfRange, binWidth);
			unboundHistograms_.at(n,m).initialise(0.0, rdfRange, binWidth);
		}
	}
}

// Reset partial arrays
void PartialSet::reset()
{
	int nTypes = atomTypes_.nItems();
	for (int n=0; n<nTypes; ++n)
	{
		for (int m=n; m<nTypes; ++m)
		{
			fullHistograms_.at(n,m).zero();
			boundHistograms_.at(n,m).zero();
			unboundHistograms_.at(n,m).zero();

			partials_.at(n,m).arrayY() = 0.0;
			boundPartials_.at(n,m).arrayY() = 0.0;
			unboundPartials_.at(n,m).arrayY() = 0.0;
			braggPartials_.at(n,m).arrayY() = 0.0;
		}
	}
	total_.arrayY() = 0.0;

	fingerprint_.clear();
}

// Return number of AtomTypes used to generate matrices
int PartialSet::nAtomTypes() const
{
	return atomTypes_.nItems();
}

// Return atom types list
const AtomTypeList& PartialSet::atomTypes() const
{
	return atomTypes_;
}

// Set new fingerprint
void PartialSet::setFingerprint(const char* fingerprint)
{
	fingerprint_ = fingerprint;
}

// Return fingerprint of partials
const char* PartialSet::fingerprint() const
{
	return fingerprint_.get();
}

// Return full histogram specified
Histogram1D& PartialSet::fullHistogram(int i, int j)
{
	return fullHistograms_.at(i, j);
}

// Return bound histogram specified
Histogram1D& PartialSet::boundHistogram(int i, int j)
{
	return boundHistograms_.at(i, j);
}

// Return unbound histogram specified
Histogram1D& PartialSet::unboundHistogram(int i, int j)
{
	return unboundHistograms_.at(i, j);
}

// Return full atom-atom partial specified
XYData& PartialSet::partial(int i, int j)
{
	return partials_.at(i, j);
}

// Return full atom-atom partial specified (const)
XYData& PartialSet::constPartial(int i, int j) const
{
	return partials_.constAt(i, j);
}

// Return atom-atom partial for pairs not joined by bonds or angles
XYData& PartialSet::unboundPartial(int i, int j)
{
	return unboundPartials_.at(i, j);
}

// Return atom-atom partial for pairs not joined by bonds or angles (const)
XYData& PartialSet::constUnboundPartial(int i, int j) const
{
	return unboundPartials_.constAt(i, j);
}

// Return atom-atom partial for pairs joined by bonds or angles
XYData& PartialSet::boundPartial(int i, int j)
{
	return boundPartials_.at(i, j);
}

// Return atom-atom partial for pairs joined by bonds or angles (const)
XYData& PartialSet::constBoundPartial(int i, int j) const
{
	return boundPartials_.constAt(i, j);
}

// Return atom-atom Bragg partial
XYData& PartialSet::braggPartial(int i, int j)
{
	return braggPartials_.at(i, j);
}

// Return atom-atom Bragg partial (const)
XYData& PartialSet::constBraggPartial(int i, int j) const
{
	return braggPartials_.constAt(i, j);
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
	total_.templateFrom(partials_.at(0,0));
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
			total_.addY(partials_.at(typeI,typeJ).arrayY(), factor);
			// TODO Does not include contributions from Bragg partials
		}
	}
}

// Return total function
XYData& PartialSet::total()
{
	return total_;
}

// Return copy of total function
XYData PartialSet::constTotal() const
{
	return total_;
}

// Calculate and return total bound function
XYData PartialSet::boundTotal(bool applyConcentrationWeights) const
{
	const int nTypes = atomTypes_.nItems();
	if (nTypes == 0) return XYData();

	XYData bound;
	bound.templateFrom(boundPartials_.constAt(0,0));

	int typeI, typeJ;
	AtomTypeData* atd1 = atomTypes_.first();
	for (typeI=0; typeI<nTypes; ++typeI, atd1 = atd1->next)
	{
		AtomTypeData* atd2 = atd1;
		for (typeJ=typeI; typeJ<nTypes; ++typeJ, atd2 = atd2->next)
		{
			// Calculate weighting factor if requested
			double factor = 1.0;
			if (applyConcentrationWeights)
			{
				double ci = atd1->fraction();
				double cj = atd2->fraction();
				factor *= ci * cj * (typeI == typeJ ? 1.0 : 2.0);
			}

			// Add contribution
			bound.addY(boundPartials_.constAt(typeI,typeJ).arrayY(), factor);
		}
	}

	return bound;
}

// Calculate and return total unbound function
XYData PartialSet::unboundTotal(bool applyConcentrationWeights) const
{
	const int nTypes = atomTypes_.nItems();
	if (nTypes == 0) return XYData();

	XYData unbound;
	unbound.templateFrom(boundPartials_.constAt(0,0));

	int typeI, typeJ;
	AtomTypeData* atd1 = atomTypes_.first();
	for (typeI=0; typeI<nTypes; ++typeI, atd1 = atd1->next)
	{
		AtomTypeData* atd2 = atd1;
		for (typeJ=typeI; typeJ<nTypes; ++typeJ, atd2 = atd2->next)
		{
			// Calculate weighting factor if requested
			double factor = 1.0;
			if (applyConcentrationWeights)
			{
				double ci = atd1->fraction();
				double cj = atd2->fraction();
				factor *= ci * cj * (typeI == typeJ ? 1.0 : 2.0);
			}

			// Add contribution
			unbound.addY(unboundPartials_.constAt(typeI,typeJ).arrayY(), factor);
		}
	}

	return unbound;
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
			const char* filename = partials_.at(typeI, typeJ).name();
			Messenger::printVerbose("Writing partial file '%s'...\n", filename);

			parser.openOutput(filename, true);
			if (!parser.isFileGoodForWriting())
			{
				Messenger::error("Couldn't open file '%s' for writing.\n", filename);
				return false;
			}
			
			XYData& full = partials_.at(typeI,typeJ);
			XYData& bound = boundPartials_.at(typeI,typeJ);
			XYData& unbound = unboundPartials_.at(typeI,typeJ);
			XYData& bragg = braggPartials_.at(typeI,typeJ);
			parser.writeLineF("# %-14s  %-16s  %-16s  %-16s  %-16s\n", abscissaUnits_.get(), "Full", "Bound", "Unbound", "Bragg"); 
			for (n=0; n<full.nPoints(); ++n) parser.writeLineF("%16.9e  %16.9e  %16.9e  %16.9e  %16.9e\n", full.constX(n), full.constY(n), bound.constY(n), unbound.constY(n), n < bragg.nPoints() ? bragg.constY(n) : 0.0);
			parser.closeFiles();
		}
	}

	Messenger::printVerbose("Writing total file '%s'...\n", total_.name());
	return total_.save(total_.name());
}

// Name all object based on the supplied prefix
void PartialSet::setObjectNames(const char* prefix, const char* suffix)
{
	// Set up suffix (if any)
	CharString actualSuffix;
	if (suffix != NULL) actualSuffix.sprintf("_%s", suffix);

	objectNamePrefix_ = prefix;

	int typeI, typeJ;
	int nTypes = atomTypes_.nItems();
	AtomTypeData* at1 = atomTypes_.first(), *at2;
	for (typeI=0; typeI<nTypes; ++typeI, at1 = at1->next)
	{
		at2 = at1;
		for (typeJ=typeI; typeJ<nTypes; ++typeJ, at2 = at2->next)
		{
			partials_.at(typeI,typeJ).setObjectName(CharString("%s//%s-%s//Full%s", prefix, at1->atomTypeName(), at2->atomTypeName(), actualSuffix.get()));
			boundPartials_.at(typeI,typeJ).setObjectName(CharString("%s//%s-%s//Bound%s", prefix, at1->atomTypeName(), at2->atomTypeName(), actualSuffix.get()));
			unboundPartials_.at(typeI,typeJ).setObjectName(CharString("%s//%s-%s//Unbound%s", prefix, at1->atomTypeName(), at2->atomTypeName(), actualSuffix.get()));
			braggPartials_.at(typeI,typeJ).setObjectName(CharString("%s//%s-%s//Bragg%s", prefix, at1->atomTypeName(), at2->atomTypeName(), actualSuffix.get()));
		}
	}

	total_.setObjectName(CharString("%s//Total%s", prefix, actualSuffix.get()));
}

// Return prefix applied to object names
const char* PartialSet::objectNamePrefix() const
{
	return objectNamePrefix_.get();
}

// Set underlying XYData file names
void PartialSet::setFileNames(const char* prefix, const char* tag, const char* suffix)
{
	int nTypes = atomTypes_.nItems();

	// Set titles for partials
	CharString title;
	AtomTypeData* at1 = atomTypes_.first(), *at2;
	for (int n=0; n<nTypes; ++n, at1 = at1->next)
	{
		at2 = at1;
		for (int m=n; m<nTypes; ++m, at2 = at2->next)
		{
			title.sprintf("%s-%s-%s-%s.%s", prefix, tag, at1->atomTypeName(), at2->atomTypeName(), suffix);
			partials_.at(n,m).setName(title.get());
			boundPartials_.at(n,m).setName(title.get());
			unboundPartials_.at(n,m).setName(title.get());
			braggPartials_.at(n,m).setName(title.get());
		}
	}

	// Set up array for total
	title.sprintf("%s-%s-total.%s", prefix, tag, suffix);
	total_.setName(title);
}

/*
 * Manipulation
 */

// Adjust all partials, adding specified delta to each
void PartialSet::adjust(double delta)
{
	int n, m;
	int nTypes = atomTypes_.nItems();

	AtomTypeData* at1 = atomTypes_.first(), *at2;
	for (n=0; n<nTypes; ++n, at1 = at1->next)
	{
		at2 = at1;
		for (m=n; m<nTypes; ++m, at2 = at2->next)
		{
			partials_.at(n, m).arrayY() += delta;
			boundPartials_.at(n, m).arrayY() += delta;
			unboundPartials_.at(n, m).arrayY() += delta;
		}
	}

	total_.arrayY() += delta;
}

// Form partials from stored Histogram data
void PartialSet::formPartials(double boxVolume, Interpolater& boxNormalisation)
{
	int n, m;
	int nTypes = atomTypes_.nItems();

	AtomTypeData* at1 = atomTypes_.first(), *at2;
	for (n=0; n<nTypes; ++n, at1 = at1->next)
	{
		at2 = at1;
		for (m=n; m<nTypes; ++m, at2 = at2->next)
		{
			calculateRDF(partials_.at(n, m), fullHistograms_.at(n, m), boxVolume, at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation);
			calculateRDF(boundPartials_.at(n, m), boundHistograms_.at(n, m), boxVolume, at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation);
			calculateRDF(unboundPartials_.at(n, m), unboundHistograms_.at(n, m), boxVolume, at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation);
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

			// Add interpolated source partials to our set
			Interpolater::addInterpolated(partials_.at(localI, localJ), source.partial(typeI, typeJ), weighting);
			Interpolater::addInterpolated(boundPartials_.at(localI, localJ), source.boundPartial(typeI, typeJ), weighting);
			Interpolater::addInterpolated(unboundPartials_.at(localI, localJ), source.unboundPartial(typeI, typeJ), weighting);
		}
	}

	// Add total function
	Interpolater::addInterpolated(total_, source.total(), weighting);

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
			partials_.at(n, m).arrayY() *= factor;
			boundPartials_.at(n, m).arrayY() *= factor;
			unboundPartials_.at(n, m).arrayY() *= factor;
		}
	}

	total_.arrayY() *= factor;
}

// Calculate and return RDF from supplied Histogram and normalisation data
void PartialSet::calculateRDF(XYData& destination, Histogram1D& histogram, double boxVolume, int nCentres, int nSurrounding, double multiplier, Interpolater& boxNormalisation)
{
	int nBins = histogram.nBins();
	double delta = histogram.binWidth();
	const Array<long int>& bins = histogram.bins();

	destination.clear();

	double shellVolume, factor, r = 0.5*delta, lowerShellLimit = 0.0, numberDensity = nSurrounding / boxVolume, normalisation;
	for (int n=0; n<nBins; ++n)
	{
		shellVolume = (4.0/3.0)*PI*(pow(lowerShellLimit+delta,3.0) - pow(lowerShellLimit,3.0));
		factor = nCentres * (shellVolume * numberDensity);
		normalisation = (multiplier / factor) * boxNormalisation.y(r+delta*0.5);

		destination.addPoint(r, bins.constAt(n)*normalisation);

		r += delta;
		lowerShellLimit += delta;
	}
}

/*
 * Operators
 */

void PartialSet::operator+=(const double delta)
{
	adjust(delta);
}

void PartialSet::operator-=(const double delta)
{
	adjust(-delta);
}

/*
 * GenericItemBase Implementations
 */

// Return class name
const char* PartialSet::itemClassName()
{
	return "PartialSet";
}

// Write data through specified LineParser
bool PartialSet::write(LineParser& parser)
{
	// TODO To reduce filesize we could write abscissa first, and then each Y datset afterwards since they all share a common scale

	if (!parser.writeLineF("%s\n", objectNamePrefix_.get())) return false;
	if (!parser.writeLineF("%s\n", abscissaUnits_.get())) return false;
	if (!parser.writeLineF("%s\n", fingerprint_.get())) return false;

	// Write out AtomTypes first
	atomTypes_.write(parser);
	int nTypes = atomTypes_.nItems();
	// Write individual XYData
	for (int typeI=0; typeI<nTypes; ++typeI)
	{
		for (int typeJ=typeI; typeJ<nTypes; ++typeJ)
		{
			if (!partials_.at(typeI, typeJ).write(parser)) return false;
			if (!boundPartials_.at(typeI, typeJ).write(parser)) return false;
			if (!unboundPartials_.at(typeI, typeJ).write(parser)) return false;
			if (!braggPartials_.at(typeI, typeJ).write(parser)) return false;
		}
	}
	if (!total_.write(parser)) return false;
	return true;
}

// Read data through specified LineParser
bool PartialSet::read(LineParser& parser)
{
	if (parser.readNextLine(LineParser::Defaults, objectNamePrefix_) != LineParser::Success) return false;
	if (parser.readNextLine(LineParser::Defaults, abscissaUnits_) != LineParser::Success) return false;
	if (parser.readNextLine(LineParser::Defaults, fingerprint_) != LineParser::Success) return false;

	atomTypes_.clear();
	if (!atomTypes_.read(parser)) return false;
	int nTypes = atomTypes_.nItems();
	partials_.initialise(nTypes, nTypes, true);
	boundPartials_.initialise(nTypes, nTypes, true);
	unboundPartials_.initialise(nTypes, nTypes, true);
	braggPartials_.initialise(nTypes, nTypes, true);

	// Read individual XYData
	for (int typeI=0; typeI<nTypes; ++typeI)
	{
		for (int typeJ=typeI; typeJ<nTypes; ++typeJ)
		{
			if (!partials_.at(typeI, typeJ).read(parser)) return false;
			if (!boundPartials_.at(typeI, typeJ).read(parser)) return false;
			if (!unboundPartials_.at(typeI, typeJ).read(parser)) return false;
			if (!braggPartials_.at(typeI, typeJ).read(parser)) return false;
		}
	}
	if (!total_.read(parser)) return false;
	return true;
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
			partials_.at(typeI, typeJ).broadcast(procPool, rootRank);
			boundPartials_.at(typeI, typeJ).broadcast(procPool, rootRank);
			unboundPartials_.at(typeI, typeJ).broadcast(procPool, rootRank);
			braggPartials_.at(typeI, typeJ).broadcast(procPool, rootRank);
		}
	}
	total_.broadcast(procPool, rootRank);
	if (!procPool.broadcast(objectNamePrefix_)) return false;
#endif
	return true;
}

// Check item equality
bool PartialSet::equality(ProcessPool& procPool)
{
#ifdef PARALLEL
	int nTypes = atomTypes_.nItems();
	for (int typeI=0; typeI<nTypes; ++typeI)
	{
		for (int typeJ=typeI; typeJ<nTypes; ++typeJ)
		{
			if (!partials_.at(typeI, typeJ).equality(procPool)) return Messenger::error("PartialSet full partial %i-%i is not equivalent.\n", typeI, typeJ);
			if (!boundPartials_.at(typeI, typeJ).equality(procPool)) return Messenger::error("PartialSet bound partial %i-%i is not equivalent.\n", typeI, typeJ);
			if (!unboundPartials_.at(typeI, typeJ).equality(procPool)) return Messenger::error("PartialSet unbound partial %i-%i is not equivalent.\n", typeI, typeJ);
			if (!braggPartials_.at(typeI, typeJ).equality(procPool)) return Messenger::error("PartialSet Bragg partial %i-%i is not equivalent.\n", typeI, typeJ);
		}
	}
	if (!total_.equality(procPool)) return Messenger::error("PartialSet total sum is not equivalent.\n");
#endif
	return true;
}
