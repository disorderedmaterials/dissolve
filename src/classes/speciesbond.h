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
	// First SpeciesAtom in interaction
	SpeciesAtom* i_;
	// Second SpeciesAtom in interaction
	SpeciesAtom* j_;

	public:
	// Set SpeciesAtoms involved in interaction
	void setAtoms(SpeciesAtom* i, SpeciesAtom* j);
	// Return first SpeciesAtom
	SpeciesAtom* i() const;
	// Return second SpeciesAtom
	SpeciesAtom* j() const;
	// Return the 'other' SpeciesAtom
	SpeciesAtom* partner(SpeciesAtom* i) const;
	// Return index (in parent Species) of first SpeciesAtom
	int indexI() const;
	// Return index (in parent Species) of second SpeciesAtom
	int indexJ() const;
	// Return index (in parent Species) of nth SpeciesAtom
	int index(int n) const;
	// Return whether SpeciesAtoms match those specified
	bool matches(SpeciesAtom* i, SpeciesAtom* j) const;


	/*
	 * Interaction Parameters
	 */
	public:
	// Bond functional forms
	enum BondFunction
	{
		HarmonicForm,
		EPSRForm,
		SoftHarmonicForm,
		nBondFunctions
	};
	// Convert string to functional form
	static BondFunction bondFunction(const char* s);
	// Return functional form text
	static const char* bondFunction(BondFunction func);
	// Return functional form array
	static const char** bondFunctions();
	// Return number of parameters required for functional form
	static int nFunctionParameters(BondFunction func);

	public:
	// Set up any necessary parameters
	void setUp();
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
