// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/structure.h"
#include "classes/species.h"

Structure::Structure() : box_(std::make_unique<SingleImageBox>()) {}

Structure::Structure(const Structure &source) { *this = source; }

Structure &Structure::operator=(const Structure &source)
{
    clear();

    for (auto &atom : source.atoms_)
        atoms_.emplace_back(std::make_unique<StructureAtom>())->set(atom->Z(), atom->r(), atom->q());

    for (auto &bond : source.bonds_)
        addBond(bond->i()->index(), bond->j()->index());

    return *this;
}

// Clear Data
void Structure::clear()
{
    bonds_.clear();
    atoms_.clear();
}

/*
 * Atoms
 */

// Renumber atoms so they are sequential in the list
void Structure::renumberAtoms() const
{
    auto count = 0;
    for (auto &i : atoms_)
        i->setIndex(count++);
}

// Add a new atom to the Species, returning its index
StructureAtom *Structure::addAtom(Elements::Element Z, Vector3 r, double q)
{
    auto &i = atoms_.emplace_back(std::make_unique<StructureAtom>());
    i->set(Z, r, q);

    i->setIndex(atoms_.size() - 1);

    return i.get();
}

// Remove the specified atom from the species
void Structure::removeAtom(const StructureAtom *atom)
{
    auto bonds = atom->bonds();
    for (auto bond : bonds)
        removeBond(bond);

    // Now remove the atom
    auto atomIt = std::find_if(atoms_.begin(), atoms_.end(), [&](const auto &i) { return i.get() == atom; });
    atoms_.erase(atomIt);

    renumberAtoms();
}

// Remove set of atom indices
void Structure::removeAtoms(const std::vector<const StructureAtom *> &atomsToRemove)
{
    for (auto atom : atomsToRemove)
    {
        auto bonds = atom->bonds();
        for (auto bond : bonds)
            removeBond(bond);
    }

    // Now remove the atoms
    auto atomIt = std::remove_if(
        atoms_.begin(), atoms_.end(), [&](const auto &i)
        { return std::ranges::find_if(atomsToRemove, [&i](const auto j) { return i.get() == j; }) != atomsToRemove.end(); });
    atoms_.erase(atomIt, atoms_.end());
    renumberAtoms();
}

// Return the number of atoms in the species (or only those with the specified presence)
int Structure::nAtoms(Atom::Presence withPresence) const
{
    return withPresence == Atom::Presence::Any ? atoms_.size()
                                               : std::count_if(atoms_.begin(), atoms_.end(), [withPresence](const auto &i)
                                                               { return i->isPresence(withPresence); });
}

// Return atoms
const std::vector<std::unique_ptr<StructureAtom>> &Structure::atoms() const { return atoms_; }
std::vector<std::unique_ptr<StructureAtom>> &Structure::atoms() { return atoms_; }

/*
 * Connectivity
 */

// Add new bond definition
StructureBond *Structure::addBond(StructureAtom *i, StructureAtom *j)
{
    // Check for existence of bond already
    auto bond = getBond(i, j);
    if (bond)
    {
        Messenger::warn("Refused to add a new bond between atoms {} and {} since it already exists.\n", i->index(), j->index());
        return bond;
    }

    // OK to add new Bond
    return bonds_.emplace_back(std::make_unique<StructureBond>(i, j)).get();
}
StructureBond *Structure::addBond(int i, int j) { return addBond(atoms_[i].get(), atoms_[j].get()); }

// Remove bond
void Structure::removeBond(StructureAtom *i, StructureAtom *j)
{
    // Find the bond
    auto it = std::remove_if(bonds_.begin(), bonds_.end(), [i, j](const auto &bond) { return bond->isBetween(i, j); });
    if (it == bonds_.end())
        return;

    auto bond = it->get();
    bond->i()->removeBond(bond);
    bond->j()->removeBond(bond);

    // Erase the bond
    bonds_.erase(it);
}
void Structure::removeBond(StructureBond *bondToRemove)
{
    auto it = std::ranges::find_if(bonds_, [bondToRemove](const auto &bond) { return bond.get() == bondToRemove; });
    if (it == bonds_.end())
        return;

    auto bond = it->get();
    bond->i()->removeBond(bond);
    bond->j()->removeBond(bond);

    bonds_.erase(it);
}

