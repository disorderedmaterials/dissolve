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
#include "classes/data.h"
#include "classes/weights.h"

// Constructor
ScatteringMatrix::ScatteringMatrix()
{
}

/*
 * Data
 */

// Return number of reference AtomType pairs
int ScatteringMatrix::nPairs()
{
	return typePairs_.nItems();
}

// Return index of specified AtomType pair
int ScatteringMatrix::pairIndex(AtomType* typeI, AtomType* typeJ)
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
void ScatteringMatrix::print()
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
	RefListIterator<Data,bool> dataIterator(data_);
	int row = 0;
	while (Data* data = dataIterator.iterate())
	{
		line.clear();
		for (int n=0; n<A_.nColumns(); ++n) line.strcatf("%10f ", A_.value(row, n));
		Messenger::print("%s  %s\n", line.get(), data->name());
		++row;
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
		// Add in contribution from each datset (row)
		RefListIterator<Data,bool> dataIterator(data_);
		int m = 0;
		while (Data* data = dataIterator.iterate())
		{
			partials[n].addInterpolated(data->data(), inverseA_.value(n, m));
			++m;
		}
	}
}

/*
 * Construction
 */

// Initialise from supplied list of AtomTypes
void ScatteringMatrix::initialise(const List<AtomType>& types, Array2D<XYData>& generatedSQ, const char* objectNamePrefix)
{
	// Copy atom types
	typePairs_.clear();
	for (AtomType* at1 = types.first(); at1 != NULL; at1 = at1->next)
	{
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next)
		{
			Pair<AtomType*, AtomType*>* pair = typePairs_.add();
			pair->a = at1;
			pair->b = at2;
		}
	}

	// Create and zero coefficients matrix
	A_.initialise(typePairs_.nItems(), typePairs_.nItems());
	A_ = 0.0;
	inverseA_.clear();

	// Create partials array
	generatedSQ.initialise(types.nItems(), types.nItems(), true);
	XYData* partials = generatedSQ.linearArray();
	int index = 0;
	for (Pair<AtomType*,AtomType*>* pair = typePairs_.first(); pair != NULL; pair = pair->next)
	{
		partials[index].setName(CharString("ScatteringMatrixPartial-%s-%s.sq", pair->a->name(), pair->b->name()));
		partials[index].setObjectName(CharString("%s//GeneratedSQ//%s-%s", objectNamePrefix, pair->a->name(), pair->b->name()));
		++index;
	}
}

// Finalise
bool ScatteringMatrix::finalise()
{
	// Check that we have the correct number of reference data to be able to invert the matrix
	if (data_.nItems() != A_.nRows())
	{
		Messenger::error("Can't finalise this scattering matrix, since there are not enough reference data (%i) compared to rows in the matrix (%i).\n", data_.nItems(), A_.nColumns());
		return false;
	}

	inverseA_ = A_;
	if (!inverseA_.invert()) return false;

	return true;
}

// Add reference data
bool ScatteringMatrix::addReferenceData(Data* data, Weights& weights)
{
	// Check current list size
	if (data_.nItems() == typePairs_.nItems())
	{
		Messenger::error("Can't add reference data to ScatteringMatrix - rowsize exceeded.\n");
		return false;
	}

	// Set coefficients in A_ - size of reference data array data_ gives us our target row index
	const int row = data_.nItems(), nUsedTypes = weights.nUsedTypes();
	AtomTypeList& usedTypes = weights.atomTypes();
	for (int n=0; n<nUsedTypes; ++n)
	{
		for (int m=n; m<nUsedTypes; ++m)
		{
			int index = pairIndex(usedTypes.atomType(n), usedTypes.atomType(m));
			if (index == -1)
			{
				Messenger::error("Weights associated to reference Data contain one or more unknown AtomTypes ('%s' and/or '%s').\n", usedTypes.atomType(n)->name(), usedTypes.atomType(m)->name());
				return false;
			}

			// Now have the local column index of the AtomType pair in our matrix A_...
			A_.ref(row, index) = weights.fullWeight(n, m);
		}
	}

	// Add reference data
	data_.add(data);

	return true;
}
