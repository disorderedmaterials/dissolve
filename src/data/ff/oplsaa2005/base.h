/*
	*** OPLSAA (2005) Base Forcefield
	*** src/data/ff/oplsaa2005/base.h
	Copyright T. Youngs 2019

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

#ifndef DISSOLVE_FORCEFIELD_OPLSAA_BASE_H
#define DISSOLVE_FORCEFIELD_OPLSAA_BASE_H

#include "data/ff.h"

// Forward Declarations
/* none */

// SPC/Fw Forcefield
class OPLSAA2005BaseForcefield : public Forcefield
{
	public:
	// Constructor / Destructor
	OPLSAA2005BaseForcefield();
	virtual ~OPLSAA2005BaseForcefield();


	/*
	 * Definition
	 */
	public:
	// Return short-range interaction style for AtomTypes
	Forcefield::ShortRangeType shortRangeType() const;
};

#endif
