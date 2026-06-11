// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/structure.h"
#include "classes/bond.h"
#include "classes/species.h"
#include "data/atomicMasses.h"
#include "templates/algorithms.h"

Structure::Structure() : box_(Box::none()) {}

Structure::Structure(const Structure &source) : box_(source.box_) { *this = source; }

Structure &Structure::operator=(const Structure &source)
{
    clear();

    // Copy source atoms
    for (auto &atom : source.atoms_)
    {
        auto &i = atoms_.emplace_back(std::make_unique<StructureAtom>());
        i->copy(*atom);
    }

    // Copy source connectivity
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
    box_ = Box::none();
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

// Return molecular species coordinates
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
const Box &Structure::box() const { return box_; }

// Remove box definition and revert to single image
void Structure::removeBox() { box_ = Box::none(); }

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
 * Manipulations
 */

// Recursive function for general manipulation
void Structure::recurseLocal(std::vector<bool> &flags, const Box *box, int indexI, ManipulationFunction action)
{
    if (flags[indexI])
        return;

    // Set the flag for indexI and get some necessary values
    flags[indexI] = true;
    auto rI = atoms_[indexI]->r();
    auto *strAtI = atoms_[indexI].get();

    // Loop over attached atoms, performing minimum image repositioning w.r.t. i, and call the action
    for (const auto *b : strAtI->bonds())
    {
        auto indexJ = b->partner(strAtI)->index();
        auto &j = atoms_[indexJ];
        if (flags[indexJ])
            continue;

        action(j.get(), box->minimumImage(j->r(), rI));

        // Recurse into bound neighbours
        recurseLocal(flags, box, indexJ, action);
    }
}
void Structure::recurseLocal(std::vector<bool> &flags, const Box *box, int indexI, ConstManipulationFunction action) const
{
    if (flags[indexI])
        return;

    // Set the flag for indexI and get some necessary values
    flags[indexI] = true;
    auto rI = atoms_[indexI]->r();
    auto *strAtI = atoms_[indexI].get();

    // Loop over attached atoms, performing minimum image repositioning w.r.t. i, and call the action
    for (const auto b : strAtI->bonds())
    {
        auto indexJ = b->partner(strAtI)->index();
        auto &j = atoms_[indexJ];
        if (flags[indexJ])
            continue;

        action(j.get(), box->minimumImage(j->r(), rI));

        // Recurse into bound neighbours
        recurseLocal(flags, box, indexJ, action);
    }
}

// General manipulation function working on reassembled molecule
void Structure::traverseLocal(const Box *box, ManipulationFunction action)
{
    std::vector<bool> flags(atoms_.size(), false);
    action(atoms_[0].get(), atoms_[0]->r());
    recurseLocal(flags, box, 0, action);
}
void Structure::traverseLocal(const Box *box, ConstManipulationFunction action) const
{
    std::vector<bool> flags(atoms_.size(), false);
    action(atoms_[0].get(), atoms_[0]->r());
    recurseLocal(flags, box, 0, action);
}

// Un-fold molecule so it is not cut by box boundaries
Vector3 Structure::unFold()
{
    Vector3 cog{0.0, 0.0, 0.0};
    traverseLocal(box_.get(),
                  [&cog](StructureAtom *j, Vector3 rJ)
                  {
                      j->setR(rJ);
                      cog += rJ;
                  });
    return cog / nAtoms();
}

// Set centre of geometry of molecule
void Structure::setCentreOfGeometry(const Vector3 &newCentre)
{
    // Calculate Molecule centre of geometry
    Vector3 newR;
    const auto cog = centreOfGeometry();

    // Apply transform
    for (auto n = 0; n < nAtoms(); ++n)
    {
        newR = box_->minimumVector(atom(n)->r(), cog) + newCentre;
        atom(n)->setR(newR);
    }
}

// Calculate and return centre of geometry
Vector3 Structure::centreOfGeometry() const
{
    if (nAtoms() == 0)
        return {};

    Vector3 cog{0.0, 0.0, 0.0};
    traverseLocal(box_.get(), [&cog](auto *j, auto rJ) { cog += rJ; });

    return cog / nAtoms();
}

// Calculate and return centre of geometry over supplied atom indices
Vector3 Structure::centreOfGeometry(const std::vector<int> &indices) const
{
    const auto ref = atoms_[indices.front()]->r();
    return std::accumulate(std::next(indices.begin()), indices.end(), ref,
                           [&](const auto &acc, const auto idx) { return acc + box_->minimumImage(atoms_[idx]->r(), ref); }) /
           indices.size();
}

// Calculate and return centre of mass over supplied atom indices
Vector3 Structure::centreOfMass(const std::vector<int> &indices) const
{
    auto mass = AtomicMass::mass(atoms_[indices.front()]->Z());
    const auto ref = atoms_[indices.front()]->r();
    auto sums = std::accumulate(std::next(indices.begin()), indices.end(), std::pair<Vector3, double>(ref * mass, mass),
                                [&](const auto &acc, const auto idx)
                                {
                                    auto mass = AtomicMass::mass(atoms_[idx]->Z());
                                    return std::pair<Vector3, double>(
                                        acc.first + box_->minimumImage(atoms_[idx]->r(), ref) * mass, acc.second + mass);
                                });
    return sums.first / sums.second;
}

// Transform molecule with supplied matrix, using centre of geometry as the origin
void Structure::transform(const Matrix3 &transformationMatrix)
{
    // Unfold and get Molecule centre of geometry
    const auto cog = unFold();

    // Apply transform
    for (auto &i : atoms())
        i->setR(transformationMatrix * (i->r() - cog) + cog);
}

// Transform molecule with supplied matrix about specified origin
void Structure::transform(const Matrix3 &transformationMatrix, const Vector3 &origin)
{
    // Unfold
    unFold();

    // Apply transform
    for (auto &i : atoms())
        i->setR(transformationMatrix * (i->r() - origin) + origin);
}

// Transform selected atoms with supplied matrix, around specified origin
void Structure::transform(const Matrix3 &transformationMatrix, const Vector3 &origin, const std::vector<int> &targetAtoms)
{
    // Loop over supplied Atoms
    Vector3 newR;
    StructureAtom *i;
    for (const auto index : targetAtoms)
    {
        i = atom(index);
        newR = transformationMatrix * box_->minimumVector(origin, i->r()) + origin;
        i->setR(newR);
    }
}

// Translate whole molecule by the delta specified
void Structure::translate(const Vector3 &delta)
{
    for (auto n = 0; n < nAtoms(); ++n)
        *atom(n) += delta;
}

// Translate specified atoms by the delta specified
void Structure::translate(const Vector3 &delta, const std::vector<int> &targetAtoms)
{
    for (const auto i : targetAtoms)
        *atom(i) += delta;
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
