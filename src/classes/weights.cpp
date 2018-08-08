/*
	*** Weights Container
	*** src/classes/weights.cpp
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

#include "classes/weights.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "data/isotopes.h"
#include "base/processpool.h"
#include "base/lineparser.h"
#include "templates/listio.h"
#include "templates/broadcastlist.h"
#include "templates/genericitemcontainer_array2ddouble.h"

// Constructor
Weights::Weights()
{
	boundCoherentSquareOfAverage_ = 0.0;
	boundCoherentAverageOfSquares_ = 0.0;
	valid_ = false;
}

// Copy Constructor
Weights::Weights(const Weights& source)
{
	(*this) = source;
}

// Assignment Operator
void Weights::operator=(const Weights& source)
{
	// Isotopologue Mix
	isotopologueMixtures_ = source.isotopologueMixtures_;
	atomTypes_ = source.atomTypes_;
	boundCoherentProducts_= source.boundCoherentProducts_;
	concentrationProducts_ = source.concentrationProducts_;
	weights_ = source.weights_;
	boundWeights_ = source.boundWeights_;
	boundCoherentSquareOfAverage_ = source.boundCoherentSquareOfAverage_;
	boundCoherentAverageOfSquares_ = source.boundCoherentAverageOfSquares_;
	valid_ = source.valid_;
}

/*
 * Construction
 */

// Clear contents
void Weights::clear()
{
	isotopologueMixtures_.clear();
	atomTypes_.clear();
	concentrationProducts_.clear();
	boundCoherentProducts_.clear();
	weights_.clear();
	boundWeights_.clear();
	boundCoherentSquareOfAverage_ = 0.0;
	boundCoherentAverageOfSquares_ = 0.0;
	valid_ = false;
}

// Add Isotopologue for Species
bool Weights::addIsotopologue(Species* sp, int speciesPopulation, Isotopologue* iso, double isotopologueRelativePopulation)
{
	// Check that the Species is in the list...
	IsotopologueMix* mix = hasSpeciesIsotopologueMixture(sp);
	if (mix == NULL)
	{
		mix = isotopologueMixtures_.add();
		mix->setSpecies(sp, speciesPopulation);
	}

	// Add/update Isotopologue provided?
	if (!mix->addIsotopologue(iso, isotopologueRelativePopulation))
	{
		Messenger::error("Failed to add Isotopologue to IsotopologueSet.\n");
		return false;
	}
	
	return true;
}

// Return whether the IsotopologueSet contains a mixtures definition for the provided Species
IsotopologueMix* Weights::hasSpeciesIsotopologueMixture(Species* sp) const
{
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next) if (mix->species() == sp) return mix;
	return NULL;
}

// Print atomtype / weights information
void Weights::print() const
{
	Messenger::print("  Species          Isotopologue     nTotMols    Fraction\n");
	Messenger::print("  ------------------------------------------------------\n");
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next)
	{
		RefListIterator<Isotopologue,double> topeIterator(mix->isotopologues());
		while (Isotopologue* tope = topeIterator.iterate())
		{
			if (topeIterator.isFirst()) Messenger::print("  %-15s  %-15s  %-10i  %f\n", mix->species()->name(), tope->name(), mix->speciesPopulation(), topeIterator.currentData());
			else Messenger::print("                   %-15s              %f\n", tope->name(), topeIterator.currentData());
		}
	}

	// Print atomtypes table
	Messenger::print("\n");
	atomTypes_.print();

	Messenger::print("\nCalculated average scattering lengths: <b>**2 = %f, <b**2> = %f\n", boundCoherentSquareOfAverage_, boundCoherentAverageOfSquares_);
}

/*
 * Data
 */

