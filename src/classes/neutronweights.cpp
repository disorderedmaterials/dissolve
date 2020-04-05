/*
	*** Neutron Weights Container
	*** src/classes/neutronweights.cpp
	Copyright T. Youngs 2012-2020

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
#include "base/processpool.h"
#include "classes/atomtype.h"
#include "classes/neutronweights.h"
#include "classes/species.h"
#include "data/isotopes.h"
#include "genericitems/array2ddouble.h"
#include "templates/broadcastlist.h"

NeutronWeights::NeutronWeights()
{
	boundCoherentSquareOfAverage_ = 0.0;
	boundCoherentAverageOfSquares_ = 0.0;
	valid_ = false;
}

NeutronWeights::NeutronWeights(const NeutronWeights &source) { (*this) = source; }

void NeutronWeights::operator=(const NeutronWeights &source)
{
	// Isotopologue Mix
	isotopologueMixtures_ = source.isotopologueMixtures_;
	atomTypes_ = source.atomTypes_;
	boundCoherentProducts_ = source.boundCoherentProducts_;
	concentrationProducts_ = source.concentrationProducts_;
	weights_ = source.weights_;
	intramolecularWeights_ = source.intramolecularWeights_;
	boundCoherentSquareOfAverage_ = source.boundCoherentSquareOfAverage_;
	boundCoherentAverageOfSquares_ = source.boundCoherentAverageOfSquares_;
	valid_ = source.valid_;
}

/*
 * Construction
 */

// Clear contents
void NeutronWeights::clear()
{
	isotopologueMixtures_.clear();
	atomTypes_.clear();
	concentrationProducts_.clear();
	boundCoherentProducts_.clear();
	weights_.clear();
	intramolecularWeights_.clear();
	boundCoherentSquareOfAverage_ = 0.0;
	boundCoherentAverageOfSquares_ = 0.0;
	valid_ = false;
}

// Add Isotopologue for Species
bool NeutronWeights::addIsotopologue(Species *sp, int speciesPopulation, const Isotopologue *iso,
				     double isotopologueRelativePopulation)
{
	// Check that the Species is in the list...
	Isotopologues *mix = hasIsotopologues(sp);
	if (mix == NULL)
	{
		mix = isotopologueMixtures_.add();
		mix->setSpecies(sp, speciesPopulation);
	}

	// Add/update Isotopologue provided?
	if (!mix->add(iso, isotopologueRelativePopulation))
	{
		Messenger::error("Failed to add Isotopologue to IsotopologueSet.\n");
		return false;
	}

	return true;
}

// Return whether the IsotopologueSet contains a mixtures definition for the provided Species
Isotopologues *NeutronWeights::hasIsotopologues(Species *sp) const
{
	for (Isotopologues *topes = isotopologueMixtures_.first(); topes != NULL; topes = topes->next())
		if (topes->species() == sp)
			return topes;
	return NULL;
}

// Print atomtype / weights information
void NeutronWeights::print() const
{
	Messenger::print("  Species          Isotopologue     nTotMols    Fraction\n");
	Messenger::print("  ------------------------------------------------------\n");
	for (Isotopologues *topes = isotopologueMixtures_.first(); topes != NULL; topes = topes->next())
	{
		ListIterator<IsotopologueWeight> weightIterator(topes->mix());
		while (IsotopologueWeight *isoWeight = weightIterator.iterate())
		{
			if (weightIterator.isFirst())
				Messenger::print("  %-15s  %-15s  %-10i  %f\n", topes->species()->name(),
						 isoWeight->isotopologue()->name(), topes->speciesPopulation(),
						 isoWeight->weight());
			else
				Messenger::print("                   %-15s              %f\n",
						 isoWeight->isotopologue()->name(), isoWeight->weight());
		}
	}

	// Print atomtypes table
	Messenger::print("\n");
	atomTypes_.print();

	Messenger::print("\nCalculated average scattering lengths: <b>**2 = %f, <b**2> = %f\n", boundCoherentSquareOfAverage_,
			 boundCoherentAverageOfSquares_);
}

/*
 * Data
 */

