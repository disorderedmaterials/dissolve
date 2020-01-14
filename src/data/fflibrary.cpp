/*
	*** Forcefield Library
	*** src/data/fflibrary.cpp
	Copyright T. Youngs 2019-2020

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

#include "data/fflibrary.h"
#include "data/ff/oplsaa2005/aromatics.h"
#include "data/ff/oplsaa2005/noblegases.h"
#include "data/ff/ludwig/ntf2.h"
#include "data/ff/ludwig/py5.h"
#include "data/ff/ludwig/py4oh.h"
#include "data/ff/spcfw.h"
#include "data/ff/uff.h"
#include "base/sysfunc.h"

// Static Members
List<Forcefield> ForcefieldLibrary::forcefields_;

/*
 * Private Functions
 */

// Register Forcefields for use
void ForcefieldLibrary::registerForcefields()
{
	forcefields_.own(new Forcefield_OPLSAA2005_Aromatics);
	forcefields_.own(new Forcefield_OPLSAA2005_NobleGases);
	forcefields_.own(new Forcefield_SPCFw);
	forcefields_.own(new Forcefield_UFF);
	forcefields_.own(new Forcefield_NTf2_Ludwig);
	forcefields_.own(new Forcefield_Py5_Ludwig);
	forcefields_.own(new Forcefield_Py4OH_Ludwig);
}

/*
 * Public Functions
 */

// Return list of available Forcefields
List<Forcefield>& ForcefieldLibrary::forcefields()
{
	// If the list is empty, we haven't yet constructed the list...
	if (forcefields_.nItems() == 0) registerForcefields();

	return forcefields_;
}

// Return named Forcefield, if it exists
Forcefield* ForcefieldLibrary::forcefield(const char* name)
{
	ListIterator<Forcefield> ffIterator(forcefields());
	while (Forcefield* ff = ffIterator.iterate()) if (DissolveSys::sameString(ff->name(), name)) return ff;

	return NULL;
}
