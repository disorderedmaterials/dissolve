/*
	*** Species Definition
	*** src/lib/classes/species.cpp
	Copyright T. Youngs 2012-2013

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

#include "main/flags.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "base/lineparser.h"
#include "base/comms.h"
#include <string.h>
#include <base/sysfunc.h>

/*!
 * \brief Constructor
 * \details Constructor for Species. 
 */
Species::Species() : ListItem<Species>()
{
	highlightedIsotopologue_ = NULL;
	highlightedGrain_ = NULL;
	relativePopulation_ = -1.0;
}

/*!
 * \brief Destructor
 * \details Constructor for Species. 
 */
Species::~Species()
{
}

/*!
 * \brief Clear Data
 * \details Empty all lists and remove all Atom, GrainDefinition, and Isotopologue data from this Species
 */
void Species::clear()
{
	highlightedIsotopologue_ = NULL;
	isotopologues_.clear();
	highlightedGrain_ = NULL;
	grains_.clear();
	angles_.clear();
	bonds_.clear();
	atoms_.clear();

	Flags::wave(Flags::SpeciesChanged);
}

/*
// Basic Information
*/

/*!
 * \brief Set name of the Species
 */
void Species::setName(const char* name)
{
	name_ = name;
	
	Flags::wave(Flags::SpeciesChanged);
}

/*!
 * \brief Return the name of the Species
 */
const char *Species::name() const
{
	return name_.get();
}

/*!
 * \brief Set relative population in the system
 */
void Species::setRelativePopulation(double relPop)
{
	relativePopulation_ = relPop;
}

/*!
 * \brief Return relative population in the system
 */
double Species::relativePopulation()
{
	return relativePopulation_;
}

/*!
 * \brief Check setup
 * \details Check Species for obvious things (such as having no atoms) doen to the validity of its GrainDefinitions.
 */
int Species::checkSetup(const List<AtomType>& atomTypes)
{
	int nErrors = 0;
	
	if (atoms_.nItems() == 0) msg.print("[[[ WARNING ]]]	Species contains no Atoms.\n");
	
	/* Check AtomTypes */
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next)
	{
		if (i->atomType() == NULL)
		{
			msg.error("Atom %i (%s) has no associated AtomType.\n", i->userIndex(), PeriodicTable::element(i->element()).symbol());
			++nErrors;
		}
		else if (!atomTypes.contains(i->atomType()))
		{
			msg.error("Atom %i (%s) references a non-existent AtomType.\n", i->userIndex(), PeriodicTable::element(i->element()).symbol());
			++nErrors;
		}
	}
	
	/* GrainDefinitions */
	/* Each Atom must be in exactly one GrainDefinition */
	RefList<SpeciesAtom,int> grainCount(atoms_, 0);
	for (SpeciesGrain* sg = grains_.first(); sg != NULL; sg = sg->next)
	{
		for (RefListItem<SpeciesAtom,int>* ri = sg->atoms(); ri != NULL; ri = ri->next)
		{
			RefListItem<SpeciesAtom,int>* rj = grainCount.contains(ri->item);
			if (rj == NULL)
			{
				msg.error("GrainDefinition '%s' references a non-existent Atom.\n", sg->name());
				++nErrors;
			}
			else ++rj->data;
		}
	}
	for (RefListItem<SpeciesAtom,int>* ri = grainCount.first(); ri != NULL; ri = ri->next)
	{
		if (ri->data == 0)
		{
			msg.error("SpeciesAtom %i (%s) is not present in any GrainDefinition.\n", ri->item->userIndex(), PeriodicTable::element(ri->item->element()).symbol());
			++nErrors;
		}
		else if (ri->data > 1)
		{
			msg.error("SpeciesAtom %i (%s) is present in more than one (%i) GrainDefinition.\n", ri->item->userIndex(), PeriodicTable::element(ri->item->element()).symbol(), ri->data);
			++nErrors;
		}
	}
	
	/* Check IntraMolecular Data */
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next)
	{
		if ((i->nBonds() == 0) && (atoms_.nItems() > 1))
		{
			msg.error("SpeciesAtom %i (%s) participates in no Bonds, but is part of a multi-atom Species.\n", i->userIndex(), PeriodicTable::element(i->element()).symbol());
			++nErrors;
		}
		
		/* Check each Bond for two-way consistency */
		for (RefListItem<SpeciesBond,int>* ri = i->bonds(); ri != NULL; ri = ri->next)
		{
			SpeciesAtom* j = ri->item->partner(i);
			if (!j->hasBond(i))
			{
				msg.error("SpeciesAtom %i references a Bond to SpeciesAtom %i, but SpeciesAtom %i does not.\n", i->userIndex(), j->userIndex(), j->userIndex());
				++nErrors;
			}
		}
	}
	
	/* Check Isotopologues */
	if (isotopologues_.nItems() == 0)
	{
		msg.error("No Isotopologues defined in Species.\n");
		++nErrors;
	}
	else for (Isotopologue* iso = isotopologues_.first(); iso != NULL; iso = iso->next)
	{
		for (RefListItem<AtomType,Isotope*>* ri = iso->isotopes(); ri != NULL; ri = ri->next)
		{
			if (!atomTypes.contains(ri->item))
			{
				msg.error("Isotopologue '%s' refers to an unknown AtomType.\n", iso->name());
				++nErrors;
			}
			else if (ri->data == NULL)
			{
				msg.error("Isotopologue '%s' does not refer to an elemental Isotope for AtomType '%s'.\n", iso->name(), ri->item->name());
				++nErrors;
			}
			else if (!PeriodicTable::element(ri->item->element()).hasIsotope(ri->data->A()))
			{
				msg.error("Isotopologue '%s' does not refer to a suitable Isotope for AtomType '%s'.\n", iso->name(), ri->item->name());
				++nErrors;
			}
		}
	}

	return nErrors;
}

