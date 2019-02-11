/*
	*** Intramolecular Base Definition
	*** src/classes/intra.cpp
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

#include "classes/intra.h"
#include <stddef.h>

// Constructor
Intra::Intra()
{
	molecule_ = NULL;
}

// Destructor
Intra::~Intra()
{
}

/*
 * Basic Data
 */

// Set Molecule that this Intra exists in
void Intra::setMolecule(Molecule* parent)
{
	molecule_ = parent;
}

// Return Molecule that this Intra exists in
Molecule* Intra::molecule() const
{
	return molecule_;
}
