/*
	*** Weights Matrix
	*** src/classes/weightsmatrix.h
	Copyright T. Youngs 2012-2017

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

#ifndef DUQ_WEIGHTSMATRIX_H
#define DUQ_WEIGHTSMATRIX_H

#include "classes/atomtypelist.h"
#include "classes/isotopologuemix.h"
#include "templates/list.h"
#include "templates/array2d.h"

// Forward Declarations
/* none */

// Weights Matrix
class WeightsMatrix
{
	public:
	// Constructor
	WeightsMatrix();
	// Copy Constructor
	WeightsMatrix(const WeightsMatrix& source);
	// Assignment operator
	void operator=(const WeightsMatrix& source);


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
	// Print full IsotopologueMix information
	void printIsotopologues();


	/*
	 * Calculated Data
	 */
	private:
	// Type list derived from Species referenced in isotopologueMixtures_
	AtomTypeList atomTypes_;
	// Atom concentration weights matrix (ci * cj)
	Array2D<double> concentrationMatrix_;
	// Scattering weights matrix (ci * cj * bi * bj)
	Array2D<double> scatteringMatrix_;
	// Bound coherent average squared scattering (<b>**2)
	double boundCoherentAverageSquared_;
	// Bound coherent squared average scattering (<b**2>)
	double boundCoherentSquaredAverage_;

	public:
	// Finalise lists and matrices based on IsotopologueMix information
	void finalise(bool quiet = false);
	// Return AtomTypeList
	AtomTypeList& atomTypes();
	// Return number of used AtomTypes
	int nUsedTypes();
	// Return concentration weighting for types i and j
	double concentrationWeight(int i, int j);
	// Return scattering weighting, including concentration weighting, for types i and j
	double scatteringWeight(int i, int j);
	// Return scattering weights matrix (ci * cj * bi * bj)
	Array2D<double>& scatteringMatrix();
};

#endif
