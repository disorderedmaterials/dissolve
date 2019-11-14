/*
	*** Atomic Radius Data
	*** src/data/atomicradius.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_DATA_ATOMICRADIUS_H
#define DISSOLVE_DATA_ATOMICRADIUS_H

#include "data/elements.h"

// Elemental atomic radius
class AtomicRadiusData : public ElementReference
{
	public:
	// Constructor
	AtomicRadiusData(int z, double radius);

	private:
	// Radius of element
	double radius_;

	public:
	// Return radius of element
	double radius() const;
};

// Atomic Radius Helper Class
class AtomicRadius : public Elements
{
	private:
	// Return radius data for specified Z
	static const AtomicRadiusData& radiusData(int Z);

	public:
	// Return atomic radius for specified Z
	static double radius(int Z);
	// Return atomic radius for specified Element
	static double radius(Element* element);
};

#endif
