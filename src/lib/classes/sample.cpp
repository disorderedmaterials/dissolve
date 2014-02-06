/*
	*** Sample Definition
	*** src/lib/classes/sample.cpp
	Copyright T. Youngs 2012-2014

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
#include "base/comms.h"
#include "base/sysfunc.h"

/*!
 * \brief Constructor
 * \details Constructor for Sample.
 */
Sample::Sample() : ListItem<Sample>()
{
	// Reference F(Q)
	hasReferenceData_ = false;
	referenceDataNormalisation_ = Sample::NoNormalisation;
	referenceDataSubtractSelf_ = false;
	referenceFitQMax_ = -1.0;
	referenceFitQMin_ = -1.0;
}

/*
// Basic Information
*/

/*!
 * \brief Set name of Sample
 */
void Sample::setName(const char* name)
{
	 name_ = name;
}

/*!
 * \brief Return name of Sample
 */
const char *Sample::name() const
{
	return name_.get();
}

/*
// Species/Isotopologue Definition
*/

/*!
 * \brief Return whether the Sample contains a mixtures definition for the provided Species
 * \details This function reconstructs the current RefList of Species/Isotopologue pairs and ensures that
 * it contains only valid Species and Isotopologue pointers.
 */
IsotopologueMix* Sample::hasSpeciesIsotopologueMixture(Species* sp) const
{
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next) if (mix->species() == sp) return mix;
	return NULL;
}

/*!
 * \brief Update IsotopologueMix List
 * \details This function reconstructs the current List of IsotopologueMix items and ensures that
 * it contains all valid component Species and Isotopologue pointers.
 */