// Calculate weighting matrices based on current AtomType / Isotope information
void Weights::calculateWeightingMatrices()
{
	// Create weights matrices and calculate average scattering lengths
	// Note: Multiplier of 0.1 on b terms converts from units of fm (1e-11 m) to barn (1e-12 m)
	concentrationProducts_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	boundCoherentProducts_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	weights_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	boundWeights_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	boundCoherentAverageOfSquares_ = 0.0;
	boundCoherentSquareOfAverage_ = 0.0;

	double ci, cj, bi, bj;

	// Determine atomic concentration products, bound coherent products, and full scattering weights
	AtomTypeData* atd1 = atomTypes_.first(), *atd2;
	for (int typeI=0; typeI<atomTypes_.nItems(); ++typeI, atd1 = atd1->next)
	{
		ci = atd1->fraction();
		bi = atd1->boundCoherent() * 0.1;

		// Update average scattering values
		boundCoherentSquareOfAverage_ += ci*bi;
		boundCoherentAverageOfSquares_ += ci*bi*bi;

		atd2 = atd1;
		for (int typeJ=typeI; typeJ<atomTypes_.nItems(); ++typeJ, atd2 = atd2->next)
		{
			cj = atd2->fraction();
			bj = atd2->boundCoherent() * 0.1;

			concentrationProducts_.at(typeI,typeJ) = ci * cj;
			boundCoherentProducts_.at(typeI,typeJ) = bi * bj;
			weights_.at(typeI,typeJ) = ci * cj * bi * bj * (typeI == typeJ ? 1 : 2);
		}
	}

	// Finalise <b>**2
	boundCoherentSquareOfAverage_ *= boundCoherentSquareOfAverage_;

	// Determine bound (intramolecular) scattering weights

	// Loop over defined Isotopologues in our defining mixtures, summing terms from (intramolecular) pairs of Atoms
	boundWeights_ = 0.0;
	Array2D<double> intraNorm(atomTypes_.nItems(), atomTypes_.nItems(), true);
	Array2D<bool> intraFlag(atomTypes_.nItems(), atomTypes_.nItems(), true);
	Array2D<bool> globalFlag(atomTypes_.nItems(), atomTypes_.nItems(), true);
	intraNorm = 0.0;
	globalFlag = false;
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next)
	{
		// Get weighting for associated Species population
		double speciesWeight = double(mix->speciesPopulation());

		// Using the underlying Species, set a multiplying matrix of 1's and zeroes that state the AtomType interactions we have present
		Species* sp = mix->species();
		const int nAtoms = sp->nAtoms();
		intraFlag = false;
		for (int i = 0; i<nAtoms; ++i)
		{
			// Get AtomType for this Atom and find it in our local AtomTypeList
			int typeI = atomTypes_.indexOf(sp->atom(i)->atomType());
			if (typeI == -1) Messenger::error("Failed to find AtomType '%s' in local Weights.\n", sp->atom(i)->atomType()->name());
			for (int j = i; j<nAtoms; ++j)
			{
				// Get AtomType for this Atom and find it in our local AtomTypeList
				int typeJ = atomTypes_.indexOf(sp->atom(j)->atomType());

				intraFlag.at(typeI, typeJ) = true;
			}
		}

		// Loop over Isotopologues defined for this mixture
		RefListIterator<Isotopologue,double> topeIterator(mix->isotopologues());
		while (Isotopologue* tope = topeIterator.iterate())
		{
			// Sum the scattering lengths of each pair of AtomTypes, weighted by the speciesWeight and the fractional Isotopologue weight in the mix.
			double weight = speciesWeight * topeIterator.currentData();

			atd1 = atomTypes_.first();
			for (int typeI=0; typeI<atomTypes_.nItems(); ++typeI, atd1 = atd1->next)
			{
				// If this AtomType is exchangeable, add the averaged scattering length from the local AtomTypesList instead of its actual isotopic length.
				if (atd1->exchangeable()) bi = atd1->boundCoherent();
				else
				{
					// Get the Isotope associated to this AtomType in the current Isotopologue
					Isotope* isotope = tope->atomTypeIsotope(atd1->atomType());
					bi = isotope->boundCoherent();
				}
				bi *= 0.1;

				atd2 = atd1;
				for (int typeJ=typeI; typeJ<atomTypes_.nItems(); ++typeJ, atd2 = atd2->next)
				{
					// Check to see if this interaction is present in the current Species
					if (!intraFlag.at(typeI, typeJ)) continue;

					// If this AtomType is exchangeable, add the averaged scattering length from the local AtomTypesList instead of its actual isotopic length.
					if (atd2->exchangeable()) bj = atd2->boundCoherent();
					else
					{
						// Get the Isotope associated to this AtomType in the current Isotopologue
						Isotope* isotope = tope->atomTypeIsotope(atd2->atomType());
						bj = isotope->boundCoherent();
					}
					bj *= 0.1;

					boundWeights_.at(typeI,typeJ) += weight * bi * bj;
					intraNorm.at(typeI,typeJ) += weight;
					globalFlag.at(typeI, typeJ) = true;
				}
			}
		}
	}

	// Normalise the boundWeights_ array, and multiply by atomic concentrations and Kronecker delta
	atd1 = atomTypes_.first();
	for (int typeI=0; typeI<atomTypes_.nItems(); ++typeI, atd1 = atd1->next)
	{
		ci = atd1->fraction();

		atd2 = atd1;
		for (int typeJ=typeI; typeJ<atomTypes_.nItems(); ++typeJ, atd2 = atd2->next)
		{
			// Skip this pair if there are no such intramolecular interactions
			if (!globalFlag.at(typeI, typeJ)) continue;

			cj = atd2->fraction();

			boundWeights_.at(typeI, typeJ) /= intraNorm.at(typeI, typeJ);
			boundWeights_.at(typeI, typeJ) *= ci * cj * (typeI == typeJ ? 1 : 2);
		}
	}

	intraNorm.print();
	boundWeights_.print();
	weights_.print();
}

