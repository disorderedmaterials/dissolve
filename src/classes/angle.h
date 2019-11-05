/*
	*** Angle Definition
	*** src/classes/angle.h
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

#ifndef DISSOLVE_ANGLE_H
#define DISSOLVE_ANGLE_H

#include "templates/dynamicarrayobject.h"
#include "templates/reflist.h"

// Forward Declarations
class Atom;
class Molecule;
class SpeciesAngle;

// Angle Definition
class Angle : public DynamicArrayObject<Angle>
{
	public:
	// Constructor
	Angle();
	// Destructor
	~Angle();


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
	// First Atom in Angle
	Atom* i_;
	// Second (central) Atom in Angle
	Atom* j_;
	// Third Atom in Angle
	Atom* k_;

	public:
	// Set Atoms involved in Angle
	void setAtoms(Atom* i, Atom* j, Atom* k);
	// Return first Atom involved in Angle
	Atom* i() const;
	// Return second (central) Atom involved in Angle
	Atom* j() const;
	// Return third Atom involved in Angle
	Atom* k() const;
	// Return whether Atoms in Angle match those specified
	bool matches(Atom* i, Atom* j, Atom* k) const;


	/*
	 * Energy / Force
	 */
	private:
	// Species Angle from which parameters are to be taken
	SpeciesAngle* speciesAngle_;

	public:
	// Set SpeciesAngle reference
	void setSpeciesAngle(SpeciesAngle* angleRef);
	// Return SpeciesAngle reference
	SpeciesAngle* speciesAngle() const;
	// Return energy for specified angle
	double energy(double angleInDegrees) const;
	// Return force multiplier for specified angle
	double force(double angleInDegrees) const;
};

#endif
