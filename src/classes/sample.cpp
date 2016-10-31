/*
	*** Sample Definition
	*** src/classes/sample.cpp
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

#include "classes/sample.h"
#include "classes/species.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include <string.h>

// Constructor
Sample::Sample() : ListItem<Sample>()
{
	// Reference F(Q)
	hasReferenceData_ = false;
	referenceDataSubtractSelf_ = false;
	referenceFitMax_ = -1.0;
	referenceFitMin_ = -1.0;
}

// Copy Constructor
Sample::Sample(const Sample& source)
{
	(*this) = source;
}

// Assignment Operator
void Sample::operator=(const Sample& source)
{
	// Basic Information
	name_ = source.name_;
	type_ = source.type_;

	// Isotopologue Mix
	isotopologueMixtures_ = source.isotopologueMixtures_;

	// RDF / S(Q) Data
	typeIndex_ = source.typeIndex_;
	boundCoherentAverageSquared_ = source.boundCoherentAverageSquared_;
	boundCoherentSquaredAverage_ = source.boundCoherentSquaredAverage_;
	pairSQMatrix_ = source.pairSQMatrix_;
	braggSQMatrix_ = source.braggSQMatrix_;
	weightsMatrix_ = source.weightsMatrix_;
	scatteringMatrix_ = source.scatteringMatrix_;
	totalGR_ = source.totalGR_;
	totalFQ_ = source.totalFQ_;

	// Reference Data
	hasReferenceData_ = source.hasReferenceData_;
	referenceDataFileName_ = source.referenceDataFileName_;
	referenceData_ = source.referenceData_;
	referenceDataSubtractSelf_ = source.referenceDataSubtractSelf_;
	referenceDataFT_ = source.referenceDataFT_;
	referenceFitMin_ = source.referenceFitMin_;
	referenceFitMax_ = source.referenceFitMax_;
	differenceData_ = source.differenceData_;
}

// Sample Type Keywords
const char* SampleTypeKeywords[] = { "NeutronFQ" };

// Convert text string to InputBlock
Sample::SampleType Sample::sampleType(const char* s)
{
	for (int n=0; n<Sample::nSampleTypes; ++n) if (strcmp(s,SampleTypeKeywords[n]) == 0) return (Sample::SampleType) n;
	return Sample::nSampleTypes;
}

// Convert InputBlock to text string
const char* Sample::sampleType(SampleType st)
{
	return SampleTypeKeywords[st];
}

/*
 * Basic Information
 */

// Set name of Sample
void Sample::setName(const char* name)
{
	 name_ = name;
}

// Return name of Sample
const char* Sample::name() const
{
	return name_.get();
}

// Set data type
void Sample::setType(Sample::SampleType type)
{
	type_ = type;
}

// Return data type
Sample::SampleType Sample::type()
{
	return type_;
}

/*
 * Species/Isotopologue Definition
 */

// Return whether the Sample contains a mixtures definition for the provided Species
IsotopologueMix* Sample::hasSpeciesIsotopologueMixture(Species* sp) const
{
	/*
	 * This function reconstructs the current RefList of Species/Isotopologue pairs and ensures that
	 * it contains only valid Species and Isotopologue pointers.
	 */
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next) if (mix->species() == sp) return mix;
	return NULL;
}

// Update IsotopologueMix List
void Sample::updateIsotopologueMixtures(const List<Species>& species)
{
	/*
	 * This function reconstructs the current List of IsotopologueMix items and ensures that
	 * it contains all valid component Species and Isotopologue pointers.
	 */

	// Construct a temporary RefList, and move all existing RefListItems to it
	List<IsotopologueMix> oldItems;
	IsotopologueMix* mix;
	while (isotopologueMixtures_.last() != NULL)
	{
		mix = isotopologueMixtures_.last();
		isotopologueMixtures_.cut(mix);
		oldItems.own(mix);
	}
	
	// Loop over Species in System, and search for an associated IsotopologueMix in the oldItems list
	for (Species* sp = species.first(); sp != NULL; sp = sp->next)
	{
		for (mix = oldItems.first(); mix != NULL; mix = mix->next) if (mix->species() == sp) break;

		// If we found the existing item, append it to the local list.
		// Otherwise, create a new one with the default (first) Isotopologue.
		if (mix != NULL)
		{
			// Update IsotopologueMix, and check that it still contains something...
			// If there are no isotopologues left in the mix, try to add one
			mix->update();
			if (mix->nIsotopologues() == 0) mix->addNextIsotopologue();
			
			// If we get here, its still valid, so store it
			oldItems.cut(mix);
			isotopologueMixtures_.own(mix);
		}
		else
		{
			mix = isotopologueMixtures_.add();
			mix->setSpecies(sp);
		}
	}
}

