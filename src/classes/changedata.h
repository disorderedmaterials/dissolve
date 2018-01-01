/*
	*** ChangeData
	*** src/classes/changedata.h
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

#ifndef DUQ_CHANGEDATA_H
#define DUQ_CHANGEDATA_H

#include "templates/vector3.h"
#include "templates/list.h"

// Forward Declarations
class Atom;

// Change Data
class ChangeData : public ListItem<ChangeData>
{
	public:
	// Constructor
	ChangeData();
	// Destructor
	~ChangeData();


	/*
	 * Target Data
	 */
	private:
	// Atom
	Atom* atom_;
	// Flag indicating whether Atom has moved
	bool moved_;
	// Current coordinates of Atom (either original or newly-accepted)
	Vec3<double> r_;

	public:
	// Set target Atom
	void setAtom(Atom* i);
	// Return target Atom
	Atom* atom();
	// Return array index of stored Atom
	int atomArrayIndex() const;
	// Update stored position, and flag as moved
	void updatePosition();
	// Revert Atom to stored position
	void revertPosition();
	// Return whether Atom has moved
	bool hasMoved();
	// Return position vector
	Vec3<double> r() const;
};

#endif
