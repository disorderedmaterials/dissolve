/*
	*** BraggPeak
	*** src/classes/braggpeak.h
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

#ifndef DUQ_BRAGGPEAK_H
#define DUQ_BRAGGPEAK_H

#include "templates/array2d.h"
#include "templates/mpilistitem.h"
#include "base/genericitembase.h"

// Forward Declarations
/* none */

// BraggPeak Class
class BraggPeak : public MPIListItem<BraggPeak>,  public GenericItemBase
{
	/*
	 *  BraggPeak acts as a 'bin' for collecting contributions arising from a set of KVectors which occur at the same Q value.
	 */
	public:
	// Constructor
	BraggPeak();
	// Destructor
	~BraggPeak();
	// Copy constructor
	BraggPeak(const BraggPeak& source);
	// Operator=
	void operator=(const BraggPeak& source);


	/*
	 * Data
	 */
	private:
	// Q-position of peak
	double q_;
	// Integer index (derived from q_)
	int index_;
	// Intensity contributions from atomtype pairs
	Array2D<double> intensities_;
	// Number of k-vectors which contributed to the intensity (for normalisation)
	int nKVectors_;

	public:
	// Initialise arrays
	void initialise(double q, int index, int nTypes);
	// Return Q value of peak
	double q() const;
	// Set index 
	void setIndex(int index);
	// Return index
	int index() const;
	// Reset stored intensities
	void resetIntensities();
	// Add intensity between specified atomtypes from k-vector
	void addIntensity(int typeI, int typeJ, double intensity);
	// Scale intensities between all atom types by factor provided
	void scaleIntensity(double factor);
	// Return literal intensity between specified atom types for this peak
	double intensity(int typeI, int typeJ);
	// Increment number of k-vectors by specified amount
	void addKVectors(int count);
	// Return number of k-vectors contributing to this peak
	int nKVectors();


	/*
	 * GenericItemBase Implementations
	 */
	public:
	// Return class name
	static const char* itemClassName();


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from root to all other processes
	bool broadcast(ProcessPool& procPool, int rootRank);
	// Check item equality
	bool equality(ProcessPool& procPool);
};

#endif