void Sample::updateIsotopologueMixtures(const List<Species>& species)
{
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

/*!
 * \brief Add Isotopologue for Species
 */
bool Sample::addIsotopologueToMixture(Species* sp, Isotopologue *iso, double relPop)
{
	// Check that the Species is in the list...
	IsotopologueMix* mix = hasSpeciesIsotopologueMixture(sp);
	if (mix == NULL)
	{
		msg.print("Warning: Sample '%s' contains no IsotopologueMix definition for Species '%s'.\n", name_.get(), sp->name());
		return false;
	}

	// Check current number of Isotopologues defined against total available
	if (sp->nIsotopologues() == mix->nIsotopologues())
	{
		msg.print("Can't add a new Isotopologue definition for Species '%s' in Sample '%s' since there are no unused Isotopologue definitions left.\n", sp->name(), name_.get());
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
		msg.error("Failed to add Isotopologue to Sample '%s'.\n", name_.get());
		return false;
	}
	
	return true;
}

/*!
 * \brief Set Isotopologue data
 */
bool Sample::setIsotopologueInMixture(Species* sp, Isotopologue* iso, double relPop, Isotopologue* newTope)
{
	// Find the requested Species in the list
	IsotopologueMix* mix = hasSpeciesIsotopologueMixture(sp);
	if (!mix)
	{
		msg.error("Species not found in Sample...\n");
		return false;
	}
	
	// Now find Isotopologue in mixture
	RefListItem<Isotopologue,double>* ri = mix->hasIsotopologue(iso);
	if (!ri)
	{
		msg.error("Isotopologue not found in mixture for Species '%s', Sample '%s'...\n", sp->name(), name_.get());
		return false;
	}
	
	// Set new data - was a new relative population given?
	if (relPop > 0.0) ri->data = relPop;
	
	// Was a new Isotopologue given?
	if (newTope != NULL)
	{
		// Must first check that the new Isotopologue isn't already present in the list
		if (mix->hasIsotopologue(newTope))
		{
			msg.error("Can't change Isotopologue '%s' into '%s' since '%s' is already present in the mix.\n", iso->name(), newTope->name(), newTope->name());
			return false;
		}
		ri->item = newTope;
	}
	return true;
}

/*!
 * \brief Return first IsotopologueMix
 */
IsotopologueMix *Sample::isotopologueMixtures() const
{
	return isotopologueMixtures_.first();
}

/*!
 * \brief Return nth IsotopologueMix
 */
IsotopologueMix *Sample::isotopologueMixture(int n)
{
	return isotopologueMixtures_[n];
}

/*!
 * \brief Assign default (first) Isotopologues for all Species
 */
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
// RDF / S(Q) Data
*/

/*!
 * \brief Create type index and RDF / S(Q) storage
 */
bool Sample::createTypeIndex(const List<Species>& species, int multiplier, int nExpectedAtoms, const AtomTypeIndex& masterIndex)
{
	// Loop over Samples and go through Isotopologues in each mixture
	typeIndex_.clear();
	int count, molCount;
	Isotope* iso;
	AtomTypeData* at1;
	double bb, cc;

	msg.print("--> Generating AtomType/Isotope index...\n");
	// Simultaneous loop over defined Species and IsotopologueMixtures (which are directly related)
	Species* sp = species.first();
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next, sp = sp->next)
	{
		// Sanity check
		if (mix->species() != sp)
		{
			msg.error("Species referred to in mixture in Sample '%s' does not correspond to that in the main Species list.\n", mix->species()->name());
			return false;
		}

		// For safety, only the master process will determine the *total* number of molecules of each component
		if (Comm.master()) molCount = sp->relativePopulation()*multiplier;
		if (!Comm.broadcast(&molCount, 1)) return false;
		
		double totalRelative = mix->totalRelative();

		// We must now loop over the Isotopologues in the mixture, bearing in mind molCount...
		for (RefListItem<Isotopologue,double>* tope = mix->isotopologues(); tope != NULL; tope = tope->next)
		{
			// Again, for safety only the master process will calculate the number of molecules
			if (Comm.master()) count = sp->relativePopulation() * (tope->data / totalRelative) * multiplier;
			if (!Comm.broadcast(&count, 1)) return false;
			
			// Check count against the remaining molCount
			// -- Too many?
			if (count > molCount)
			{
				msg.print("Warning: Molecule count (%i) for Species '%s', Isotopologue '%s' exceeds remaining number to add (%i).\n", count, mix->species()->name(), tope->item->name(), molCount);
				if ((count - molCount) == 1)
				{
					count = molCount;
					msg.print("Molecule count reduced to %i.\n", count);
				}
				else
				{
					msg.error("Count exceeds remaining number by too many molecules.\n");
					return false;
				}
			}
			else if ((tope->next == NULL) && (molCount > count))
			{
				// Too few, and last Isotopologue in list
				msg.print("Warning: Molecule count (%i) for Species '%s', Isotopologue '%s' is less than the remaining number to add (%i).\n", count, mix->species()->name(), tope->item->name(), molCount);
				if ((molCount - count) == 1)
				{
					count = molCount;
					msg.print("Molecule count increased to %i.\n", count);
				}
				else
				{
					msg.error("Count is less than remaining number by too many molecules.\n");
					return false;
				}
			}

			// Loop over Atoms in the Species, searching for the AtomType/Isotope entry in the isotopes list of the Isotopologue
			for (SpeciesAtom* i = sp->atoms(); i != NULL; i = i->next)
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
			msg.print("INTERNAL_ERROR - Couldn't find entry for first AtomType '%s' in masterIndex.\n", at1->name());
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
	
	msg.print("--> Calculated average scattering lengths: <b>**2 = %f, <b**2> = %f\n", boundCoherentAverageSquared_, boundCoherentSquaredAverage_);

	return true;
}

/*!
 * \brief Return < b >**2
 */
double Sample::boundCoherentAverageSquared()
{
	return boundCoherentAverageSquared_;
}

/*!
 * \brief Return < b**2 >
 */
double Sample::boundCoherentSquaredAverage()
{
	return boundCoherentSquaredAverage_;
}

/*!
 * \brief Setup pair correlations
 */
bool Sample::setupPairCorrelations(double volume, double range, double binWidth, Data2D& boxNormalisation, double rho)
{
	// Get a nice sample name (i.e. no spaces, slashes etc.)
	Dnchar otherName, niceSampleName = name_;
	niceSampleName.replace(' ', '_');
	niceSampleName.replace('/', '_');
	double cc, bb;

	// Construct S(Q) and weights matrices based on the typeIndex_ population
	int typeI, typeJ;
	msg.print("--> Creating S(Q) matrices (%ix%i)...\n", typeIndex_.nItems(), typeIndex_.nItems());
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
			weightsMatrix_.ref(typeI,typeJ) = (typeI == typeJ ? 1.0 : 2.0) * cc * bb;
			scatteringMatrix_.ref(typeI,typeJ) = bb;
		}
	}

	// Total g(r)
	int nBins = range / binWidth;
	totalGR_.initialise(nBins);
	for (int n=0; n<nBins; ++n) totalGR_.setX(n, (n+0.5)*binWidth);
	totalGR_.setName(niceSampleName.get());

	// Total, reference and difference F(Q) (set names only)
	totalFQ_.setName(niceSampleName.get());
	otherName.sprintf("%s (Ref)", niceSampleName.get());
	referenceFQ_.setName(otherName.get());
	otherName.sprintf("%s (Diff)", niceSampleName.get());
	differenceFQ_.setName(otherName.get());
	
	// Perform FT of reference data (if present)
	if (referenceFQ_.nPoints() > 0)
	{
		referenceDataFT_ = referenceFQ_;
// 		referenceDataFT_.rebin();
		referenceDataFT_.transformLorch(rho, 0.025, 50.0, 0.5, 0.08, true);
	}

	return true;
}

