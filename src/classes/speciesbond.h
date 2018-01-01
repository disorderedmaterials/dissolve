/*
	*** SpeciesBond Definition
	*** src/classes/speciesbond.h
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

#ifndef DUQ_SPECIESBOND_H
#define DUQ_SPECIESBOND_H

#include "classes/speciesintra.h"
#include "templates/list.h"
#include "templates/listitem.h"
#include "templates/reflist.h"

// Forward Declarations
class SpeciesAtom;
class Species;
class ProcessPool;

// SpeciesBond Definition
class SpeciesBond : public SpeciesIntra, public ListItem<SpeciesBond>
{
	public:
	// Constructor
	SpeciesBond();
	// Destructor
	~SpeciesBond();


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

	public:
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
