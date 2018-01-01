/*
	*** Weights Container
	*** src/classes/weights.cpp
	Copyright T. Youngs 2012-2018

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

#include "classes/weights.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "base/processpool.h"
#include "templates/broadcastlist.h"

// Constructor
Weights::Weights()
{
	boundCoherentSquareOfAverage_ = 0.0;
	boundCoherentAverageOfSquares_ = 0.0;
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
	boundCoherentMatrix_= source.boundCoherentMatrix_;
	concentrationMatrix_ = source.concentrationMatrix_;
	fullMatrix_ = source.fullMatrix_;
	boundCoherentSquareOfAverage_ = source.boundCoherentSquareOfAverage_;
	boundCoherentAverageOfSquares_ = source.boundCoherentAverageOfSquares_;
}

/*
 * Construction
 */

// Clear contents
void Weights::clear()
{
	isotopologueMixtures_.clear();
	atomTypes_.clear();
	boundCoherentSquareOfAverage_ = 0.0;
	boundCoherentAverageOfSquares_ = 0.0;
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
		for (RefListItem<Isotopologue,double>* tope = mix->isotopologues(); tope != NULL; tope = tope->next)
		{
			if (tope == mix->isotopologues()) Messenger::print("  %-15s  %-15s  %-10i  %f\n", mix->species()->name(), tope->item->name(), mix->speciesPopulation(), tope->data);
			else Messenger::print("                   %-15s              %f\n", tope->item->name(), tope->data);
		}
	}

	// Print atomtypes table
	Messenger::print("\n");
	atomTypes_.print();

	Messenger::print("\nCalculated average scattering lengths: <b>**2 = %f, <b**2> = %f\n", boundCoherentSquareOfAverage_, boundCoherentAverageOfSquares_);
}

/*
 * Calculated Data
 */

// Finalise lists and matrices based on IsotopologueMix information
void Weights::finalise(AtomTypeList exchangeableTypes)
{
	// Loop over IsotopologueMix entries and ensure relative populations of Isotopologues sum to 1.0
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next) mix->normalise();

	// Fill atomTypes_ list with AtomType populations, based on IsotopologueMix relative populations and associated Species populations
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next)
	{
		// We must now loop over the Isotopologues in the mixture
		for (RefListItem<Isotopologue,double>* tope = mix->isotopologues(); tope != NULL; tope = tope->next)
		{
			// Loop over Atoms in the Species, searching for the AtomType/Isotope entry in the isotopes list of the Isotopologue
			for (SpeciesAtom* i = mix->species()->atoms(); i != NULL; i = i->next)
			{
				bool exchangeable = exchangeableTypes.contains(i->atomType());

				Isotope* iso = tope->item->atomTypeIsotope(i->atomType());
				atomTypes_.addIsotope(i->atomType(), iso, tope->data*mix->speciesPopulation(), exchangeable);
			}
		}
	}
	atomTypes_.finalise();

	// Create weights matrices and calculate average scattering lengths
	// Note: Multiplier of 0.1 on b terms converts from units of fm (1e-11 m) to barn (1e-12 m)
	concentrationMatrix_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	boundCoherentMatrix_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	fullMatrix_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	boundCoherentAverageOfSquares_ = 0.0;
	boundCoherentSquareOfAverage_ = 0.0;
	double ci, cj, bi, bj;
	AtomTypeData* at1 = atomTypes_.first(), *at2;
	for (int typeI=0; typeI<atomTypes_.nItems(); ++typeI, at1 = at1->next)
	{
		ci = at1->fraction();
		bi = at1->boundCoherent() * 0.1;

		// Update average scattering values
		boundCoherentSquareOfAverage_ += ci*bi;
		boundCoherentAverageOfSquares_ += ci*bi*bi;

		at2 = at1;
		for (int typeJ=typeI; typeJ<atomTypes_.nItems(); ++typeJ, at2 = at2->next)
		{
			cj = at2->fraction();
			bj = at2->boundCoherent() * 0.1;

			concentrationMatrix_.ref(typeI,typeJ) = ci * cj;
			boundCoherentMatrix_.ref(typeI,typeJ) = bi * bj;
			fullMatrix_.ref(typeI,typeJ) = ci * cj * bi * bj * (typeI == typeJ ? 1 : 2);
		}
	}
	boundCoherentSquareOfAverage_ *= boundCoherentSquareOfAverage_;
}

// Return AtomTypeList
AtomTypeList& Weights::atomTypes()
{
	return atomTypes_;
}

// Return number of used AtomTypes
int Weights::nUsedTypes()
{
	return atomTypes_.nItems();
}

// Return concentration weighting for types i and j
double Weights::concentrationWeight(int i, int j)
{
	return concentrationMatrix_.ref(i, j);
}

// Return full weighting, including atomic concentration, bound coherent scattering weights, and i !+ j weighting for types i and j
double Weights::fullWeight(int i, int j)
{
	return fullMatrix_.ref(i, j);
}

// Return bound coherent scattering weighting for types i and j
double Weights::boundCoherentWeight(int i, int j)
{
	return boundCoherentMatrix_.ref(i, j);
}

// Return full scattering weights matrix (ci * cj * bi * bj * (i == j ? 1 : 2))
Array2D<double>& Weights::fullWeightsMatrix()
{
	return fullMatrix_;
}

// Return bound coherent average squared scattering (<b>**2)
double Weights::boundCoherentSquareOfAverage()
{
	return boundCoherentSquareOfAverage_;
}

// Return bound coherent squared average scattering (<b**2>)
double Weights::boundCoherentAverageOfSquares()
{
	return boundCoherentAverageOfSquares_;
}

/*
 * GenericItemBase Implementations
 */

// Return class name
const char* Weights::itemClassName()
{
	return "Weights";
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
	if (!procPool.broadcast(concentrationMatrix_, root)) return false;
	if (!procPool.broadcast(boundCoherentMatrix_, root)) return false;
	if (!procPool.broadcast(fullMatrix_, root)) return false;
	if (!procPool.broadcast(boundCoherentAverageOfSquares_, root)) return false;
	if (!procPool.broadcast(boundCoherentSquareOfAverage_, root)) return false;
#endif
	return true;
}
