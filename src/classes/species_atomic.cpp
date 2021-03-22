// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/species.h"
#include "data/atomicmasses.h"

// Add a new atom to the Species
SpeciesAtom &Species::addAtom(Elements::Element Z, Vec3<double> r, double q)
{
    auto &i = atoms_.emplace_back();
    i.setSpecies(this);
    i.set(Z, r.x, r.y, r.z, q);
    i.setIndex(atoms_.size() - 1);
    ++version_;
    return i;
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
    selectedAtoms_.remove(i);

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

// Return the first Atom in the Species
const SpeciesAtom &Species::firstAtom() const { return atoms_.front(); }

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

// Return a vector of of immutable SpeciesAtoms
const std::list<SpeciesAtom> &Species::atoms() const { return atoms_; }

// Return vector of mutable atoms
std::list<SpeciesAtom> &Species::atoms() { return atoms_; }

// Set coordinates of specified atom
void Species::setAtomCoordinates(SpeciesAtom *i, Vec3<double> r)
{
    if (!i)
        return;

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

    selectedAtoms_.clear();

    ++atomSelectionVersion_;
}

// Add Atom to selection
void Species::selectAtom(SpeciesAtom *i)
{
    if (!i->isSelected())
    {
        i->setSelected(true);

        selectedAtoms_.append(i);

        ++atomSelectionVersion_;
    }
}

// Remove atom from selection
void Species::deselectAtom(SpeciesAtom *i)
{
    if (i->isSelected())
    {
        i->setSelected(false);

        selectedAtoms_.remove(i);

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
void Species::selectFromAtom(SpeciesAtom *i, SpeciesBond &exclude, OptionalReferenceWrapper<SpeciesBond> excludeToo)
{
    // Loop over Bonds on specified Atom
    selectAtom(i);
    for (const SpeciesBond &bond : i->bonds())
    {
        // Is this either of the excluded bonds?
        if (&exclude == &bond)
            continue;
        if (excludeToo && &(*excludeToo).get() == &bond)
            continue;

        // Get the partner atom in the bond and select it (if it is not selected already)
        auto *partner = bond.partner(i);

        if (selectedAtoms_.contains(partner))
            continue;
        selectFromAtom(partner, exclude);
    }
}

// Return current atom selection
const RefList<SpeciesAtom> &Species::selectedAtoms() const { return selectedAtoms_; }

// Return nth selected Atom
SpeciesAtom *Species::selectedAtom(int n)
{
    RefListItem<SpeciesAtom> *ri = selectedAtoms_[n];
    if (ri == nullptr)
        return nullptr;
    else
        return ri->item();
}

// Return number of selected Atoms
int Species::nSelectedAtoms() const { return selectedAtoms_.nItems(); }

// Return whether specified Atom is selected
bool Species::isAtomSelected(SpeciesAtom *i) const { return selectedAtoms_.contains(i); }

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
void Species::updateUsedAtomTypes()
{
    usedAtomTypes_.clear();
    for (const auto &i : atoms_)
        if (i.atomType())
            usedAtomTypes_.add(i.atomType(), 1);

    // Update our isotopologue definitions while we're here
    updateIsotopologues();
}

// Return used atom types list
const AtomTypeList &Species::usedAtomTypes() const { return usedAtomTypes_; }

// Clear AtomType assignments for all atoms
void Species::clearAtomTypes()
{
    for (auto &i : atoms_)
        i.setAtomType(nullptr);

    usedAtomTypes_.clear();
}

// Return total charge of species from local/atomtype atomic charges
double Species::totalCharge(bool useAtomTypes)
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