// Calculate weighting matrices based on current AtomType / Isotope information
void NeutronWeights::calculateWeightingMatrices()
{
	// Create weights matrices and calculate average scattering lengths
	// Note: Multiplier of 0.1 on b terms converts from units of fm (1e-11 m) to barn (1e-12 m)
	concentrationProducts_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	boundCoherentProducts_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	weights_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	intramolecularWeights_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	boundCoherentAverageOfSquares_ = 0.0;
	boundCoherentSquareOfAverage_ = 0.0;

	double ci, cj, bi, bj;

	// Determine atomic concentration products, bound coherent products, and full scattering weights
	AtomTypeData *atd1 = atomTypes_.first(), *atd2;
	for (int typeI = 0; typeI < atomTypes_.nItems(); ++typeI, atd1 = atd1->next())
	{
		ci = atd1->fraction();
		bi = atd1->boundCoherent() * 0.1;

		// Update average scattering values
		boundCoherentSquareOfAverage_ += ci * bi;
		boundCoherentAverageOfSquares_ += ci * bi * bi;

		atd2 = atd1;
		for (int typeJ = typeI; typeJ < atomTypes_.nItems(); ++typeJ, atd2 = atd2->next())
		{
			cj = atd2->fraction();
			bj = atd2->boundCoherent() * 0.1;

			concentrationProducts_.at(typeI, typeJ) = ci * cj;
			boundCoherentProducts_.at(typeI, typeJ) = bi * bj;
			weights_.at(typeI, typeJ) = ci * cj * bi * bj * (typeI == typeJ ? 1 : 2);
		}
	}

	// Finalise <b>**2
	boundCoherentSquareOfAverage_ *= boundCoherentSquareOfAverage_;

	// Determine bound (intramolecular) scattering weights
	// Loop over defined Isotopologues in our defining mixtures, summing terms from (intramolecular) pairs of Atoms
	intramolecularWeights_ = 0.0;
	Array2D<double> intraNorm(atomTypes_.nItems(), atomTypes_.nItems(), true);
	Array2D<bool> intraFlag(atomTypes_.nItems(), atomTypes_.nItems(), true);
	Array2D<bool> globalFlag(atomTypes_.nItems(), atomTypes_.nItems(), true);
	intraNorm = 0.0;
	globalFlag = false;
	for (Isotopologues *topes = isotopologueMixtures_.first(); topes != NULL; topes = topes->next())
	{
		// Get weighting for associated Species population
		double speciesWeight = double(topes->speciesPopulation());

		// Using the underlying Species, construct a flag matrix which states the AtomType interactions we have present
		Species *sp = topes->species();
		const AtomTypeList &speciesAtomTypes = sp->usedAtomTypes();
		const int nAtoms = sp->nAtoms();
		intraFlag = false;
		for (atd1 = speciesAtomTypes.first(); atd1 != NULL; atd1 = atd1->next())
		{
			// Find this AtomType in our local AtomTypeList
			int typeI = atomTypes_.indexOf(atd1->atomType());
			if (typeI == -1)
				Messenger::error("Failed to find AtomType '%s' in local NeutronWeights.\n",
						 atd1->atomTypeName());

			// Inner loop
			for (atd2 = atd1; atd2 != NULL; atd2 = atd2->next())
			{
				// Get AtomType for this Atom and find it in our local AtomTypeList
				int typeJ = atomTypes_.indexOf(atd2->atomType());
				if (typeJ == -1)
					Messenger::error("Failed to find AtomType '%s' in local NeutronWeights.\n",
							 atd2->atomTypeName());

				intraFlag.at(typeI, typeJ) = true;
			}
		}

		// Loop over Isotopologues defined for this mixture
		ListIterator<IsotopologueWeight> weightIterator(topes->mix());
		while (IsotopologueWeight *isoWeight = weightIterator.iterate())
		{
			// Sum the scattering lengths of each pair of AtomTypes, weighted by the speciesWeight and the
			// fractional Isotopologue weight in the mix.
			double weight = speciesWeight * isoWeight->weight();

			const Isotopologue *tope = isoWeight->isotopologue();

			for (atd1 = speciesAtomTypes.first(); atd1 != NULL; atd1 = atd1->next())
			{
				// Get the local index of this AtomType, as well as its pointer
				int typeI = atomTypes_.indexOf(atd1->atomType());
				AtomTypeData *localI = atomTypes_[typeI];

				// If this AtomType is exchangeable, add the averaged scattering length from the local
				// AtomTypesList instead of its actual isotopic length.
				if (localI->exchangeable())
					bi = localI->boundCoherent();
				else
				{
					// Get the Isotope associated to this AtomType in the current Isotopologue
					Isotope *isotope = tope->atomTypeIsotope(atd1->atomType());
					bi = isotope->boundCoherent();
				}
				bi *= 0.1;

				// Inner loop
				for (atd2 = atd1; atd2 != NULL; atd2 = atd2->next())
				{
					// Get the local index of this AtomType, as well as its pointer
					int typeJ = atomTypes_.indexOf(atd2->atomType());
					AtomTypeData *localJ = atomTypes_[typeJ];

					// Check to see if this interaction is present in the current Species
					if (!intraFlag.at(typeI, typeJ))
						continue;

					// If this AtomType is exchangeable, add the averaged scattering length from the local
					// AtomTypesList instead of its actual isotopic length.
					if (localJ->exchangeable())
						bj = localJ->boundCoherent();
					else
					{
						// Get the Isotope associated to this AtomType in the current Isotopologue
						Isotope *isotope = tope->atomTypeIsotope(atd2->atomType());
						bj = isotope->boundCoherent();
					}
					bj *= 0.1;

					intramolecularWeights_.at(typeI, typeJ) += weight * bi * bj;
					intraNorm.at(typeI, typeJ) += weight;
					globalFlag.at(typeI, typeJ) = true;
				}
			}
		}
	}

	// Normalise the intramolecularWeights_ array, and multiply by atomic concentrations and Kronecker delta
	atd1 = atomTypes_.first();
	for (int typeI = 0; typeI < atomTypes_.nItems(); ++typeI, atd1 = atd1->next())
	{
		ci = atd1->fraction();

		atd2 = atd1;
		for (int typeJ = typeI; typeJ < atomTypes_.nItems(); ++typeJ, atd2 = atd2->next())
		{
			// Skip this pair if there are no such intramolecular interactions
			if (!globalFlag.at(typeI, typeJ))
				continue;

			cj = atd2->fraction();

			intramolecularWeights_.at(typeI, typeJ) /= intraNorm.at(typeI, typeJ);
			intramolecularWeights_.at(typeI, typeJ) *= ci * cj * (typeI == typeJ ? 1 : 2);
		}
	}
}

