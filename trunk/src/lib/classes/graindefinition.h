/*
	*** Grain Definition
	*** src/lib/classes/graindefinition.h
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

#ifndef DUQ_GRAINDEFINITION_H
#define DUQ_GRAINDEFINITION_H

#include "classes/atom.h"
#include "base/dnchar.h"
#include "templates/list.h"
#include "templates/reflist.h"

// Forward Declarations
class GrainDefinition;
class Species;

/*!
 * \brief Grain Definition
 * \details A GrainDefinition contains a list of indices of Atoms in a Species which should be treated as a 'group' of atoms
 * when calculating neighbour lists, low-Q scattering etc. TODO
 */
class GrainDefinition : public ListItem<GrainDefinition>
{
	public:
	// Constructor
	GrainDefinition();
	// Destructor
	~GrainDefinition();


	/*!
	 * \name Basic Character
	 */
	///@{
	private:
	// Parent Species
	Species* parent_;
	// Name of this GrainDefinition
	Dnchar name_;
	
	public:
	// Set parent Species
	void setParent(Species* parent);
	// Return parent Species
	Species* parent() const;
	// Set name of GrainDefinition
	void setName(const char* s);
	// Return name of GrainDefinition
	const char* name() const;
	///@}
	

	/*!
	 * \name Atoms
	 */
	///@{
	private:
	// List of Atoms (in parent Species)
	RefList<Atom,int> atoms_;
	
	public:
	// Search current list for Atom
	bool containsAtom(Atom* i) const;
	// Add specified Atom to list
	void addAtom(Atom* i);
	// Remove specified Atom from list
	void removeAtom(Atom* i);
	// Return number of Atoms in the list
	int nAtoms() const;
	// Return first Atom reference in list
	RefListItem<Atom,int>* atoms() const;
	// Return nth Atom reference in list
	RefListItem<Atom,int>* atom(int n);
	// Create name for GrainDefinition from constituent Atoms
	const char* nameFromAtoms();
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