/*
// Atomic Information
*/

/*!
 * \brief Add a new atom to the Species
 * \details Create a new Atom within the Species, of the specified atomic element and at the position specified.
 * The coordinates are used to determine bonding within the species and to provide initial starting
 * geometries for all occurrences of this Species within the Model. As such, they should be set so as to provide
 * a reasonable starting geometry for the species (i.e. a chemically reasonable one).
 */
SpeciesAtom *Species::addAtom(int element, double rx, double ry, double rz)
{
	SpeciesAtom* i = atoms_.add();
	i->setParent(this);
	i->set(element, rx, ry, rz);
	i->setIndex(atoms_.nItems()-1);
	return i;
}

/*!
 * \brief Return the number of Atoms in the Species
 * \details Returns the total number of Atoms defined in the Species.
 */
int Species::nAtoms() const
{
	return atoms_.nItems();
}

/*!
 * \brief Return the first Atom in the Species
 * \details Return the first Atom defined in the Species (if one exists, otherwise NULL will be returned)
 */
SpeciesAtom *Species::atoms() const
{
	return atoms_.first();
}

/*!
 * \brief Return the nth Atom in the Species
 * \details Return the nth Atom defined in the Species (or NULL if n is out of range)
 */
SpeciesAtom *Species::atom(int n)
{
	return atoms_[n];
}

/*!
 * \brief Clear current Atom selection
 * \details Remove all Atoms from the current selection.
 */
void Species::clearAtomSelection()
{
	selectedAtoms_.clear();
}

/*!
 * \brief Add Atom to selection
 * \details Add the specified Atom to the list of selected Atoms. Atom selections are used solely by
 * the GUI, in order to allow easy selection of Grains etc.
 */
void Species::selectAtom(SpeciesAtom* i)
{
	selectedAtoms_.addUnique(i);
}

/*!
 * \brief Select Atoms along any path from the specified one
 */
void Species::selectFromAtom(SpeciesAtom* i, SpeciesBond* exclude)
{
	// Loop over Bonds on specified Atom
	selectAtom(i);
	SpeciesAtom* j;
	for (RefListItem<SpeciesBond,int>* refBond = i->bonds(); refBond != NULL; refBond = refBond->next)
	{
		// Is this the excluded Bond?
		if (exclude == refBond->item) continue;
		j = refBond->item->partner(i);
		if (selectedAtoms_.contains(j)) continue;
		selectFromAtom(j, exclude);
	}
}

/*!
 * \brief Return first selected Atom reference
 */
RefListItem<SpeciesAtom,int>* Species::selectedAtoms() const
{
	return selectedAtoms_.first();
}

/*!
 * \brief Return nth selected Atom
 */
SpeciesAtom* Species::selectedAtom(int n)
{
	RefListItem<SpeciesAtom,int>* ri = selectedAtoms_[n];
	if (ri == NULL) return NULL;
	else return ri->item;
}

/*!
 * \brief Return number of selected Atoms
 */
int Species::nSelectedAtoms() const
{
	return selectedAtoms_.nItems();
}

/*!
 * \brief Return whether specified Atom is selected
 */
bool Species::isAtomSelected(SpeciesAtom* i) const
{
	return selectedAtoms_.contains(i);
}

/*!
 * \brief Change element of specified Atom
 * \details Transmute the specified Atom into the element specified, assigning the provided AtomType.
 * Note that it is necessary to update Isotopologue data after any calls to this function, through either
 * of the Species-specific or global (dUQ) updateIsotopologues() functions.
 */
void Species::changeAtomElement(SpeciesAtom* i, int el, AtomType* at)
{
	// Check for NULL pointer
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer passed to Species::changeAtomElement.\n");
		return;
	}
	
	// First, change element within Atom (if necessary)
	if (i->element() != el) i->setElement(el);
	else return;
	
	// Now, must assign an AtomType. Isotopologue data should be updated after this function!
	i->setAtomType(at);

	Flags::wave(Flags::SpeciesChanged);
}

/*!
 * \brief Return total atomic mass of Species
 */
double Species::mass() const
{
	double m = 0.0;
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next) m += PeriodicTable::element(i->element()).isotopes()->atomicWeight();
	return m;
}

/*!
 * \brief Centre coordinates at origin
 */
