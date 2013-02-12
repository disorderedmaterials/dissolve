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

#include "classes/bond.h"
#include "base/dnchar.h"
#include "templates/list.h"
#include "templates/reflist.h"
#include "templates/vector3.h"

// Forward Declarations
class Grain;
class AtomType;

/*!
 * \brief Atom Definition
 * \details 
 */
class Atom : public ListItem<Atom>
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
	// Atomic Element
	int element_;
	// Coordinates
	Vec3<double> r_;
	// Charge (if contained in file)
	double charge_;
	// Assigned AtomType
	AtomType* atomType_;
	// Index of Atom (in parent's List *or* full model)
	int index_;

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
	// Set charge of Atom
	void setCharge(double charge);
	// Return charge of Atom
	double charge() const;
	// Set AtomType of Atom
	void setAtomType(AtomType* at);
	// Return AtomType of Atom
	AtomType* atomType() const;
	// Set List index (0->[N-1])
	void setIndex(int id);
	// Return List index (0->[N-1])
	int index() const;
	// Return 'user' index (1->N)
	int userIndex() const;
	// Copy properties from supplied Atom
	void copyProperties(const Atom* source);
	///@}


	/*!
	 * \name Bond Information
	 */
	///@{
	private:
	// List of Bonds which this Atom participates in
	RefList<Bond,int> bonds_;
	
	public:
	// Add Bond reference
	void addBond(Bond* b);
	// Remove Bond reference
	void removeBond(Bond* b);
	// Clear all Bond references
	void clearBonds();
	// Return number of bonds
	int nBonds() const;
	// Return first Bond reference
	RefListItem<Bond,int>* bonds();
	// Return whether Bond to specified Atom exists
	Bond* hasBond(Atom* j);
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
	bool broadcast(const List<AtomType>& atomTypes);
	///@}
};

#endif
