// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/atomVector.h"
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
    std::vector<AtomRef> atoms_;
    // Offset of first Atom in main configuration vector
    int globalAtomOffset_{0};
    // Object's index within the parent DynamicArray
    int arrayIndex_{-1};

    public:
    // Set Species that this Molecule represents
    void setSpecies(const Species *sp);
    // Return Species that this Molecule represents
    const Species *species() const;
    // Add Atom to Molecule
    void addAtom(AtomRef i);
    // Return number of atoms in the molecule
    int nAtoms() const;
    // Return Atoms vector
    std::vector<AtomRef> &atoms();
    const std::vector<AtomRef> &atoms() const;
    // Return nth Atom pointer
    AtomRef atom(int n) const;
    // Update local atom pointers from main vector
    void updateAtoms(AtomVector &mainAtoms, int offset);
    // Return global atom offset of first atom
    int globalAtomOffset() const;
    // Sets the index of the object within the parent DynamicArray
    void setArrayIndex(int index);
    // Gets the index of the object within the parent DynamicArray
    int arrayIndex() const;

    /*
     * Manipulations
     */
    private:
    // Typedef for manipulation functions
    using ManipulationFunction = std::function<void(AtomRef j, Vec3<double> rJ)>;
    using ConstManipulationFunction = std::function<void(const AtomRef j, Vec3<double> rJ)>;
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