// Add Isotopologue for Species
bool Sample::addIsotopologueToMixture(Species* sp, Isotopologue *iso, double relPop)
{
	// Check that the Species is in the list...
	IsotopologueMix* mix = hasSpeciesIsotopologueMixture(sp);
	if (mix == NULL)
	{
		Messenger::print("Warning: Sample '%s' contains no IsotopologueMix definition for Species '%s'.\n", name_.get(), sp->name());
		return false;
	}

	// Check current number of Isotopologues defined against total available
	if (sp->nIsotopologues() == mix->nIsotopologues())
	{
		Messenger::print("Can't add a new Isotopologue definition for Species '%s' in Sample '%s' since there are no unused Isotopologue definitions left.\n", sp->name(), name_.get());
		return false;
	}

	// Was a specific Isotopologue provided?
	if (iso == NULL)
	{
		// Add next available Isotopologue to mixture
		if (!mix->addNextIsotopologue()) return false;
	}
	else if (!mix->addIsotopologue(iso, relPop))
	{
		Messenger::error("Failed to add Isotopologue to Sample '%s'.\n", name_.get());
		return false;
	}
	
	return true;
}

// Return first IsotopologueMix
IsotopologueMix *Sample::isotopologueMixtures() const
{
	return isotopologueMixtures_.first();
}

// Return nth IsotopologueMix
IsotopologueMix *Sample::isotopologueMixture(int n)
{
	return isotopologueMixtures_[n];
}

// Assign default (first) Isotopologues for all Species
void Sample::assignDefaultIsotopes()
{
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next)
	{
		if (mix->nIsotopologues() != 0) continue;
		if (mix->species()->nIsotopologues() == 0) continue;
		mix->addNextIsotopologue();
	}
}

/*
 * RDF / S(Q) Data
 */

