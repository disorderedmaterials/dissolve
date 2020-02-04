/*
	*** Reference Point
	*** src/gui/referencepoint.h
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

#ifndef DISSOLVE_REFERENCEPOINT_H
#define DISSOLVE_REFERENCEPOINT_H

#include "base/charstring.h"
#include "templates/listitem.h"

// Forward Declarations
/* none */

// Reference Point
class ReferencePoint : public ListItem<ReferencePoint>
{
	public:
	// Constructor / Destructor
	ReferencePoint();
	~ReferencePoint();


	/*
	 * Data
	 */
	private:
	// Suffix applied to data items from the referenced file
	CharString suffix_;
	// Restart file from which the reference point data was read
	CharString restartFile_;

	public:
	// Set suffix for data items
	void setSuffix(const char* suffix);
	// Return suffix for data items
	const char* suffix() const;
	// Set restart file from which the reference point data was read
	void setRestartFile(const char* restartFile);
	// Return restart file from which the reference point data was read
	const char* restartFile() const;
};

#endif
