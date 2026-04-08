// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/species.h"
#include "data/atomicMasses.h"
#include <numeric>

// Recursively add atoms along any path from the specified one, ignoring the bond(s) provided
void Species::getIndicesRecursive(std::vector<int> &indices, int index, OptionalReferenceWrapper<SpeciesBond> exclude,
                                  OptionalReferenceWrapper<SpeciesBond> excludeToo) const
{
    // Loop over Bonds on specified Atom
    indices.emplace_back(index);
    const auto &i = atoms_[index];
    for (const SpeciesBond &bond : i.bonds())
    {
        // Is this either of the excluded bonds?
        if (exclude && &(*exclude).get() == &bond)
            continue;
        if (excludeToo && &(*excludeToo).get() == &bond)
            continue;

        // Get the partner atom in the bond and select it (if it is not selected already)
        auto *j = bond.partner(&i);
        if (std::find(indices.begin(), indices.end(), j->index()) == indices.end())
            getIndicesRecursive(indices, j->index(), exclude, excludeToo);
    }
}

// Add a new atom to the Species, returning its index
int Species::addAtom(Elements::Element Z, Vector3 r, double q, const AtomType *atomType)
{
    auto &i = atoms_.emplace_back(this);
    i.set(Z, r.x, r.y, r.z, q);
    i.setIndex(atoms_.size() - 1);
    i.setAtomType(atomType);
    ++version_;
    return i.index();
}

