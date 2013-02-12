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

#include "classes/species.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "base/lineparser.h"
#include "base/flag.h"
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
	highlightedGrainDefinition_ = NULL;
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
	highlightedGrainDefinition_ = NULL;
	grainDefinitions_.clear();
	angles_.clear();
	bonds_.clear();
	atoms_.clear();
	
	SET_MODIFIED
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
	
	SET_MODIFIED
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
	for (Atom* i = atoms_.first(); i != NULL; i = i->next)
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
	RefList<Atom,int> grainCount(atoms_, 0);
	for (GrainDefinition* gd = grainDefinitions_.first(); gd != NULL; gd = gd->next)
	{
		for (RefListItem<Atom,int>* ri = gd->atoms(); ri != NULL; ri = ri->next)
		{
			RefListItem<Atom,int>* rj = grainCount.contains(ri->item);
			if (rj == NULL)
			{
				msg.error("GrainDefinition '%s' references a non-existent Atom.\n", gd->name());
				++nErrors;
			}
			else ++rj->data;
		}
	}
	for (RefListItem<Atom,int>* ri = grainCount.first(); ri != NULL; ri = ri->next)
	{
		if (ri->data == 0)
		{
			msg.error("Atom %i (%s) is not present in any GrainDefinition.\n", ri->item->userIndex(), PeriodicTable::element(ri->item->element()).symbol());
			++nErrors;
		}
		else if (ri->data > 1)
		{
			msg.error("Atom %i (%s) is present in more than one (%i) GrainDefinition.\n", ri->item->userIndex(), PeriodicTable::element(ri->item->element()).symbol(), ri->data);
			++nErrors;
		}
	}
	
	/* Check IntraMolecular Data */
	for (Atom* i = atoms_.first(); i != NULL; i = i->next)
	{
		if ((i->nBonds() == 0) && (atoms_.nItems() > 1))
		{
			msg.error("Atom %i (%s) participates in no Bonds, but is part of a multi-atom Species.\n", i->userIndex(), PeriodicTable::element(i->element()).symbol());
			++nErrors;
		}
		
		/* Check each Bond for two-way consistency */
		for (RefListItem<Bond,int>* ri = i->bonds(); ri != NULL; ri = ri->next)
		{
			Atom* j = ri->item->partner(i);
			if (!j->hasBond(i))
			{
				msg.error("Atom %i references a Bond to Atom %i, but Atom %i does not.\n", i->userIndex(), j->userIndex(), j->userIndex());
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
Atom *Species::addAtom(int element, double rx, double ry, double rz)
{
	Atom* i = atoms_.add();
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
Atom *Species::atoms() const
{
	return atoms_.first();
}

/*!
 * \brief Return the nth Atom in the Species
 * \details Return the nth Atom defined in the Species (or NULL if n is out of range)
 */
Atom *Species::atom(int n)
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
void Species::selectAtom(Atom* i)
{
	selectedAtoms_.addUnique(i);
}

/*!
 * \brief Select Atoms along any path from the specified one
 */
void Species::selectFromAtom(Atom* i, Bond* exclude)
{
	// Loop over Bonds on specified Atom
	selectAtom(i);
	Atom* j;
	for (RefListItem<Bond,int>* refBond = i->bonds(); refBond != NULL; refBond = refBond->next)
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
RefListItem<Atom,int>* Species::selectedAtoms() const
{
	return selectedAtoms_.first();
}

/*!
 * \brief Return nth selected Atom
 */
Atom* Species::selectedAtom(int n)
{
	RefListItem<Atom,int>* ri = selectedAtoms_[n];
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
bool Species::isAtomSelected(Atom* i) const
{
	return selectedAtoms_.contains(i);
}

/*!
 * \brief Change element of specified Atom
 * \details Transmute the specified Atom into the element specified, assigning the provided AtomType.
 * Note that it is necessary to update Isotopologue data after any calls to this function, through either
 * of the Species-specific or global (dUQ) updateIsotopologues() functions.
 */
void Species::changeAtomElement(Atom* i, int el, AtomType* at)
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
	
	SET_MODIFIED
}

/*!
 * \brief Return total atomic mass of Species
 */
double Species::mass() const
{
	double m = 0.0;
	for (Atom* i = atoms_.first(); i != NULL; i = i->next) m += PeriodicTable::element(i->element()).isotopes()->atomicWeight();
	return m;
}

/*!
 * \brief Centre coordinates at origin
 */
void Species::centreAtOrigin()
{
	Vec3<double> centre;
	for (Atom* i = atoms_.first(); i != NULL; i = i->next) centre += i->r();
	centre /= atoms_.nItems();
	for (Atom* i = atoms_.first(); i != NULL; i = i->next) i->translateCoordinatesNasty(-centre);
}

/*
// Intramolecular Data
*/

/*!
 * \brief Add new Bond definition (from Atoms*)
 * \details Add a new Bond definition to the Species, between the Atoms specified.
 */
Bond* Species::addBond(Atom* i, Atom* j)
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
	Bond* b = bonds_.add();
	b->setParent(this);
	b->setAtoms(i, j);
	i->addBond(b);
	j->addBond(b);

	SET_MODIFIED
	
	return b;
}

/*!
 * \brief Add new Bond definition
 * \details Add a new Bond definition to the Species, between the Atoms with indices specified.
 */
Bond* Species::addBond(int i, int j)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'i' supplied to Species::addBond() is out of range (%i) for Species '%s'\n", i, name_.get());
		return FALSE;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'j' supplied to Species::addBond() is out of range (%i) for Species '%s'\n", j, name_.get());
		return FALSE;
	}

	return addBond(atoms_[i], atoms_[j]);
}

/*!
 * \brief Return number of Bonds in list
 */
int Species::nBonds() const
{
	return bonds_.nItems();
}

/*!
 * \brief Return list of Bonds
 */
Bond *Species::bonds() const
{
	return bonds_.first();
}

/*!
 * \brief Return nth Bond
 */
Bond *Species::bond(int n)
{
	return bonds_[n];
}

/*!
 * \brief Return whether Bond between Atoms exists
 */
Bond* Species::hasBond(Atom* i, Atom* j) const
{
	for (Bond* b = bonds_.first(); b != NULL; b = b->next) if (b->matches(i, j)) return b;
	return NULL;
}

/*!
 * \brief Add new Angle definition (from Atoms*)
 * \details Add a new Angle definition to the Species, between the Atoms specified.
 */
Angle* Species::addAngle(Atom* i, Atom* j, Atom* k)
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

	// OK to add new Angle
	Angle* a = angles_.add();
	a->setParent(this);
	a->setAtoms(i, j, k);
	
	SET_MODIFIED
	
	return a;
}

/*!
 * \brief Add new Angle definition
 * \details Add a new Angle definition to the Species, between the Atoms with indices specified.
 */
Angle* Species::addAngle(int i, int j, int k)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'i' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", i, name_.get());
		return FALSE;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'j' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", j, name_.get());
		return FALSE;
	}
	if ((k < 0) || (k >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'k' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", k, name_.get());
		return FALSE;
	}
	return addAngle(atoms_[i], atoms_[j], atoms_[k]);
}