// Create AtomType list and matrices based on stored IsotopologueMix information
void Weights::createFromIsotopologues(const AtomTypeList& exchangeableTypes)
{
	// Loop over IsotopologueMix entries and ensure relative populations of Isotopologues sum to 1.0
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next) mix->normalise();

	// Fill atomTypes_ list with AtomType populations, based on IsotopologueMix relative populations and associated Species populations
	atomTypes_.clear();
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next)
	{
		// We must now loop over the Isotopologues in the mixture
		RefListIterator<Isotopologue,double> topeIterator(mix->isotopologues());
		while (Isotopologue* tope = topeIterator.iterate())
		{
			// Loop over Atoms in the Species, searching for the AtomType/Isotope entry in the isotopes list of the Isotopologue
			for (SpeciesAtom* i = mix->species()->firstAtom(); i != NULL; i = i->next)
			{
				Isotope* iso = tope->atomTypeIsotope(i->atomType());
				atomTypes_.addIsotope(i->atomType(), iso, topeIterator.currentData() * mix->speciesPopulation());
			}
		}
	}
	atomTypes_.finalise(exchangeableTypes);

	calculateWeightingMatrices();

	valid_ = true;
}

// Reduce data to be naturally-weighted
void Weights::naturalise()
{
	atomTypes_.naturalise();

	calculateWeightingMatrices();

	valid_ = true;
}

// Return AtomTypeList
AtomTypeList& Weights::atomTypes()
{
	return atomTypes_;
}

// Return number of used AtomTypes
int Weights::nUsedTypes() const
{
	return atomTypes_.nItems();
}

// Return concentration product for types i and j
double Weights::concentrationProduct(int i, int j) const
{
	return concentrationProducts_.constAt(i, j);
}

// Return bound coherent scattering product for types i 
double Weights::boundCoherentProduct(int i, int j) const
{
	return boundCoherentProducts_.constAt(i, j);
}

// Return full weighting for types i and j (ci * cj * bi * bj * [2-dij])
double Weights::weight(int i, int j) const
{
	return weights_.constAt(i, j);
}

// Return full bound weighting for types i and j
double Weights::boundWeight(int i, int j) const
{
	return boundWeights_.constAt(i, j);
}

// Return full weights matrix
Array2D<double>& Weights::weights()
{
	return weights_;
}

// Return full bound scattering weights matrix
Array2D<double>& Weights::boundWeights()
{
	return boundWeights_;
}

// Return bound coherent average squared scattering (<b>**2)
double Weights::boundCoherentSquareOfAverage() const
{
	return boundCoherentSquareOfAverage_;
}

// Return bound coherent squared average scattering (<b**2>)
double Weights::boundCoherentAverageOfSquares() const
{
	return boundCoherentAverageOfSquares_;
}

// Return whether the structure is valid (i.e. has been finalised)
bool Weights::isValid() const
{
	return valid_;
}

/*
 * GenericItemBase Implementations
 */

// Return class name
const char* Weights::itemClassName()
{
	return "Weights";
}

