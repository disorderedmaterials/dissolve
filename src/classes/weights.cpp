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
	boundCoherentMatrix_= source.boundCoherentMatrix_;
	concentrationMatrix_ = source.concentrationMatrix_;
	fullMatrix_ = source.fullMatrix_;
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
	concentrationMatrix_.clear();
	boundCoherentMatrix_.clear();
	fullMatrix_.clear();
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
 * Data
 */

// Calculate weighting matrices based on current AtomType / Isotope information
void Weights::calculateWeightingMatrices()
{
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

// Create AtomType list and matrices based on stored IsotopologueMix information
void Weights::createFromIsotopologues()
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
			for (SpeciesAtom* i = mix->species()->firstAtom(); i != NULL; i = i->next)
			{
				Isotope* iso = tope->item->atomTypeIsotope(i->atomType());
				atomTypes_.addIsotope(i->atomType(), iso, tope->data*mix->speciesPopulation());
			}
		}
	}
	atomTypes_.finalise();

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

// Set (sole) Isotope for specified AtomType and recalculate matrices
bool Weights::setAtomTypeIsotope(AtomType* atomType, Isotope* isotope)
{
	// First, make sure the AtomType is in our list
	AtomTypeData* atd = atomTypes_.atomTypeData(atomType);
	if (!atd) return Messenger::error("AtomType '%s' is not in the Weights' list.\n", atomType->name());

	// Make sure the Isotope specified is related to the AtomType's element
	if (!PeriodicTable::element(atomType->element()).hasIsotope(isotope->A())) return Messenger::error("Specified AtomType (%s) and Isotope (A = %i) are incompatible.\n", atomType->name(), isotope->A());

	// Set a single Isotope for this AtomType, maintaining the population
	atd->setSingleIsotope(isotope);

	atd = atomTypes_.atomTypeData(atomTypes_.atomType(1));
	atd->setSingleIsotope(PeriodicTable::element(atd->atomType()->element()).hasIsotope(-2));

	calculateWeightingMatrices();

	return true;
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

// Return concentration weighting for types i and j
double Weights::concentrationWeight(int i, int j) const
{
	return concentrationMatrix_.value(i, j);
}

// Return full weighting, including atomic concentration, bound coherent scattering weights, and i != j weighting for types i and j
double Weights::fullWeight(int i, int j) const
{
	return fullMatrix_.value(i, j);
}

// Return bound coherent scattering weighting for types i and j
double Weights::boundCoherentWeight(int i, int j) const
{
	return boundCoherentMatrix_.value(i, j);
}

// Return full scattering weights matrix (ci * cj * bi * bj * (i == j ? 1 : 2))
Array2D<double>& Weights::fullWeightsMatrix()
{
	return fullMatrix_;
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
	if (!GenericItemContainer< Array2D<double> >::write(concentrationMatrix_, parser)) return false;
	if (!GenericItemContainer< Array2D<double> >::write(boundCoherentMatrix_, parser)) return false;
	if (!GenericItemContainer< Array2D<double> >::write(fullMatrix_, parser)) return false;

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
	if (!GenericItemContainer< Array2D<double> >::read(concentrationMatrix_, parser)) return false;
	if (!GenericItemContainer< Array2D<double> >::read(boundCoherentMatrix_, parser)) return false;
	if (!GenericItemContainer< Array2D<double> >::read(fullMatrix_, parser)) return false;

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
	if (!procPool.broadcast(concentrationMatrix_, root)) return false;
	if (!procPool.broadcast(boundCoherentMatrix_, root)) return false;
	if (!procPool.broadcast(fullMatrix_, root)) return false;
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
	if (!procPool.equality(concentrationMatrix_)) return Messenger::error("Weights concentration matrix is not equivalent.\n");
	if (!procPool.equality(boundCoherentMatrix_)) return Messenger::error("Weights bound coherent matrix is not equivalent.\n");
	if (!procPool.equality(fullMatrix_)) return Messenger::error("Weights full matrix is not equivalent.\n");
	if (!procPool.equality(boundCoherentAverageOfSquares_)) return Messenger::error("Weights bound coherent average of squares is not equivalent (process %i has %e).\n", procPool.poolRank(), boundCoherentAverageOfSquares_);
	if (!procPool.equality(boundCoherentSquareOfAverage_)) return Messenger::error("Weights bound coherent square of average is not equivalent (process %i has %e).\n", procPool.poolRank(), boundCoherentSquareOfAverage_);
	if (!procPool.equality(valid_)) return Messenger::error("Weights validity is not equivalent (process %i has %i).\n", procPool.poolRank(), valid_);
#endif
	return true;
}