/*!
 * \brief Calculate weighted pair correlations
 * \details Calculate the neutron-weighted pair correlations, including S(Q), F(Q), and total RDF, from the supplied unweighted data.
 */
bool Sample::calculatePairCorrelations(Array2D<Histogram>& masterRDFs, Array2D<Data2D>& masterPairSQ, Array2D<Data2D>& masterBraggSQ, Array2D<Data2D>& masterPartialSQ)
{
	AtomTypeData* at1 = typeIndex_.first(), *at2;
	double factor, sumFactor = 0.0, braggMax;
	int typeI, typeJ, masterI, masterJ;
	
	msg.print("--> Calculating RDFs/S(Q)/F(Q) for Sample '%s'...\n", name_.get());
	
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
			msg.print("INTERNAL_ERROR - Master index for AtomType '%s' has not been set.\n", at1->name());
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
				msg.print("INTERNAL_ERROR - Master index for AtomType '%s' has not been set.\n", at2->name());
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

			// Sum partialSQ into totalFQ_
			totalFQ_.addY(partialSQ.arrayY(), 1.0);
		}
	}

	// Finally, interpolate F(Q)
	totalFQ_.interpolate();

	// Calculate difference F(Q)
	differenceFQ_.clear();
	if (hasReferenceData_)
	{
		double Q;
		for (int n=0; n<referenceFQ_.nPoints(); ++n)
		{
			Q = referenceFQ_.x(n);
			if (Q > totalFQ_.arrayX().last()) break;
			differenceFQ_.addPoint(Q, totalFQ_.interpolated(Q) - referenceFQ_.y(n));
		}
	}

	return true;
}

/*!
 * \brief Return reference to array of partials
 */
Array2D<Data2D>& Sample::partialSQMatrix()
{
	return pairSQMatrix_;
}

/*!
 * \brief Return reference to weights matrix
 */
Array2D<double>& Sample::weightsMatrix()
{
	return weightsMatrix_;
}

/*!
 * \brief Return total F(Q)
 */
Data2D& Sample::totalFQ()
{
	return totalFQ_;
}

/*!
 * \brief Return total G(R)
 */
Data2D& Sample::totalGR()
{
	return totalGR_;
}

/*!
 * \brief Save all neutron-weighted RDFs
 */