// Create AtomType list and matrices based on stored Isotopologues information
void NeutronWeights::createFromIsotopologues(const AtomTypeList &exchangeableTypes)
{
	// Loop over Isotopologues entries and ensure relative populations of Isotopologues sum to 1.0
	for (Isotopologues *topes = isotopologueMixtures_.first(); topes != NULL; topes = topes->next())
		topes->normalise();

	// Fill atomTypes_ list with AtomType populations, based on Isotopologues relative populations and associated Species
	// populations
	atomTypes_.clear();
	for (Isotopologues *topes = isotopologueMixtures_.first(); topes != NULL; topes = topes->next())
	{
		// We must now loop over the Isotopologues in the topesture
		ListIterator<IsotopologueWeight> weightIterator(topes->mix());
		while (IsotopologueWeight *isoWeight = weightIterator.iterate())
		{
			const Isotopologue *tope = isoWeight->isotopologue();

			// Loop over Atoms in the Species, searching for the AtomType/Isotope entry in the isotopes list of the
			// Isotopologue
			for (SpeciesAtom *i = topes->species()->firstAtom(); i != NULL; i = i->next())
			{
				Isotope *iso = tope->atomTypeIsotope(i->atomType());
				atomTypes_.addIsotope(i->atomType(), iso, isoWeight->weight() * topes->speciesPopulation());
			}
		}
	}
	atomTypes_.finalise(exchangeableTypes);

	calculateWeightingMatrices();

	valid_ = true;
}

// Reduce data to be naturally-weighted
void NeutronWeights::naturalise()
{
	atomTypes_.naturalise();

	calculateWeightingMatrices();

	valid_ = true;
}

// Return AtomTypeList
AtomTypeList &NeutronWeights::atomTypes() { return atomTypes_; }

// Return number of used AtomTypes
int NeutronWeights::nUsedTypes() const { return atomTypes_.nItems(); }

// Return concentration product for types i and j
double NeutronWeights::concentrationProduct(int i, int j) const { return concentrationProducts_.constAt(i, j); }

// Return bound coherent scattering product for types i
double NeutronWeights::boundCoherentProduct(int i, int j) const { return boundCoherentProducts_.constAt(i, j); }

// Return full weighting for types i and j (ci * cj * bi * bj * [2-dij])
double NeutronWeights::weight(int i, int j) const { return weights_.constAt(i, j); }

// Return full intramolecular weighting for types i and j
double NeutronWeights::intramolecularWeight(int i, int j) const { return intramolecularWeights_.constAt(i, j); }

// Return full weights matrix
Array2D<double> &NeutronWeights::weights() { return weights_; }

// Return full intramolecular scattering weights matrix
Array2D<double> &NeutronWeights::intramolecularWeights() { return intramolecularWeights_; }

// Return bound coherent average squared scattering (<b>**2)
double NeutronWeights::boundCoherentSquareOfAverage() const { return boundCoherentSquareOfAverage_; }

// Return bound coherent squared average scattering (<b**2>)
double NeutronWeights::boundCoherentAverageOfSquares() const { return boundCoherentAverageOfSquares_; }

// Return whether the structure is valid (i.e. has been finalised)
bool NeutronWeights::isValid() const { return valid_; }

/*
 * GenericItemBase Implementations
 */

// Return class name
const char *NeutronWeights::itemClassName() { return "NeutronWeights"; }