void Species::centreAtOrigin()
{
	Vec3<double> centre;
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next) centre += i->r();
	centre /= atoms_.nItems();
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next) i->translateCoordinates(-centre);
}

/*
// Intramolecular Data
*/

/*!
 * \brief Add new Bond definition (from Atoms*)
 * \details Add a new Bond definition to the Species, between the Atoms specified.
 */
SpeciesBond* Species::addBond(SpeciesAtom* i, SpeciesAtom* j)
{
	// Check ownership of these Atoms
	if (!atoms_.contains(i))
	{
		msg.print("BAD_OWNERSHIP - Atom 'i' is not owned by Species '%s' in Species::addBond.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(j))
	{
		msg.print("BAD_OWNERSHIP - Atom 'j' is not owned by Species '%s' in Species::addBond.\n", name_.get());
		return NULL;
	}

	// Check for existence of Bond already
	if (hasBond(i, j))
	{
		msg.print("Warning: Refused to add a new Bond between atoms %i and %i in Species '%s' since it already exists.\n", i->userIndex(), j->userIndex(), name_.get());
		return NULL;
	}
	
	// OK to add new Bond
	SpeciesBond* b = bonds_.add();
	b->setParent(this);
	b->setAtoms(i, j);
	i->addBond(b);
	j->addBond(b);

	Flags::wave(Flags::SpeciesChanged);
	
	return b;
}

/*!
 * \brief Add new Bond definition
 * \details Add a new Bond definition to the Species, between the Atoms with indices specified.
 */
SpeciesBond* Species::addBond(int i, int j)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'i' supplied to Species::addBond() is out of range (%i) for Species '%s'\n", i, name_.get());
		return false;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'j' supplied to Species::addBond() is out of range (%i) for Species '%s'\n", j, name_.get());
		return false;
	}

	return addBond(atoms_[i], atoms_[j]);
}

/*!
 * \brief Return number of bonds in list
 */
int Species::nBonds() const
{
	return bonds_.nItems();
}

/*!
 * \brief Return list of bonds
 */
SpeciesBond *Species::bonds() const
{
	return bonds_.first();
}

/*!
 * \brief Return nth bond
 */
SpeciesBond *Species::bond(int n)
{
	return bonds_[n];
}

/*!
 * \brief Return whether bond between atoms exists
 */
SpeciesBond* Species::hasBond(SpeciesAtom* i, SpeciesAtom* j) const
{
	for (SpeciesBond* b = bonds_.first(); b != NULL; b = b->next) if (b->matches(i, j)) return b;
	return NULL;
}

/*!
 * \brief Add new angle definition (from supplied atom pointers)
 * \details Add a new angle definition to the species, between the atoms specified.
 */
