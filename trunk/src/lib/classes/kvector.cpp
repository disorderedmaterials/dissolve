/*
	*** KVector
	*** src/lib/classes/kvector.cpp
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

#include "classes/kvector.h"

/*!
 * \brief Constructor
 * \details Constructor for KVector. 
 */
KVector::KVector()
{
	magnitude_ = 0.0;
	prev = NULL;
	next = NULL;
}

/*!
 * \brief Destructor
 * \details Destructor for KVector. 
 */
KVector::~KVector()
{
}

/*
// KVector Data
*/

/*!
 * \brief Set kvector data
 */

void KVector::set(int h, int k, int l, double magnitude)
{
	hkl_.set(h, k, l);
	magnitude_ = magnitude;
}

/*!
 * \brief Return integer hkl indices
 */
const Vec3<int>& KVector::hkl() const
{
	return hkl_;
}

/*!
 * \brief Return magnitude
 */
double KVector::magnitude()
{
	return magnitude_;
}
