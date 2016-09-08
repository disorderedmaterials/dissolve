/*
	*** BraggPeak
	*** src/classes/braggpeak.cpp
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

#include "classes/braggpeak.h"

/*!
 * \brief Constructor
 * \details Constructor for BraggPeak. 
 */
BraggPeak::BraggPeak(double Q, int index)
{
	q_ = Q;
	index_ = index;
	nKVectors_ = 0;
}

/*!
 * \brief Destructor
 * \details Destructor for BraggPeak. 
 */
BraggPeak::~BraggPeak()
{
}

/*!
 * \brief Copy constructor
 */
BraggPeak::BraggPeak(const BraggPeak& source)
{
	(*this) = source;
}

/*!
 * \brief Operator=
 */
void BraggPeak::operator=(const BraggPeak& source)
{
	q_ = source.q_;
	index_ = source.index_;
	intensities_ = source.intensities_;
	nKVectors_ = source.nKVectors_;
}

/*
// BraggPeak Data
*/

/*!
 * \brief Initialise arrays
 */
void BraggPeak::initialise(int nTypes)
{
	intensities_.initialise(nTypes, nTypes, true);
	nKVectors_ = 0;
}

/*!
 * \brief Return Q value
 */
const double BraggPeak::q() const
{
	return q_;
}

/*!
 * \brief Return index
 * \details Return the index of the BraggPeak, so that they can be stored usefully in an OrderedPointerList.
 */
const int BraggPeak::index() const
{
	return index_;
}

/*!
 * \brief Reset stored intensities
 */
void BraggPeak::resetIntensities()
{
	intensities_ = 0.0;
	nKVectors_ = 0;
}

/*!
 * \brief Add intensity between specified atomtypes
 */
void BraggPeak::addIntensity(int typeI, int typeJ, double intensity)
{
	intensities_.ref(typeI, typeJ) += intensity;
}

/*!
 * \brief Return literal intensity between specified atom types for this peak
 */
double BraggPeak::intensity(int typeI, int typeJ)
{
	return intensities_.value(typeI, typeJ);
}

/*!
 * \brief Increment number of k-vectors by specified amount
 */
void BraggPeak::addKVectors(int count)
{
	nKVectors_ += count;
}

/*!
 * \brief Return number of k-vectors contributing to this peak
 */
int BraggPeak::nKVectors()
{
	return nKVectors_;
}
