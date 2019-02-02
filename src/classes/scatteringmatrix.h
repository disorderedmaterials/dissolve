/*
	*** Scattering Matrix Helper Class
	*** src/classes/scatteringmatrix.h
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

#ifndef DISSOLVE_SCATTERINGMATRIX_H
#define DISSOLVE_SCATTERINGMATRIX_H

#include "math/data1d.h"
#include "templates/list.h"
#include "templates/reflist.h"
#include "templates/pair.h"
#include "templates/array2d.h"

// Forward Declarations
class AtomType;
class Weights;

// Scattering Matrix Container
class ScatteringMatrix
{
	public:
	// Constructor
	ScatteringMatrix();


	/*
	 * Data
	 * 
	 * Representing: AX = B
	 * 
	 * 	[  1,1 ... 1,n ] [ x1 ]   [ B2 ]
	 * 	[  2,1     2,n ] [ x2 ] = [ B2 ]
	 * 	[  n,1     n,n ] [ xn ]   [ Bn ]
	 */
	private:
	// Reference pairs of AtomTypes
	List< Pair<AtomType*,AtomType*> > typePairs_;	
	// Coefficients matrix (A) (ci * cj * bi * bj * (typei == typej ? 1 : 2)) (n * n)
	Array2D<double> A_;
	// Inverse of coefficients matrix
	Array2D<double> inverseA_;
	// Reference data (B) (n * 1)
	Array<Data1D> data_;

	public:
	// Return number of reference AtomType pairs
	int nPairs() const;
	// Return index of specified AtomType pair
	int pairIndex(AtomType* typeI, AtomType* typeJ) const;
	// Return weight of the specified AtomType pair in the inverse matrix
	int pairWeightInverse(AtomType* typeI, AtomType* typeJ, int dataIndex) const;
	// Print the matrix
	void print() const;
	// Print the inverse matrix
	void printInverse() const;
	// Generate partials from reference data using inverse matrix
	void generatePartials(Array2D<Data1D>& generatedSQ);
	// Return if the scattering matrix is underdetermined
	bool underDetermined() const;
	// Return the product of inverseA_ and A_ (which should be the identity matrix)
	Array2D<double> matrixProduct() const;

	/*
	 * Construction
	 */
	public:
	// Initialise from supplied list of AtomTypes
	void initialise(const List<AtomType>& types, Array2D<Data1D>& generatedSQ, const char* objectNamePrefix, const char* groupName);
	// Finalise
	bool finalise();
	// Add reference data with its associated Weights, applying optional factor to those weights and the data itself
	bool addReferenceData(const Data1D& weightedData, Weights& dataWeights, double factor = 1.0);
	// Add reference partial data between specified AtomTypes, applying optional factor to the weight and the data itself
	bool addPartialReferenceData(Data1D& weightedData, AtomType* at1, AtomType* at2, double dataWeight, double factor = 1.0);
};

#endif