// Return vector of bonds
std::vector<std::unique_ptr<StructureBond>> &Structure::bonds() { return bonds_; }
const std::vector<std::unique_ptr<StructureBond>> &Structure::bonds() const { return bonds_; }

// Return whether bond between specified atoms exists
bool Structure::hasBond(const StructureAtom *i, const StructureAtom *j) const
{
    return std::any_of(bonds_.cbegin(), bonds_.cend(), [i, j](const auto &bond) { return bond->isBetween(i, j); });
}

// Return the bond between the specified atoms
StructureBond *Structure::getBond(StructureAtom *i, StructureAtom *j)
{
    auto it = std::find_if(bonds_.begin(), bonds_.end(), [i, j](auto &bond) { return bond->isBetween(i, j); });
    if (it == bonds_.end())
        return {};

    return it->get();
}
StructureBond *Structure::getBond(const StructureAtom *i, const StructureAtom *j) const
{
    auto it = std::find_if(bonds_.cbegin(), bonds_.cend(), [i, j](const auto &bond) { return bond->isBetween(i, j); });
    if (it == bonds_.end())
        return {};

    return it->get();
}

/*
 * Operations
 */

// Recursively add atoms along any path from the specified one, ignoring the bond(s) provided
void Structure::getIndicesRecursive(std::vector<int> &indices, int index, StructureBond *exclude,
                                    StructureBond *excludeToo) const
{
    // Loop over Bonds on specified Atom
    indices.emplace_back(index);
    const auto &i = atoms_[index];
    for (const auto *bond : i->bonds())
    {
        // Is this either of the excluded bonds?
        if (exclude == bond or excludeToo == bond)
            continue;

        // Get the partner atom in the bond and select it (if it is not selected already)
        auto *j = bond->partner(i.get());
        if (std::find(indices.begin(), indices.end(), j->index()) == indices.end())
            getIndicesRecursive(indices, j->index(), exclude, excludeToo);
    }
}

// Return the fragment containing the specified atom, optionally ignoring paths along the bond(s) provided
std::vector<int> Structure::fragment(int startIndex, StructureBond *exclude, StructureBond *excludeToo) const
{
    std::vector<int> indices;
    getIndicesRecursive(indices, startIndex, exclude, excludeToo);
    return indices;
}

/*
 * Box Definition
 */

// Return periodic box
const Box *Structure::box() const { return box_.get(); }

// Remove box definition and revert to single image
void Structure::removeBox() { box_ = std::make_unique<SingleImageBox>(); }

// Create box definition with specified lengths and angles
void Structure::createBox(const Vector3 lengths, const Vector3 angles, bool nonPeriodic)
{
    box_ = nonPeriodic ? std::make_unique<NonPeriodicBox>() : Box::generate(lengths, angles);
}

/*
 * Serialisation
 */

// Express as a serialisable value
void Structure::serialise(std::string tag, SerialisedValue &target) const
{
    auto &result = target[tag];
    Serialisable::fromVector<>(atoms_, "atoms", result);
    Serialisable::fromVector<>(bonds_, "bonds", result);
}

// Read values from a serialisable value
void Structure::deserialise(const SerialisedValue &node)
{
    Serialisable::toVector(node, "atoms", [this](const SerialisedValue &atom) { atoms_.emplace_back()->deserialise(atom); });

    Serialisable::toVector(node, "bonds",
                           [this](const SerialisedValue &bond)
                           {
                               auto &i = atoms_.at(toml::find<int>(bond, "i"));
                               auto &j = atoms_.at(toml::find<int>(bond, "j"));
                               bonds_.emplace_back(std::make_unique<StructureBond>(i.get(), j.get()));
                           });
}
