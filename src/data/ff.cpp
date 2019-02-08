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
#include "classes/box.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"

/*
 * Atom Environment Helpers
 */

// Calculate and return the geometry of the specified SpeciesAtom
Forcefield::AtomGeometry Forcefield::geometryOfAtom(SpeciesAtom* i) const
{
	AtomGeometry result = nAtomGeometries;
	double angle, largest;
	SpeciesBond* b1, *b2;
	SpeciesAtom* h, *j;
// 	RefListItem<SpeciesBond,int>* bref1, *bref2;

	// Work based on the number of bound atoms
	switch (i->nBonds())
	{
		// 'Simple' cases first
		case (0):
			result = Forcefield::UnboundGeometry;
			break;
		case (1):
			result = Forcefield::TerminalGeometry;
			break;
		case (5):
			result = Forcefield::TrigonalBipyramidalGeometry;
			break;
		case (6):
			result = Forcefield::OctahedralGeometry;
			break;
		// For the remaining types, take averages of bond angles about the atom
		case (2):
			h = i->bond(0)->partner(i);
			j = i->bond(1)->partner(i);
			angle = NonPeriodicBox::angleInDegrees(h->r(), i->r(), j->r());
			if (angle > 150.0) result = Forcefield::LinearGeometry;
// 			else if ((angle > 100.0) && (angle < 115.0)) result = Forcefield::TetrahedralGeometry;
			else result = Forcefield::TetrahedralGeometry;
			break;
		case (3):
// 			bref1 = bonds();
// 			bref2 = bonds()->next;
// 			b1 = bref1->item;
// 			b2 = bref2->item;
// 			angle = parent_->angle(b1->partner(this),this,b2->partner(this));
// 			largest = angle;
// 			b2 = bref2->next->item;
// 			angle = parent_->angle(b1->partner(this),this,b2->partner(this));
// 			if (angle > largest) largest = angle;
// 			b1 = bref1->next->item;
// 			angle = parent_->angle(b1->partner(this),this,b2->partner(this));
// 			if (angle > largest) largest = angle;
// 			if (largest > 170.0) result = Forcefield::TShapeGeometry;
// 			else if ((largest > 115.0) && (largest < 125.0)) result = Forcefield::TrigPlanarGeometry;
// 			else if ((largest < 115.0) && (largest > 100.0)) result = Forcefield::TetrahedralGeometry;
			// Get largest of the three angles around the central atom
			h = i->bond(0)->partner(i);
			j = i->bond(1)->partner(i);
			angle = NonPeriodicBox::angleInDegrees(h->r(), i->r(), j->r());
			largest = angle;
			j = i->bond(2)->partner(i);
			angle = NonPeriodicBox::angleInDegrees(h->r(), i->r(), j->r());
			if (angle > largest) largest = angle;
			h = i->bond(1)->partner(i);
			angle = NonPeriodicBox::angleInDegrees(h->r(), i->r(), j->r());
			if (angle > largest) largest = angle;
			if (largest > 150.0) result = Forcefield::TShapeGeometry;
			else if ((largest > 115.0) && (largest < 125.0)) result = Forcefield::TrigonalPlanarGeometry;
// 			else if ((largest < 115.0) && (largest > 100.0)) result = Forcefield::TetrahedralGeometry;
			else result = Forcefield::TetrahedralGeometry;
			break;
		case (4):
			// Two possibilities - tetrahedral or square planar. Tetrahedral will have an
			// average of all angles of ~ 109.5, for square planar (1/6) * (4*90 + 2*180) = 120
			angle = 0.0;
			for (int n = 0; n < i->nBonds(); ++n)
			{
				h = i->bond(n)->partner(i);
				for (int m = n+1; m < i->nBonds(); ++m)
				{
					j = i->bond(m)->partner(i);
					angle += NonPeriodicBox::angleInDegrees(h->r(), i->r(), j->r());
				}
			}
			angle /= 6.0;
			if ((angle > 100.0) && (angle < 115.0)) result = Forcefield::TetrahedralGeometry;
// 			else if ((angle >= 115.0) && (angle < 125.0)) result = Forcefield::SquarePlanarGeometry;
			else result = Forcefield::SquarePlanarGeometry;
			break;
	}

	return result;
}

// Return whether the specified SpeciesAtom exists in the specified geometry
bool Forcefield::isAtomGeometry(SpeciesAtom* i, AtomGeometry geom) const
{
	return (geometryOfAtom(i) == geom);
}

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

	// Check sums - a supplied value of -1 means 'any number'
	if ((nSingle != -1) && (nSingle != actualNSingle)) return false;
	if ((nDouble != -1) && (nDouble != actualNDouble)) return false;
	if ((nTriple != -1) && (nTriple != actualNTriple)) return false;
	if ((nQuadruple != -1) && (nQuadruple != actualNQuadruple)) return false;
	if ((nAromatic != -1) && (nAromatic != actualNAromatic)) return false;

	return true;
}

// Return whether the specified atom is bound to a specific element (and count thereof)
bool Forcefield::isBoundTo(const SpeciesAtom* i, Element* element, const int count, bool allowMoreThanCount) const
{
	int found = 0;

	RefListIterator<SpeciesBond,int> bondIterator(i->bonds());
	while (SpeciesBond* bond = bondIterator.iterate()) if (bond->partner(i)->element() == element) ++found;

	return (found < count ? false : (found == count ? true : allowMoreThanCount));
}

// Guess and return oxidation state for the specified SpeciesAtom
int Forcefield::guessOxidationState(const SpeciesAtom* i) const
{
	/*
	 * Approximate the OS of the supplied atom by considering its local environment.
	 * We won't consider the whole molecule, and will assume the following rules in addition to the standard:
	 *   - A singly-bound Oxygen is considered to be -1 (which effectively includes it's 'R' group
	 *   - An R-group is considered to be +1
	 */
	int osBound = 0;

	// Keep track of the number of bound elements that are the same as our own, as a crude check for elemental environments (OS == 0)
	int nSameElement = 0;

	RefListIterator<SpeciesBond,int> bondIterator(i->bonds());
	while (SpeciesBond* bond = bondIterator.iterate())
	{
		Element* element = bond->partner(i)->element();
		switch (element->Z())
		{
			// Group 1A - Alkali earth metals (includes Hydrogen)
			case (ELEMENT_H):
			case (ELEMENT_LI):
			case (ELEMENT_NA):
			case (ELEMENT_K):
			case (ELEMENT_RB):
			case (ELEMENT_CS):
			case (ELEMENT_FR):
				osBound += 1;
				break;
			// Group 2A - Alkaline earth metals
			case (ELEMENT_BE):
			case (ELEMENT_MG):
			case (ELEMENT_CA):
			case (ELEMENT_SR):
			case (ELEMENT_BA):
			case (ELEMENT_RA):
				osBound += 1;
				break;
			// Oxygen
			case (ELEMENT_O):
				if (bond->bondType() == SpeciesBond::DoubleBond) osBound -= 2;
				else osBound -= 1;
				break;
			// Halogens (F, Cl, Br, I)
			case (ELEMENT_F):
			case (ELEMENT_CL):
			case (ELEMENT_BR):
			case (ELEMENT_I):
				osBound -= 1;
				break;
			default:
				break;
		}

		// Check for same element
		if (element == i->element()) ++ nSameElement;
	}

	// Return the negative of the bound OS, or zero if we were only bound to the same element as ourselves
	return (nSameElement == i->nBonds() ? 0 : -osBound);
}
