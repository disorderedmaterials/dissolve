/*
	*** Forcefield
	*** src/data/ff.cpp
	Copyright T. Youngs 2019

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

#include "data/ff.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"

/*
 * Atom Environment Helpers
 */

// Return whether supplied bonding pattern around the SpeciesAtom matches *exactly*
bool Forcefield::isBondPattern(const SpeciesAtom* i, const int nSingle, const int nDouble, const int nTriple, const int nQuadruple, const int nAromatic) const
{
	int actualNSingle = 0, actualNDouble = 0, actualNTriple = 0, actualNQuadruple = 0, actualNAromatic = 0;
	RefListIterator<SpeciesBond,int> bondIterator(i->bonds());
	while (SpeciesBond* bond = bondIterator.iterate())
	{
		switch (bond->bondType())
		{
			case (SpeciesBond::SingleBond):
				if (nSingle == actualNSingle) return false;
				++actualNSingle;
				break;
			case (SpeciesBond::DoubleBond):
				if (nDouble == actualNDouble) return false;
				++actualNDouble;
				break;
			case (SpeciesBond::TripleBond):
				if (nTriple == actualNTriple) return false;
				++actualNTriple;
				break;
			case (SpeciesBond::QuadrupleBond):
				if (nQuadruple == actualNQuadruple) return false;
				++actualNQuadruple;
				break;
			case (SpeciesBond::AromaticBond):
				if (nAromatic == actualNAromatic) return false;
				++actualNAromatic;
				break;
			default:
				Messenger::error("Bond has unassigned bond type.\n");
				return false;
		}
	}

	return true;
}
