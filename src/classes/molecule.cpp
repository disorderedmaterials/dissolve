// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/molecule.h"
#include "classes/atom.h"
#include "classes/box.h"

Molecule::Molecule() { species_ = nullptr; }

Molecule::~Molecule() {}

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
void Molecule::addAtom(std::shared_ptr<Atom> i)
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
        return Vec3<double>();

    // Calculate center relative to first atom in molecule
    auto cog = atom(0)->r();
    for (auto n = 1; n < nAtoms(); ++n)
        cog += box->minimumImage(atom(n), atom(0)->r());

    return (cog / nAtoms());
}

// Transform molecule with supplied matrix, using centre of geometry as the origin
void Molecule::transform(const Box *box, const Matrix3 &transformationMatrix)
{
    // Calculate Molecule centre of geometry
    Vec3<double> newR;
    const auto cog = centreOfGeometry(box);

    // Apply transform
    for (auto n = 0; n < nAtoms(); ++n)
    {
        newR = transformationMatrix * box->minimumVector(cog, atom(n)->r()) + cog;
        atom(n)->setCoordinates(newR);
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