/*!
 * \brief Return number of Angles in list
 */
int Species::nAngles() const
{
	return angles_.nItems();
}

/*!
 * \brief Return list of Angles
 */
Angle *Species::angles() const
{
	return angles_.first();
}

/*!
 * \brief Return nth Angle
 */
Angle *Species::angle(int n)
{
	return angles_[n];
}

/*!
 * \brief Return whether Angle between Atoms exists
 */
bool Species::hasAngle(Atom* i, Atom* j, Atom* k) const
{
	for (Angle* a = angles_.first(); a != NULL; a = a->next) if (a->matches(i, j, k)) return TRUE;
	return FALSE;
}

/*!
 * \brief Recalculate intramolecular terms between Atoms in the Species
 * \details Determines chemical Bonds within the Species, based on the distances between defined Atoms and a table of
 * representative atomic radii. Once Bonds have been determined, Angle terms are automatically generated.
 */
void Species::recalculateIntramolecular()
{
	Atom* i, *j;
	Bond* b;
	Angle* a;
	RefListItem<Bond,int>* ri, *rj;
	int n = 0;
	double dist, radius_i;

	// Clear existing Bond and Angle definitions
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
	
	SET_MODIFIED
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
	for (Bond* b = bonds_.first(); b != NULL; b = b->next)
	{
		// Atom 'i'
		clearAtomSelection();
		selectFromAtom(b->i(), b);
		if (selectedAtoms_.contains(b->j()))
		{
			msg.error("Species '%s' contains one or more cycles, and these cannot be dealt with at present.\n", name_.get());
			return FALSE;
		}
		b->setAttachedAtoms(0, selectedAtoms_);

		// Atom 'j'
		clearAtomSelection();
		selectFromAtom(b->j(), b);
		if (selectedAtoms_.contains(b->i()))
		{
			msg.error("Species '%s' contains one or more cycles, and these cannot be dealt with at present.\n", name_.get());
			return FALSE;
		}
		b->setAttachedAtoms(1, selectedAtoms_);
	}

	// Angles
	for (Angle* a = angles_.first(); a != NULL; a = a->next)
	{
		// Atom 'i'
		clearAtomSelection();
		Bond* b = hasBond(a->i(), a->j());
		if (b == NULL)
		{
			msg.error("Species '%s' contains an Angle %i-%i-%i, but a Bond %i-%i is not defined.\n", name_.get(), a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->indexI()+1, a->indexJ()+1 );
			return FALSE;
		}
		selectFromAtom(a->i(), b);
		if (selectedAtoms_.contains(a->j()))
		{
			msg.error("Species '%s' contains one or more cycles, and these cannot be dealt with at present.\n", name_.get());
			return FALSE;
		}
		a->setAttachedAtoms(0, selectedAtoms_);

		// Atom 'k'
		clearAtomSelection();
		b = hasBond(a->j(), a->k());
		if (b == NULL)
		{
			msg.error("Species '%s' contains an Angle %i-%i-%i, but a Bond %i-%i is not defined.\n", name_.get(), a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->indexJ()+1, a->indexK()+1 );
			return FALSE;
		}
		selectFromAtom(a->k(), b);
		if (selectedAtoms_.contains(a->j()))
		{
			msg.error("Species '%s' contains one or more cycles, and these cannot be dealt with at present.\n", name_.get());
			return FALSE;
		}
		a->setAttachedAtoms(1, selectedAtoms_);
	}
	
	return TRUE;
}