void Sample::saveRDFs(const char* baseName)
{
	if (!Comm.master()) return;

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

/*!
 * \brief Save all partial S(Q)
 */
void Sample::saveSQ(const char* baseName)
{
	if (!Comm.master()) return;

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
	if (referenceFQ_.arrayX().nItems() > 0)
	{
		fileName.sprintf("%s-%s-reference.fq", baseName, niceSampleName.get());
		referenceFQ_.saveWithInterpolation(fileName);
	}
}

/*
 * Reference Data
 */

/*!
 * \brief Return whether reference data exists
 */
bool Sample::hasReferenceData()
{
	return hasReferenceData_;
}

/*!
 * \brief Load reference F(Q) data
 */
bool Sample::loadReferenceData(const char* fileName)
{
	// Check that the specified file actually exists...
	if (!fileExists(fileName))
	{
		msg.error("Sample reference data '%s' not found.\n", fileName);
		return false;
	}

	referenceDataFileName_ = fileName;
	referenceFQ_.clear();

	// Open file first...
	LineParser parser;
	parser.openInput(fileName);
	if (!parser.isFileGoodForReading())
	{
		msg.error("Couldn't open datafile '%s'.\n", fileName);
		return false;
	}

	msg.print("--> Loading reference F(Q) data from '%s'...\n", referenceDataFileName_.get());

	// Read data, assuming that column 0 = Q and column 1 = F(Q)...
	bool result = true;
	while (!parser.eofOrBlank())
	{
		if (parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments) == 1)
		{
			msg.error("File error while reading '%s'.\n", referenceDataFileName_.get());
			result = false;
			break;
		}
		referenceFQ_.addPoint(parser.argd(0), parser.argd(1));
	}

	// Tidy up
	parser.closeFiles();
	hasReferenceData_ = result;

	return result;
}

/*!
 * \brief Return reference data filename (if any)
 */
Dnchar& Sample::referenceDataFileName()
{
	return referenceDataFileName_;
}

/*!
 * \brief Return reference data
 */
Data2D& Sample::referenceFQ()
{
	return referenceFQ_;
}

/*!
 * \brief Set reference data normalisation type
 */
void Sample::setReferenceDataNormalisation(NormalisationType norm)
{
	referenceDataNormalisation_ = norm;
}

/*!
 * \brief Return normalisation type for reference data
 */
Sample::NormalisationType Sample::referenceDataNormalisation()
{
	return referenceDataNormalisation_;
}

/*!
 * \brief Set reference data self-scattering subtraction flag
 */
void Sample::setReferenceSubtractSelf(bool b)
{
	referenceDataSubtractSelf_ = true;
}

/*!
 * \brief Return whether reference data should have self-scattering subtracted
 */
bool Sample::referenceSubtractSelf()
{
	return referenceDataSubtractSelf_;
}

/*!
 * \brief Set minimum Q-value for empirical fitting
 */
void Sample::setReferenceFitQMin(double q)
{
	referenceFitQMin_ = q;
}

/*!
 * \brief Return minimum Q-value for empirical fitting
 */
double Sample::referenceFitQMin()
{
	return referenceFitQMin_;
}

/*!
 * \brief Set maximum Q-value for empirical fitting
 */
void Sample::setReferenceFitQMax(double q)
{
	referenceFitQMax_ = q;
}

/*!
 * \brief Return maximum Q-value for empirical fitting
 */
double Sample::referenceFitQMax()
{
	return referenceFitQMax_;
}

/*!
 * \brief Return difference FQ
 */
Data2D& Sample::differenceFQ()
{
	return differenceFQ_;
}

/*!
 * \brief Finalise reference data
 */
bool Sample::finaliseReferenceData()
{
	// Do we actually have some reference data?
	if (referenceFQ_.nPoints() == 0) return true;

	// Set name to be same as Sample (for PlotWidget)
	Dnchar niceSampleName = name_;
	niceSampleName.replace(' ', '_');
	niceSampleName.replace('/', '_');
	referenceFQ_.setName(niceSampleName);
	
	// Subtract self-scattering background, calculated from high-Q region
	if (referenceDataSubtractSelf_)
	{
		msg.print("--> Subtracting self-scattering background from reference data...\n");
		double highQLevel = 0.0;
		// Take last 10% of points to calculate average
		for (int n=referenceFQ_.nPoints()*0.9; n<referenceFQ_.nPoints(); ++n) highQLevel += referenceFQ_.y(n);
		highQLevel /= (referenceFQ_.nPoints()*0.1);
		msg.print("--> High-Q average level is %f.\n", highQLevel);
		referenceFQ_.arrayY() -= highQLevel;
	}

	// Is data normalised?
	if (referenceDataNormalisation_ == Sample::AverageSquaredNormalisation)
	{
		msg.print("--> Removing normalisation (multiplying by <b>**2 = %f).\n", boundCoherentAverageSquared_);
		referenceFQ_.arrayY() *= boundCoherentAverageSquared_;
	}
	else if (referenceDataNormalisation_ == Sample::SquaredAverageNormalisation)
	{
		msg.print("--> Removing normalisation (multiplying by <b**2> = %f).\n", boundCoherentSquaredAverage_);
		referenceFQ_.arrayY() *= boundCoherentSquaredAverage_;
	}

	// Check min/max Q ranges for fit
	if (referenceFitQMin_ < -0.5)
	{
		referenceFitQMin_ = referenceFQ_.arrayX().first();
		msg.print("--> No minimum Q value given for fit - assuming minimum available (Q = %10.4e).\n", referenceFitQMin_);
	}
	if (referenceFitQMax_ < -0.5)
	{
		referenceFitQMax_ = referenceFQ_.arrayX().last();
		msg.print("--> No maximum Q value given for fit - assuming maximum available (Q = %10.4e).\n", referenceFitQMax_);
	}
	msg.print("--> Q range over which to fit empirical potential: %10.4e to %10.4e Angstroms-1.\n", referenceFitQMin_, referenceFitQMax_);

	return true;
}

