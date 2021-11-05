// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/species.h"
#include "data/atomicmasses.h"

// Recursively select atoms along any path from the specified one, ignoring the bond(s) provided
void Species::selectFromAtomRecursive(std::vector<SpeciesAtom *> selection, SpeciesAtom *i, SpeciesBond &exclude,
                                      OptionalReferenceWrapper<SpeciesBond> excludeToo) const
{
    // Loop over Bonds on specified Atom
    selection.emplace_back(i);
    for (const SpeciesBond &bond : i->bonds())
    {
        // Is this either of the excluded bonds?
        if (&exclude == &bond)
            continue;
        if (excludeToo && &(*excludeToo).get() == &bond)
            continue;

        // Get the partner atom in the bond and select it (if it is not selected already)
        auto *j = bond.partner(i);
        if (std::find(selection.begin(), selection.end(), j) == selection.end())
            selectFromAtomRecursive(selection, j, exclude, excludeToo);
    }
}

// Add a new atom to the Species, returning its index
int Species::addAtom(Elements::Element Z, Vec3<double> r, double q)
{
    auto &i = atoms_.emplace_back();
    i.setSpecies(this);
    i.set(Z, r.x, r.y, r.z, q);
    i.setIndex(atoms_.size() - 1);
    ++version_;
    return i.index();
}

// Remove the specified atom from the species
void Species::removeAtom(SpeciesAtom *i)
{
    /*
     * Note: This is a deliberately simplistic function, and is intended only for use when creating / editing basic
     * species definitions upon which the simulation has no dependencies.
     */

    // Clear higher-order terms
    angles_.clear();
    torsions_.clear();
    impropers_.clear();

    // Detach & remove any bond terms that involve 'i'
    auto it = std::remove_if(bonds_.begin(), bonds_.end(), [i](auto &bond) {
        if (bond.i() == i || bond.j() == i)
        {
            bond.detach();
            return true;
        }
        else
            return false;
    });
    if (it != bonds_.end())
        bonds_.erase(it, bonds_.end());

    // Now remove the atom
    auto atomIt = std::find_if(atoms_.begin(), atoms_.end(), [&](const auto &p) { return i == &p; });
    atoms_.erase(atomIt);
    renumberAtoms();

    ++version_;
}

// Return the number of Atoms in the Species
int Species::nAtoms() const { return atoms_.size(); }

// Renumber atoms so they are sequential in the list
void Species::renumberAtoms()
{
    auto count = 0;
    for (auto &i : atoms_)
        i.setIndex(count++);
}

// Return the nth Atom in the Species
SpeciesAtom &Species::atom(int n)
{
    assert(n >= 0 && n < atoms_.size());
    auto it = std::next(atoms_.begin(), n);
    return *it;
}

const SpeciesAtom &Species::atom(int n) const
{
    assert(n >= 0 && n < atoms_.size());
    const auto it = std::next(atoms_.begin(), n);
    return *it;
}

// Return a vector of SpeciesAtoms
const std::vector<SpeciesAtom> &Species::atoms() const { return atoms_; }
std::vector<SpeciesAtom> &Species::atoms() { return atoms_; }

// Set coordinates of specified atom
void Species::setAtomCoordinates(SpeciesAtom *i, Vec3<double> r)
{
    assert(i);

    i->setCoordinates(r);

    ++version_;
}

// Set coordinates of specified atom (by index and individual coordinates)
void Species::setAtomCoordinates(int id, double x, double y, double z) { atom(id).setCoordinates(x, y, z); }

// Transmute specified SpeciesAtom
void Species::transmuteAtom(SpeciesAtom *i, Elements::Element newZ)
{
    assert(i);

    // Nothing to do if current element matches that supplied
    if (i->Z() == newZ)
        return;

    // Remove any existing AtomType assignment
    i->setAtomType(nullptr);
    i->setZ(newZ);

    ++version_;
}

// Clear current Atom selection
void Species::clearAtomSelection()
{
    for (auto &i : atoms_)
        i.setSelected(false);

    ++atomSelectionVersion_;
}

