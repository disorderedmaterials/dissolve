/*
	*** KVector
	*** src/lib/classes/changedata.h
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

#ifndef DUQ_KVECTOR_H
#define DUQ_KVECTOR_H

#include "templates/vector3.h"

// Forward Declarations
/* none */

/*!
 * \brief K-Vector
 */
class KVector
{
	public:
	// Constructor
	KVector();
	// Destructor
	~KVector();
	// List pointers
	KVector* next, *prev;


	/*!
	 * \name KVector Data
	 */
	private:
	// Integer hkl indices of vector
	Vec3<int> hkl_;
	// Magnitude of k-vector
	double magnitude_;

	public:
	// Set kvector data
	void set(int h, int k, int l, double magnitude);
	// Return hkl indices
	const Vec3<int>& hkl() const;
	// Return magnitude
	double magnitude();
};

#endif
