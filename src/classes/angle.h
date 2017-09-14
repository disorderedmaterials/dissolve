/*
	*** Angle Definition
	*** src/classes/angle.h
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_ANGLE_H
#define DUQ_ANGLE_H

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
	 * Basic Data
	 */
	private:
	// Molecule in which this Angle exists
	Molecule* molecule_;
	
	public:
	// Set Molecule in which this Angle exists
	void setMolecule(Molecule* parent);
	// Return Molecule in which this Angle exists
	Molecule* molecule() const;


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
	 * Connections
	 */
	private:
	// Number of Atoms attached to termini
	int nAttached_[2];
	// Arrays of Atoms (in)directly attached to termini
	Atom** attached_[2];

	public:
	// Set equilibrium angle
	void setEquilibrium(double rEq);
	// Return equlibrium angle
	double equilibrium() const;
	// Set force constant
	void setForceConstant(double k);
	// Return force constant
	double forceConstant() const;
	// Create attached Atom array
	void createAttachedAtomArray(int terminus, int size);
	// Set attached Atoms for terminus specified
	void setAttachedAtoms(int terminus, const RefList<Atom,int>& atoms);
	// Return number of attached Atoms for terminus specified
	int nAttached(int terminus) const;
	// Return array of attached Atoms for terminus specified
	Atom** attached(int terminus) const;


	/*
	 * Energy / Force
	 */
	private:
	// Species Angle from which parameters are to be taken
	SpeciesAngle* speciesAngle_;

	public:
	// Set SpeciesAngle reference
	void setSpeciesAngle(SpeciesAngle* angleRef);
	// Return energy for specified angle
	double energy(double angleInDegrees) const;
	// Return force multiplier for specified angle
	double force(double angleInDegrees) const;
};

#endif
