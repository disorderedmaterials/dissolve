/*
	*** BraggPeak
	*** src/lib/classes/braggpeak.h
	Copyright T. Youngs 2012-2014

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

#include "templates/vector3.h"
#include "templates/array.h"

// Forward Declarations
/* none */

/*!
 * \brief BraggPeak Class
 * \details BraggPeak acts as a 'bin' for collecting contributions arising from a set of KVectors which occur at the same Q value.
 */
class BraggPeak
{
	public:
	// Constructor
	BraggPeak(double Q = 0.0, int index = 0);
	// Destructor
	~BraggPeak();
	// Copy constructor
	BraggPeak(const BraggPeak& source);
	// Operator=
	void operator=(const BraggPeak& source);


	/*!
	 * \name BraggPeak Data
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
	void initialise(int nTypes);
	// Return Q value of peak
	const double q() const;
	// Return index
	const int index() const;
	// Reset stored intensities
	void resetIntensities();
	// Add intensity from k-vector
	void addIntensity(int typeI, int typeJ, double intensity);
	// Return literal intensity between specified atom types for this peak
	double intensity(int typeI, int typeJ);
	// Return broadened intensity between specified atom types for this peak, at the Q value specified
	double intensity(int typeI, int typeJ, double qValue);
};

#endif
