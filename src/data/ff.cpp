/*
	*** Forcefield
	*** src/data/ff.cpp
	Copyright T. Youngs 2019-2020

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

// Add new atom type with its own parameters
void Forcefield::addAtomType(int Z, int index, const char* name, const char* netaDefinition, const char* description, double q, double data0, double data1, double data2, double data3)
{
	ForcefieldAtomType* atomType = new ForcefieldAtomType(this, Z, index, name, netaDefinition, description, q, data0, data1, data2, data3);

	atomTypes_.own(atomType);
	
	atomTypesByElementPrivate_[Z].append(atomType);
}

// Add new atom type referencing existing parameters by name
void Forcefield::addAtomType(int Z, int index, const char* name, const char* netaDefinition, const char* description, double q, const char* parameterReference)
{
	ForcefieldAtomType* atomType = new ForcefieldAtomType(this, Z, index, name, netaDefinition, description, q, parameterReference);

	atomTypes_.own(atomType);
	
	atomTypesByElementPrivate_[Z].append(atomType);
}

// Copy existing atom type
void Forcefield::copyAtomType(const ForcefieldAtomType& sourceType, const char* newTypeName, const char* netaDefinition, const char* equivalentName)
{
	ForcefieldAtomType* atomType = new ForcefieldAtomType(this, sourceType, newTypeName, netaDefinition, equivalentName);

	atomTypes_.own(atomType);
	
	atomTypesByElementPrivate_[atomType->Z()].append(atomType);
}

// Determine and return atom type for specified SpeciesAtom from supplied Array of types
ForcefieldAtomType* Forcefield::determineAtomType(SpeciesAtom* i, const Array< RefList<ForcefieldAtomType> >& atomTypes)
{
	// Go through AtomTypes defined for the target's element, and check NETA scores
	int bestScore = -1;
	ForcefieldAtomType* bestType = NULL;
	for(auto type : atomTypes.constAt(i->element()->Z()))
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

// Determine and return atom type for specified SpeciesAtom
ForcefieldAtomType* Forcefield::determineAtomType(SpeciesAtom* i) const
{
	return determineAtomType(i, atomTypesByElementPrivate_);
}

// Ass short-range parameters
void Forcefield::addParameters(const char* name, double data0, double data1, double data2, double data3)
{
	ForcefieldParameters* params = new ForcefieldParameters(name, data0, data1, data2, data3);

	shortRangeParameters_.own(params);
}

// Return named short-range parameters (if they exist)
ForcefieldParameters* Forcefield::shortRangeParameters(const char* name) const
{
	ListIterator<ForcefieldParameters> paramsIterator(shortRangeParameters_);
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
		for(auto type : atomTypesByElementPrivate_.constAt(Z)) if (DissolveSys::sameString(type->name(), name)) return type;
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
		for(auto type : atomTypesByElementPrivate_.constAt(Z)) if (type->index() == id) return type;
	}

	return NULL;
}

/*
 * Term Data
 */

// Add bond term
void Forcefield::addBondTerm(const char* typeI, const char* typeJ, SpeciesBond::BondFunction form, double data0, double data1, double data2, double data3)
{
	ForcefieldBondTerm* term = new ForcefieldBondTerm(typeI, typeJ, form, data0, data1, data2, data3);

	bondTerms_.own(term);
}

// Add angle term
void Forcefield::addAngleTerm(const char* typeI, const char* typeJ, const char* typeK, SpeciesAngle::AngleFunction form, double data0, double data1, double data2, double data3)
{
	ForcefieldAngleTerm* term = new ForcefieldAngleTerm(typeI, typeJ, typeK, form, data0, data1, data2, data3);

	angleTerms_.own(term);
}

// Add torsion term
void Forcefield::addTorsionTerm(const char* typeI, const char* typeJ, const char* typeK, const char* typeL, SpeciesTorsion::TorsionFunction form, double data0, double data1, double data2, double data3)
{
	ForcefieldTorsionTerm* term = new ForcefieldTorsionTerm(typeI, typeJ, typeK, typeL, form, data0, data1, data2, data3);

	torsionTerms_.own(term);
}