// Add Atom to selection
void Species::selectAtom(SpeciesAtom *i)
{
    if (!i->isSelected())
    {
        i->setSelected(true);

        ++atomSelectionVersion_;
    }
}

void Species::selectAtom(int index) { selectAtom(&atoms_[index]); }

// Remove atom from selection
void Species::deselectAtom(SpeciesAtom *i)
{
    if (i->isSelected())
    {
        i->setSelected(false);

        ++atomSelectionVersion_;
    }
}

// Toggle selection state of specified atom
void Species::toggleAtomSelection(SpeciesAtom *i)
{
    if (i->isSelected())
        deselectAtom(i);
    else
        selectAtom(i);
}

// Select Atoms along any path from the specified one, ignoring the bond(s) provided
std::vector<SpeciesAtom *> Species::selectFromAtom(SpeciesAtom *i, SpeciesBond &exclude,
                                                   OptionalReferenceWrapper<SpeciesBond> excludeToo) const
{
    std::vector<SpeciesAtom *> selection;
    selectFromAtomRecursive(selection, i, exclude, excludeToo);
    return selection;
}

// Return current atom selection
std::vector<SpeciesAtom *> Species::selectedAtoms()
{
    std::vector<SpeciesAtom *> selectedAtoms;
    for (auto &i : atoms_)
        if (i.isSelected())
            selectedAtoms.emplace_back(&i);

    return selectedAtoms;
}
const std::vector<const SpeciesAtom *> Species::selectedAtoms() const
{
    std::vector<const SpeciesAtom *> selectedAtoms;
    for (auto &i : atoms_)
        if (i.isSelected())
            selectedAtoms.emplace_back(&i);

    return selectedAtoms;
}

// Return whether the current selection comprises atoms of a single element
bool Species::isSelectionSingleElement() const
{
    auto selection = selectedAtoms();
    return std::all_of(selection.begin(), selection.end(), [&](const auto *i) { return i->Z() == selection.front()->Z(); });
}

// Return version of the atom selection
int Species::atomSelectionVersion() const { return atomSelectionVersion_; }

// Return total atomic mass of Species
double Species::mass() const
{
    auto m = 0.0;
    for (const auto &i : atoms_)
        m += AtomicMass::mass(i.Z());
    return m;
}

// Update used atom types
void Species::updateAtomTypes()
{
    atomTypes_.clear();
    for (const auto &i : atoms_)
        if (i.atomType())
            atomTypes_.add(i.atomType(), 1);

    // Update our isotopologue definitions while we're here
    updateIsotopologues();
}

// Return used atom types list
const AtomTypeMix &Species::atomTypes() const { return atomTypes_; }

// Clear AtomType assignments for all atoms
void Species::clearAtomTypes()
{
    for (auto &i : atoms_)
        i.setAtomType(nullptr);

    atomTypes_.clear();
}

// Simplify atom types, merging together those with identical parameters
int Species::simplifyAtomTypes()
{
    // Iterate over all atoms bar the first
    auto nModified = 0;
    for (auto it = std::next(atoms_.begin()); it != atoms_.end(); ++it)
    {
        // Search all used atom types looking for a match, up to the current one
        auto matchingIt = std::find_if(
            atoms_.begin(), it, [&](auto &i) { return i.atomType() && i.atomType()->sameParametersAs(it->atomType().get()); });
        if (matchingIt == it)
            continue;

        it->setAtomType(matchingIt->atomType());
        ++nModified;
    }

    updateAtomTypes();

    return nModified;
}

// Return total charge of species from local/atomtype atomic charges
double Species::totalCharge(bool useAtomTypes) const
{
    double totalQ = 0.0;
    if (useAtomTypes)
        for (const auto &i : atoms_)
        {
            if (!i.atomType())
            {
                Messenger::warn(
                    "No atom type assigned to atom index {} in species '{}', so can't calculate correct total charge.\n",
                    i.userIndex(), name_);
                continue;
            }
            totalQ += i.atomType()->charge();
        }
    else
        for (const auto &i : atoms_)
            totalQ += i.charge();
    return totalQ;
}
