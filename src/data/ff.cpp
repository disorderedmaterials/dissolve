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
#include "data/ffangleterm.h"
#include "data/ffatomtype.h"
#include "data/ffbondterm.h"
#include "data/ffimproperterm.h"
#include "data/ffparameters.h"
#include "data/fftorsionterm.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "base/sysfunc.h"

// Constructor / Destructor
Forcefield::Forcefield() : ListItem<Forcefield>()
{
	// Initialise the Element RefList Array
	Elements::createElementRefListArray<ForcefieldAtomType>(atomTypesByElementPrivate_);
}

Forcefield::~Forcefield()
{
}

/*
 * Definition
 */

// Return enum options for ShortRangeType
EnumOptions<Forcefield::ShortRangeType> Forcefield::shortRangeTypes()
{
	static EnumOptionsList ShortRangeTypeOptions = EnumOptionsList() <<
		EnumOption(Forcefield::UndefinedType, 			"Undefined") <<
		EnumOption(Forcefield::NoInteractionType, 		"None") <<
		EnumOption(Forcefield::LennardJonesType, 		"LJ") <<
		EnumOption(Forcefield::LennardJonesGeometricType, 	"LJGeometric");

	static EnumOptions<Forcefield::ShortRangeType> options("ShortRangeType", ShortRangeTypeOptions);

	return options;
}

/*
 * Atom Type Data
 */

// Determine and return atom type for specified SpeciesAtom
ForcefieldAtomType* Forcefield::determineAtomType(SpeciesAtom* i) const
{
	// Go through AtomTypes defined for the target's element, and check NETA scores
	int bestScore = -1;
	ForcefieldAtomType* bestType = NULL;
	RefListIterator<ForcefieldAtomType> typeIterator(atomTypesByElementPrivate_.constAt(i->element()->Z()));
	while (ForcefieldAtomType* type = typeIterator.iterate())
	{
		// Get the scoring for this type
		int score = type->neta().score(i);
		if (score > bestScore)
		{
			bestScore = score;
			bestType = type;
		}
	}

	return bestType;
}

// Register the specified short-range parameters
void Forcefield::registerParameters(ForcefieldParameters* params)
{
	shortRangeParameters_.append(params);
}

// Register specified atom type to given Element
void Forcefield::registerAtomType(ForcefieldAtomType* atomType, int Z)
{
	atomTypesByElementPrivate_[Z].append(atomType);
}

// Return named short-range parameters (if they exist)
ForcefieldParameters* Forcefield::shortRangeParameters(const char* name) const
{
	RefListIterator<ForcefieldParameters> paramsIterator(shortRangeParameters_);
	while (ForcefieldParameters* params = paramsIterator.iterate()) if (DissolveSys::sameString(name, params->name())) return params;

	return NULL;
}

// Return the named ForcefieldAtomType (if it exists)
ForcefieldAtomType* Forcefield::atomTypeByName(const char* name, Element* element) const
{
	int startZ = (element ? element->Z() : 0);
	int endZ = (element ? element->Z() : nElements()-1);
	for (int Z=startZ; Z<=endZ; ++Z)
	{
		// Go through types associated to the Element
		RefListIterator<ForcefieldAtomType> typeIterator(atomTypesByElementPrivate_.constAt(Z));
		while (ForcefieldAtomType* type = typeIterator.iterate()) if (DissolveSys::sameString(type->name(), name)) return type;
	}

	return NULL;
}

// Return the ForcefieldAtomType with specified id (if it exists)
ForcefieldAtomType* Forcefield::atomTypeById(int id, Element* element) const
{
	int startZ = (element ? element->Z() : 0);
	int endZ = (element ? element->Z() : nElements()-1);
	for (int Z=startZ; Z<=endZ; ++Z)
	{
		// Go through types associated to the Element
		RefListIterator<ForcefieldAtomType> typeIterator(atomTypesByElementPrivate_.constAt(Z));
		while (ForcefieldAtomType* type = typeIterator.iterate()) if (type->index() == id) return type;
	}

	return NULL;
}