// Create type index and RDF / S(Q) storage
bool Sample::createTypeIndex(const RefList<Species,double>& allSpecies, int multiplier, int nExpectedAtoms, const AtomTypeIndex& masterIndex)
{
	// Loop over Samples and go through Isotopologues in each mixture
	typeIndex_.clear();
	int count, molCount;
	Isotope* iso;
	AtomTypeData* at1;
	double bb, cc;

	Messenger::print("--> Generating AtomType/Isotope index...\n");
	// Simultaneous loop over defined Species and IsotopologueMixtures (which are directly related)
	RefListItem<Species,double>* refSp = usedSpecies.first();
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next, refSp = refSp->next)
	{
		// Sanity check
		if (mix->species() != refSp->item)
		{
			Messenger::error("Species referred to in mixture in Sample '%s' does not correspond to that in the main Species list.\n", mix->species()->name());
			return false;
		}

		// Determine the *total* number of molecules of each component
		molCount = refSp->data*multiplier;
		
		double totalRelative = mix->totalRelative();

		// We must now loop over the Isotopologues in the mixture, bearing in mind molCount...
		for (RefListItem<Isotopologue,double>* tope = mix->isotopologues(); tope != NULL; tope = tope->next)
		{
			// Calculate the number of molecules
			 count = refSp->data * (tope->data / totalRelative) * multiplier;

			// Check for zero count
			if (count == 0)
			{
				Messenger::error("Relative population for Isotopologue '%s' of Species '%s' in Sample '%s' is too low (%e) to provide any Molecules.\n", tope->item->name(), refSp->item->name(), name(), tope->data);
				return false;
			}

			// Check count against the remaining molCount
			// -- Too many?
			if (count > molCount)
			{
				Messenger::print("Warning: Molecule count (%i) for Species '%s', Isotopologue '%s' exceeds remaining number to add (%i).\n", count, mix->species()->name(), tope->item->name(), molCount);
				if ((count - molCount) == 1)
				{
					count = molCount;
					Messenger::print("Molecule count reduced to %i.\n", count);
				}
				else
				{
					Messenger::error("Count exceeds remaining number by too many molecules.\n");
					return false;
				}
			}
			else if ((tope->next == NULL) && (molCount > count))
			{
				// Too few, and last Isotopologue in list
				Messenger::print("Warning: Molecule count (%i) for Species '%s', Isotopologue '%s' is less than the remaining number to add (%i).\n", count, mix->species()->name(), tope->item->name(), molCount);
				if ((molCount - count) == 1)
				{
					count = molCount;
					Messenger::print("Molecule count increased to %i.\n", count);
				}
				else
				{
					Messenger::error("Count is less than remaining number by too many molecules.\n");
					return false;
				}
			}

			// Loop over Atoms in the Species, searching for the AtomType/Isotope entry in the isotopes list of the Isotopologue
			for (SpeciesAtom* i = refSp->item->atoms(); i != NULL; i = i->next)
			{
				iso = tope->item->atomTypeIsotope(i->atomType());
				typeIndex_.add(i->atomType(), iso, count);
			}
			
			molCount -= count;
		}
	}

	// Calculate fractional populations
	typeIndex_.finalise();

	// Check that the correct number of atoms were counted into the typeIndex_
	if (typeIndex_.totalPopulation() != nExpectedAtoms)
	{
		printf("INTERNAL_ERROR - Total AtomType usage (%i) does not correspond to number of Atoms in the system (%i).\n", typeIndex_.totalPopulation(), nExpectedAtoms);
		return false;
	}

	// Set master type indices
	for (at1 = typeIndex_.first(); at1 != NULL; at1 = at1->next)
	{
		// Find entry in the master index which contains the AtomType of 'at1'
		int id = masterIndex.indexOf(at1->atomType());
		if (id < 0)
		{
			Messenger::print("INTERNAL_ERROR - Couldn't find entry for first AtomType '%s' in masterIndex.\n", at1->name());
			return false;
		}
		at1->setMasterIndex(id);
	}

	// Print AtomType populations (including isotopes)
	typeIndex_.print();

	// Calculate average scattering lengths for sample
	boundCoherentAverageSquared_ = 0.0;
	boundCoherentSquaredAverage_ = 0.0;
	for (at1 = typeIndex_.first(); at1 != NULL; at1 = at1->next)
	{
		// Note: Divisor of 10.0 in calculation of bb converts from units of fm (1e-11 m) to barn (1e-12 m)
		cc = at1->fraction();
		bb = at1->isotope()->boundCoherent() * 0.1;
		boundCoherentAverageSquared_ += cc*bb;
		boundCoherentSquaredAverage_ += cc*bb*bb;
	}
	boundCoherentAverageSquared_ *= boundCoherentAverageSquared_;
	
	Messenger::print("--> Calculated average scattering lengths: <b>**2 = %f, <b**2> = %f\n", boundCoherentAverageSquared_, boundCoherentSquaredAverage_);

	return true;
}

// Return < b >**2
double Sample::boundCoherentAverageSquared()
{
	return boundCoherentAverageSquared_;
}

// Return < b**2 >
double Sample::boundCoherentSquaredAverage()
{
	return boundCoherentSquaredAverage_;
}

