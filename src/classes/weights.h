/*
	*** Weights Container
	*** src/classes/weights.h
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

#ifndef DUQ_WEIGHTS_H
#define DUQ_WEIGHTS_H

#include "classes/atomtypelist.h"
#include "classes/isotopologuemix.h"
#include "templates/list.h"
#include "templates/array2d.h"
#include "base/genericitembase.h"

// Forward Declarations
/* none */

// Weights Container
class Weights : public GenericItemBase
{
	public:
	// Constructor
	Weights();
	// Copy Constructor
	Weights(const Weights& source);
	// Assignment operator
	void operator=(const Weights& source);


	/*
	 * Construction
	 */
	private:
	// List of IsotopologueMix-tures for Species
	List<IsotopologueMix> isotopologueMixtures_;

	public:
	// Clear contents
	void clear();
	// Add Species Isotopologue to the relevant mixture
	bool addIsotopologue(Species* sp, int speciesPopulation, Isotopologue* iso, double isotopologueRelativePopulation);
	// Return whether we have a mixtures definition for the provided Species
	IsotopologueMix* hasSpeciesIsotopologueMixture(Species* sp) const;
	// Print atomtype / weights information
	void print() const;


	/*
	 * Data
	 */
	private:
	// Type list derived from Species referenced in isotopologueMixtures_
	AtomTypeList atomTypes_;
	// Atom concentration weights matrix (ci * cj)
	Array2D<double> concentrationMatrix_;
	// Bound coherent weights matrix (bi * bj)
	Array2D<double> boundCoherentMatrix_;
	// Full weights matrix (ci * cj * bi * bj * (i == j ? 1 : 2))
	Array2D<double> fullMatrix_;
	// Bound coherent average squared scattering (<b>**2)
	double boundCoherentSquareOfAverage_;
	// Bound coherent squared average scattering (<b**2>)
	double boundCoherentAverageOfSquares_;
	// Whether the structure is valid (i.e. has been finalised)
	bool valid_;

	private:
	// Calculate weighting matrices based on current AtomType / Isotope information
	void calculateWeightingMatrices();

	public:
	// Finalise lists and matrices based on IsotopologueMix information
	void finalise(const AtomTypeList& exchangeableTypes);
	// Reduce data to be naturally-weighted
	void naturalise();
	// Set (sole) Isotope for specified AtomType and recalculate matrices
	bool setAtomTypeIsotope(AtomType* atomType, Isotope* isotope);
	// Return AtomTypeList
	AtomTypeList& atomTypes();
	// Return number of used AtomTypes
	int nUsedTypes();
	// Return concentration weighting for types i and j
	double concentrationWeight(int i, int j);
	// Return bound coherent scattering weighting for types i and j
	double boundCoherentWeight(int i, int j);
	// Return full weighting, including atomic concentration, bound coherent scattering weights, and i != j weighting for types i and j
	double fullWeight(int i, int j);
	// Return full scattering weights matrix (ci * cj * bi * bj * (i == j ? 1 : 2))
	Array2D<double>& fullWeightsMatrix();
	// Return bound coherent average squared scattering (<b>**2)
	double boundCoherentSquareOfAverage();
	// Return bound coherent squared average scattering (<b**2>)
	double boundCoherentAverageOfSquares();
	// Return whether the structure is valid (i.e. has been finalised)
	bool isValid();


	/*
	 * GenericItemBase Implementations
	 */
	public:
	// Return class name
	static const char* itemClassName();
	// Write data through specified LineParser
	bool write(LineParser& parser);
	// Read data through specified LineParser
	bool read(LineParser& parser);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contents
	bool broadcast(ProcessPool& procPool, int root);
	// Check item equality
	bool equality(ProcessPool& procPool);
};

#endif