/*
 * Term Data
 */

// Register specified bond term
void Forcefield::registerBondTerm(ForcefieldBondTerm* bondTerm)
{
	bondTerms_.append(bondTerm);
}

// Return bond term for the supplied atom type pair (if it exists)
ForcefieldBondTerm* Forcefield::bondTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j) const
{
	RefListIterator<ForcefieldBondTerm> termIterator(bondTerms_);
	while (ForcefieldBondTerm* term = termIterator.iterate()) if (term->matches(i, j)) return term;

	return NULL;
}

// Register specified angle term
void Forcefield::registerAngleTerm(ForcefieldAngleTerm* angleTerm)
{
	angleTerms_.append(angleTerm);
}

// Return angle term for the supplied atom type trio (if it exists)
ForcefieldAngleTerm* Forcefield::angleTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k) const
{
	RefListIterator<ForcefieldAngleTerm> termIterator(angleTerms_);
	while (ForcefieldAngleTerm* term = termIterator.iterate()) if (term->matches(i, j, k)) return term;

	return NULL;
}

// Register specified torsion term
void Forcefield::registerTorsionTerm(ForcefieldTorsionTerm* torsionTerm)
{
	torsionTerms_.append(torsionTerm);
}

// Return torsion term for the supplied atom type quartet (if it exists)
ForcefieldTorsionTerm* Forcefield::torsionTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k, const ForcefieldAtomType* l) const
{
	RefListIterator<ForcefieldTorsionTerm> termIterator(torsionTerms_);
	while (ForcefieldTorsionTerm* term = termIterator.iterate()) if (term->matches(i, j, k, l)) return term;

	return NULL;
}

// Register specified improper term
void Forcefield::registerImproperTerm(ForcefieldImproperTerm* improperTerm)
{
	improperTerms_.append(improperTerm);
}

// Return improper term for the supplied atom type quartet (if it exists)
ForcefieldImproperTerm* Forcefield::improperTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k, const ForcefieldAtomType* l) const
{
	RefListIterator<ForcefieldImproperTerm> termIterator(improperTerms_);
	while (ForcefieldImproperTerm* term = termIterator.iterate()) if (term->matches(i, j, k, l)) return term;

	return NULL;
}

/*
 * Term Assignment
 */

// Assign suitable atom types to the supplied Species
bool Forcefield::assignAtomTypes(Species* sp, CoreData& coreData, bool keepExisting) const
{
	Messenger::print("Assigning atomtypes to species '%s' from forcefield '%s'...\n", sp->name(), name());

	// Loop over Species atoms
	for (SpeciesAtom* i = sp->atoms().first(); i != NULL; i = i->next())
	{
		// If keepExisting == true, don't reassign a type to this atom if one already exists
		if (keepExisting && i->atomType()) continue;

		ForcefieldAtomType* atomType = determineAtomType(i);
		if (!atomType) Messenger::print("No forcefield type available for Atom %i of Species (%s).\n", i->index()+1, i->element()->symbol());
		else
		{
			// Check if an AtomType of the same name already exists - if it does, just use that one
			AtomType* at = coreData.findAtomType(atomType->name());
			if (!at)
			{
				at = coreData.addAtomType(i->element());
				at->setName(atomType->name());

				// Copy parameters from the Forcefield's atom type
				at->parameters() = atomType->parameters();
				at->setShortRangeType(shortRangeType());

				// The atomType may reference parameters, rather than owning them, so set charge explicitly
				at->parameters().setCharge(atomType->charge());

				Messenger::print("Adding AtomType '%s' for atom %i (%s).\n", at->name(), i->userIndex(), i->element()->symbol());
			}
			else
			{
				Messenger::print("Re-using AtomType '%s' for atom %i (%s).\n", at->name(), i->userIndex(), i->element()->symbol());

				// If the current atomtype is empty, set its parameters
				if (at->parameters().empty())
				{
					// Copy parameters from the Forcefield's atom type
					at->parameters() = atomType->parameters();
					at->setShortRangeType(shortRangeType());

					// The atomType may reference parameters, rather than owning them, so set charge explicitly
					at->parameters().setCharge(atomType->charge());
				}
			}

			i->setAtomType(at);
		}
	}

	return true;
}

