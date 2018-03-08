/*
	*** SpeciesAngle Definition
	*** src/classes/speciesangle.h
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

#ifndef DUQ_SPECIESANGLE_H
#define DUQ_SPECIESANGLE_H

#include "classes/speciesintra.h"
#include "templates/list.h"
#include "templates/listitem.h"
#include "templates/reflist.h"

// Forward Declarations
class SpeciesAtom;
class Species;
class ProcessPool;

// SpeciesAngle Definition
class SpeciesAngle : public SpeciesIntra, public ListItem<SpeciesAngle>
{
	public:
	// Constructor
	SpeciesAngle();
	// Destructor
	~SpeciesAngle();


	/*
	 * SpeciesAtom Information
	 */
	private:
	// First SpeciesAtom in interaction
	SpeciesAtom* i_;
	// Second (central) SpeciesAtom in interaction
	SpeciesAtom* j_;
	// Third SpeciesAtom in interaction
	SpeciesAtom* k_;

	public:
	// Set SpeciesAtoms involved in interaction
	void setAtoms(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k);
	// Return first SpeciesAtom
	SpeciesAtom* i() const;
	// Return second (central) SpeciesAtom
	SpeciesAtom* j() const;
	// Return third SpeciesAtom
	SpeciesAtom* k() const;
	// Return index (in parent Species) of first SpeciesAtom
	int indexI() const;
	// Return index (in parent Species) of second (central) SpeciesAtom
	int indexJ() const;
	// Return index (in parent Species) of third SpeciesAtom
	int indexK() const;
	// Return index (in parent Species) of nth SpeciesAtom
	int index(int n) const;
	// Return whether SpeciesAtom match those specified
	bool matches(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k) const;


	/*
	 * Interaction Parameters
	 */
	public:
	// Angle functional forms
	enum AngleFunction
	{
		HarmonicForm,
		nAngleFunctions
	};
	// Convert string to functional form
	static AngleFunction angleFunction(const char* s);
	// Return functional form text
	static const char* angleFunction(AngleFunction func);
	// Return functional form array
	static const char** angleFunctions();
	// Return number of parameters required for functional form
	static int nFunctionParameters(AngleFunction func);

	public:
	// Return energy for specified angle
	double energy(double angleInDegrees) const;
	// Return force multiplier for specified angle
	double force(double angleInDegrees) const;


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, const List<SpeciesAtom>& atoms);
};

#endif