// Write data through specified LineParser
bool Weights::write(LineParser& parser)
{
	// Write AtomTypeList
	if (!atomTypes_.write(parser)) return false;

	// Write IsotopologueMix-tures
	if (!ListIO<IsotopologueMix>::write(isotopologueMixtures_, parser)) return false;

	// Write arrays using static methods in the relevant GenericItemContainer
	if (!GenericItemContainer< Array2D<double> >::write(concentrationProducts_, parser)) return false;
	if (!GenericItemContainer< Array2D<double> >::write(boundCoherentProducts_, parser)) return false;
	if (!GenericItemContainer< Array2D<double> >::write(weights_, parser)) return false;
	if (!GenericItemContainer< Array2D<double> >::write(boundWeights_, parser)) return false;

	// Write averages
	if (!parser.writeLineF("%f %f\n", boundCoherentAverageOfSquares_, boundCoherentSquareOfAverage_)) return false;

	return true;
}

// Read data through specified LineParser
bool Weights::read(LineParser& parser)
{
	clear();

	// Read AtomTypeList
	if (!atomTypes_.read(parser)) return false;

	// Read IsotopologueMix-tures
	if (!ListIO<IsotopologueMix>::read(isotopologueMixtures_, parser)) return false;

	// Read arrays using static methods in the relevant GenericItemContainer
	if (!GenericItemContainer< Array2D<double> >::read(concentrationProducts_, parser)) return false;
	if (!GenericItemContainer< Array2D<double> >::read(boundCoherentProducts_, parser)) return false;
	if (!GenericItemContainer< Array2D<double> >::read(weights_, parser)) return false;
	if (!GenericItemContainer< Array2D<double> >::read(boundWeights_, parser)) return false;

	// Read averages
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	boundCoherentAverageOfSquares_ = parser.argd(0);
	boundCoherentSquareOfAverage_ = parser.argd(1);

	return true;
}

/*
 * Parallel Comms
 */

// Broadcast item contents
bool Weights::broadcast(ProcessPool& procPool, int root)
{
#ifdef PARALLEL
	BroadcastList<IsotopologueMix> isoMixBroadcaster(procPool, root, isotopologueMixtures_);
	if (isoMixBroadcaster.failed()) return false;
	if (!atomTypes_.broadcast(procPool, root)) return false;
	if (!procPool.broadcast(concentrationProducts_, root)) return false;
	if (!procPool.broadcast(boundCoherentProducts_, root)) return false;
	if (!procPool.broadcast(weights_, root)) return false;
	if (!procPool.broadcast(boundWeights_, root)) return false;
	if (!procPool.broadcast(boundCoherentAverageOfSquares_, root)) return false;
	if (!procPool.broadcast(boundCoherentSquareOfAverage_, root)) return false;
	if (!procPool.broadcast(valid_, root)) return false;
#endif
	return true;
}

// Check item equality
bool Weights::equality(ProcessPool& procPool)
{
#ifdef PARALLEL
	if (!atomTypes_.equality(procPool)) return Messenger::error("Weights AtomTypes are not equivalent.\n");
	if (!procPool.equality(concentrationProducts_)) return Messenger::error("Weights concentration matrix is not equivalent.\n");
	if (!procPool.equality(boundCoherentProducts_)) return Messenger::error("Weights bound coherent matrix is not equivalent.\n");
	if (!procPool.equality(weights_)) return Messenger::error("Unbound weights matrix is not equivalent.\n");
	if (!procPool.equality(boundWeights_)) return Messenger::error("Bound weights matrix is not equivalent.\n");
	if (!procPool.equality(boundCoherentAverageOfSquares_)) return Messenger::error("Weights bound coherent average of squares is not equivalent (process %i has %e).\n", procPool.poolRank(), boundCoherentAverageOfSquares_);
	if (!procPool.equality(boundCoherentSquareOfAverage_)) return Messenger::error("Weights bound coherent square of average is not equivalent (process %i has %e).\n", procPool.poolRank(), boundCoherentSquareOfAverage_);
	if (!procPool.equality(valid_)) return Messenger::error("Weights validity is not equivalent (process %i has %i).\n", procPool.poolRank(), valid_);
#endif
	return true;
}
