/*
	*** Scattering Matrix Helper Class
	*** src/classes/scatteringmatrix.h
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

#ifndef DUQ_SCATTERINGMATRIX_H
#define DUQ_SCATTERINGMATRIX_H

#include "base/xydata.h"
#include "templates/list.h"
#include "templates/reflist.h"
#include "templates/pair.h"
#include "templates/array2d.h"

// Forward Declarations
class AtomType;
class Weights;
class Data;

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
	RefList<Data,bool> data_;

	public:
	// Return number of reference AtomType pairs
	int nPairs() const;
	// Return index of specified AtomType pair
	int pairIndex(AtomType* typeI, AtomType* typeJ) const;
	// Print the matrix
	void print() const;
	// Generate partials from reference data using inverse matrix
	void generatePartials(Array2D<XYData>& generatedSQ);
	// Return if the scattering matrix is incomplete (i.e. has fewer rows than there are columns)
	bool incomplete() const;


	/*
	 * Construction
	 */
	public:
	// Initialise from supplied list of AtomTypes
	void initialise(const List<AtomType>& types, Array2D<XYData>& generatedSQ, const char* objectNamePrefix);
	// Finalise
	bool finalise();
	// Add reference data
	bool addReferenceData(Data* data);
};

#endif
