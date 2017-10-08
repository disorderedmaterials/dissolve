/*
	*** Atom Definition
	*** src/classes/atom.h
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

#ifndef DUQ_ATOM_H
#define DUQ_ATOM_H

#include "templates/vector3.h"
#include "templates/reflist.h"
#include "templates/orderedpointerdatalist.h"
#include "templates/dynamicarrayobject.h"

// Forward Declarations
class Angle;
class Bond;
class Cell;
class Grain;
class Molecule;
class ProcessPool;
class Torsion;

// Atom Definition
class Atom : public DynamicArrayObject<Atom>
{
	public:
	// Constructor
	Atom();
	// Destructor
	~Atom();


	/*
	 * Properties
	 */
	private:
	// Coordinates
	Vec3<double> r_;
	// Assigned AtomType index, local to Configuration (for partial indexing etc.)
	int localTypeIndex_;
	// Assigned master AtomType index (for pair potential indexing)
	int masterTypeIndex_;
	// Charge on atom
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
	const Vec3<double>& r() const;
	// Set charge of atom
	void setCharge(double charge);
	// Return charge of atom
	double charge() const;
	// Set local AtomType index
	void setLocalTypeIndex(int id);
	// Return local AtomType index
	int localTypeIndex() const;
	// Set master AtomType index 
	void setMasterTypeIndex(int id);
	// Return master AtomType index 
	int masterTypeIndex() const;
	// Copy properties from supplied Atom
	void copyProperties(const Atom* source);


	/*
	 * Location
	 */
	private:
	// Molecule which this Atom is in
	Molecule* molecule_;
	// Grain to which this Atom belongs (if any)
	Grain* grain_;
	// Cell in which the atom exists
	Cell* cell_;

	public:
	// Set molecule and local atom index (0->[N-1])
	void setMolecule(Molecule* mol);
	// Return molecule
	Molecule* molecule() const;
	// Set associated Grain
	void setGrain(Grain* grain);
	// Return associated Grain
	Grain* grain() const;
	// Set cell in which the atom exists
	void setCell(Cell* cell);
	// Return cell in which the atom exists
	Cell* cell() const;


	/*
	 * Connectivity
	 */
	private:
	// Reference list of Bonds in which this Atom exists
	RefList<Bond,bool> bonds_;
	// Reference list of Angles in which this Atom exists
	RefList<Angle,bool> angles_;
	// Reference list of Torsions in which this Atom exists
	RefList<Torsion,double> torsions_;
	// Ordered list of Atoms with scaled or excluded interactions
	OrderedPointerDataList<Atom,double> exclusions_;

	public:
	// Add reference to specified Bond
	void addBond(Bond* bond);
	// Return reference list of Bonds
	const RefList<Bond,bool>& bonds() const;
	// Add reference to specified Angle
	void addAngle(Angle* angle);
	// Return reference list of Angles
	const RefList<Angle,bool>& angles() const;
	// Add reference to specified Torsion
	void addTorsion(Torsion* torsion, double scaling14);
	// Return reference list of Torsions
	const RefList<Torsion,double>& torsions() const;
	// Return scaling factor to employ with specified Atom
	double scaling(Atom* j) const;


	/*
	 * Coordinate Manipulation
	 */
	public:
	// Set coordinates
	void setCoordinates(const Vec3<double>& newr);
	// Set coordinates
	void setCoordinates(double dx, double dy, double dz);
	// Translate coordinates
	void translateCoordinates(const Vec3<double>& delta);
	// Translate coordinates
	void translateCoordinates(double dx, double dy, double dz);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool);
};

#endif
