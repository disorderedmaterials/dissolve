/*
	*** Weights Matrix
	*** src/classes/werightsmatrix.cpp
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

#include "classes/weightsmatrix.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "base/processpool.h"

// Constructor
WeightsMatrix::WeightsMatrix()
{
	boundCoherentAverageSquared_ = 0.0;
	boundCoherentSquaredAverage_ = 0.0;
}

// Copy Constructor
WeightsMatrix::WeightsMatrix(const WeightsMatrix& source)
{
	(*this) = source;
}

// Assignment Operator
void WeightsMatrix::operator=(const WeightsMatrix& source)
{
	// Isotopologue Mix
	isotopologueMixtures_ = source.isotopologueMixtures_;
	atomTypes_ = source.atomTypes_;
	scatteringMatrix_ = source.scatteringMatrix_;
	concentrationMatrix_ = source.concentrationMatrix_;
	boundCoherentAverageSquared_ = source.boundCoherentAverageSquared_;
	boundCoherentSquaredAverage_ = source.boundCoherentSquaredAverage_;
}

/*
 * Construction
 */

// Clear contents
void WeightsMatrix::clear()
{
	isotopologueMixtures_.clear();
	atomTypes_.clear();
	boundCoherentAverageSquared_ = 0.0;
	boundCoherentSquaredAverage_ = 0.0;
}

// Add Isotopologue for Species
bool WeightsMatrix::addIsotopologue(Species* sp, int speciesPopulation, Isotopologue* iso, double isotopologueRelativePopulation)
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
IsotopologueMix* WeightsMatrix::hasSpeciesIsotopologueMixture(Species* sp) const
{
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next) if (mix->species() == sp) return mix;
	return NULL;
}

// Print full isotopologue information
void WeightsMatrix::printIsotopologues()
{
	Messenger::print("  Species          Isotopologue     nTotMols    TopeFrac\n");
	Messenger::print("  ------------------------------------------------------\n");
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next)
	{
		for (RefListItem<Isotopologue,double>* tope = mix->isotopologues(); tope != NULL; tope = tope->next)
		{
			if (tope == mix->isotopologues()) Messenger::print("  %-15s  %-15s  %-10i  %f\n", mix->species()->name(), tope->item->name(), mix->speciesPopulation(), tope->data);
			else Messenger::print("                   %-15s              %f\n", tope->item->name(), tope->data);
		}
	}
}

/*
 * Calculated Data
 */

// Finalise lists and matrices based on IsotopologueMix information
void WeightsMatrix::finalise(bool quiet)
{
	// Loop over IsotopologueMix entries and ensure relative populations of Isotopologues sum to 1.0
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next) mix->normalise();
	if (!quiet)
	{
		Messenger::print("\n");
		printIsotopologues();
	}

	// Fill atomTypes_ list with AtomType populations, based on IsotopologueMix relative populations and associated Species populations
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next)
	{
		// We must now loop over the Isotopologues in the mixture
		for (RefListItem<Isotopologue,double>* tope = mix->isotopologues(); tope != NULL; tope = tope->next)
		{
			// Loop over Atoms in the Species, searching for the AtomType/Isotope entry in the isotopes list of the Isotopologue
			for (SpeciesAtom* i = mix->species()->atoms(); i != NULL; i = i->next)
			{
				Isotope* iso = tope->item->atomTypeIsotope(i->atomType());
				atomTypes_.add(i->atomType(), iso, tope->data*mix->speciesPopulation());
			}
		}
	}
	atomTypes_.finalise();
	if (!quiet)
	{
		Messenger::print("\n");
		atomTypes_.print();
	}

	// Create weights matrices and calculate average scattering lengths
	// Note: Multiplier of 0.1 on b terms converts from units of fm (1e-11 m) to barn (1e-12 m)
	concentrationMatrix_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	scatteringMatrix_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	boundCoherentAverageSquared_ = 0.0;
	boundCoherentSquaredAverage_ = 0.0;
	double ci, cj, bi, bj;
	AtomTypeData* at1 = atomTypes_.first(), *at2;
	for (int typeI=0; typeI<atomTypes_.nItems(); ++typeI, at1 = at1->next)
	{
		ci = at1->fraction();
		bi = at1->isotope()->boundCoherent() * 0.1;
		// Update average scattering values
		boundCoherentAverageSquared_ += ci*bi;
		boundCoherentSquaredAverage_ += ci*bi*bi;

		at2 = at1;
		for (int typeJ=typeI; typeJ<atomTypes_.nItems(); ++typeJ, at2 = at2->next)
		{
			cj = at2->fraction();
			bj = at2->isotope()->boundCoherent() * 0.1;

			concentrationMatrix_.ref(typeI,typeJ) = ci * cj;
			scatteringMatrix_.ref(typeI,typeJ) = ci * cj * bi * bj;
		}
	}
	boundCoherentAverageSquared_ *= boundCoherentAverageSquared_;

	if (!quiet) Messenger::print("  \nCalculated average scattering lengths: <b>**2 = %f, <b**2> = %f\n", boundCoherentAverageSquared_, boundCoherentSquaredAverage_);
}

// Return AtomTypeList
AtomTypeList& WeightsMatrix::atomTypes()
{
	return atomTypes_;
}

// Return number of used AtomTypes
int WeightsMatrix::nUsedTypes()
{
	return atomTypes_.nItems();
}

// Return concentration weighting for types i and j
double WeightsMatrix::concentrationWeight(int i, int j)
{
	return concentrationMatrix_.ref(i, j);
}

// Return scattering weighting for types i and j
double WeightsMatrix::scatteringWeight(int i, int j)
{
	return scatteringMatrix_.ref(i, j);
}

// Return scattering weights matrix (ci * cj * bi * bj)
Array2D<double>& WeightsMatrix::scatteringMatrix()
{
	return scatteringMatrix_;
}
