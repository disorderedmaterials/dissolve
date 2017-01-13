/*
	*** Species Atom Definition
	*** src/classes/speciesatom.h
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

#ifndef DUQ_SPECIESATOM_H
#define DUQ_SPECIESATOM_H

#include "templates/list.h"
#include "templates/vector3.h"
#include "templates/reflist.h"

// Forward Declarations
class AtomType;
class Species;
class SpeciesBond;
class SpeciesGrain;

/*
 * SpeciesAtom Definition
 */
class SpeciesAtom : public ListItem<SpeciesAtom>
{
	public:
	// Constructor
	SpeciesAtom();
	// Destructor
	~SpeciesAtom();


	/*
	 * Properties
	 */
	private:
	// Parent Species
	Species* parent_;
	// Atomic Element
	int element_;
	// Coordinates
	Vec3<double> r_;
	// Charge (if contained in file)
	double charge_;
	// Assigned AtomType
	AtomType* atomType_;
	// Index in Species
	int index_;

	public:
	// Set Species parent
	void setParent(Species* sp);
	// Return species parent
	Species* parent();
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
	void copyProperties(const SpeciesAtom* source);


	/*
	 * Containing Grain
	 */
	private:
	// Grain to which this atom belongs
	SpeciesGrain* grain_;

	public:
	// Set grain to which this atom belongs
	void setGrain(SpeciesGrain* grain);
	// Return grain to which this atom belongs
	SpeciesGrain* grain();
	
	
	/*
	 * Bond Information
	 */
	private:
	// List of bonds which this atom participates in
	RefList<SpeciesBond,int> bonds_;
	
	public:
	// Add bond reference
	void addBond(SpeciesBond* b);
	// Remove bond reference
	void removeBond(SpeciesBond* b);
	// Clear all bond references
	void clearBonds();
	// Return number of bonds
	int nBonds() const;
	// Return first bond reference
	RefListItem<SpeciesBond,int>* bonds();
	// Return whether bond to specified atom exists
	SpeciesBond* hasBond(SpeciesAtom* j);


	/*
	 * Coordinate Manipulation
	 */
	public:
	// Set coordinates
	void setCoordinates(const Vec3<double>& newr);
	// Translate coordinates
	void translateCoordinates(const Vec3<double>& delta);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, const List<AtomType>& atomTypes);
};

#endif
