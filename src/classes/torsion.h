/*
	*** Torsion Definition
	*** src/classes/torsion.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_TORSION_H
#define DISSOLVE_TORSION_H

#include "templates/dynamicarrayobject.h"
#include "templates/reflist.h"

// Forward Declarations
class Atom;
class Molecule;
class SpeciesTorsion;

// Torsion Definition
class Torsion : public DynamicArrayObject<Torsion>
{
	public:
	// Constructor
	Torsion();
	// Destructor
	~Torsion();


	/*
	 * DynamicArrayObject Virtuals
	 */
	public:
	// Clear object, ready for re-use
	void clear();


	/*
	 * Atom Information
	 */
	private:
	// First Atom in Torsion
	Atom* i_;
	// Second Atom in Torsion
	Atom* j_;
	// Third Atom in Torsion
	Atom* k_;
	// Fourth Atom in Torsion
	Atom* l_;

	public:
	// Set Atoms involved in Torsion
	void setAtoms(Atom* i, Atom* j, Atom* k, Atom* l);
	// Return first Atom involved in Torsion
	Atom* i() const;
	// Return second Atom involved in Torsion
	Atom* j() const;
	// Return third Atom involved in Torsion
	Atom* k() const;
	// Return fourth Atom involved in Torsion
	Atom* l() const;
	// Return whether Atoms in Torsion match those specified
	bool matches(Atom* i, Atom* j, Atom* k, Atom* l) const;


	/*
	 * Energy / Force
	 */
	private:
	// Species Torsion from which parameters are to be taken
	SpeciesTorsion* speciesTorsion_;

	public:
	// Set SpeciesTorsion reference
	void setSpeciesTorsion(SpeciesTorsion* torsionRef);
	// Return SpeciesTorsion reference
	SpeciesTorsion* speciesTorsion();
	// Return energy for specified torsion
	double energy(double torsionInDegrees) const;
	// Return force multiplier for specified torsion
	double force(double torsionInDegrees) const;
};

#endif