// Assign intramolecular parameters to the supplied Species
bool Forcefield::assignIntramolecular(Species* sp, bool useExistingTypes, bool assignBonds, bool assignAngles, bool assignTorsions, bool generateImpropers) const
{
	/*
	 * Default implementation - search through term lists for suitable ones to apply, based on ForcefieldAtomType names.
	 */

	Messenger::print("Assigning intramolecular terms to species '%s' from forcefield '%s'...\n", sp->name(), name());

	// Create an array of the ForcefieldAtomTypes for the atoms in the Species for speed
	Array<ForcefieldAtomType*> atomTypes;
	if (useExistingTypes)
	{
		// For each SpeciesAtom, search for the AtomType by name...
		ListIterator<SpeciesAtom> atomIterator(sp->atoms());
		while (SpeciesAtom* i = atomIterator.iterate())
		{
			if (!i->atomType()) return Messenger::error("No AtomType assigned to SpeciesAtom %i, so can't generate intramolecular terms based on existing types.\n", i->userIndex());
			ForcefieldAtomType* at = atomTypeByName(i->atomType()->name(), i->element());
			if (!at) return Messenger::error("Existing AtomType name '%s' does not correspond to a type in this forcefield.\n", i->atomType()->name());
			atomTypes.add(at);
		}
	}
	else
	{
		// Use on-the-fly generated types for all atoms
		ListIterator<SpeciesAtom> atomIterator(sp->atoms());
		while (SpeciesAtom* i = atomIterator.iterate())
		{
			ForcefieldAtomType* at = determineAtomType(i);
			if (!at) return Messenger::error("Couldn't determine a suitable AtomType for atom %i.\n", i->userIndex());
			atomTypes.add(at);
		}
	}

	// Assign bond terms
	if (assignBonds)
	{
		DynamicArrayIterator<SpeciesBond> bondIterator(sp->bonds());
		while (SpeciesBond* bond = bondIterator.iterate())
		{
			ForcefieldAtomType* i = atomTypes[bond->indexI()];
			ForcefieldAtomType* j = atomTypes[bond->indexJ()];

			ForcefieldBondTerm* term = bondTerm(i, j);
			if (!term) return Messenger::error("Failed to locate parameters for bond %i-%i (%s-%s).\n", bond->indexI()+1, bond->indexJ()+1, i->equivalentName(), j->equivalentName());

			// Functional form is Harmonic : U = 0.5 * k * (r - eq)**2
			bond->setForm(term->form());
			bond->setParameters(term->parameters());
		}
	}

	// Assign angle terms
	if (assignAngles)
	{
		// Generate angle parameters
		DynamicArrayIterator<SpeciesAngle> angleIterator(sp->angles());
		while (SpeciesAngle* angle = angleIterator.iterate())
		{
			ForcefieldAtomType* i = atomTypes[angle->indexI()];
			ForcefieldAtomType* j = atomTypes[angle->indexJ()];
			ForcefieldAtomType* k = atomTypes[angle->indexK()];

			ForcefieldAngleTerm* term = angleTerm(i, j, k);
			if (!term) return Messenger::error("Failed to locate parameters for angle %i-%i-%i (%s-%s-%s).\n", angle->indexI()+1, angle->indexJ()+1, angle->indexK()+1, i->equivalentName(), j->equivalentName(), k->equivalentName());

			angle->setForm(term->form());
			angle->setParameters(term->parameters());
		}
	}

	// Assign torsion terms
	if (assignTorsions)
	{
		// Generate torsion parameters
		DynamicArrayIterator<SpeciesTorsion> torsionIterator(sp->torsions());
		while (SpeciesTorsion* torsion = torsionIterator.iterate())
		{
			ForcefieldAtomType* i = atomTypes[torsion->indexI()];
			ForcefieldAtomType* j = atomTypes[torsion->indexJ()];
			ForcefieldAtomType* k = atomTypes[torsion->indexK()];
			ForcefieldAtomType* l = atomTypes[torsion->indexL()];

			ForcefieldTorsionTerm* term = torsionTerm(i, j, k, l);
			if (!term) return Messenger::error("Failed to locate parameters for torsion %i-%i-%i-%i (%s-%s-%s-%s).\n", torsion->indexI()+1, torsion->indexJ()+1, torsion->indexK()+1, torsion->indexL()+1, i->equivalentName(), j->equivalentName(), k->equivalentName(), l->equivalentName());

			torsion->setForm(term->form());
			torsion->setParameters(term->parameters());
		}
	}

	// Generate improper terms
	if (generateImpropers && (improperTerms_.nItems() > 0))
	{
		// Loop over potential improper sites in the Species and see if any match terms in the forcefield
		ListIterator<SpeciesAtom> atomIterator(sp->atoms());
		while (SpeciesAtom* ii = atomIterator.iterate())
		{
			// If we have less than three bonds to the central atom 'i', can continue now
			if (ii->nBonds() < 3) continue;

			// Loop over combinations of bonds to the central atom
			for (int indexJ = 0; indexJ < ii->nBonds()-2; ++indexJ)
			{
				// Get SpeciesAtom 'jj'
				SpeciesAtom* jj = ii->bond(indexJ)->partner(ii);
				for (int indexK = indexJ+1; indexK < ii->nBonds()-1; ++indexK)
				{
					// Get SpeciesAtom 'kk'
					SpeciesAtom* kk = ii->bond(indexK)->partner(ii);
					for (int indexL = indexK+1; indexL < ii->nBonds(); ++indexL)
					{
						// Get SpeciesAtom 'll'
						SpeciesAtom* ll = ii->bond(indexL)->partner(ii);

						// Get forcefield atom types and search for this improper
						ForcefieldAtomType* i = atomTypes[ii->index()];
						ForcefieldAtomType* j = atomTypes[jj->index()];
						ForcefieldAtomType* k = atomTypes[kk->index()];
						ForcefieldAtomType* l = atomTypes[ll->index()];

						ForcefieldImproperTerm* term = improperTerm(i, j, k, l);
						if (term)
						{
							// Check to see if the Species already has an improper definition - if not create one
							SpeciesImproper* improper = sp->improper(ii, jj, kk, ll);
							if (!improper) improper = sp->addImproper(ii, jj, kk, ll);

							Messenger::print("Added improper between atoms %i-%i-%i-%i (%s-%s-%s-%s).\n", improper->indexI()+1, improper->indexJ()+1, improper->indexK()+1, improper->indexL()+1, i->equivalentName(), j->equivalentName(), k->equivalentName(), l->equivalentName());

							improper->setForm(term->form());
							improper->setParameters(term->parameters());
						}
					}
				}
			}
		}
	}

	return true;
}

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
			angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
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
			angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
			largest = angle;
			j = i->bond(2)->partner(i);
			angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
			if (angle > largest) largest = angle;
			h = i->bond(1)->partner(i);
			angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
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
					angle += NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
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
	const PointerArray<SpeciesBond>& bonds = i->bonds();
	for (int n=0; n<bonds.nItems(); ++n)
	{
		switch (bonds.at(n)->bondType())
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

	const PointerArray<SpeciesBond>& bonds = i->bonds();
	for (int n=0; n<bonds.nItems(); ++n) if (bonds.at(n)->partner(i)->element() == element) ++found;

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

	const PointerArray<SpeciesBond>& bonds = i->bonds();
	for (int n=0; n<bonds.nItems(); ++n)
	{
		const SpeciesBond* bond = bonds.at(n);
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
