/*
	*** Forcefield Data
	*** src/data/ff.h
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

#ifndef DISSOLVE_FORCEFIELD_H
#define DISSOLVE_FORCEFIELD_H

#include "data/elements.h"

// Forward Declarations
class Species;

// Forcefield Base Class
class Forcefield : public Elements
{
	/*
	 * Definition
	 */
	public:
	// Return name of Forcefield
	virtual const char* name() = 0;


	/*
	 * Term Generation
	 */
	public:
	// Create and assign suitable AtomTypes for the supplied Species
	virtual bool createAtomTypes(Species* sp, CoreData& coreData) = 0;
	// Generate missing bond terms for the supplied Species
	virtual bool generateBondTerms(Species* sp) = 0;
	// Generate missing angle terms for the supplied Species
	virtual bool generateAngleTerms(Species* sp) = 0;
	// Generate missing torsion terms for the supplied Species
	virtual bool generateTorsionTerms(Species* sp) = 0;


	/*
	 * Atom Environment Helpers
	 */
	protected:
	// 
};

#endif
