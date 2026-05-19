// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/species.h"
#include "data/atomicMasses.h"
#include <numeric>

// Recursively add atoms along any path from the specified one, ignoring the bond(s) provided
void Species::getIndicesRecursive(std::vector<int> &indices, int index, const SpeciesBond *exclude,
                                  const SpeciesBond *excludeToo) const
{
    // Loop over Bonds on specified Atom
    indices.emplace_back(index);
    const auto &i = atoms_[index];
    for (const auto *bond : i.bonds())
    {
        // Is this either of the excluded bonds?
        if (exclude == bond || excludeToo == bond)
            continue;

        // Get the partner atom in the bond and select it (if it is not selected already)
        auto *j = bond->partner(&i);
        if (std::find(indices.begin(), indices.end(), j->index()) == indices.end())
            getIndicesRecursive(indices, j->index(), exclude, excludeToo);
    }
}

// Return the number of atoms in the species (or only those with the specified presence)
int Species::nAtoms(AtomConstants::Presence withPresence) const
{
    return withPresence == AtomConstants::Presence::Any
               ? atoms_.size()
               : std::count_if(atoms_.begin(), atoms_.end(),
                               [withPresence](const auto &i) { return i.isPresence(withPresence); });
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

// Return the fragment containing the specified atom, optionally ignoring paths along the bond(s) provided
std::vector<int> Species::fragment(int startIndex, const SpeciesBond *exclude, const SpeciesBond *excludeToo) const
{
    std::vector<int> indices;
    getIndicesRecursive(indices, startIndex, exclude, excludeToo);
    return indices;
}

// Return total atomic mass of Species
double Species::mass() const
{
    return std::accumulate(atoms_.begin(), atoms_.end(), 0.0, [](const auto acc, const auto &i)
                           { return acc + (i.isPresence(AtomConstants::Presence::Physical) ? AtomicMass::mass(i.Z()) : 0.0); });
}

// Add new atom type to atom types
AtomType *Species::addAtomType(Elements::Element Z, std::string_view name)
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
std::vector<const AtomType *> Species::atomTypesRaw() const
{
    std::vector<const AtomType *> result;
    std::transform(atomTypes_.begin(), atomTypes_.end(), std::back_inserter(result), [](const auto &at) { return at.get(); });
    return result;
}

// Calculate and return atom type populations
KeyedVector<const AtomType *, int> Species::atomTypePopulations() const
{
    KeyedVector<const AtomType *, int> result;
    for (const auto &i : atoms_)
        if (i.atomType() && i.isPresence((AtomConstants::Presence::Physical)))
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

// Return total charge of species from local/atomtype atomic charges
double Species::totalCharge(bool useAtomTypes) const
{
    if (useAtomTypes)
        return std::accumulate(atoms_.begin(), atoms_.end(), 0.0, [](const auto acc, const auto &i)
                               { return acc + (i.atomType() ? i.atomType()->charge() : 0.0); });
    else
        return std::accumulate(atoms_.begin(), atoms_.end(), 0.0, [](const auto acc, const auto &i) { return acc + i.q(); });
}

// Apply random noise to atoms
void Species::randomiseCoordinates(double maxDisplacement)
{
    for (auto &i : atoms_)
        i += Vector3::randomUnit() * maxDisplacement;
}