/*!
 * \brief Calculate RMSE error between corrected S(Q)s and reference data
 */
double Sample::referenceRMSE(double deltaQ)
{
	// Do we have actual reference data to compare?
	if (!hasReferenceData_) return 0.0;

	AtomTypeData* at1, *at2;
	double deltafq, testFQ, rmse = 0.0, q;
	int typeI, typeJ, masterI, masterJ, count = 0;

	q = 0.0;
	while (q <= referenceFitQMax_)
	{
		// Check that current Q value is within fit limits for this sample
		if (q < referenceFitQMin_)
		{
			q += deltaQ;
			continue;
		}

		// Increment RMSE
		deltafq = referenceFQ_.interpolated(q) - totalFQ_.interpolated(q);
		rmse += deltafq*deltafq;
		
		// Increment count and q
		++count;
		q += deltaQ;
	}

	return sqrt(rmse/count);
}

/*
// Parallel Comms
*/

/*!
 * \brief Broadcast data from Master to all Slaves
 */
bool Sample::broadcast(const List<Species>& species)
{
#ifdef PARALLEL
	int index, topeCount;
	double relPop;
	IsotopologueMix* iso;

	// Send name
	if (!Comm.broadcast(name_)) return false;
	
	// Mixture information
	// Component/Species RefList will have already been constructed in DUQ::addSample(), so just update constituent Isotopologue
	
	for (iso = isotopologueMixtures_.first(); iso != NULL; iso = iso->next)
	{
		// Master needs to determine Species index
		if (Comm.master()) index = species.indexOf(iso->species());
		if (!Comm.broadcast(&index, 1)) return false;
		iso->setSpecies(species.item(index));

		// Now sent number of isotopes in mixture
		topeCount = iso->nIsotopologues();
		if (!Comm.broadcast(&topeCount, 1)) return false;
		
		if (Comm.master()) for (RefListItem<Isotopologue,double>* ri = iso->isotopologues(); ri != NULL; ri = ri->next)
		{
			// Get Isotopologue index from Species
			index = iso->species()->indexOfIsotopologue(ri->item);
			if (!Comm.broadcast(&index, 1)) return false;
			// Send relative population
			relPop = ri->data;
			if (!Comm.broadcast(&relPop, 1)) return false;
		}
		else
		{
			for (int m = 0; m<topeCount; ++m)
			{
				// Receive Isotopologue index in associated Species
				if (!Comm.broadcast(&index, 1)) return false;
				// Receive relative population
				if (!Comm.broadcast(&relPop, 1)) return false;
				// Add new mixture component
				iso->addIsotopologue(iso->species()->isotopologue(index), relPop);
			}
		}
	}

	// Reference F(Q) data
	if (!Comm.broadcast(&hasReferenceData_, 1)) return false;
	if (!referenceFQ_.broadcast()) return false;
	if (!Comm.broadcast(referenceDataFileName_)) return false;
	if (!Comm.broadcast((int*)&referenceDataNormalisation_, 1)) return false;
	if (!Comm.broadcast(&referenceDataSubtractSelf_, 1)) return false;
	if (!Comm.broadcast(&referenceFitQMin_, 1)) return false;
	if (!Comm.broadcast(&referenceFitQMax_, 1)) return false;
#endif
	return true;
}
