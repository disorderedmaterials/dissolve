// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/molecule.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"

Molecule::Molecule() { species_ = nullptr; }

Molecule::~Molecule() = default;

/*
 * DynamicArrayObject Virtuals
 */

// Clear object, ready for re-use
void Molecule::clear()
{
    species_ = nullptr;

    atoms_.clear();
}

/*
 * Contents
 */

// Set Species that this Molecule represents
void Molecule::setSpecies(const Species *sp) { species_ = sp; }

// Return Species that this Molecule represents
const Species *Molecule::species() const { return species_; }

// Add Atom to Molecule
void Molecule::addAtom(const std::shared_ptr<Atom> &i)
{
    atoms_.push_back(i);

    if (i->molecule() != nullptr)
        Messenger::warn("Molecule parent is already set in Atom id {}, and we are about to overwrite it...\n", i->arrayIndex());
    std::shared_ptr<Molecule> parent = shared_from_this();
    i->setMolecule(parent);
}

// Return size of Atom array
int Molecule::nAtoms() const { return atoms_.size(); }

// Return atoms array
std::vector<std::shared_ptr<Atom>> &Molecule::atoms() { return atoms_; }
const std::vector<std::shared_ptr<Atom>> &Molecule::atoms() const { return atoms_; }

// Return nth Atom pointer
std::shared_ptr<Atom> Molecule::atom(int n) const { return atoms_[n]; }

/*
 * Manipulations
 */

// Recursive function for general manipulation
void Molecule::recurseLocal(std::vector<bool> &flags, const Box *box, int indexI, ManipulationFunction action)
{
    if (flags[indexI])
        return;

    // Set the flag for indexI and get some necessary values
    flags[indexI] = true;
    auto rI = atoms_[indexI]->r();
    auto *spI = atoms_[indexI]->speciesAtom();

    // Loop over attached atoms, performing minimum image repositioning w.r.t. i, and call the action
    for (const SpeciesBond &b : spI->bonds())
    {
        auto indexJ = b.partner(spI)->index();
        auto &j = atoms_[indexJ];
        if (flags[indexJ])
            continue;

        action(j, box->minimumImage(j->r(), rI));

        // Recurse into bound neighbours
        recurseLocal(flags, box, indexJ, action);
    }
}
void Molecule::recurseLocal(std::vector<bool> &flags, const Box *box, int indexI, ConstManipulationFunction action) const
{
    if (flags[indexI])
        return;

    // Set the flag for indexI and get some necessary values
    flags[indexI] = true;
    auto rI = atoms_[indexI]->r();
    auto *spI = atoms_[indexI]->speciesAtom();

    // Loop over attached atoms, performing minimum image repositioning w.r.t. i, and call the action
    for (const SpeciesBond &b : spI->bonds())
    {
        auto indexJ = b.partner(spI)->index();
        auto &j = atoms_[indexJ];
        if (flags[indexJ])
            continue;

        action(j, box->minimumImage(j->r(), rI));

        // Recurse into bound neighbours
        recurseLocal(flags, box, indexJ, action);
    }
}

// General manipulation function working on reassembled molecule
void Molecule::traverseLocal(const Box *box, ManipulationFunction action)
{
    std::vector<bool> flags(atoms_.size(), false);
    action(atoms_[0], atoms_[0]->r());
    recurseLocal(flags, box, 0, action);
}
void Molecule::traverseLocal(const Box *box, ConstManipulationFunction action) const
{
    std::vector<bool> flags(atoms_.size(), false);
    action(atoms_[0], atoms_[0]->r());
    recurseLocal(flags, box, 0, action);
}

// Un-fold molecule so it is not cut by box boundaries
Vec3<double> Molecule::unFold(const Box *box)
{
    Vec3<double> cog{0.0, 0.0, 0.0};
    traverseLocal(box, [&cog](std::shared_ptr<Atom> &j, Vec3<double> rJ) {
        j->setCoordinates(rJ);
        cog += rJ;
    });
    return cog / nAtoms();
}

// Set centre of geometry of molecule
void Molecule::setCentreOfGeometry(const Box *box, const Vec3<double> newCentre)
{
    // Calculate Molecule centre of geometry
    Vec3<double> newR;
    const auto cog = centreOfGeometry(box);

    // Apply transform
    for (auto n = 0; n < nAtoms(); ++n)
    {
        newR = box->minimumVector(atom(n), cog) + newCentre;
        atom(n)->setCoordinates(newR);
    }
}

// Calculate and return centre of geometry
Vec3<double> Molecule::centreOfGeometry(const Box *box) const
{
    if (nAtoms() == 0)
        return {};

    Vec3<double> cog{0.0, 0.0, 0.0};
    traverseLocal(box, [&cog](auto &j, auto rJ) { cog += rJ; });

    return cog / nAtoms();
}

// Transform molecule with supplied matrix, using centre of geometry as the origin
void Molecule::transform(const Box *box, const Matrix3 &transformationMatrix)
{
    // Calculate Molecule centre of geometry
    Vec3<double> newR;
    const auto cog = unFold(box);

    // Apply transform
    for (auto &i : atoms())
    {
        newR = transformationMatrix * (i->r() - cog) + cog;
        i->setCoordinates(newR);
    }
}

// Transform selected atoms with supplied matrix, around specified origin
void Molecule::transform(const Box *box, const Matrix3 &transformationMatrix, const Vec3<double> &origin,
                         const std::vector<int> &targetAtoms)
{
    // Loop over supplied Atoms
    Vec3<double> newR;
    std::shared_ptr<Atom> i;
    for (const auto index : targetAtoms)
    {
        i = atom(index);
        newR = transformationMatrix * box->minimumVector(origin, i->r()) + origin;
        i->setCoordinates(newR);
    }
}

// Translate whole molecule by the delta specified
void Molecule::translate(const Vec3<double> delta)
{
    for (auto n = 0; n < nAtoms(); ++n)
        atom(n)->translateCoordinates(delta);
}

// Translate specified atoms by the delta specified
void Molecule::translate(const Vec3<double> &delta, const std::vector<int> &targetAtoms)
{
    for (const auto i : targetAtoms)
        atom(i)->translateCoordinates(delta);
}