// Read data through specified LineParser
bool NeutronWeights::read(LineParser &parser, const CoreData &coreData)
{
	clear();

	// Read AtomTypeList
	if (!atomTypes_.read(parser, coreData))
		return false;

	// Read Isotopologues-tures
	isotopologueMixtures_.clear();
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
		return false;
	int nItems = parser.argi(0);
	for (int n = 0; n < nItems; ++n)
	{
		Isotopologues *mix = isotopologueMixtures_.add();
		if (!mix->read(parser, coreData))
			return false;
	}

	// Read arrays using static methods in the relevant GenericItemContainer
	if (!GenericItemContainer<Array2D<double>>::read(concentrationProducts_, parser))
		return false;
	if (!GenericItemContainer<Array2D<double>>::read(boundCoherentProducts_, parser))
		return false;
	if (!GenericItemContainer<Array2D<double>>::read(weights_, parser))
		return false;
	if (!GenericItemContainer<Array2D<double>>::read(boundWeights_, parser))
		return false;
	if (!GenericItemContainer<Array2D<double>>::read(intramolecularWeights_, parser))
		return false;

	// Read averages
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
		return false;
	boundCoherentAverageOfSquares_ = parser.argd(0);
	boundCoherentSquareOfAverage_ = parser.argd(1);

	return true;
}

// Write data through specified LineParser
bool NeutronWeights::write(LineParser &parser)
{
	// Write AtomTypeList
	if (!atomTypes_.write(parser))
		return false;

	// Write Isotopologues-tures
	if (!parser.writeLineF("%i  # nItems\n", isotopologueMixtures_.nItems()))
		return false;
	ListIterator<Isotopologues> mixIterator(isotopologueMixtures_);
	while (Isotopologues *mix = mixIterator.iterate())
		if (!mix->write(parser))
			return false;

	// Write arrays using static methods in the relevant GenericItemContainer
	if (!GenericItemContainer<Array2D<double>>::write(concentrationProducts_, parser))
		return false;
	if (!GenericItemContainer<Array2D<double>>::write(boundCoherentProducts_, parser))
		return false;
	if (!GenericItemContainer<Array2D<double>>::write(weights_, parser))
		return false;
	if (!GenericItemContainer<Array2D<double>>::write(intramolecularWeights_, parser))
		return false;

	// Write averages
	if (!parser.writeLineF("%f %f\n", boundCoherentAverageOfSquares_, boundCoherentSquareOfAverage_))
		return false;

	return true;
}

/*
 * Parallel Comms
 */

// Broadcast item contents
bool NeutronWeights::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
	BroadcastList<Isotopologues> isoMixBroadcaster(procPool, root, isotopologueMixtures_, coreData);
	if (isoMixBroadcaster.failed())
		return false;
	if (!atomTypes_.broadcast(procPool, root, coreData))
		return false;
	if (!procPool.broadcast(concentrationProducts_, root))
		return false;
	if (!procPool.broadcast(boundCoherentProducts_, root))
		return false;
	if (!procPool.broadcast(weights_, root))
		return false;
	if (!procPool.broadcast(intramolecularWeights_, root))
		return false;
	if (!procPool.broadcast(boundCoherentAverageOfSquares_, root))
		return false;
	if (!procPool.broadcast(boundCoherentSquareOfAverage_, root))
		return false;
	if (!procPool.broadcast(valid_, root))
		return false;
#endif
	return true;
}

// Check item equality
bool NeutronWeights::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
	if (!atomTypes_.equality(procPool))
		return Messenger::error("NeutronWeights AtomTypes are not equivalent.\n");
	if (!procPool.equality(concentrationProducts_))
		return Messenger::error("NeutronWeights concentration matrix is not equivalent.\n");
	if (!procPool.equality(boundCoherentProducts_))
		return Messenger::error("NeutronWeights bound coherent matrix is not equivalent.\n");
	if (!procPool.equality(weights_))
		return Messenger::error("Unbound weights matrix is not equivalent.\n");
	if (!procPool.equality(intramolecularWeights_))
		return Messenger::error("Intramolecular weights matrix is not equivalent.\n");
	if (!procPool.equality(boundCoherentAverageOfSquares_))
		return Messenger::error(
			"NeutronWeights bound coherent average of squares is not equivalent (process %i has %e).\n",
			procPool.poolRank(), boundCoherentAverageOfSquares_);
	if (!procPool.equality(boundCoherentSquareOfAverage_))
		return Messenger::error(
			"NeutronWeights bound coherent square of average is not equivalent (process %i has %e).\n",
			procPool.poolRank(), boundCoherentSquareOfAverage_);
	if (!procPool.equality(valid_))
		return Messenger::error("NeutronWeights validity is not equivalent (process %i has %i).\n", procPool.poolRank(),
					valid_);
#endif
	return true;
}