// Add improper term
void Forcefield::addImproperTerm(const char* typeI, const char* typeJ, const char* typeK, const char* typeL, SpeciesImproper::ImproperFunction form, double data0, double data1, double data2, double data3)
{
	ForcefieldImproperTerm* term = new ForcefieldImproperTerm(typeI, typeJ, typeK, typeL, form, data0, data1, data2, data3);

	improperTerms_.own(term);
}

// Return bond term for the supplied atom type pair (if it exists)
ForcefieldBondTerm* Forcefield::bondTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j) const
{
	ListIterator<ForcefieldBondTerm> termIterator(bondTerms_);
	while (ForcefieldBondTerm* term = termIterator.iterate()) if (term->matches(i, j)) return term;

	return NULL;
}

// Return angle term for the supplied atom type trio (if it exists)
ForcefieldAngleTerm* Forcefield::angleTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k) const
{
	ListIterator<ForcefieldAngleTerm> termIterator(angleTerms_);
	while (ForcefieldAngleTerm* term = termIterator.iterate()) if (term->matches(i, j, k)) return term;

	return NULL;
}

// Return torsion term for the supplied atom type quartet (if it exists)
ForcefieldTorsionTerm* Forcefield::torsionTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k, const ForcefieldAtomType* l) const
{
	ListIterator<ForcefieldTorsionTerm> termIterator(torsionTerms_);
	while (ForcefieldTorsionTerm* term = termIterator.iterate()) if (term->matches(i, j, k, l)) return term;

	return NULL;
}

// Return improper term for the supplied atom type quartet (if it exists)
ForcefieldImproperTerm* Forcefield::improperTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k, const ForcefieldAtomType* l) const
{
	ListIterator<ForcefieldImproperTerm> termIterator(improperTerms_);
	while (ForcefieldImproperTerm* term = termIterator.iterate()) if (term->matches(i, j, k, l)) return term;

	return NULL;
}

/*
 * Term Assignment
 */

// Assign suitable AtomType to the supplied atom
bool Forcefield::assignAtomType(SpeciesAtom* i, CoreData& coreData) const
{
	ForcefieldAtomType* atomType = determineAtomType(i);
	if (atomType) 
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
			if (at->parameters().isEmpty())
			{
				// Copy parameters from the Forcefield's atom type
				at->parameters() = atomType->parameters();
				at->setShortRangeType(shortRangeType());

				// The atomType may reference parameters, rather than owning them, so set charge explicitly
				at->parameters().setCharge(atomType->charge());
			}
		}

		// Update SpeciesAtom
		i->setAtomType(at);
		i->setCharge(at->parameters().charge());
	}

	return (atomType != NULL);
}

// Assign suitable atom types to the supplied Species, returning the number of failures
int Forcefield::assignAtomTypes(Species* sp, CoreData& coreData, AtomTypeAssignmentStrategy strategy) const
{
	Messenger::print("Assigning atomtypes to species '%s' from forcefield '%s'...\n", sp->name(), name());

	// Loop over Species atoms
	int nFailed = 0;
	for (SpeciesAtom* i = sp->atoms().first(); i != NULL; i = i->next())
	{
		// Obey the supplied strategy:
		// -- Don't reassign a type to this atom if one already exists (strategy == Forcefield::TypeMissing)
		if ((strategy == Forcefield::TypeMissing) && i->atomType()) continue;
		// -- Don't assign a type unless the atom is selected (strategy == Forcefield::TypeSelection)
		if ((strategy == Forcefield::TypeSelection) && (!i->isSelected())) continue;

		// Messenger::print("No forcefield type available for Atom %i of Species (%s).\n", i->index()+1, i->element()->symbol());
		if (!assignAtomType(i, coreData)) ++nFailed;
	}

	if (nFailed) Messenger::error("Failed to assign atom %s to %i %s.\n", (nFailed == 1 ? "type" : "types"), nFailed, (nFailed == 1 ? "atom" : "atoms"));

	return nFailed;
}

