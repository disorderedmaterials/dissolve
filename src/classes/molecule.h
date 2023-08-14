// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "templates/vector3.h"
#include <functional>
#include <memory>
#include <vector>

// Forward Declarations
class Atom;
class Box;
class Matrix3;
class Species;

// Molecule Definition
class Molecule : public std::enable_shared_from_this<Molecule>
{
    /*
     * Contents
     */
    private:
    // Species that this Molecule represents
    const Species *species_{nullptr};
    // Vector of pointers to Atoms that belong to this Molecule
    std::vector<Atom *> atoms_;
    // Offset of first Atom in main configuration vector
    int globalAtomOffset_{0};

    public:
    // Set Species that this Molecule represents
    void setSpecies(const Species *sp);
    // Return Species that this Molecule represents
    const Species *species() const;
    // Add Atom to Molecule
    void addAtom(Atom *i);
    // Return number of atoms in the molecule
    int nAtoms() const;
    // Return Atoms vector
    std::vector<Atom *> &atoms();
    const std::vector<Atom *> &atoms() const;
    // Return nth Atom pointer
    Atom *atom(int n) const;
    // Update local atom pointers from main vector
    void updateAtoms(std::vector<Atom> &mainAtoms, int offset);
    // Return global atom offset of first atom
    int globalAtomOffset() const;
    // Return global index of supplied atom
    int globalAtomIndex(const Atom *i) const;

    /*
     * Manipulations
     */
    private:
    // Typedef for manipulation functions
    using ManipulationFunction = std::function<void(Atom *j, Vec3<double> rJ)>;
    using ConstManipulationFunction = std::function<void(const Atom *j, Vec3<double> rJ)>;
    // Recursive function for general manipulation
    void recurseLocal(std::vector<bool> &flags, const Box *box, int indexI, ManipulationFunction action);
    void recurseLocal(std::vector<bool> &flags, const Box *box, int indexI, ConstManipulationFunction action) const;
    // General manipulation function working on reassembled molecule
    void traverseLocal(const Box *box, ManipulationFunction action);
    void traverseLocal(const Box *box, ConstManipulationFunction action) const;

    public:
    // Un-fold molecule so it is not cut by box boundaries, returning the centre of geometry
    Vec3<double> unFold(const Box *box);
    // Set centre of geometry
    void setCentreOfGeometry(const Box *box, const Vec3<double> newCentre);
    // Calculate and return centre of geometry
    Vec3<double> centreOfGeometry(const Box *box) const;
    // Transform molecule with supplied matrix, using centre of geometry as the origin
    void transform(const Box *box, const Matrix3 &transformationMatrix);
    // Transform molecule with supplied matrix about specified origin
    void transform(const Box *box, const Matrix3 &transformationMatrix, const Vec3<double> &origin);
    // Transform selected atoms with supplied matrix, around specified origin
    void transform(const Box *box, const Matrix3 &transformationMatrix, const Vec3<double> &origin,
                   const std::vector<int> &targetAtoms);
    // Translate whole molecule by the delta specified
    void translate(const Vec3<double> delta);
    // Translate specified atoms by the delta specified
    void translate(const Vec3<double> &delta, const std::vector<int> &targetAtoms);
};
