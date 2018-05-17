/*
	*** Scattering Matrix Helper Class
	*** src/classes/scatteringmatrix.cpp
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

#include "classes/scatteringmatrix.h"
#include "classes/atomtype.h"
#include "classes/weights.h"

// Constructor
ScatteringMatrix::ScatteringMatrix()
{
}

/*
 * Data
 */

// Return number of reference AtomType pairs
int ScatteringMatrix::nPairs() const
{
	return typePairs_.nItems();
}

// Return index of specified AtomType pair
int ScatteringMatrix::pairIndex(AtomType* typeI, AtomType* typeJ) const
{
	int index = 0;
	for (Pair<AtomType*,AtomType*>* pair = typePairs_.first(); pair != NULL; pair = pair->next)
	{
		if ((pair->a == typeI) && (pair->b == typeJ)) return index;
		if ((pair->a == typeJ) && (pair->b == typeI)) return index;
		++index;
	}

	return -1;
}

// Print the matrix
void ScatteringMatrix::print() const
{
	// Write header
	CharString text, line;
	for (Pair<AtomType*,AtomType*>* pair = typePairs_.first(); pair != NULL; pair = pair->next)
	{
		text.sprintf("%s-%s", pair->a->name(), pair->b->name());
		line.strcatf("%10s ", text.get());
	}
	Messenger::print("%s", line.get());

	// Loop over reference data
	for (int row = 0; row < data_.nItems(); ++row)
	{
		line.clear();
		for (int n=0; n<A_.nColumns(); ++n) line.strcatf("%10f ", A_.value(row, n));
		Messenger::print("%s  %s\n", line.get(), data_.value(row).name());
	}
}

// Generate partials from reference data using the inverse coefficients matrix
void ScatteringMatrix::generatePartials(Array2D<XYData>& generatedSQ)
{
	// Get linear array from generatedSQ
	XYData* partials = generatedSQ.linearArray();

	// Clear current partials
	for (int n=0; n<generatedSQ.linearArraySize(); ++n) partials[n].clear();

	/*
	 * Currently our scattering matrix / data look as follows:
	 * 
	 * [ c11 c12 ... c1N ] [ P1 ]   [ D1 ]
	 * [ c21 c22 ... c2N ] [ P2 ] = [ D2 ]		N is number of partials
	 * [         ...     ] [ .. ]   [ .. ]		M is number of data (where M >= N)
	 * [ cM1 cM2 ... cMN ] [ PN ]   [ DM ]
	 * 
	 * ... where the coefficients in the matrix are the partial weights, P are the (unknown) partial S(Q), and D are the (known) data.
	 * 
	 * Take the matrix inverse and multiply it by the known data to generate the estimated partials.
	 */

	// Generate new partials (nPartials = nColumns)
	for (int n=0; n<A_.nColumns(); ++n)
	{
		// Add in contribution from each datset (row).
		// We multiply any contribution by the stored factor (in the RefList's data variable).
		for (int m=0; m<data_.nItems(); ++m)
		{
			partials[n].addInterpolated(data_[m], inverseA_.value(n, m) * factors_[m]);
		}
	}
}

// Return if the scattering matrix is underdetermined
bool ScatteringMatrix::underDetermined() const
{
	return (data_.nItems() < A_.nColumns());
}

/*
 * Construction
 */

// Initialise from supplied list of AtomTypes
void ScatteringMatrix::initialise(const List<AtomType>& types, Array2D<XYData>& generatedSQ, const char* objectNamePrefix, const char* groupName)
{
	// Clear coefficients matrix and its inverse_, and empty our typePairs_ and data_ lists
	A_.clear();
	inverseA_.clear();
	data_.clear();
	typePairs_.clear();

	// Copy atom types
	for (AtomType* at1 = types.first(); at1 != NULL; at1 = at1->next)
	{
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next)
		{
			Pair<AtomType*, AtomType*>* pair = typePairs_.add();
			pair->a = at1;
			pair->b = at2;
		}
	}

	// Create partials array
	generatedSQ.initialise(types.nItems(), types.nItems(), true);
	XYData* partials = generatedSQ.linearArray();
	int index = 0;
	for (Pair<AtomType*,AtomType*>* pair = typePairs_.first(); pair != NULL; pair = pair->next)
	{
		partials[index].setName(CharString("ScatteringMatrixPartial-%s-%s-%s.sq", pair->a->name(), pair->b->name(), groupName));
		partials[index].setObjectName(CharString("%s//GeneratedSQ//%s//%s-%s", objectNamePrefix, groupName, pair->a->name(), pair->b->name()));
		++index;
	}
}

// Finalise
bool ScatteringMatrix::finalise()
{
	// Check that we have the correct number of reference data to be able to invert the matrix
	if (data_.nItems() < A_.nColumns())
	{
		Messenger::error("Can't finalise this scattering matrix, since there are not enough reference data (%i) compared to rows in the matrix (%i).\n", data_.nItems(), A_.nColumns());
		return false;
	}

	inverseA_ = A_;
	if (!DUQMath::pseudoinverse(inverseA_)) return false;

	return true;
}

// Add reference data
bool ScatteringMatrix::addReferenceData(XYData& data, Weights& weights, double factor)
{
	// Make sure that there are valid scattering weights in the supplied Data
	if (!weights.isValid())
	{
		return Messenger::error("Reference data '%s' does not have valid scattering weights.\n", data.name());
	}

	// Extend the scattering matrix by one row
	A_.addRow(typePairs_.nItems());
	const int rowIndex = A_.nRows() - 1;

	// Set coefficients in A_
	const int nUsedTypes = weights.nUsedTypes();
	AtomTypeList& usedTypes = weights.atomTypes();
	for (int n=0; n<nUsedTypes; ++n)
	{
		for (int m=n; m<nUsedTypes; ++m)
		{
			int colIndex = pairIndex(usedTypes.atomType(n), usedTypes.atomType(m));
			if (colIndex == -1)
			{
				Messenger::error("Weights associated to reference data contain one or more unknown AtomTypes ('%s' and/or '%s').\n", usedTypes.atomType(n)->name(), usedTypes.atomType(m)->name());
				return false;
			}

			// Now have the local column index of the AtomType pair in our matrix A_...
			A_.ref(rowIndex, colIndex) = weights.fullWeight(n, m) * factor;
		}
	}

	// Add reference data and its associated factor
	data_.add(data);
	factors_.add(factor);

	return true;
}

// Add reference partial data between specified AtomTypes
bool ScatteringMatrix::addPartialReferenceData(XYData& data, AtomType* at1, AtomType* at2, double weight, double factor)
{
	// Extend the scattering matrix by one row
	A_.addRow(typePairs_.nItems());
	const int rowIndex = A_.nRows() - 1;

	int colIndex = pairIndex(at1, at2);
	if (colIndex == -1)
	{
		Messenger::error("Weights associated to reference data contain one or more unknown AtomTypes ('%s' and/or '%s').\n", at1->name(), at2->name());
		return false;
	}

	// Now have the local column index of the AtomType pair in our matrix A_...
	A_.setRow(rowIndex, 0.0);
	A_.ref(rowIndex, colIndex) = weight * factor;

	// Add reference data and its associated factor
	data_.add(data);
	factors_.add(factor);

	return true;
}