// Remove the specified atom from the species
void Species::removeAtom(int index)
{
    /*
     * Note: This is a deliberately simplistic function, and is intended only for use when creating / editing basic
     * species definitions upon which the simulation has no dependencies.
     */

    auto *i = &atoms_[index];

    // Clear higher-order terms
    angles_.clear();
    torsions_.clear();
    impropers_.clear();

    // Detach & remove any bond terms that involve 'i'
    auto it = std::remove_if(bonds_.begin(), bonds_.end(),
                             [i](auto &bond)
                             {
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

// Remove set of atom indices
void Species::removeAtoms(std::vector<int> indices)
{
    // Clear higher-order terms
    angles_.clear();
    torsions_.clear();
    impropers_.clear();

    // Detach & remove any bond terms that involve any of the supplied atom
    auto it =
        std::remove_if(bonds_.begin(), bonds_.end(),
                       [&indices](auto &bond)
                       {
                           if (std::find_if(indices.begin(), indices.end(), [&bond](const auto i)
                                            { return (bond.i()->index() == i || bond.j()->index() == i); }) != indices.end())
                           {
                               bond.detach();
                               return true;
                           }
                           else
                               return false;
                       });
    if (it != bonds_.end())
        bonds_.erase(it, bonds_.end());

    // Now remove the atoms
    auto atomIt = std::remove_if(atoms_.begin(), atoms_.end(), [&](const auto &i)
                                 { return std::find(indices.begin(), indices.end(), i.index()) != indices.end(); });
    atoms_.erase(atomIt, atoms_.end());
    renumberAtoms();

    ++version_;
}

// Return the number of atoms in the species (or only those with the specified presence)
int Species::nAtoms(SpeciesAtom::Presence withPresence) const
{
    return withPresence == SpeciesAtom::Presence::Any
               ? atoms_.size()
               : std::count_if(atoms_.begin(), atoms_.end(),
                               [withPresence](const auto &i) { return i.isPresence(withPresence); });
}

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
void Species::setAtomCoordinates(SpeciesAtom *i, Vector3 r)
{
    assert(i);

    i->setCoordinates(r);

    ++version_;
}

// Set coordinates of specified atom (by index and individual coordinates)
void Species::setAtomCoordinates(int id, double x, double y, double z) { atom(id).setCoordinates(x, y, z); }

// Transmute specified SpeciesAtom
void Species::transmuteAtom(int index, Elements::Element newZ)
{
    auto &i = atoms_[index];

    // Nothing to do if current element matches that supplied
    if (i.Z() == newZ)
        return;

    // Remove any existing AtomType assignment
    i.setAtomType(nullptr);
    i.setZ(newZ);

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
void Species::selectAtom(int index)
{
    auto &i = atoms_[index];
    if (!i.isSelected())
    {
        i.setSelected(true);

        ++atomSelectionVersion_;
    }
}

// Remove atom from selection
void Species::deselectAtom(int index)
{
    auto &i = atoms_[index];
    if (i.isSelected())
    {
        i.setSelected(false);

        ++atomSelectionVersion_;
    }
}

// Toggle selection state of specified atom
void Species::toggleAtomSelection(int index)
{
    if (atoms_[index].isSelected())
        deselectAtom(index);
    else
        selectAtom(index);
}

// Return the fragment containing the specified atom, optionally ignoring paths along the bond(s) provided
std::vector<int> Species::fragment(int startIndex, OptionalReferenceWrapper<SpeciesBond> exclude,
                                   OptionalReferenceWrapper<SpeciesBond> excludeToo) const
{
    std::vector<int> indices;
    getIndicesRecursive(indices, startIndex, exclude, excludeToo);
    return indices;
}

// Return current atom selection for modification
const std::vector<SpeciesAtom *> Species::modifiableSelectedAtoms()
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
    if (selection.empty())
        return false;
    return std::all_of(selection.begin(), selection.end(), [&](const auto *i) { return i->Z() == selection.front()->Z(); });
}

// Return version of the atom selection
int Species::atomSelectionVersion() const { return atomSelectionVersion_; }

// Return total atomic mass of Species
double Species::mass() const
{
    return std::accumulate(atoms_.begin(), atoms_.end(), 0.0, [](const auto acc, const auto &i)
                           { return acc + (i.isPresence(SpeciesAtom::Presence::Physical) ? AtomicMass::mass(i.Z()) : 0.0); });
}

// Add new atom type to atom types
AtomType* Species::addAtomType(Elements::Element Z, std::string_view name)
{
    // Create a suitable unique name
    auto uniqueName = DissolveSys::uniqueName(name == "" ? Elements::symbol(Z) : name, atomTypes_,
                                              [&](const auto &at) { return at->name(); });

    // Create atom type and set data
    auto newAtomType = std::make_shared<AtomType>(Z, uniqueName);
    atomTypes_.push_back(newAtomType);
    newAtomType->setIndex(atomTypes_.size() - 1);

    return newAtomType.get();
}

// Find and return the named atom type
AtomType *Species::findAtomType(std::string_view name) const
{
    auto it = std::find_if(atomTypes_.begin(), atomTypes_.end(), [name](const auto at) { return at->name() == name; });
    if (it != atomTypes_.end())
        return it->get();
    return nullptr;
}

// Return atom types in the species
std::vector<std::shared_ptr<AtomType>> &Species::atomTypes() { return atomTypes_; };
const std::vector<std::shared_ptr<AtomType>> &Species::atomTypes() const { return atomTypes_; }

// Calculate and return atom type populations
KeyedVector<const AtomType *, int> Species::atomTypePopulations() const
{
    KeyedVector<const AtomType *, int> result;
    for (const auto &i : atoms_)
        if (i.atomType() && i.isPresence((SpeciesAtom::Presence::Physical)))
            result[i.atomType()] += 1;

    return result;
}

// Clear AtomType assignments for all atoms
void Species::clearAtomTypes()
{
    for (auto &i : atoms_)
        i.setAtomType(nullptr);

    updateIsotopologues();
}

// Simplify atom types, merging together those with identical parameters
int Species::simplifyAtomTypes()
{
    // Iterate over all atoms bar the first
    auto nModified = 0;
    for (auto it = std::next(atoms_.begin()); it != atoms_.end(); ++it)
    {
        // Search all used atom types looking for a match, up to the current one
        auto matchingIt = std::find_if(atoms_.begin(), it,
                                       [&](auto &i) { return i.atomType() && i.atomType()->sameParametersAs(it->atomType()); });
        if (matchingIt == it)
            continue;

        it->setAtomType(matchingIt->atomType());
        ++nModified;
    }

    updateIsotopologues();

    return nModified;
}

// Set charge of specified atom
void Species::setAtomCharge(SpeciesAtom *i, double q)
{
    assert(i);

    i->setCharge(q);

    ++version_;
}

// Return total charge of species from local/atomtype atomic charges
double Species::totalCharge(bool useAtomTypes) const
{
    if (useAtomTypes)
        return std::accumulate(atoms_.begin(), atoms_.end(), 0.0, [](const auto acc, const auto &i)
                               { return acc + (i.atomType() ? i.atomType()->charge() : 0.0); });
    else
        return std::accumulate(atoms_.begin(), atoms_.end(), 0.0,
                               [](const auto acc, const auto &i) { return acc + i.charge(); });
}

// Apply random noise to atoms
void Species::randomiseCoordinates(double maxDisplacement)
{
    for (auto &i : atoms_)
        i.translateCoordinates(Vector3::randomUnit() * maxDisplacement);
}