// Setup pair correlations
bool Sample::setupPairCorrelations(double volume, double range, double binWidth, Data2D& boxNormalisation, double rho)
{
	// Get a nice sample name (i.e. no spaces, slashes etc.)
	Dnchar otherName, niceSampleName = name_;
	niceSampleName.replace(' ', '_');
	niceSampleName.replace('/', '_');
	double cc, bb;

	// Construct S(Q) and weights matrices based on the typeIndex_ population
	int typeI, typeJ;
	Messenger::print("--> Creating S(Q) matrices (%ix%i)...\n", typeIndex_.nItems(), typeIndex_.nItems());
	pairSQMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
	braggSQMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
	weightsMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
	scatteringMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);

	// Set names of elements in the S(Q) arrays, and calculate weights and self-scattering values
	Dnchar title;
	AtomTypeData* at1 = typeIndex_.first(), *at2;
	for (typeI=0; typeI<typeIndex_.nItems(); ++typeI, at1 = at1->next)
	{
		at2 = at1;
		for (typeJ=typeI; typeJ<typeIndex_.nItems(); ++typeJ, at2 = at2->next)
		{
			// Partial S(Q) and Bragg S(Q)
			title.sprintf("%s[%i]-%s[%i] (%s)", at1->name(), at1->isotope()->A(), at2->name(), at2->isotope()->A(), niceSampleName.get());
			pairSQMatrix_.ref(typeI,typeJ).setName(title.get());
			braggSQMatrix_.ref(typeI,typeJ).setName(title.get());
			
			// Store weighting factor for this partial
			// Note: Divisor of 100.0 in calculation of bb converts from units of fm (1e-11 m) to barn (1e-12 m) squared
			cc = at1->fraction() * at2->fraction();
			bb = at1->isotope()->boundCoherent() * at2->isotope()->boundCoherent() * 0.01;
			weightsMatrix_.ref(typeI,typeJ) = cc * bb;
			scatteringMatrix_.ref(typeI,typeJ) = bb;
		}
	}

	// Total g(r)
	int nBins = range / binWidth;
	totalGR_.initialise(nBins);
	for (int n=0; n<nBins; ++n) totalGR_.setX(n, (n+0.5)*binWidth);
	totalGR_.setName(niceSampleName.get());

	// Total, reference and difference data (set names only)
	totalFQ_.setName(niceSampleName.get());
	otherName.sprintf("%s (Ref)", niceSampleName.get());
	referenceData_.setName(otherName.get());
	otherName.sprintf("%s (Diff)", niceSampleName.get());
	differenceData_.setName(otherName.get());
	
	// Perform FT of reference data (if present)
	if (referenceData_.nPoints() > 0)
	{
		referenceDataFT_ = referenceData_;
// 		referenceDataFT_.rebin();
		referenceDataFT_.transformLorch(rho, 0.025, 50.0, 0.5, 0.08, true);
	}

	return true;
}

// Calculate weighted pair correlations from supplied unweighted data
bool Sample::calculatePairCorrelations(Array2D<Histogram>& masterRDFs, Array2D<Data2D>& masterPairSQ, Array2D<Data2D>& masterBraggSQ, Array2D<Data2D>& masterPartialSQ)
{
	AtomTypeData* at1 = typeIndex_.first(), *at2;
	double factor, sumFactor = 0.0, braggMax;
	int typeI, typeJ, masterI, masterJ;
	
	Messenger::print("--> Calculating RDFs/S(Q)/F(Q) for Sample '%s'...\n", name_.get());
	
	// Setup totalGR_ and totalFQ_ arrays...
	totalGR_.arrayY() = 0.0;
	if (totalFQ_.nPoints() == 0)
	{
		totalFQ_.arrayX() = masterPairSQ.ref(0,0).arrayX();
		totalFQ_.arrayY() = masterPairSQ.ref(0,0).arrayY();
	}
	totalFQ_.arrayY() = 0.0;
	
	// Loop over atom types and sum data
	for (typeI=0; typeI<typeIndex_.nItems(); ++typeI, at1 = at1->next)
	{
		// Grab master index of AtomType at1
		masterI = at1->masterIndex();
#ifdef CHECKS
		if (masterI == -1)
		{
			Messenger::print("INTERNAL_ERROR - Master index for AtomType '%s' has not been set.\n", at1->name());
			return false;
		}
#endif

		at2 = at1;
		for (typeJ=typeI; typeJ<typeIndex_.nItems(); ++typeJ, at2 = at2->next)
		{
			// Grab master index of AtomType at2 and references to partial and bragg S(Q) matrix items
			masterJ = at2->masterIndex();
			Data2D& pairSQ = pairSQMatrix_.ref(typeI, typeJ);
			Data2D& braggSQ = braggSQMatrix_.ref(typeI, typeJ);
			Data2D& partialSQ = partialSQMatrix().ref(typeI, typeJ);
#ifdef CHECKS
			if (masterJ == -1)
			{
				Messenger::print("INTERNAL_ERROR - Master index for AtomType '%s' has not been set.\n", at2->name());
				return false;
			}
#endif
			// Copy unweighted S(Q) and Bragg data
			pairSQ  = masterPairSQ.ref(masterI, masterJ);
			braggSQ = masterBraggSQ.ref(masterI, masterJ);
			partialSQ = masterPartialSQ.ref(masterI, masterJ);

			// Weight S(Q), Bragg S(Q) and full partial S(Q)
			factor = weightsMatrix_.ref(typeI,typeJ);
			pairSQ.arrayY() *= factor;
			braggSQ.arrayY() *= factor;
			partialSQ.arrayY() *= factor;

			// Sum G(r) into totals
			totalGR_.addY(masterRDFs.ref(masterI,masterJ).normalisedData().arrayY(), factor);
			sumFactor += factor;

			// Sum partialSQ into totalFQ_, weighting dissimilar partials accordingly
			totalFQ_.addY(partialSQ.arrayY(), typeI == typeJ ? 1.0 : 2.0);
		}
	}

	// Finally, interpolate F(Q)
	totalFQ_.interpolate();

	// Calculate difference F(Q)
	differenceData_.clear();
	if (hasReferenceData_)
	{
		double Q;
		for (int n=0; n<referenceData_.nPoints(); ++n)
		{
			Q = referenceData_.x(n);
			if (Q > totalFQ_.arrayX().last()) break;
			differenceData_.addPoint(Q, totalFQ_.interpolated(Q) - referenceData_.y(n));
		}
	}

	return true;
}

