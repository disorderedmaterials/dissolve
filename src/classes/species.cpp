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
template <class Species> std::string_view ObjectStore<Species>::objectTypeName_ = "Species";

Species::Species() : ListItem<Species>(), ObjectStore<Species>(this)
{
    forcefield_ = nullptr;
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
void Species::setName(std::string_view name) { name_ = name; }

// Return the name of the Species
std::string_view Species::name() const { return name_; }

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
    for (auto *i = atoms_.first(); i != nullptr; i = i->next())
    {
        if (i->atomType() == nullptr)
        {
            Messenger::error("Atom {} ({}) has no associated AtomType.\n", i->userIndex(), i->element()->symbol());
            ++nErrors;
        }
    }
    if (nErrors > 0)
        return false;

    /*
     * IntraMolecular Data
     */
    for (auto *i = atoms_.first(); i != nullptr; i = i->next())
    {
        if ((i->nBonds() == 0) && (atoms_.nItems() > 1))
        {
            Messenger::error("SpeciesAtom {} ({}) participates in no Bonds, but is part of a multi-atom Species.\n",
                             i->userIndex(), i->element()->symbol());
            ++nErrors;
        }

        // Check each Bond for two-way consistency
        for (const SpeciesBond &bond : i->bonds())
        {
            auto *partner = bond.partner(i);
            if (!partner->hasBond(i))
            {
                Messenger::error("SpeciesAtom {} references a Bond to SpeciesAtom {}, but SpeciesAtom {} does not.\n",
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
    for (auto *iso = isotopologues_.first(); iso != nullptr; iso = iso->next())
    {
        for (auto [atomType, isotope] : iso->isotopes())
        {
            if (!isotope)
            {
                Messenger::error("Isotopologue '{}' does not refer to an elemental Isotope for AtomType '{}'.\n", iso->name(),
                                 atomType->name());
                ++nErrors;
            }
            else if (!Isotopes::isotope(atomType->element(), isotope->A()))
            {
                Messenger::error("Isotopologue '{}' does not refer to a suitable Isotope for AtomType '{}'.\n", iso->name(),
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
        Messenger::print("    {:4d}  {:3}  {:4} ({:2d})  {:12.4e}  {:12.4e}  {:12.4e}  {:12.4e}\n", n + 1,
                         i->element()->symbol(), (i->atomType() ? i->atomType()->name() : "??"),
                         (i->atomType() ? i->atomType()->index() : -1), i->r().x, i->r().y, i->r().z, i->charge());
    }

    if (nBonds() > 0)
    {
        Messenger::print("\n  Bonds:\n");
        Messenger::print("      I     J    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------\n");
        for (const auto &bond : bonds_)
        {
            std::string line =
                fmt::format("   {:4d}  {:4d}    {}{:<12}", bond.indexI() + 1, bond.indexJ() + 1,
                            bond.masterParameters() ? '@' : ' ', SpeciesBond::bondFunctions().keywordFromInt(bond.form()));
            for (const auto param : bond.parameters())
                line += fmt::format("  {:12.4e}", param);
            Messenger::print(line);
        }
    }

    if (nAngles() > 0)
    {
        Messenger::print("\n  Angles:\n");
        Messenger::print("      I     J     K    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------------\n");
        for (const auto &angle : angles_)
        {
            std::string line =
                fmt::format("   {:4d}  {:4d}  {:4d}    {}{:<12}", angle.indexI() + 1, angle.indexJ() + 1, angle.indexK() + 1,
                            angle.masterParameters() ? '@' : ' ', SpeciesAngle::angleFunctions().keywordFromInt(angle.form()));
            for (const auto param : angle.parameters())
                line += fmt::format("  {:12.4e}", param);
            Messenger::print(line);
        }
    }

    if (nTorsions() > 0)
    {
        Messenger::print("\n  Torsions:\n");
        Messenger::print("      I     J     K     L    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------------------\n");
        // Loop over Torsions
        for (const auto &torsion : torsions())
        {
            std::string line =
                fmt::format("   {:4d}  {:4d}  {:4d}  {:4d}    {}{:<12}", torsion.indexI() + 1, torsion.indexJ() + 1,
                            torsion.indexK() + 1, torsion.indexL() + 1, torsion.masterParameters() ? '@' : ' ',
                            SpeciesTorsion::torsionFunctions().keywordFromInt(torsion.form()));
            for (const auto param : torsion.parameters())
                line += fmt::format("  {:12.4e}", param);
            Messenger::print(line);
        }
    }

    if (nImpropers() > 0)
    {
        Messenger::print("\n  Impropers:\n");
        Messenger::print("      I     J     K     L    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------------------\n");
        // Loop over Impropers
        for (auto &improper : impropers())
        {
            std::string line =
                fmt::format("   {:4d}  {:4d}  {:4d}  {:4d}    {}{:<12}", improper.indexI() + 1, improper.indexJ() + 1,
                            improper.indexK() + 1, improper.indexL() + 1, improper.masterParameters() ? '@' : ' ',
                            SpeciesImproper::improperFunctions().keywordFromInt(improper.form()));
            for (const auto param : improper.parameters())
                line += fmt::format("  {:12.4e}", param);
            Messenger::print(line);
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