/*
// Grains
*/

/*!
 * \brief Update GrainDefinitions after change
 * \details Update the current list of GrainDefinitions, checking for duplicate Atoms and empty definitions
 */
void Species::updateGrainDefinitions()
{
	// Construct a list of Atom references as we go along, so we can check for duplicates
	RefList<Atom,GrainDefinition*> refAtoms;
	for (GrainDefinition* gd = grainDefinitions_.first(); gd != NULL; gd = gd->next)
	{
		// Loop over Atoms in GrainDefinition
		RefListItem<Atom,int>* ri = gd->atoms(), *next;
		RefListItem<Atom,GrainDefinition*>* rj;
		while (ri != NULL)
		{
			next = ri->next;
			rj = refAtoms.contains(ri->item);
			if (rj)
			{
				gd->removeAtom(rj->item);
				msg.print("Removed Atom %i from GrainDefinition '%s' since it already exists in '%s'.\n", rj->item->userIndex(), gd->name(), rj->data->name());
				
				SET_MODIFIED
			}
			else refAtoms.add(ri->item, gd);
					  
			ri = next;
		}
	}
	
	// Check for empty GrainDefinitions
	GrainDefinition* gd = grainDefinitions_.first(), *next;
	while (gd != NULL)
	{
		next = gd->next;
		if (gd->nAtoms() == 0)
		{
			msg.print("Removing GrainDefinition '%s' since it no longer contains any Atoms.\n", gd->name());
			removeGrainDefinition(gd);

			SET_MODIFIED
		}
		gd = next;
	}
}

/*!
 * \brief Add default GrainDefinition (i.e. one which contains all atoms) for this Species 
 */
