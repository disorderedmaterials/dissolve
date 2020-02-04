/*
	*** Forcefield Library
	*** src/data/fflibrary.h
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

#ifndef DISSOLVE_FORCEFIELD_LIBRARY_H
#define DISSOLVE_FORCEFIELD_LIBRARY_H

#include "data/ff.h"

// Forward Declarations
/* none */

// Forcefield Library
class ForcefieldLibrary
{
	private:
	// List of all available forcefields
	static List<Forcefield> forcefields_;

	private:
	// Register Forcefields for use
	static void registerForcefields();

	public:
	// Return list of available Forcefields
	static List<Forcefield>& forcefields();
	// Return named Forcefield, if it exists
	static Forcefield* forcefield(const char* name);
};

#endif
