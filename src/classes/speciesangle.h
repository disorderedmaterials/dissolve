/*
	*** SpeciesAngle Definition
	*** src/classes/speciesangle.h
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

/*
 * Angle Definition
 */
class SpeciesAngle : public SpeciesIntra, public ListItem<SpeciesAngle>
{
	public:
	// Constructor
	SpeciesAngle();
	// Destructor
	~SpeciesAngle();


	/*
	 * Atom Information
	 */
	private:
	// First Atom in Angle
	SpeciesAtom* i_;
	// Second (central) Atom in Angle
	SpeciesAtom* j_;
	// Third Atom in Angle
	SpeciesAtom* k_;

	public:
	// Set Atoms involved in Angle
	void setAtoms(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k);
	// Return first Atom involved in Angle
	SpeciesAtom* i() const;
	// Return second (central) Atom involved in Angle
	SpeciesAtom* j() const;
	// Return third Atom involved in Angle
	SpeciesAtom* k() const;
	// Return index (in parent Species) of first Atom
	int indexI() const;
	// Return index (in parent Species) of second (central) Atom
	int indexJ() const;
	// Return index (in parent Species) of third Atom
	int indexK() const;
	// Return whether Atoms in Angle match those specified
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
	// Return number of parameters required for functional form
	static int nFunctionParameters(AngleFunction func);

	private:
	// Interaction functional form
	AngleFunction form_;

	public:
	// Set functional form of interaction
	void setForm(SpeciesAngle::AngleFunction form);
	// Return functional form of interaction
	SpeciesAngle::AngleFunction form();
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
