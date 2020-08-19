/*
    *** Species Definition
    *** src/classes/species.cpp
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

#include "classes/species.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "classes/atomtype.h"
#include "classes/masterintra.h"
#include "data/isotopes.h"

// Static Members (ObjectStore)
template <class Species> RefDataList<Species, int> ObjectStore<Species>::objects_;
template <class Species> int ObjectStore<Species>::objectCount_ = 0;
template <class Species> int ObjectStore<Species>::objectType_ = ObjectInfo::SpeciesObject;
template <class Species> const char *ObjectStore<Species>::objectTypeName_ = "Species";

Species::Species() : ListItem<Species>(), ObjectStore<Species>(this)
{
    forcefield_ = NULL;
    autoUpdateIntramolecularTerms_ = true;
    attachedAtomListsGenerated_ = false;
    usedAtomTypesPoint_ = -1;

    // Set up natural Isotopologue
    naturalIsotopologue_.setName("Natural");
    naturalIsotopologue_.setParent(this);
    naturalIsotopologuePoint_ = -1;
}

Species::~Species() {}

// Clear Data
void Species::clear()
{
    isotopologues_.clear();
    angles_.clear();
    bonds_.clear();
    atoms_.clear();

    ++version_;
}

/*
 * Basic Information
 */

// Set name of the Species
void Species::setName(const char *name) { name_ = name; }

// Return the name of the Species
const char *Species::name() const { return name_.get(); }

// Check set-up of Species
bool Species::checkSetUp()
{
    auto nErrors = 0;

    // Must have at least one atom...
    if (atoms_.nItems() == 0)
    {
        Messenger::error("Species contains no Atoms.\n");
        return false;
    }

    /*
     * AtomTypes
     */
    for (auto *i = atoms_.first(); i != NULL; i = i->next())
    {
        if (i->atomType() == NULL)
        {
            Messenger::error("Atom %i (%s) has no associated AtomType.\n", i->userIndex(), i->element()->symbol());
            ++nErrors;
        }
    }
    if (nErrors > 0)
        return false;

    /*
     * IntraMolecular Data
     */
    for (auto *i = atoms_.first(); i != NULL; i = i->next())
    {
        if ((i->nBonds() == 0) && (atoms_.nItems() > 1))
        {
            Messenger::error("SpeciesAtom %i (%s) participates in no Bonds, but is part of a multi-atom Species.\n",
                             i->userIndex(), i->element()->symbol());
            ++nErrors;
        }

        // Check each Bond for two-way consistency
        for (const auto *bond : i->bonds())
        {
            SpeciesAtom *partner = bond->partner(i);
            if (!partner->hasBond(i))
            {
                Messenger::error("SpeciesAtom %i references a Bond to SpeciesAtom %i, but SpeciesAtom %i does not.\n",
                                 i->userIndex(), partner->userIndex(), partner->userIndex());
                ++nErrors;
            }
        }
    }
    if (nErrors > 0)
        return false;

    /*
     * Check Isotopologues
     */
    for (auto *iso = isotopologues_.first(); iso != NULL; iso = iso->next())
    {
        for (auto [atomType, isotope] : iso->isotopes())
        {
            if (!isotope)
            {
                Messenger::error("Isotopologue '%s' does not refer to an elemental Isotope for AtomType '%s'.\n", iso->name(),
                                 atomType->name());
                ++nErrors;
            }
            else if (!Isotopes::isotope(atomType->element(), isotope->A()))
            {
                Messenger::error("Isotopologue '%s' does not refer to a suitable Isotope for AtomType '%s'.\n", iso->name(),
                                 atomType->name());
                ++nErrors;
            }
        }
    }

    return (nErrors == 0);
}

// Print Species information
void Species::print()
{
    Messenger::print("  Atoms:\n");
    Messenger::print("      ID   El  Type (ID)        X             Y             Z             Q\n");
    Messenger::print("    ----------------------------------------------------------------------------\n");
    for (int n = 0; n < nAtoms(); ++n)
    {
        SpeciesAtom *i = atoms_[n];
        Messenger::print("    %4i  %3s  %4s (%2i)  %12.4e  %12.4e  %12.4e  %12.4e\n", n + 1, i->element()->symbol(),
                         (i->atomType() ? i->atomType()->name() : "??"), (i->atomType() ? i->atomType()->index() : -1),
                         i->r().x, i->r().y, i->r().z, i->charge());
    }

    if (nBonds() > 0)
    {
        Messenger::print("\n  Bonds:\n");
        Messenger::print("      I     J    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------\n");
        for (const auto &bond : bonds_)
        {
            CharString s("   %4i  %4i    %c%-12s", bond.indexI() + 1, bond.indexJ() + 1, bond.masterParameters() ? '@' : ' ',
                         SpeciesBond::bondFunctions().keywordFromInt(bond.form()));
            for (auto param : bond.parameters())
                s.strcatf("  %12.4e", param);
            Messenger::print("%s\n", s.get());
        }
    }

    if (nAngles() > 0)
    {
        Messenger::print("\n  Angles:\n");
        Messenger::print("      I     J     K    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------------\n");
        for (const auto &angle : angles_)
        {
            CharString s("   %4i  %4i  %4i    %c%-12s", angle.indexI() + 1, angle.indexJ() + 1, angle.indexK() + 1,
                         angle.masterParameters() ? '@' : ' ', SpeciesAngle::angleFunctions().keywordFromInt(angle.form()));
            for (auto param : angle.parameters())
                s.strcatf("  %12.4e", param);
            Messenger::print("%s\n", s.get());
        }
    }

    if (nTorsions() > 0)
    {
        Messenger::print("\n  Torsions:\n");
        Messenger::print("      I     J     K     L    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------------------\n");
        // Loop over Torsions
        for (const auto &t : torsions())
        {
            CharString s("   %4i  %4i  %4i  %4i    %c%-12s", t.indexI() + 1, t.indexJ() + 1, t.indexK() + 1, t.indexL() + 1,
                         t.masterParameters() ? '@' : ' ', SpeciesTorsion::torsionFunctions().keywordFromInt(t.form()));
            for (auto param : t.parameters())
                s.strcatf("  %12.4e", param);
            Messenger::print("%s\n", s.get());
        }
    }

    if (nImpropers() > 0)
    {
        Messenger::print("\n  Impropers:\n");
        Messenger::print("      I     J     K     L    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------------------\n");
        // Loop over Impropers
        for (auto &imp : impropers())
        {
            CharString s("   %4i  %4i  %4i  %4i    %c%-12s", imp.indexI() + 1, imp.indexJ() + 1, imp.indexK() + 1,
                         imp.indexL() + 1, imp.masterParameters() ? '@' : ' ',
                         SpeciesImproper::improperFunctions().keywordFromInt(imp.form()));
            for (auto param : imp.parameters())
                s.strcatf("  %12.4e", param);
            Messenger::print("%s\n", s.get());
        }
    }
}

// Return version
int Species::version() const { return version_; }

/*
 * Coordinate Sets
 */

// Clear coordinate sets
void Species::clearCoordinateSets() { coordinateSets_.clear(); }

// Add new coordinate set
CoordinateSet *Species::addCoordinateSet()
{
    CoordinateSet *coordSet = coordinateSets_.add();
    coordSet->initialise(atoms_.nItems());

    return coordSet;
}

// Return number of defined coordinate sets
int Species::nCoordinateSets() const { return coordinateSets_.nItems(); }

// Return coordinates sets
const List<CoordinateSet> &Species::coordinateSets() const { return coordinateSets_; }
