/*
	*** OPLSAA (2005) Aromatics Forcefield
	*** src/data/ff/oplsaa2005/aromatics.h
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

#ifndef DISSOLVE_FORCEFIELD_OPLSAA_AROMATICS_H
#define DISSOLVE_FORCEFIELD_OPLSAA_AROMATICS_H

#include "data/ff/oplsaa2005/base.h"

// Forward Declarations
/* none */

// SPC/Fw Forcefield
class Forcefield_OPLSAA2005_Aromatics : public OPLSAA2005BaseForcefield
{
	public:
	// Constructor / Destructor
	Forcefield_OPLSAA2005_Aromatics();
	~Forcefield_OPLSAA2005_Aromatics();


	/*
	 * Definition
	 */
	public:
	// Return name of Forcefield
	const char* name() const;
	// Return description for Forcefield
	const char* description() const;
};

#endif
