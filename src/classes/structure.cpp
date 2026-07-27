// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/structure.h"
#include "base/serialiserLibrary.h"
#include "classes/bond.h"
#include "classes/species.h"
#include "templates/algorithms.h"

Structure::Structure(const Structure &source) { *this = source; }

Structure &Structure::operator=(const Structure &source)
{
    clear();

    // Copy atoms
    for (auto &atom : source.atoms_)
    {
        auto &i = atoms_.emplace_back(std::make_unique<StructureAtom>());
        i->copy(*atom);
    }

    // Copy bonds
    for (auto &bond : source.bonds_)
        addBond(bond->i()->index(), bond->j()->index());

    // Copy instances
    instances_ = source.instances_;

    // Copy source box
    box_ = source.box_;

    return *this;
}

// Clear Data
void Structure::clear()
{
    bonds_.clear();
    atoms_.clear();
    box_ = Box();
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

// Add a new atom
StructureAtom *Structure::addAtom(Elements::Element Z, Vector3 r, double q)
{
    auto &i = atoms_.emplace_back(std::make_unique<StructureAtom>());
    i->Atom::set(Z, r, q);

    i->setIndex(atoms_.size() - 1);

    return i.get();
}
StructureAtom *Structure::addAtom(const std::string &name, Vector3 r, double q)
{
    auto &i = atoms_.emplace_back(std::make_unique<StructureAtom>());
    i->set(name, r, q);

    i->setIndex(atoms_.size() - 1);

    return i.get();
}

// Remove the specified atom
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
void Structure::removeAtom(int index) { removeAtom(atom(index)); }

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

// Return the number of atoms in the structure (or only those with the specified presence)
int Structure::nAtoms(AtomConstants::Presence withPresence) const
{
    return withPresence == AtomConstants::Presence::Any
               ? atoms_.size()
               : std::count_if(atoms_.begin(), atoms_.end(),
                               [withPresence](const auto &i) { return i->isPresence(withPresence); });
}

// Return atom at index
StructureAtom *Structure::atom(int i) { return atoms_[i].get(); }
const StructureAtom *Structure::atom(int i) const { return atoms_[i].get(); }

// Return atoms
const std::vector<std::unique_ptr<StructureAtom>> &Structure::atoms() const { return atoms_; }
std::vector<std::unique_ptr<StructureAtom>> &Structure::atoms() { return atoms_; }

// Return positional instances
const std::vector<std::vector<Vector3>> &Structure::instances() const { return instances_; }
std::vector<std::vector<Vector3>> &Structure::instances() { return instances_; }

/*
 * Connectivity
 */

// Add new bond definition
Bond<StructureAtom> *Structure::addBond(StructureAtom *i, StructureAtom *j)
{
    // Check for existence of bond already
    auto bond = getBond(i, j);
    if (bond)
    {
        Messenger::warn("Refused to add a new bond between atoms {} and {} since it already exists.\n", i->index(), j->index());
        return bond;
    }

    // OK to add new Bond
    auto *newBond = bonds_.emplace_back(std::make_unique<Bond<StructureAtom>>(i, j)).get();
    i->addBond(newBond);
    j->addBond(newBond);
    return newBond;
}
Bond<StructureAtom> *Structure::addBond(int i, int j) { return addBond(atoms_[i].get(), atoms_[j].get()); }

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
void Structure::removeBond(Bond<StructureAtom> *bondToRemove)
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
std::vector<std::unique_ptr<Bond<StructureAtom>>> &Structure::bonds() { return bonds_; }
const std::vector<std::unique_ptr<Bond<StructureAtom>>> &Structure::bonds() const { return bonds_; }

// Return whether bond between specified atoms exists
bool Structure::hasBond(const StructureAtom *i, const StructureAtom *j) const
{
    return std::any_of(bonds_.cbegin(), bonds_.cend(), [i, j](const auto &bond) { return bond->isBetween(i, j); });
}

// Return the bond between the specified atoms
Bond<StructureAtom> *Structure::getBond(StructureAtom *i, StructureAtom *j)
{
    auto it = std::find_if(bonds_.begin(), bonds_.end(), [i, j](auto &bond) { return bond->isBetween(i, j); });
    if (it == bonds_.end())
        return {};

    return it->get();
}
Bond<StructureAtom> *Structure::getBond(const StructureAtom *i, const StructureAtom *j) const
{
    auto it = std::find_if(bonds_.cbegin(), bonds_.cend(), [i, j](const auto &bond) { return bond->isBetween(i, j); });
    if (it == bonds_.end())
        return {};

    return it->get();
}

// Clear bonds
void Structure::clearBonds()
{
    for (auto &bond : bonds_)
    {
        bond->i()->removeBond(bond.get());
        bond->j()->removeBond(bond.get());
    }
    bonds_.clear();
}

/*
 * Box Definition
 */

// Return periodic box
Box &Structure::box() { return box_; }
const Box &Structure::box() const { return box_; }

/*
 * Manipulations
 */

// Recursive function for general manipulation
void Structure::recurseLocal(std::set<StructureAtom *> &fragmentAtoms, StructureAtom *i, ManipulationFunction action)
{
    if (fragmentAtoms.contains(i))
        return;

    fragmentAtoms.insert(i);

    // Loop over attached atoms, performing minimum image repositioning w.r.t. i, and call the action
    for (const auto *b : i->bonds())
    {
        auto j = b->partner(i);
        if (fragmentAtoms.contains(j))
            continue;

        action(j, box_.minimumImage(j->r(), i->r()));

        // Recurse into bound neighbours
        recurseLocal(fragmentAtoms, j, action);
    }
}
void Structure::recurseLocal(std::set<StructureAtom *> &fragmentAtoms, StructureAtom *i, ConstManipulationFunction action) const
{
    if (fragmentAtoms.contains(i))
        return;

    fragmentAtoms.insert(i);

    // Loop over attached atoms, performing minimum image repositioning w.r.t. i, and call the action
    for (const auto b : i->bonds())
    {
        auto j = b->partner(i);
        if (fragmentAtoms.contains(j))
            continue;

        action(j, box_.minimumImage(j->r(), i->r()));

        // Recurse into bound neighbours
        recurseLocal(fragmentAtoms, j, action);
    }
}

// Return atoms in the same fragment as the specified atom, unfolding the fragment at the same time
std::set<StructureAtom *> Structure::getUnfoldedFragment(StructureAtom *containing, ManipulationFunction action)
{
    std::set<StructureAtom *> fragmentAtoms;
    action(containing, containing->r());
    recurseLocal(fragmentAtoms, containing, action);
    return fragmentAtoms;
}
std::set<StructureAtom *> Structure::getUnfoldedFragment(StructureAtom *containing, ConstManipulationFunction action) const
{
    std::set<StructureAtom *> fragmentAtoms;
    action(containing, containing->r());
    recurseLocal(fragmentAtoms, containing, action);
    return fragmentAtoms;
}

// Un-fold bound fragments in the structure
void Structure::unFold()
{
    std::set<StructureAtom *> fragmentAtoms;

    for (auto &atom : atoms())
    {
        if (fragmentAtoms.contains(atom.get()))
            break;

        fragmentAtoms.merge(getUnfoldedFragment(atom.get(), [](StructureAtom *j, Vector3 rJ) { j->setR(rJ); }));
    }
}

/*
 * Serialisation
 */

// Express as a serialisable value
void Structure::serialise(std::string tag, SerialisedValue &target) const
{
    auto &result = target[tag];
    Serialisable::vector<>(atoms_, "atoms", result);
    Serialisable::vector<>(bonds_, "bonds", result);
}

// Read values from a serialisable value
void Structure::deserialise(const SerialisedValue &node)
{
    using namespace Deserialisable;
    vector(node, "atoms", [this](const SerialisedValue &atom) { atoms_.emplace_back()->deserialise(atom); });

    vector(node, "bonds",
           [this](const SerialisedValue &bond)
           {
               auto &i = atoms_.at(de<int>(bond.at("i")));
               auto &j = atoms_.at(de<int>(bond.at("j")));
               bonds_.emplace_back(std::make_unique<Bond<StructureAtom>>(i.get(), j.get()));
           });
}