// Assign intramolecular parameters to the supplied Species
bool Forcefield::assignIntramolecular(Species* sp, int flags) const
{
	/*
	 * Default implementation - search through term lists for suitable ones to apply, based on ForcefieldAtomType names.
	 */

	Messenger::print("Assigning intramolecular terms to species '%s' from forcefield '%s'...\n", sp->name(), name());

	bool determineTypes = flags&Forcefield::DetermineTypesFlag;
	bool selectionOnly = flags&Forcefield::SelectionOnlyFlag;

	// Assign bond terms
	DynamicArrayIterator<SpeciesBond> bondIterator(sp->bonds());
	while (SpeciesBond* bond = bondIterator.iterate())
	{
		SpeciesAtom* i = bond->i();
		SpeciesAtom* j = bond->j();

		if (selectionOnly && (!bond->isSelected())) continue;

		ForcefieldAtomType* typeI = determineTypes ? determineAtomType(i) : atomTypeByName(i->atomType()->name(), i->element());
		if (!typeI) return Messenger::error("Couldn't locate object for atom type named '%s'.\n", i->atomType()->name());
		ForcefieldAtomType* typeJ = determineTypes ? determineAtomType(j) : atomTypeByName(j->atomType()->name(), j->element());
		if (!typeJ) return Messenger::error("Couldn't locate object for atom type named '%s'.\n", j->atomType()->name());

		ForcefieldBondTerm* term = bondTerm(typeI, typeJ);
		if (!term) return Messenger::error("Failed to locate parameters for bond %i-%i (%s-%s).\n", i->userIndex(), j->userIndex(), typeI->equivalentName(), typeJ->equivalentName());

		bond->setForm(term->form());
		bond->setParameters(term->parameters());
	}

	// Generate angle parameters
	DynamicArrayIterator<SpeciesAngle> angleIterator(sp->angles());
	while (SpeciesAngle* angle = angleIterator.iterate())
	{
		SpeciesAtom* i = angle->i();
		SpeciesAtom* j = angle->j();
		SpeciesAtom* k = angle->k();

		if (selectionOnly && (!angle->isSelected())) continue;

		ForcefieldAtomType* typeI = determineTypes ? determineAtomType(i) : atomTypeByName(i->atomType()->name(), i->element());
		if (!typeI) return Messenger::error("Couldn't locate object for atom type named '%s'.\n", i->atomType()->name());
		ForcefieldAtomType* typeJ = determineTypes ? determineAtomType(j) : atomTypeByName(j->atomType()->name(), j->element());
		if (!typeJ) return Messenger::error("Couldn't locate object for atom type named '%s'.\n", j->atomType()->name());
		ForcefieldAtomType* typeK = determineTypes ? determineAtomType(k) : atomTypeByName(k->atomType()->name(), k->element());
		if (!typeK) return Messenger::error("Couldn't locate object for atom type named '%s'.\n", k->atomType()->name());

		ForcefieldAngleTerm* term = angleTerm(typeI, typeJ, typeK);
		if (!term) return Messenger::error("Failed to locate parameters for angle %i-%i-%i (%s-%s-%s).\n", i->userIndex(), j->userIndex(), k->userIndex(), typeI->equivalentName(), typeJ->equivalentName(), typeK->equivalentName());

		angle->setForm(term->form());
		angle->setParameters(term->parameters());
	}

	// Generate torsion parameters
	DynamicArrayIterator<SpeciesTorsion> torsionIterator(sp->torsions());
	while (SpeciesTorsion* torsion = torsionIterator.iterate())
	{
		SpeciesAtom* i = torsion->i();
		SpeciesAtom* j = torsion->j();
		SpeciesAtom* k = torsion->k();
		SpeciesAtom* l = torsion->l();

		if (selectionOnly && (!torsion->isSelected())) continue;

		ForcefieldAtomType* typeI = determineTypes ? determineAtomType(i) : atomTypeByName(i->atomType()->name(), i->element());
		if (!typeI) return Messenger::error("Couldn't locate object for atom type named '%s'.\n", i->atomType()->name());
		ForcefieldAtomType* typeJ = determineTypes ? determineAtomType(j) : atomTypeByName(j->atomType()->name(), j->element());
		if (!typeJ) return Messenger::error("Couldn't locate object for atom type named '%s'.\n", j->atomType()->name());
		ForcefieldAtomType* typeK = determineTypes ? determineAtomType(k) : atomTypeByName(k->atomType()->name(), k->element());
		if (!typeK) return Messenger::error("Couldn't locate object for atom type named '%s'.\n", k->atomType()->name());
		ForcefieldAtomType* typeL = determineTypes ? determineAtomType(l) : atomTypeByName(l->atomType()->name(), l->element());
		if (!typeL) return Messenger::error("Couldn't locate object for atom type named '%s'.\n", l->atomType()->name());

		ForcefieldTorsionTerm* term = torsionTerm(typeI, typeJ, typeK, typeL);
		if (!term) return Messenger::error("Failed to locate parameters for torsion %i-%i-%i-%i (%s-%s-%s-%s).\n", i->userIndex(), j->userIndex(), k->userIndex(), l->userIndex(), typeI->equivalentName(), typeJ->equivalentName(), typeK->equivalentName(), typeL->equivalentName());

		torsion->setForm(term->form());
		torsion->setParameters(term->parameters());
	}

	// Generate improper terms
	if ((flags&Forcefield::GenerateImpropersFlag) && (improperTerms_.nItems() > 0))
	{
		// Loop over potential improper sites in the Species and see if any match terms in the forcefield
		ListIterator<SpeciesAtom> atomIterator(sp->atoms());
		while (SpeciesAtom* i = atomIterator.iterate())
		{
			// If we have less than three bonds to the central atom 'i', can continue now
			if (i->nBonds() < 3) continue;

			ForcefieldAtomType* typeI = determineTypes ? determineAtomType(i) : atomTypeByName(i->atomType()->name(), i->element());
			if (!typeI) return Messenger::error("Couldn't locate object for atom type named '%s'.\n", i->atomType()->name());
			if (selectionOnly && (!i->isSelected())) continue;

			// Loop over combinations of bonds to the central atom
			for (int indexJ = 0; indexJ < i->nBonds()-2; ++indexJ)
			{
				// Get SpeciesAtom 'j'
				SpeciesAtom* j = i->bond(indexJ)->partner(i);
				ForcefieldAtomType* typeJ = determineTypes ? determineAtomType(j) : atomTypeByName(j->atomType()->name(), j->element());
				if (!typeJ) return Messenger::error("Couldn't locate object for atom type named '%s'.\n", j->atomType()->name());
				if (selectionOnly && (!j->isSelected())) continue;
				for (int indexK = indexJ+1; indexK < i->nBonds()-1; ++indexK)
				{
					// Get SpeciesAtom 'k'
					SpeciesAtom* k = i->bond(indexK)->partner(i);
					ForcefieldAtomType* typeK = determineTypes ? determineAtomType(k) : atomTypeByName(k->atomType()->name(), k->element());
					if (!typeK) return Messenger::error("Couldn't locate object for atom type named '%s'.\n", k->atomType()->name());
					if (selectionOnly && (!k->isSelected())) continue;
					for (int indexL = indexK+1; indexL < i->nBonds(); ++indexL)
					{
						// Get SpeciesAtom 'l'
						SpeciesAtom* l = i->bond(indexL)->partner(i);
						ForcefieldAtomType* typeL = determineTypes ? determineAtomType(l) : atomTypeByName(l->atomType()->name(), l->element());
						if (!typeL) return Messenger::error("Couldn't locate object for atom type named '%s'.\n", l->atomType()->name());
						if (selectionOnly && (!l->isSelected())) continue;

						ForcefieldImproperTerm* term = improperTerm(typeI, typeJ, typeK, typeL);
						if (term)
						{
							// Check to see if the Species already has an improper definition - if not create one
							SpeciesImproper* improper = sp->improper(i, j, k, l);
							if (!improper) improper = sp->addImproper(i, j, k, l);

							Messenger::print("Added improper between atoms %i-%i-%i-%i (%s-%s-%s-%s).\n", i->userIndex(), j->userIndex(), k->userIndex(), l->userIndex(), typeI->equivalentName(), typeJ->equivalentName(), typeK->equivalentName(), typeL->equivalentName());

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
	for (const auto* bond : i->bonds())
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

	for (const auto* bond : i->bonds()) if (bond->partner(i)->element() == element) ++found;

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

	const std::vector<SpeciesBond*>& bonds = i->bonds();
	for (const auto* bond : bonds)
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
