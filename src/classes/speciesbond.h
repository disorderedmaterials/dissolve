/*
	*** SpeciesBond Definition
	*** src/classes/speciesbond.h
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

#ifndef DUQ_SPECIESBOND_H
#define DUQ_SPECIESBOND_H

#include "templates/list.h"
#include "templates/listitem.h"
#include "templates/reflist.h"

#define MAXBONDPARAMS 4

// Forward Declarations
class SpeciesAtom;
class Species;
class ProcessPool;

// SpeciesBond Definition
class SpeciesBond : public ListItem<SpeciesBond>
{
	public:
	// Constructor
	SpeciesBond();
	// Destructor
	~SpeciesBond();


	/*
	 * Parent Data
	 */
	private:
	// Parent Species
	Species* parent_;
	
	
	public:
	// Set parent Species
	void setParent(Species* parent);
	// Return parent Species
	Species* parent() const;


	/*
	 * SpeciesAtom Information
	 */
	private:
	// First SpeciesAtom in SpeciesBond
	SpeciesAtom* i_;
	// Second SpeciesAtom in SpeciesBond
	SpeciesAtom* j_;

	public:
	// Set SpeciesAtoms involved in SpeciesBond
	void setAtoms(SpeciesAtom* i, SpeciesAtom* j);
	// Return first SpeciesAtom involved in SpeciesBond
	SpeciesAtom* i() const;
	// Return second SpeciesAtom involved in SpeciesBond
	SpeciesAtom* j() const;
	// Return the 'other' SpeciesAtom in the SpeciesBond
	SpeciesAtom* partner(SpeciesAtom* i) const;
	// Return index (in parent Species) of first SpeciesAtom
	int indexI() const;
	// Return index (in parent Species) of second SpeciesAtom
	int indexJ() const;
	// Return whether SpeciesAtoms in SpeciesBond match those specified
	bool matches(SpeciesAtom* i, SpeciesAtom* j) const;


	/*
	 * Interaction Parameters
	 */
	public:
	// Bond functional forms
	enum BondFunction
	{
		HarmonicForm,
		nBondFunctions
	};
	// Convert string to functional form
	static BondFunction bondFunction(const char* s);
	// Return functional form text
	static const char* bondFunction(BondFunction func);
	// Return number of parameters required for functional form
	static int nFunctionParameters(BondFunction func);

	private:
	// Interaction functional form
	BondFunction form_;
	// Parameters for interaction
	double parameters_[MAXBONDPARAMS];
	// Number of SpeciesAtoms attached to termini
	int nAttached_[2];
	// Arrays of SpeciesAtoms (in)directly attached to termini
	SpeciesAtom** attachedAtoms_[2];
	// Arrays of atom indices (in)directly attached to termini
	int* attachedAtomIndices_[2];

	public:
	// Set functional form of interaction
	void setForm(SpeciesBond::BondFunction form);
	// Return functional form of interaction
	SpeciesBond::BondFunction form();
	// Set nth parameter
	void setParameter(int id, double value);
	// Return nth parameter
	double parameter(int id) const;
	// Create attached SpeciesAtom array
	void createAttachedAtomArrays(int terminus, int size);
	// Set attached SpeciesAtoms for terminus specified
	void setAttachedAtoms(int terminus, const RefList<SpeciesAtom,int>& atoms);
	// Return number of attached SpeciesAtoms for terminus specified
	int nAttached(int terminus) const;
	// Return array of attached SpeciesAtoms for terminus specified
	SpeciesAtom** attachedAtoms(int terminus) const;
	// Return array of attached indices for terminus specified
	int* attachedIndices(int terminus) const;
	// Return energy for specified distance
	double energy(double distance) const;
	// Return force multiplier for specified distance
	double force(double distance) const;


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, const List<SpeciesAtom>& atoms);
};

#endif
