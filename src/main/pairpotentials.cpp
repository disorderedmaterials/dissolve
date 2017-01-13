/*
	*** dUQ - PairPotentials
	*** src/main/pairpotentials.cpp
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

#include "main/duq.h"
#include "classes/atomtype.h"

// Set maximum distance for tabulated PairPotentials
void DUQ::setPairPotentialRange(double range)
{
	pairPotentialRange_ = range;
	pairPotentialRangeSquared_ = range*range;
}

// Return maximum distance for tabulated PairPotentials
double DUQ::pairPotentialRange() const
{
	return pairPotentialRange_;
}

// Return maximum squared distance for tabulated PairPotentials
double DUQ::pairPotentialRangeSquared() const
{
	return pairPotentialRangeSquared_;
}

// Set width of PairPotential tail over which to truncate
void DUQ::setPairPotentialTruncationWidth(double width)
{
	pairPotentialTruncationWidth_ = width;
}

// Return width of PairPotential tail over which to truncate
double DUQ::pairPotentialTruncationWidth() const
{
	return pairPotentialTruncationWidth_;
}

// Set delta to use in tabulations
void DUQ::setPairPotentialDelta(double delta)
{
	pairPotentialDelta_ = delta;
}

// Return delta to use in tabulations
double DUQ::pairPotentialDelta() const
{
	return pairPotentialDelta_;
}

// Set whether atomic charges are included in generated PairPotentials
void DUQ::setPairPotentialIncludesCharges(bool b)
{
	pairPotentialIncludesCharges_ = b;
}

// Return whether atomic charges are included in generated PairPotentials
bool DUQ::pairPotentialIncludesCharges()
{
	return pairPotentialIncludesCharges_;
}

// Return index of specified PairPotential
int DUQ::indexOf(PairPotential* pp)
{
	return pairPotentials_.indexOf(pp);
}

// Return number of defined PairPotentials
int DUQ::nPairPotentials() const
{
	return pairPotentials_.nItems();
}

// Add new pair potential to list
PairPotential* DUQ::addPairPotential(AtomType* at1, AtomType* at2)
{
	PairPotential* pp = pairPotentials_.add();
	pp->setAtomTypes(at1, at2);

	return pp;
}

// Return first PaiPotential in list
PairPotential* DUQ::pairPotentials() const
{
	return pairPotentials_.first();
}

// Return nth PairPotential in list
PairPotential* DUQ::pairPotential(int n)
{
	return pairPotentials_[n];
}

// Return whether specified PairPotential is defined
PairPotential* DUQ::hasPairPotential(AtomType* at1, AtomType* at2) const
{
	for (PairPotential* pot = pairPotentials_.first(); pot != NULL; pot = pot->next)
	{
		if ((pot->atomTypeI() == at1) && (pot->atomTypeJ() == at2)) return pot;
		if ((pot->atomTypeI() == at2) && (pot->atomTypeJ() == at1)) return pot;
	}
	return NULL;
}

// Save all PairPotentials
bool DUQ::savePairPotentials(const char* baseName) const
{
	bool result = true;

	for (PairPotential* pp = pairPotentials_.first(); pp != NULL; pp = pp->next)
	{
		// Generate filename
		result = pp->save(CharString("%s-%s-%s.pp", baseName, pp->atomTypeNameI(), pp->atomTypeNameJ()));
		if (!result) break;
	}
	
	return result;
}

// Return map for PairPotentials
const PotentialMap& DUQ::potentialMap()
{
	return potentialMap_;
}
