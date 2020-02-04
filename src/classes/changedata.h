/*
	*** ChangeData
	*** src/classes/changedata.h
	Copyright T. Youngs 2012-2020

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

#ifndef DISSOLVE_CHANGEDATA_H
#define DISSOLVE_CHANGEDATA_H

#include "templates/vector3.h"
#include "templates/list.h"

// Forward Declarations
class Atom;
class Cell;

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
	// Stored coordinates of Atom
	Vec3<double> r_;
	// Stored Cell of Atom
	Cell* cell_;

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
