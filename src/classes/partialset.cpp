/*
	*** Set of Partials
	*** src/classes/partialset.cpp
	Copyright T. Youngs 2012-2019

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
#include "genericitems/array2dbool.h"

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
	emptyBoundPartials_.clear();
	unboundPartials_.clear();
	braggPartials_.clear();
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
	emptyBoundPartials_.initialise(nTypes, nTypes, true);

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
			fullHistograms_.at(n,m).zeroBins();
			boundHistograms_.at(n,m).zeroBins();
			unboundHistograms_.at(n,m).zeroBins();

			partials_.at(n,m).values() = 0.0;
			boundPartials_.at(n,m).values() = 0.0;
			unboundPartials_.at(n,m).values() = 0.0;
			braggPartials_.at(n,m).values() = 0.0;
			emptyBoundPartials_.at(n,m) = true;
		}
	}
	total_.values() = 0.0;

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
Data1D& PartialSet::partial(int i, int j)
{
	return partials_.at(i, j);
}

// Return full atom-atom partial specified (const)
Data1D& PartialSet::constPartial(int i, int j) const
{
	return partials_.constAt(i, j);
}

// Return atom-atom partial for unbound pairs
Data1D& PartialSet::unboundPartial(int i, int j)
{
	return unboundPartials_.at(i, j);
}

// Return atom-atom partial for unbound pairs (const)
Data1D& PartialSet::constUnboundPartial(int i, int j) const
{
	return unboundPartials_.constAt(i, j);
}

// Return atom-atom partial for bound pairs
Data1D& PartialSet::boundPartial(int i, int j)
{
	return boundPartials_.at(i, j);
}

// Return atom-atom partial for bound pairs (const)
Data1D& PartialSet::constBoundPartial(int i, int j) const
{
	return boundPartials_.constAt(i, j);
}

// Return atom-atom Bragg partial
Data1D& PartialSet::braggPartial(int i, int j)
{
	return braggPartials_.at(i, j);
}

// Return atom-atom Bragg partial (const)
Data1D& PartialSet::constBraggPartial(int i, int j) const
{
	return braggPartials_.constAt(i, j);
}

// Return whether specified bound partial is empty
bool PartialSet::isBoundPartialEmpty(int i, int j) const
{
	return emptyBoundPartials_.constAt(i, j);
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
	total_.initialise(partials_.at(0,0));
	total_.values() = 0.0;

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
			total_.values() += partials_.at(typeI,typeJ).values() * factor;
			// TODO Does not include contributions from Bragg partials
		}
	}
}

// Return total function
Data1D& PartialSet::total()
{
	return total_;
}

// Return copy of total function
Data1D PartialSet::constTotal() const
{
	return total_;
}

// Calculate and return total bound function
Data1D PartialSet::boundTotal(bool applyConcentrationWeights) const
{
	const int nTypes = atomTypes_.nItems();
	if (nTypes == 0) return Data1D();

	Data1D bound;
	bound.initialise(boundPartials_.constAt(0,0));

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
			bound.values() += boundPartials_.constAt(typeI,typeJ).values() * factor;
		}
	}

	return bound;
}

// Calculate and return total unbound function
Data1D PartialSet::unboundTotal(bool applyConcentrationWeights) const
{
	const int nTypes = atomTypes_.nItems();
	if (nTypes == 0) return Data1D();

	Data1D unbound;
	unbound.initialise(boundPartials_.constAt(0,0));

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
			unbound.values() += unboundPartials_.constAt(typeI,typeJ).values() * factor;
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
			
			Data1D& full = partials_.at(typeI,typeJ);
			Data1D& bound = boundPartials_.at(typeI,typeJ);
			Data1D& unbound = unboundPartials_.at(typeI,typeJ);
			Data1D& bragg = braggPartials_.at(typeI,typeJ);
			parser.writeLineF("# %-14s  %-16s  %-16s  %-16s  %-16s\n", abscissaUnits_.get(), "Full", "Bound", "Unbound", "Bragg"); 
			for (n=0; n<full.nValues(); ++n) parser.writeLineF("%16.9e  %16.9e  %16.9e  %16.9e  %16.9e\n", full.constXAxis(n), full.constValue(n), bound.constValue(n), unbound.constValue(n), bragg.constValue(n));
			parser.closeFiles();
		}
	}

	Messenger::printVerbose("Writing total file '%s'...\n", total_.name());
	return total_.save(total_.name());
}

// Name all object based on the supplied prefix
void PartialSet::setObjectTags(const char* prefix, const char* suffix)
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
			partials_.at(typeI,typeJ).setObjectTag(CharString("%s//%s-%s//Full%s", prefix, at1->atomTypeName(), at2->atomTypeName(), actualSuffix.get()));
			boundPartials_.at(typeI,typeJ).setObjectTag(CharString("%s//%s-%s//Bound%s", prefix, at1->atomTypeName(), at2->atomTypeName(), actualSuffix.get()));
			unboundPartials_.at(typeI,typeJ).setObjectTag(CharString("%s//%s-%s//Unbound%s", prefix, at1->atomTypeName(), at2->atomTypeName(), actualSuffix.get()));
			braggPartials_.at(typeI,typeJ).setObjectTag(CharString("%s//%s-%s//Bragg%s", prefix, at1->atomTypeName(), at2->atomTypeName(), actualSuffix.get()));
		}
	}

	total_.setObjectTag(CharString("%s//Total%s", prefix, actualSuffix.get()));
}

// Return prefix applied to object names
const char* PartialSet::objectNamePrefix() const
{
	return objectNamePrefix_.get();
}

// Set underlying Data1D file names
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
			partials_.at(n, m).values() += delta;
			boundPartials_.at(n, m).values() += delta;
			unboundPartials_.at(n, m).values() += delta;
		}
	}

	total_.values() += delta;
}

// Form partials from stored Histogram data
void PartialSet::formPartials(double boxVolume, Interpolator& boxNormalisation)
{
	int n, m;
	int nTypes = atomTypes_.nItems();

	AtomTypeData* at1 = atomTypes_.first(), *at2;
	for (n=0; n<nTypes; ++n, at1 = at1->next)
	{
		at2 = at1;
		for (m=n; m<nTypes; ++m, at2 = at2->next)
		{
			// Calculate RDFs from histogram data
			calculateRDF(partials_.at(n, m), fullHistograms_.at(n, m), boxVolume, at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation);
			calculateRDF(boundPartials_.at(n, m), boundHistograms_.at(n, m), boxVolume, at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation);
			calculateRDF(unboundPartials_.at(n, m), unboundHistograms_.at(n, m), boxVolume, at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation);

			// Set flags for bound partials specifying if they are empty (i.e. there are no contributions of that type)
			emptyBoundPartials_.at(n, m) = boundHistograms_.at(n, m).nBinned() == 0;
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
			Interpolator::addInterpolated(partials_.at(localI, localJ), source.partial(typeI, typeJ), weighting);
			Interpolator::addInterpolated(boundPartials_.at(localI, localJ), source.boundPartial(typeI, typeJ), weighting);
			Interpolator::addInterpolated(unboundPartials_.at(localI, localJ), source.unboundPartial(typeI, typeJ), weighting);
			Interpolator::addInterpolated(braggPartials_.at(localI, localJ), source.braggPartial(typeI, typeJ), weighting);

			// If the source data bound partial is *not* empty, ensure that our emptyBoundPartials_ flag is set correctly
			if (!source.isBoundPartialEmpty(typeI, typeJ)) emptyBoundPartials_.at(typeI, typeJ) = false;
		}
	}

	// Add total function
	Interpolator::addInterpolated(total_, source.total(), weighting);

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
			partials_.at(n, m).values() *= factor;
			boundPartials_.at(n, m).values() *= factor;
			unboundPartials_.at(n, m).values() *= factor;
			braggPartials_.at(n, m).values() *= factor;
		}
	}

	total_.values() *= factor;
}

// Calculate and return RDF from supplied Histogram and normalisation data
void PartialSet::calculateRDF(Data1D& destination, Histogram1D& histogram, double boxVolume, int nCentres, int nSurrounding, double multiplier, Interpolator& boxNormalisation)
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

// Read data through specified LineParser
bool PartialSet::read(LineParser& parser, const CoreData& coreData)
{
	if (parser.readNextLine(LineParser::Defaults, objectNamePrefix_) != LineParser::Success) return false;
	if (parser.readNextLine(LineParser::Defaults, abscissaUnits_) != LineParser::Success) return false;
	if (parser.readNextLine(LineParser::Defaults, fingerprint_) != LineParser::Success) return false;

	// Read atom types
	atomTypes_.clear();
	if (!atomTypes_.read(parser, coreData)) return false;
	int nTypes = atomTypes_.nItems();

	// Read partials
	partials_.initialise(nTypes, nTypes, true);
	boundPartials_.initialise(nTypes, nTypes, true);
	unboundPartials_.initialise(nTypes, nTypes, true);
	braggPartials_.initialise(nTypes, nTypes, true);
	for (int typeI=0; typeI<nTypes; ++typeI)
	{
		for (int typeJ=typeI; typeJ<nTypes; ++typeJ)
		{
			if (!partials_.at(typeI, typeJ).read(parser, coreData)) return false;
			if (!boundPartials_.at(typeI, typeJ).read(parser, coreData)) return false;
			if (!unboundPartials_.at(typeI, typeJ).read(parser, coreData)) return false;
			if (!braggPartials_.at(typeI, typeJ).read(parser, coreData)) return false;
		}
	}

	// Read total
	if (!total_.read(parser, coreData)) return false;

	// Read empty bound flags
	if (!GenericItemContainer< Array2D<bool> >::read(emptyBoundPartials_, parser)) return false;

	return true;
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

	// Write individual Data1D
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

	// Write total
	if (!total_.write(parser)) return false;

	// Write empty bound flags
	if (!GenericItemContainer< Array2D<bool> >::write(emptyBoundPartials_, parser)) return false;

	return true;
}

/*
 * Parallel Comms
 */