// Return reference to array of partials
Array2D<Data2D>& Sample::partialSQMatrix()
{
	return pairSQMatrix_;
}

// Return reference to weights matrix
Array2D<double>& Sample::weightsMatrix()
{
	return weightsMatrix_;
}

// Return total F(Q)
Data2D& Sample::totalFQ()
{
	return totalFQ_;
}

// Return total G(R)
Data2D& Sample::totalGR()
{
	return totalGR_;
}

// Save all neutron-weighted RDFs
void Sample::saveRDFs(const char* baseName)
{
	// Get a nice sample name (i.e. no spaces, slashes etc.)
	Dnchar niceSampleName = name_;
	niceSampleName.replace(' ', '_');
	niceSampleName.replace('/', '_');

	Dnchar fileName(-1, "%s-%s-total.rdf", baseName, niceSampleName.get());
	totalGR_.save(fileName);

	// If we have an FT of the reference data, save that as well
	if (referenceDataFT_.arrayX().nItems() > 0)
	{
		fileName.sprintf("%s-%s-reference.rdf", baseName, niceSampleName.get());
		referenceDataFT_.save(fileName);
	}
}

// Save all partial S(Q)
void Sample::saveSQ(const char* baseName)
{
	// Get a nice sample name (i.e. no spaces, slashes etc.)
	Dnchar fileName, niceSampleName = name_;
	niceSampleName.replace(' ', '_');
	niceSampleName.replace('/', '_');

	for (int n=0; n<typeIndex_.nItems(); ++n)
	{
		for (int m=0; m<typeIndex_.nItems(); ++m)
		{
			fileName.sprintf("%s-%s-%s%i-%s%i.sq", baseName, niceSampleName.get(), typeIndex_[n]->name(), typeIndex_[n]->isotope()->A(), typeIndex_[m]->name(), typeIndex_[m]->isotope()->A());
			pairSQMatrix_.ref(n,m).save(fileName);
		}
	}

	fileName.sprintf("%s-%s-total.fq", baseName, niceSampleName.get());
	totalFQ_.save(fileName);
	
	// If we have reference data, save that as well, along with its interpolated values
	if (referenceData_.arrayX().nItems() > 0)
	{
		fileName.sprintf("%s-%s-reference.fq", baseName, niceSampleName.get());
		referenceData_.saveWithInterpolation(fileName);
	}
}

/*
 * Reference Data
 */

// Return whether reference data exists
bool Sample::hasReferenceData()
{
	return hasReferenceData_;
}