void Species::addDefaultGrainDefinition()
{
	GrainDefinition* gd = grainDefinitions_.add();
	
	for (Atom* i = atoms_.first(); i != NULL; i = i->next) gd->addAtom(i);
	
	gd->setName(gd->nameFromAtoms());
	
	SET_MODIFIED
}

/*!
 * \brief Automatically determine Grains based on chemical connectivity
 */
void Species::autoAddGrains()
{
	grainDefinitions_.clear();
	
	// Make a list of bonds for each atom
	RefList<Bond,int> bondList[atoms_.nItems()];
	for (Bond* b = bonds_.first(); b != NULL; b = b->next)
	{
		bondList[b->indexI()].add(b,1);
		bondList[b->indexJ()].add(b,1);
	}
	
	// Now loop over Atoms
	GrainDefinition* gd;
	int n, m, *group = new int[atoms_.nItems()];
	for (n=0; n<atoms_.nItems(); ++n) group[n] = 0;
	for (n=0; n<atoms_.nItems(); ++n)
	{
		if (group[n] == 1) continue;
		else if (bondList[n].nItems() == 0)
		{
			group[n] = 1;
			gd = grainDefinitions_.add();
			gd->setParent(this);
			gd->addAtom(atoms_[n]);
		}
		else if (bondList[n].nItems() == 1) continue;
		else
		{
			gd = grainDefinitions_.add();
			gd->setParent(this);
			group[n] = 1;
			gd->addAtom(atoms_[n]);
			for (RefListItem<Bond,int>* ri = bondList[n].first(); ri != NULL; ri = ri->next)
			{
				m = ri->item->indexI() == n ? ri->item->indexJ() : ri->item->indexI();
				if ((bondList[m].nItems() != 1) || (group[m] == 1)) continue;
				group[m] = 1;
				gd->addAtom(atoms_[m]);
			}
		}
	}
	
	// Name each grain....
	for (GrainDefinition* gd = grainDefinitions_.first(); gd != NULL; gd = gd->next) gd->setName(uniqueGrainDefinitionName(gd->nameFromAtoms()));
	
	SET_MODIFIED
}

/*!
 * \brief Add new GrainDefinition for this Species
 */
GrainDefinition *Species::addGrainDefinition()
{
	GrainDefinition* gd = grainDefinitions_.add();
	
	SET_MODIFIED

	return gd;
}

/*!
 * \brief Remove GrainDefinition specified
 */
void Species::removeGrainDefinition(GrainDefinition* gd)
{
	if (gd == NULL) msg.error("NULL_POINTER - NULL GrainDefinition passed to Species::removeGrainDefinition().\n");
	else if (grainDefinitions_.contains(gd))
	{
		if (highlightedGrainDefinition_ == gd) highlightedGrainDefinition_ = NULL;
		grainDefinitions_.remove(gd);
		msg.print("Removed GrainDefinition from Species '%s'.\n", name_.get());
		
		SET_MODIFIED
	}
	else msg.print("BAD_REMOVE - Can't remove specified GrainDefinition from Species '%s' since it doesn't exist.\n", name_.get());
}

/*!
 * \brief Return number of GrainDefinitions present for this Species
 */
int Species::nGrainDefinitions() const
{
	return grainDefinitions_.nItems();
}

/*!
 * \brief Return first GrainDefinition in list
 */
GrainDefinition *Species::grainDefinitions() const
{
	return grainDefinitions_.first();
}

/*!
 * \brief Return nth GrainDefinition in list
 */
GrainDefinition *Species::grainDefinition(int n)
{
	return grainDefinitions_[n];
}

/*!
 * \brief Add Atom to GrainDefinition
 */
void Species::addAtomToGrainDefinition(Atom* i, GrainDefinition* gd)
{
	// Check for presence of Atom in another definition - if it exists there, remove it...
	for (GrainDefinition* def = grainDefinitions_.first(); def != NULL; def = def->next)
	{
		if (def->containsAtom(i))
		{
			// Is this the target GrainDefinition?
			if (def == gd)
			{
				msg.print("Warning: Won't add duplicate of Atom %i to GrainDefinition '%s' in Species '%s'.\n", i->userIndex(), gd->name(), name());
				return;
			}
			def->removeAtom(i);
			msg.print("Removed Atom %i from GrainDefinition '%s' so it can be added to '%s'.\n", i->userIndex(), def->name(), gd->name());
			break;
		}
	}
	gd->addAtom(i);
	
	SET_MODIFIED
}

