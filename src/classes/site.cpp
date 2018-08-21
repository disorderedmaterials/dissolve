/*
	*** Site
	*** src/classes/site.cpp
	Copyright T. Youngs 2012-2018

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

#include "classes/site.h"
#include "base/messenger.h"

/*
 * Site
 */

// Constructor
Site::Site()
{
	molecule_ = NULL;
}

// Destructor
Site::~Site()
{
}

// Return site origin
const Vec3<double>& Site::origin() const
{
	return origin_;
}

// Return Molecule to which site is related (if relevant)
const Molecule* Site::molecule() const
{
	return molecule_;
}

// Return whether local axes are present
bool Site::hasAxes() const
{
	return false;
}

// Return local axes
const Matrix3& Site::axes() const
{
	static Matrix3 dummy;
	Messenger::warn("Returning empty axes for this Site, since it doesn't have any.\n");
	return dummy;
}

/*
 * Site with Axes
 */

// Constructor
OrientedSite::OrientedSite() : Site()
{
}

// Destructor
OrientedSite::~OrientedSite()
{
}

// Return whether local axes are present
bool OrientedSite::hasAxes() const
{
	return true;
}

// Return local axes
const Matrix3& OrientedSite::axes() const
{
	return axes_;
}
