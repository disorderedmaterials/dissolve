/*
	*** Atom Definition
	*** src/lib/classes/atom.h
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

#ifndef DUQ_ATOM_H
#define DUQ_ATOM_H

#include "templates/vector3.h"

// Forward Declarations
class AtomType;
class Cell;
class Grain;
class Molecule;

/*!
 * \brief Atom Definition
 * \details 
 */
class Atom
{
	public:
	// Constructor
	Atom();
	// Destructor
	~Atom();


	/*!
	 * \name Properties
	 */
	///@{
	private:
	// Coordinates
	Vec3<double> r_;
	// Assigned AtomType index
	int atomTypeIndex_;
	// Index of Atom
	int index_;
	// Molecule which this atom is in
	Molecule* molecule_;
	// Local index of Atom in source molecule
	int moleculeAtomIndex_;
	// Cell in which the atom exists
	Cell* cell_;
	// Charge (if contained in file)
	double charge_;
	// Atomic Element
	int element_;

	public:
	// Set basic atom properties
	void set(int element, double rx, double ry, double rz);
	// Set basic atom properties
	void set(int element, const Vec3<double> r);
	// Set atomic element
	void setElement(int el);
	// Return atomic element
	int element() const;
	// Return coordinates (read-only)
	const Vec3<double> &r() const;
	// Set charge of atom
	void setCharge(double charge);
	// Return charge of atom
	double charge() const;
	// Set AtomType index of atom
	void setAtomTypeIndex(int id);
	// Return AtomType of atom
	int atomTypeIndex() const;
	// Set index (0->[N-1])
	void setIndex(int id);
	// Return index (0->[N-1])
	int index() const;
	// Return 'user' index (1->N)
	int userIndex() const;
	// Set molecule and local atom index (0->[N-1])
	void setMolecule(Molecule* mol, int id);
	// Return molecule
	Molecule* molecule() const;
	// Return local atom index in molecule (0->[N-1])
	int moleculeAtomIndex() const;
	// Set cell in which the atom exists
	void setCell(Cell* cell);
	// Return cell in which the atom exists
	Cell* cell();
	// Copy properties from supplied Atom
	void copyProperties(const Atom* source);
	///@}


	/*!
	 * \name Coordinate Manipulation
	 */
	///@{
	private:
	// Grain to which this Atom belongs
	Grain* grain_;

	public:
	// Set associated Grain
	void setGrain(Grain* grain);
	// Return associated Grain
	Grain* grain() const;
	// Set coordinates
	void setCoordinates(const Vec3<double>& newr);
	// Set coordinates
	void setCoordinates(double dx, double dy, double dz);
	// Translate coordinates
	void translateCoordinates(const Vec3<double>& delta);
	// Translate coordinates
	void translateCoordinates(double dx, double dy, double dz);
	// Set coordinates with no Grain update
	void setCoordinatesNasty(const Vec3<double>& newr);
	// Translate coordinates with no Grain update
	void translateCoordinatesNasty(const Vec3<double>& delta);
	///@}


	/*!
	 * \name Parallel Comms
	 */
	///@{
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast();
	///@}
};

#endif
