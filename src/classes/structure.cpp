// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/structure.h"
#include "classes/bond.h"
#include "classes/species.h"
#include "templates/algorithms.h"

Structure::Structure() : box_(Box::singleImage()) {}

Structure::Structure(const Structure &source) : box_(source.box_) { *this = source; }

Structure &Structure::operator=(const Structure &source)
{
    clear();

    for (auto &atom : source.atoms_)
    {
        auto &i = atoms_.emplace_back(std::make_unique<StructureAtom>());
        i->copy(*atom);
    }

    for (auto &bond : source.bonds_)
        addBond(bond->i()->index(), bond->j()->index());

    // Copy source box
    createBox(source.box_.axisLengths(), source.box_.axisAngles(), source.box_.type() == Box::BoxType::None);

    return *this;
}

// Clear Data
void Structure::clear()
{
    bonds_.clear();
    atoms_.clear();
    box_ = Box::singleImage();
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
const Box *Structure::box() const { return &box_; }

// Remove box definition and revert to single image
void Structure::removeBox() { box_ = Box::singleImage(); }

// Create box definition with specified lengths and angles
void Structure::createBox(const Vector3 lengths, const Vector3 angles, bool nonPeriodic)
{
    box_ = Box::generate(lengths, angles, nonPeriodic);
}

// Create Box definition from axes matrix
void Structure::createBox(const Matrix3 &axes)
{
    // Calculate cell lengths
    Vector3 lengths(axes.columnMagnitude(0), axes.columnMagnitude(1), axes.columnMagnitude(2));

    // Calculate cell angles
    Vector3 vecx, vecy, vecz;
    vecx = axes.columnAsVec3(0);
    vecy = axes.columnAsVec3(1);
    vecz = axes.columnAsVec3(2);
    vecx.normalise();
    vecy.normalise();
    vecz.normalise();

    Vector3 angles(acos(vecy.dp(vecz)), acos(vecx.dp(vecz)), acos(vecx.dp(vecy)));
    angles.toDegrees();

    box_ = Box::generate(lengths, angles);
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
                               bonds_.emplace_back(std::make_unique<Bond<StructureAtom>>(i.get(), j.get()));
                           });
}
