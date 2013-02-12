/*
	*** Angle Definition
	*** src/lib/classes/angle.h
	Copyright T. Youngs 2012-2013

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

#include "templates/list.h"
#include "templates/reflist.h"

// Forward Declarations
class Atom;
class Molecule;
class Species;

/*!
 * \brief Angle Definition
 * \details XXX TODO
 */
class Angle : public ListItem<Angle>
{
	public:
	// Constructor
	Angle();
	// Destructor
	~Angle();


	/*!
	 * \name Basic Data
	 */
	///@{
	private:
	// Parent Species (in setup)
	Species* parent_;
	// Parent Molecule (in simulation)
	Molecule* molecule_;
	
	public:
	// Set parent Species
	void setParent(Species* parent);
	// Return parent Species
	Species* parent() const;
	// Set parent Molecule
	void setMolecule(Molecule* parent);
	// Return parent Molecule
	Molecule* molecule() const;
	///@}


	/*!
	 * \name Atom Information
	 */
	///@{
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
	// Return index (in parent Species) of first Atom
	int indexI() const;
	// Return index (in parent Species) of second (central) Atom
	int indexJ() const;
	// Return index (in parent Species) of third Atom
	int indexK() const;
	// Return whether Atoms in Angle match those specified
	bool matches(Atom* i, Atom* j, Atom* k) const;
	///@}


	/*!
	 * \name Interaction Parameters
	 */
	///@{
	private:
	// Nominal equilibrium Bond angle
	double equilibrium_;
	// Width of Gaussian defining variation
	double forceConstant_;
	// Number of Atoms attached to termini
	int nAttached_[2];
	// Arrays of Atoms (in)directly attached to termini
	Atom** attached_[2];
	// Flag specifying whether this is an inter-Grain connection
	bool interGrain_;

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
	// Set whether this Angle is interGrain
	void setInterGrain(bool b);
	// Return whether this Angle is interGrain
	bool interGrain() const;
	// Return energy for specified angle
	double energy(double angleInDegrees) const;
	// Return force multiplier for specified angle
	double force(double angleInDegrees) const;
	///@}


	/*!
	 * \name Parallel Comms
	 */
	///@{
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(const List<Atom>& atoms);
	///@}
};

#endif