// Load reference F(Q) data
bool Sample::loadReferenceData(const char* fileName)
{
	// Check that the specified file actually exists...
	if (!DUQSys::fileExists(fileName))
	{
		Messenger::error("Sample reference data '%s' not found.\n", fileName);
		return false;
	}

	referenceDataFileName_ = fileName;
	referenceData_.clear();

	// Open file first...
	LineParser parser;
	parser.openInput(fileName);
	if (!parser.isFileGoodForReading())
	{
		Messenger::error("Couldn't open datafile '%s'.\n", fileName);
		return false;
	}

	Messenger::print("--> Loading reference F(Q) data from '%s'...\n", referenceDataFileName_.get());

	// Read data, assuming that column 0 = Q and column 1 = F(Q)...
	bool result = true;
	while (!parser.eofOrBlank())
	{
		if (parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments) == 1)
		{
			Messenger::error("File error while reading '%s'.\n", referenceDataFileName_.get());
			result = false;
			break;
		}
		referenceData_.addPoint(parser.argd(0), parser.argd(1));
	}

	// Tidy up
	parser.closeFiles();
	hasReferenceData_ = result;

	return result;
}

// Return reference data filename (if any)
Dnchar& Sample::referenceDataFileName()
{
	return referenceDataFileName_;
}

// Return reference data
Data2D& Sample::referenceData()
{
	return referenceData_;
}

// Return calculated data
Data2D& Sample::calculatedData()
{
	return calculatedData_;
}


// Calculate RMSE error between corrected S(Q)s and reference data
double Sample::referenceRMSE(double deltaQ)
{
	// Do we have actual reference data to compare?
	if (!hasReferenceData_) return 0.0;

	AtomTypeData* at1, *at2;
	double deltafq, testFQ, rmse = 0.0, q;
	int typeI, typeJ, masterI, masterJ, count = 0;

	q = 0.0;
	while (q <= referenceFitMax_)
	{
		// Check that current Q value is within fit limits for this sample
		if (q < referenceFitMin_)
		{
			q += deltaQ;
			continue;
		}

		// Increment RMSE
		deltafq = referenceData_.interpolated(q) - totalFQ_.interpolated(q);
		rmse += deltafq*deltafq;
		
		// Increment count and q
		++count;
		q += deltaQ;
	}

	return sqrt(rmse/count);
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool Sample::broadcast(ProcessPool& procPool, const List<Species>& species)
{
#ifdef PARALLEL
	int index, topeCount;
	double relPop;
	IsotopologueMix* iso;

	// Send name
	if (!procPool.broadcast(name_)) return false;

	// Mixture information
	// Component/Species RefList will have already been constructed in DUQ::addSample(), so just update constituent Isotopologue
	for (iso = isotopologueMixtures_.first(); iso != NULL; iso = iso->next)
	{
		// Master needs to determine Species index
		if (procPool.isMaster()) index = species.indexOf(iso->species());
		if (!procPool.broadcast(&index, 1)) return false;
		iso->setSpecies(species.item(index));

		// Now sent number of isotopes in mixture
		topeCount = iso->nIsotopologues();
		if (!procPool.broadcast(&topeCount, 1)) return false;
		
		if (procPool.isMaster()) for (RefListItem<Isotopologue,double>* ri = iso->isotopologues(); ri != NULL; ri = ri->next)
		{
			// Get Isotopologue index from Species
			index = iso->species()->indexOfIsotopologue(ri->item);
			if (!procPool.broadcast(&index, 1)) return false;
			// Send relative population
			relPop = ri->data;
			if (!procPool.broadcast(&relPop, 1)) return false;
		}
		else
		{
			for (int m = 0; m<topeCount; ++m)
			{
				// Receive Isotopologue index in associated Species
				if (!procPool.broadcast(&index, 1)) return false;
				// Receive relative population
				if (!procPool.broadcast(&relPop, 1)) return false;
				// Add new mixture component
				iso->addIsotopologue(iso->species()->isotopologue(index), relPop);
			}
		}
	}

	// Reference data
	if (!procPool.broadcast(&hasReferenceData_, 1)) return false;
	if (!referenceData_.broadcast(procPool)) return false;
	if (!procPool.broadcast(referenceDataFileName_)) return false;
	if (!procPool.broadcast(&referenceDataSubtractSelf_, 1)) return false;
	if (!procPool.broadcast(&referenceFitMin_, 1)) return false;
	if (!procPool.broadcast(&referenceFitMax_, 1)) return false;
#endif
	return true;
}
