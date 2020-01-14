/*
	*** Atomic Mass Data
	*** src/data/atomicmass.h
	Copyright T. Youngs 2012-2020

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_ATOMICMASSDATA_H
#define DISSOLVE_ATOMICMASSDATA_H

#include "data/elements.h"

// Elemental (natural) atomic mass
class AtomicMassData : public ElementReference
{
	public:
	// Constructor
	AtomicMassData(int z, double massMin, double massMax = -1.0);

	private:
	// Mass of element
	double mass_;

	public:
	// Return mass of element
	double mass() const;
};

// Atomic Mass Helper Class
class AtomicMass : public Elements
{
	private:
	// Return mass data for specified Z
	static const AtomicMassData& massData(int Z);

	public:
	// Return atomic mass for specified Z
	static double mass(int Z);
	// Return atomic mass for specified Element
	static double mass(Element* element);
	// Return reduced mass for specified pair of Z's
	static double reducedMass(int Z1, int Z2);
	// Return reduced mass for specified pair of Elements
	static double reducedMass(Element* e1, Element* e2);
};

#endif
