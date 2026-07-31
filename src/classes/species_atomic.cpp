// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/species.h"
#include "data/atomicMasses.h"
#include <numeric>

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

// Return atom type index map
std::map<const AtomType *, int> Species::atomTypeIndexMap() const
{
    auto populations = atomTypePopulations();

    std::map<const AtomType *, int> typeMap;
    for (auto n = 0; n < populations.size(); ++n)
        typeMap[populations.key(n)] = n;

    return typeMap;
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

// Update type indices per Atom
void Species::updateTypeIndexing()
{
    // Are we currently up-to-date
    if (typeIndicesValid_)
        return;

    // Get the atom type index map
    auto typeMap = atomTypeIndexMap();

    // Loop over atoms
    for (auto &atom : atoms_)
    {
        if (atom.isPresence(AtomConstants::Presence::Physical))
            atom.setAtomTypeIndex(typeMap[atom.atomType()]);
        else
            atom.setAtomTypeIndex(AtomConstants::TypeIndex::Ignore);
    }

    typeIndicesValid_ = true;
}

// Return positional instances
const std::vector<std::vector<Vector3>> &Species::instances() const { return instances_; }

// Return as a Structure
Structure Species::asStructure(bool includeInstances) const
{
    Structure result;

    result.box() = box_;

    for (auto &i : atoms_)
        result.addAtom(i.Z(), i.r(), i.q())->setIndex(i.index());

    for (auto &bond : bonds_)
        result.addBond(bond.i()->index(), bond.j()->index());

    if (includeInstances)
        result.instances() = instances_;

    return result;
}