SpeciesAngle* Species::addAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k)
{
	// Check ownership of these Atoms
	if (!atoms_.contains(i))
	{
		msg.print("BAD_OWNERSHIP - Atom 'i' is not owned by Species '%s' in Species::addAngle.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(j))
	{
		msg.print("BAD_OWNERSHIP - Atom 'j' is not owned by Species '%s' in Species::addAngle.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(k))
	{
		msg.print("BAD_OWNERSHIP - Atom 'k' is not owned by Species '%s' in Species::addAngle.\n", name_.get());
		return NULL;
	}

	// Check for existence of Angle already
	if (hasAngle(i, j, k))
	{
		msg.print("Warning: Refused to add a new Angle between atoms %i, %i and %i in Species '%s' since it already exists.\n", i->userIndex()+1, j->userIndex()+1, k->userIndex()+1, name_.get());
		return NULL;
	}

	// OK to add new angle
	SpeciesAngle* a = angles_.add();
	a->setParent(this);
	a->setAtoms(i, j, k);
	
	Flags::wave(Flags::SpeciesChanged);
	
	return a;
}

/*!
 * \brief Add new angle definition
 * \details Add a new angle definition to the species, between the atoms with indices specified.
 */
SpeciesAngle* Species::addAngle(int i, int j, int k)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'i' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", i, name_.get());
		return false;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'j' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", j, name_.get());
		return false;
	}
	if ((k < 0) || (k >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'k' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", k, name_.get());
		return false;
	}
	return addAngle(atoms_[i], atoms_[j], atoms_[k]);
}

/*!
 * \brief Return number of angles in list
 */
int Species::nAngles() const
{
	return angles_.nItems();
}

/*!
 * \brief Return list of angles
 */
SpeciesAngle *Species::angles() const
{
	return angles_.first();
}

/*!
 * \brief Return nth angle
 */
SpeciesAngle *Species::angle(int n)
{
	return angles_[n];
}

/*!
 * \brief Return whether angle between atoms exists
 */
bool Species::hasAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k) const
{
	for (SpeciesAngle* a = angles_.first(); a != NULL; a = a->next) if (a->matches(i, j, k)) return true;
	return false;
}

/*!
 * \brief Recalculate intramolecular terms between Atoms in the Species
 * \details Determines chemical Bonds within the Species, based on the distances between defined Atoms and a table of
 * representative atomic radii. Once bonds have been determined, angle terms are automatically generated.
 */
void Species::recalculateIntramolecular()
{
	SpeciesAtom* i, *j;
	SpeciesBond* b;
	SpeciesAngle* a;
	RefListItem<SpeciesBond,int>* ri, *rj;
	int n = 0;
	double dist, radius_i;

	// Clear existing bond and angle definitions
	for (i = atoms_.first(); i != NULL; i = i->next) i->clearBonds();
	bonds_.clear();
	angles_.clear();

	// First, determine which Atoms are within bonding distance
	for (i = atoms_.first(); i != atoms_.last(); i = i->next)
	{
		radius_i = PeriodicTable::element(i->element()).atomicRadius();
		for (j = i->next; j != NULL; j = j->next)
		{
			dist = (i->r()-j->r()).magnitude();
			if (dist <= (radius_i+PeriodicTable::element(j->element()).atomicRadius())*1.15)
			{
				// Create Bond and set initial equilibrium distance
				b = addBond(i, j);
				b->setEquilibrium(dist);
			}
		}
		++n;
	}
	msg.print("Found %i Bonds in Species '%s'.\n", bonds_.nItems(), name_.get());

	// Construct Angles list, looping over central Atom
	for (j = atoms_.first(); j != NULL; j = j->next)
	{
		// Double loop over Bonds
		printf("Angle is on Atom %i which has %i bonds\n", j->userIndex(), j->nBonds());;
		for (ri = j->bonds(); ri != NULL; ri = ri->next)
		{
			i = ri->item->partner(j);
			for (rj = ri->next; rj != NULL; rj = rj->next)
			{
				// Create Angle and set initial equilibrium angle
				a = addAngle(i, j, rj->item->partner(j));
				a->setEquilibrium(Box::angle(i->r(), j->r(), rj->item->partner(j)->r()));
			}
		}
	}
	
	Flags::wave(Flags::SpeciesChanged);
}

/*!
 * \brief Calculate local Atom index lists for interactions
 * \details For each Bond and Angle defined within the Species, all directly or indirectly attached Atoms at each terminus of the
 * interaction are selected and the indices added to the relevant local lists for the interaction. If, in the process, we find that
 * when selecting from Atom 'i' (of a Bond) we end up selecting Atom 'j', then the two Atoms are present in a cycle and we must explicitly
 * deal with such cases. But how!!! XXX TODO
 */
bool Species::calculateIndexLists()
{
	// Bonds
	for (SpeciesBond* b = bonds_.first(); b != NULL; b = b->next)
	{
		// Atom 'i'
		clearAtomSelection();
		selectFromAtom(b->i(), b);
		if (selectedAtoms_.contains(b->j()))
		{
// 			msg.error("Species '%s' contains one or more cycles, and these cannot be dealt with at present.\n", name_.get());
// 			return false;
			msg.print("Bond between Atoms %i and %i in Species '%s' exists in a cycle, so a minimal set of attached atoms will be used.\n", b->indexI()+1, b->indexJ()+1, name_.get());
			clearAtomSelection();
// 			for (RefListItem<Bond,int>* ri = b->i()->bonds(); ri != NULL; ri = ri->next) if (ri->item->partner(b->i()) != b->j()) selectedAtoms_.add(ri->item->partner(b->i()));
			selectedAtoms_.add(b->j());
		}
		b->setAttachedAtoms(0, selectedAtoms_);

		// Atom 'j'
		clearAtomSelection();
		selectFromAtom(b->j(), b);
		if (selectedAtoms_.contains(b->i()))
		{
// 			msg.error("Species '%s' contains one or more cycles, and these cannot be dealt with at present.\n", name_.get());
// 			return false;
			msg.print("Bond between Atoms %i and %i in Species '%s' exists in a cycle, so a minimal set of attached atoms will be used.\n", b->indexI()+1, b->indexJ()+1, name_.get());
			clearAtomSelection();
// 			for (RefListItem<Bond,int>* ri = b->j()->bonds(); ri != NULL; ri = ri->next) if (ri->item->partner(b->j()) != b->i()) selectedAtoms_.add(ri->item->partner(b->j()));
			selectedAtoms_.add(b->j());
		}
		b->setAttachedAtoms(1, selectedAtoms_);
	}

	// Angles
	for (SpeciesAngle* a = angles_.first(); a != NULL; a = a->next)
	{
		// Atom 'i'
		clearAtomSelection();
		SpeciesBond* b = hasBond(a->i(), a->j());
		if (b == NULL)
		{
			msg.error("Species '%s' contains an angle %i-%i-%i, but a bond %i-%i is not defined.\n", name_.get(), a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->indexI()+1, a->indexJ()+1 );
			return false;
		}
		selectFromAtom(a->i(), b);
		if (selectedAtoms_.contains(a->j()))
		{
// 			msg.error("Species '%s' contains one or more cycles, and these cannot be dealt with at present.\n", name_.get());
// 			return false;
			msg.print("Angle between atoms %i-%i-%i in species '%s' exists in a cycle, so a minimal set of attached atoms will be used.\n", a->indexI()+1, a->indexJ()+1, a->indexK()+1, name_.get());
			clearAtomSelection();
			selectedAtoms_.add(a->i());
		}
		a->setAttachedAtoms(0, selectedAtoms_);

		// Atom 'k'
		clearAtomSelection();
		b = hasBond(a->j(), a->k());
		if (b == NULL)
		{
			msg.error("Species '%s' contains an angle %i-%i-%i, but a bond %i-%i is not defined.\n", name_.get(), a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->indexJ()+1, a->indexK()+1 );
			return false;
		}
		selectFromAtom(a->k(), b);
		if (selectedAtoms_.contains(a->j()))
		{
// 			msg.error("Species '%s' contains one or more cycles, and these cannot be dealt with at present.\n", name_.get());
// 			return false;
			msg.print("Angle between atoms %i-%i-%i in species '%s' exists in a cycle, so a minimal set of attached atoms will be used.\n", a->indexI()+1, a->indexJ()+1, a->indexK()+1, name_.get());
			clearAtomSelection();
			selectedAtoms_.add(a->k());
		}
		a->setAttachedAtoms(1, selectedAtoms_);
	}
}

/*!
 * \brief Create scaling matrix
 */
void Species::createScalingMatrix()
{
	int n, m, rootIndex = atoms_[0]->index();

	scalingMatrix_.initialise(atoms_.nItems(), atoms_.nItems());

	// Unitise matrix (i.e. set all Atom pairs to interact fully)
	scalingMatrix_ = 1.0;
	
	// 'Torsion' interactions (set to 0.5)
	for (SpeciesBond* b = bonds_.first(); b != NULL; b = b->next)
	{
		// TODO
	}
	
	// Bond interactions (set to 0.0)
	for (SpeciesBond* b = bonds_.first(); b != NULL; b = b->next)
	{
		n = b->indexI() - rootIndex;
		m = b->indexJ() - rootIndex;
		scalingMatrix_.ref(n,m) = 0.0;
		scalingMatrix_.ref(m,n) = 0.0;
	}

	// Angle interactions (set to 0.0)
	for (SpeciesAngle* a = angles_.first(); a != NULL; a = a->next)
	{
		n = a->indexI() - rootIndex;
		m = a->indexK() - rootIndex;
		scalingMatrix_.ref(n,m) = 0.0;
		scalingMatrix_.ref(m,n) = 0.0;
	}
}

/*!
 * \brief Return scaling factor for supplied indices
 */
double Species::scaling(int indexI, int indexJ)
{
#ifdef CHECKS
	if ((indexI < 0) || (indexI >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - After rooting, supplied Atom indexI is out of range (%i) (nAtoms = %i).\n", indexI, atoms_.nItems());
		return 0.0;
	}
	if ((indexJ < 0) || (indexJ >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - After rooting, supplied Atom indexJ is out of range (%i) (nAtoms = %i).\n", indexJ, atoms_.nItems());
		return 0.0;
	}
#endif
	return scalingMatrix_.value(indexI, indexJ);
}

/*!
 * \brief Identify inter-Grain terms
 */
void Species::identifyInterGrainTerms()
{
	// Clear existing connections in grains
	for (SpeciesGrain* sg = grains_.first(); sg != NULL; sg = sg->next) sg->clearConnections();

	// Bonds
	for (SpeciesBond* b = bonds_.first(); b != NULL; b = b->next)
	{
#ifdef CHECKS
		if ((b->i() == NULL) || (b->j() == NULL))
		{
			msg.error("NULL_POINTER - One or both Atom pointers in a Bond are NULL (%p, %p)\n", b->i(), b->j());
			continue;
		}
		if ((b->i()->grain() == NULL) || (b->j()->grain() == NULL))
		{
			msg.error("NULL_POINTER - One or both Grain pointers in a Bond are NULL (%p, %p)\n", b->i()->grain(), b->j()->grain());
			continue;
		}
#endif

		// Is this an entirely INTRA-Grain Bond?
		if (b->i()->grain() == b->j()->grain())
		{
			b->i()->grain()->addInternalBond(b);
			continue;
		}

		// Bond is between two Grains, so add it to both and set the interGrain flag
		b->setInterGrain(true);
		b->i()->grain()->addBondConnection(b);
		b->j()->grain()->addBondConnection(b);
	}

	// Angles
	for (SpeciesAngle* a = angles_.first(); a != NULL; a = a->next)
	{
#ifdef CHECKS
		if ((a->i() == NULL) || (a->j() == NULL) || (a->k() == NULL))
		{
			msg.error("NULL_POINTER - One or more Atom pointers in an Angle are NULL (%p, %p, %p)\n", a->i(), a->j(), a->k());
			continue;
		}
		if ((a->i()->grain() == NULL) || (a->j()->grain() == NULL) || (a->k()->grain() == NULL))
		{
			msg.error("NULL_POINTER - One or more Grain pointers in an Angle are NULL (%p, %p, %p)\n", a->i()->grain(), a->j()->grain(), a->k()->grain());
			continue;
		}
#endif

		// Is this an entirely INTRA-Grain Angle?
		if ((a->i()->grain() == a->j()->grain()) && (a->j()->grain() == a->k()->grain()))
		{
			a->i()->grain()->addInternalAngle(a);
			continue;
		}
		
		// Angle is between at least two Grains, so add it to the relevant parties.
		// Always add to central Atom...
		a->j()->grain()->addAngleConnection(a);
		
		// If Atom 'i' is in a different Grain, add it to that...
		if (a->j()->grain() != a->i()->grain()) a->i()->grain()->addAngleConnection(a);
		
		// If Atom 'k' is in *another* different Grain, add it to that too
		
		// Only add to third Atom if its different again...
		if ((a->k()->grain() != a->i()->grain()) && (a->k()->grain() != a->j()->grain())) a->k()->grain()->addAngleConnection(a);
		
		// Finally, if 'i' and 'k' are in different Grains, set the intraGrain flag to intramolecular correction energies are calculated correctly
		if (a->i()->grain() != a->k()->grain()) a->setInterGrain(true);
	}
}

/*
// Grains
*/

/*!
 * \brief Update grains after change
 * \details Update the current list of grains, checking for duplicate Atoms and empty definitions
 */
void Species::updateGrains()
{
	// Construct a list of Atom references as we go along, so we can check for duplicates
	RefList<SpeciesAtom,SpeciesGrain*> refAtoms;
	for (SpeciesGrain* sg = grains_.first(); sg != NULL; sg = sg->next)
	{
		// Loop over Atoms in GrainDefinition
		RefListItem<SpeciesAtom,int>* ri = sg->atoms(), *next;
		RefListItem<SpeciesAtom,SpeciesGrain*>* rj;
		while (ri != NULL)
		{
			next = ri->next;
			rj = refAtoms.contains(ri->item);
			if (rj)
			{
				sg->removeAtom(rj->item);
				msg.print("Removed Atom %i from GrainDefinition '%s' since it already exists in '%s'.\n", rj->item->userIndex(), sg->name(), rj->data->name());
				
				Flags::wave(Flags::GrainsChanged);
			}
			else refAtoms.add(ri->item, sg);
					  
			ri = next;
		}
	}
	
	// Check for empty GrainDefinitions
	SpeciesGrain* sg = grains_.first(), *next;
	while (sg != NULL)
	{
		next = sg->next;
		if (sg->nAtoms() == 0)
		{
			msg.print("Removing grain '%s' since it no longer contains any atoms.\n", sg->name());
			removeGrain(sg);

			Flags::wave(Flags::GrainsChanged);
		}
		sg = next;
	}
}

/*!
 * \brief Add default GrainDefinition (i.e. one which contains all atoms) for this Species 
 */
void Species::addDefaultGrain()
{
	SpeciesGrain* sg = grains_.add();
	
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next) sg->addAtom(i);
	
	sg->setName(sg->nameFromAtoms());

	// Locate inter/intra-grain terms
	identifyInterGrainTerms();

	Flags::wave(Flags::GrainsChanged);
}

/*!
 * \brief Automatically determine Grains based on chemical connectivity
 */
void Species::autoAddGrains()
{
	grains_.clear();
	
	// Make a list of bonds for each atom
	RefList<SpeciesBond,int> bondList[atoms_.nItems()];
	for (SpeciesBond* b = bonds_.first(); b != NULL; b = b->next)
	{
		bondList[b->indexI()].add(b,1);
		bondList[b->indexJ()].add(b,1);
	}
	
	// Now loop over Atoms
	SpeciesGrain* sg;
	int n, m, *group = new int[atoms_.nItems()];
	for (n=0; n<atoms_.nItems(); ++n) group[n] = 0;
	for (n=0; n<atoms_.nItems(); ++n)
	{
		if (group[n] == 1) continue;
		else if (bondList[n].nItems() == 0)
		{
			group[n] = 1;
			sg = grains_.add();
			sg->setParent(this);
			sg->addAtom(atoms_[n]);
		}
		else if (bondList[n].nItems() == 1) continue;
		else
		{
			sg = grains_.add();
			sg->setParent(this);
			group[n] = 1;
			sg->addAtom(atoms_[n]);
			for (RefListItem<SpeciesBond,int>* ri = bondList[n].first(); ri != NULL; ri = ri->next)
			{
				m = ri->item->indexI() == n ? ri->item->indexJ() : ri->item->indexI();
				if ((bondList[m].nItems() != 1) || (group[m] == 1)) continue;
				group[m] = 1;
				sg->addAtom(atoms_[m]);
			}
		}
	}
	
	// Name each grain....
	for (SpeciesGrain* sg = grains_.first(); sg != NULL; sg = sg->next) sg->setName(uniqueGrainName(sg->nameFromAtoms()));

	// Locate inter/intra-grain terms
	identifyInterGrainTerms();

	Flags::wave(Flags::GrainsChanged);
}

/*!
 * \brief Add new GrainDefinition for this Species
 */
SpeciesGrain* Species::addGrain()
{
	SpeciesGrain* sg = grains_.add();
	
	Flags::wave(Flags::GrainsChanged);

	return sg;
}

/*!
 * \brief Remove GrainDefinition specified
 */
void Species::removeGrain(SpeciesGrain* sg)
{
	if (sg == NULL) msg.error("NULL_POINTER - NULL GrainDefinition passed to Species::removeGrainDefinition().\n");
	else if (grains_.contains(sg))
	{
		if (highlightedGrain_ == sg) highlightedGrain_ = NULL;
		grains_.remove(sg);
		msg.print("Removed grain from Species '%s'.\n", name_.get());
		
		Flags::wave(Flags::GrainsChanged);
	}
	else msg.print("BAD_REMOVE - Can't remove specified grain from Species '%s' since it doesn't exist.\n", name_.get());
}

/*!
 * \brief Return number of GrainDefinitions present for this Species
 */
int Species::nGrains() const
{
	return grains_.nItems();
}

/*!
 * \brief Return first GrainDefinition in list
 */
SpeciesGrain* Species::grains() const
{
	return grains_.first();
}

/*!
 * \brief Return nth GrainDefinition in list
 */
SpeciesGrain* Species::grain(int n)
{
	return grains_[n];
}

/*!
 * \brief Add Atom to GrainDefinition
 */
void Species::addAtomToGrain(SpeciesAtom* i, SpeciesGrain* gd)
{
	// Check for presence of Atom in another definition - if it exists there, remove it...
	for (SpeciesGrain* def = grains_.first(); def != NULL; def = def->next)
	{
		if (def->containsAtom(i))
		{
			// Is this the target GrainDefinition?
			if (def == gd)
			{
				msg.print("Warning: Won't add duplicate of atom %i to grain '%s' in Species '%s'.\n", i->userIndex(), gd->name(), name());
				return;
			}
			def->removeAtom(i);
			msg.print("Removed atom %i from grain '%s' so it can be added to '%s'.\n", i->userIndex(), def->name(), gd->name());
			break;
		}
	}
	gd->addAtom(i);
	
	Flags::wave(Flags::GrainsChanged);
}

/*!
 * \brief Generate unique GrainDefinition name with base name provided
 */
const char* Species::uniqueGrainName(const char* base, SpeciesGrain* exclude) const
{
	static Dnchar uniqueName;
	Dnchar baseName = base;
	SpeciesGrain* sg;
	int highest = -1;
	
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find all existing names which are the same as 'baseName' up to the first '_', and get the highest appended number
	for (sg = grains_.first(); sg != NULL; sg = sg->next)
	{
		if (sg == exclude) continue;
		if (strcmp(baseName, sg->name()) == 0) highest = 0;
		else if (strcmp(baseName,beforeLastChar(sg->name(),'_')) == 0) highest = atoi(afterLastChar(sg->name(), '_'));
	}
	if (highest > -1) uniqueName.sprintf("%s_%i", baseName.get(), ++highest);
	else uniqueName = baseName;
	
	return uniqueName;
}

/*!
 * \brief Order atoms within grains
 * \details Reorders atoms within the Species such that all grains contain a consecutive set of atom indices, and that indices
 * present within one grain are *higher* than those in preceeding Grains in the list.
 */
void Species::orderAtomsWithinGrains()
{
	// Loop over Grains.
	// The variable 'index' will represent the target position in the Atom list of the next Atom to be considered within a grain.
	int n, index = 0, oldIndex;
	for (SpeciesGrain* sg = grains_.first(); sg != NULL; sg = sg->next)
	{
		for (n=0; n<sg->nAtoms(); ++n)
		{
			oldIndex = atoms_.indexOf(sg->atom(n)->item);
			atoms_.move(oldIndex, index - oldIndex);
			++index;
		}
	}
	
	// Now need to renumber Atoms
	index = 0;
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next) i->setIndex(index++);
}

/*!
 * \brief Set highlighted grain
 */
void Species::setHighlightedGrain(SpeciesGrain* sg)
{
	highlightedGrain_ = sg;
}

/*!
 * \brief Return highlighted grain
 */
SpeciesGrain* Species::highlightedGrain()
{
	return highlightedGrain_;
}

/*
// Isotopologues
*/

/*!
 * \brief Update current Isotopologues
 */
void Species::updateIsotopologues(const List<AtomType>& atomTypes)
{
	for (Isotopologue* iso = isotopologues_.first(); iso != NULL; iso = iso->next) iso->update(atomTypes);
}

/*!
 * \brief Add a new Isotopologue to this species
 */
Isotopologue *Species::addIsotopologue(const char *baseName)
{
	Isotopologue* itp = isotopologues_.add();
	itp->setParent(this);
	itp->setName(uniqueIsotopologueName(baseName));

	Flags::wave(Flags::IsotopologuesChanged);

	return itp;
}

/*!
 * \brief Remove specified Isotopologue from this Species
 */
void Species::removeIsotopologue(Isotopologue* iso)
{
	if (iso == NULL) msg.error("NULL_POINTER - NULL Isotopologue passed to Species::removeIsotopologue().\n");
	else if (isotopologues_.contains(iso))
	{
		if (highlightedIsotopologue_ == iso) highlightedIsotopologue_ = NULL;
		Dnchar tempName = iso->name();
		isotopologues_.remove(iso);
		msg.print("Removed Isotopologue '%s' from Species '%s'.\n", tempName.get(), name_.get());
		
		Flags::wave(Flags::IsotopologuesChanged);
	}
	else
	{
		msg.print("BAD_REMOVE - Can't remove specified Isotopologue '%s' from Species '%s' since it doesn't exist.\n", iso->name(), name_.get());
		if (iso->parent() == NULL) msg.print("BAD_CLASS - No parent pointer set in Isotopologue '%s'.\n", iso->name());
		else msg.print("BAD_REMOVE - Parent Species (%s) of Isotopologue '%s' is different from this one (%s).\n", iso->parent()->name(), iso->name(), name());
	}
}

/*!
 * \brief Return number of defined Isotopologues
 */
int Species::nIsotopologues() const
{
	return isotopologues_.nItems();
}

/*!
 * \brief Return first Isotopologue defined
 */
Isotopologue *Species::isotopologues() const
{
	return isotopologues_.first();
}

/*!
 * \brief Return nth Isotopologue defined
 */
Isotopologue *Species::isotopologue(int n)
{
	return isotopologues_[n];
}

/*!
 * \brief Return whether the specified Isotopologue exists
 */
bool Species::hasIsotopologue(Isotopologue* iso) const
{
	return isotopologues_.contains(iso);
}

/*!
 * \brief Generate unique Isotopologue name with base name provided
 */
const char* Species::uniqueIsotopologueName(const char* base, Isotopologue* exclude) const
{
	static Dnchar uniqueName;
	Dnchar baseName = base;
	Isotopologue* iso;
	int highest = -1;
	
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find all existing names which are the same as 'baseName' up to the first '_', and get the highest appended number
	for (iso = isotopologues_.first(); iso != NULL; iso = iso->next)
	{
		if (iso == exclude) continue;
		if (strcmp(baseName, iso->name()) == 0) highest = 0;
		else if (strcmp(baseName,beforeLastChar(iso->name(),'_')) == 0) highest = atoi(afterLastChar(iso->name(), '_'));
	}
	if (highest > -1) uniqueName.sprintf("%s_%i", baseName.get(), ++highest);
	else uniqueName = baseName;
	
	return uniqueName;
}

/*!
 * \brief Search for Isotopologue by name
 */
Isotopologue *Species::findIsotopologue(const char* name) const
{
	for (Isotopologue *iso = isotopologues_.first(); iso != NULL; iso = iso->next) if (strcmp(name,iso->name()) == 0) return iso;
	return NULL;
}

/*!
 * \brief Return index of specified Isotopologue
 */
int Species::indexOfIsotopologue(Isotopologue* iso) const
{
	return isotopologues_.indexOf(iso);
}

/*!
 * \brief Set highlighted Isotopologue
 */
void Species::setHighlightedIsotopologue(Isotopologue* iso)
{
	highlightedIsotopologue_ = iso;
}

/*!
 * \brief Return highlighted Isotopologue
 */
Isotopologue* Species::highlightedIsotopologue()
{
	return highlightedIsotopologue_;
}

/*
// Parallel Comms
*/

/*!
 * \brief Broadcast data from Master to all Slaves
 */
bool Species::broadcast(const List<AtomType>& atomTypes)
{
#ifdef PARALLEL
	int n, count;

	// Send basic information
	Comm.broadcast(name_);
	Comm.broadcast(&relativePopulation_, 1);

	// Atoms
	msg.printVerbose("[MPI] Broadcasting Atoms...\n");
	count = atoms_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) addAtom();
		atoms_[n]->broadcast(atomTypes);
	}
	
	// Bonds
	msg.printVerbose("[MPI] Broadcasting bonds...\n");
	count = bonds_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		// This is not ideal, since we are not using the safer addBond() function
		if (Comm.slave())
		{
			SpeciesBond* b = bonds_.add();
			b->setParent(this);
			b->broadcast(atoms_);
		}
		else bonds_[n]->broadcast(atoms_);
	}

	// Angles
	msg.printVerbose("[MPI] Broadcasting angles...\n");
	count = angles_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		// This is not ideal, since we are not using the safer addAngle() function
		if (Comm.slave())
		{
			SpeciesAngle* a = angles_.add();
			a->setParent(this);
			a->broadcast(atoms_);
		}
		else angles_[n]->broadcast(atoms_);
	}
	
	// Grains
	msg.printVerbose("[MPI] Broadcasting grains...\n");
	count = grains_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) addGrain();
		grains_[n]->broadcast(atoms_, bonds_, angles_);
	}

	// Isotopologues
	msg.printVerbose("[MPI] Broadcasting isotopologues...\n");
	count = isotopologues_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) addIsotopologue();
		isotopologues_[n]->broadcast(atomTypes);
	}
#endif
	return true;
}