// Broadcast data from root to all other processes
bool PartialSet::broadcast(ProcessPool& procPool, const int root, const CoreData& coreData)
{
#ifdef PARALLEL
	// The structure should have already been setup(), so arrays should be ready to copy
	int nTypes = atomTypes_.nItems();
	for (int typeI=0; typeI<nTypes; ++typeI)
	{
		for (int typeJ=typeI; typeJ<nTypes; ++typeJ)
		{
			if (!partials_.at(typeI, typeJ).broadcast(procPool, root, coreData)) return Messenger::error("Failed to broadcast partials_ array.\n");
			if (!boundPartials_.at(typeI, typeJ).broadcast(procPool, root, coreData))  return Messenger::error("Failed to broadcast boundPartials_ array.\n");
			if (!unboundPartials_.at(typeI, typeJ).broadcast(procPool, root, coreData))  return Messenger::error("Failed to broadcast unboundPartials_ array.\n");
			if (!braggPartials_.at(typeI, typeJ).broadcast(procPool, root, coreData))  return Messenger::error("Failed to broadcast braggPartials_ array.\n");
		}
	}

	if (!total_.broadcast(procPool, root, coreData)) return Messenger::error("Failed to broadcast total_.\n");

	if (!procPool.broadcast(emptyBoundPartials_)) return Messenger::error("Failed to broadcast emptyBoundPartials_ array.\n");
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
	if (!procPool.equality(emptyBoundPartials_)) return Messenger::error("PartialSet emptyBoundPartials array is not equivalent.\n");
#endif
	return true;
}
