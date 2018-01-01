/*
	*** SpeciesGrain Definition
	*** src/classes/speciesgrain.h
	Copyright T. Youngs 2012-2018

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

#ifndef DUQ_SPECIESGRAIN_H
#define DUQ_SPECIESGRAIN_H

#include "classes/atom.h"
#include "base/charstring.h"
#include "templates/list.h"
#include "templates/reflist.h"

// Forward Declarations
class Species;
class SpeciesAtom;
class SpeciesBond;
class SpeciesAngle;

/*
 * SpeciesGrain Definition
 */
class SpeciesGrain : public ListItem<SpeciesGrain>
{
	public:
	// Constructor
	SpeciesGrain();
	// Destructor
	~SpeciesGrain();


	/*
	 * Basic Character
	 */
	private:
	// Parent Species
	Species* parent_;
	// Name of this SpeciesGrain
	CharString name_;
	
	public:
	// Set parent Species
	void setParent(Species* parent);
	// Return parent Species
	Species* parent() const;
	// Set name of SpeciesGrain
	void setName(const char* s);
	// Return name of SpeciesGrain
	const char* name() const;
	

	/*
	 * Atoms
	 */
	private:
	// List of Atoms (in parent Species)
	RefList<SpeciesAtom,int> atoms_;
	
	public:
	// Search current list for Atom
	bool containsAtom(SpeciesAtom* i) const;
	// Add specified Atom to list
	void addAtom(SpeciesAtom* i);
	// Remove specified Atom from list
	void removeAtom(SpeciesAtom* i);
	// Return number of Atoms in the list
	int nAtoms() const;
	// Return first Atom reference in list
	RefListItem<SpeciesAtom,int>* atoms() const;
	// Return nth Atom reference in list
	RefListItem<SpeciesAtom,int>* atom(int n);
	// Create name for SpeciesGrain from constituent SpeciesAtoms
	const char* nameFromAtoms();


	/*
	 * Connections
	 * \details Each grain maintains a list of connections it has with other grain, which arise because of intramolecular atomic terms.
	 * In this way, once a grain has been moved the resulting effect on the intramolecular terms present in the encompassing Molecule
	 * can be immediately evaluated, and the inter-grain energy corrected for PairPotential interactions which should not have been added
	 * into the total inter-grain energy in the first place (because two Atoms are bound, for example).
	 */
	private:
	// Pointers to local intra-Grain bonds
	RefList<SpeciesBond,int> internalBonds_;
	// Pointers to local intra-Grain angles
	RefList<SpeciesAngle,int> internalAngles_;
	// Pointers to local inter-Grain bonds
	RefList<SpeciesBond,int> bondConnections_;
	// Pointers to local inter-Grain angles
	RefList<SpeciesAngle,int> angleConnections_;

	public:
	// Clear all intra- and inter-Grain terms
	void clearConnections();
	// Add intra-Grain Bond
	void addInternalBond(SpeciesBond* b);
	// Return first local intra-Grain Bond
	RefListItem<SpeciesBond,int>* internalBonds() const;
	// Add intra-Grain angle
	void addInternalAngle(SpeciesAngle* a);
	// Return first local intra-Grain angle
	RefListItem<SpeciesAngle,int>* internalAngles() const;
	// Add bond connection
	void addBondConnection(SpeciesBond* b);
	// Return first bond connection
	RefListItem<SpeciesBond,int>* bondConnections() const;
	// Add angle connection
	void addAngleConnection(SpeciesAngle* a);
	// Return first angle connection
	RefListItem<SpeciesAngle,int>* angleConnections() const;


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, const List<SpeciesAtom>& atoms, const List<SpeciesBond>& bonds, const List<SpeciesAngle>& angles);
};

#endif