/*!
 * \brief Generate unique GrainDefinition name with base name provided
 */
const char* Species::uniqueGrainDefinitionName(const char* base, GrainDefinition* exclude) const
{
	static Dnchar uniqueName;
	Dnchar baseName = base;
	GrainDefinition* gd;
	int highest = -1;
	
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find all existing names which are the same as 'baseName' up to the first '_', and get the highest appended number
	for (gd = grainDefinitions_.first(); gd != NULL; gd = gd->next)
	{
		if (gd == exclude) continue;
		if (strcmp(baseName, gd->name()) == 0) highest = 0;
		else if (strcmp(baseName,beforeLastChar(gd->name(),'_')) == 0) highest = atoi(afterLastChar(gd->name(), '_'));
	}
	if (highest > -1) uniqueName.sprintf("%s_%i", baseName.get(), ++highest);
	else uniqueName = baseName;
	
	return uniqueName;
}

/*!
 * \brief Order Atoms within Grains
 * \details Reorders Atoms within the Species such that all Grains contain a consecutive set of Atom indices, and that indices
 * present within one Grain are *higher* than those in preceeding Grains in the list.
 */
void Species::orderAtomsWithinGrains()
{
	// Loop over Grains.
	// The variable 'index' will represent the target position in the Atom list of the next Atom to be considered within a GrainDefinition.
	int n, index = 0, oldIndex;
	for (GrainDefinition* gd = grainDefinitions_.first(); gd != NULL; gd = gd->next)
	{
		for (n=0; n<gd->nAtoms(); ++n)
		{
			oldIndex = atoms_.indexOf(gd->atom(n)->item);
			atoms_.move(oldIndex, index - oldIndex);
			++index;
		}
	}
	
	// Now need to renumber Atoms
	index = 0;
	for (Atom* i = atoms_.first(); i != NULL; i = i->next) i->setIndex(index++);
}

/*!
 * \brief Set highlighted GrainDefinition
 */
void Species::setHighlightedGrainDefinition(GrainDefinition *gd)
{
	highlightedGrainDefinition_ = gd;
}

/*!
 * \brief Return highlighted GrainDefinition
 */
GrainDefinition* Species::highlightedGrainDefinition()
{
	return highlightedGrainDefinition_;
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

	SET_MODIFIED

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
		
		SET_MODIFIED
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
	if (!Comm.broadcast(&count, 1)) return FALSE;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) addAtom();
		atoms_[n]->broadcast(atomTypes);
	}
	
	// Bonds
	msg.printVerbose("[MPI] Broadcasting Bonds...\n");
	count = bonds_.nItems();
	if (!Comm.broadcast(&count, 1)) return FALSE;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		// This is not ideal, since we are not using the safer addBond() function
		if (Comm.slave())
		{
			Bond* b = bonds_.add();
			b->setParent(this);
			b->broadcast(atoms_);
		}
		else bonds_[n]->broadcast(atoms_);
	}

	// Angles
	msg.printVerbose("[MPI] Broadcasting Angles...\n");
	count = angles_.nItems();
	if (!Comm.broadcast(&count, 1)) return FALSE;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		// This is not ideal, since we are not using the safer addAngle() function
		if (Comm.slave()) angles_.add();
		angles_[n]->broadcast(atoms_);
	}
	
	// GrainDefinitions
	msg.printVerbose("[MPI] Broadcasting GrainDefinitions...\n");
	count = grainDefinitions_.nItems();
	if (!Comm.broadcast(&count, 1)) return FALSE;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) addGrainDefinition();
		grainDefinitions_[n]->broadcast(atoms_);
	}

	// Isotopologues
	msg.printVerbose("[MPI] Broadcasting Isotopologues...\n");
	count = isotopologues_.nItems();
	if (!Comm.broadcast(&count, 1)) return FALSE;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) addIsotopologue();
		isotopologues_[n]->broadcast(atomTypes);
	}
#endif
	return TRUE;
}

