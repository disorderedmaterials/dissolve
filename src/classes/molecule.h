// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/vector3.h"
#include <functional>
#include <memory>
#include <vector>

// Forward Declarations
class ConfigurationAtom;
class Box;
class Matrix3;
class Species;

// Molecule Definition
class Molecule : public std::enable_shared_from_this<Molecule>
{
    /*
     * Contents
     */
    protected:
    // Species that this Molecule represents
    const Species *species_{nullptr};
    // Vector of pointers to Atoms that belong to this Molecule
    std::vector<ConfigurationAtom *> atoms_;
    // Offset of first Atom in main configuration vector
    int globalAtomOffset_{0};
    // Object's index within the parent DynamicArray
    int arrayIndex_{-1};

    public:
    // Set Species that this Molecule represents
    virtual void setSpecies(const Species *sp);
    // Return Species that this Molecule represents
    const Species *species() const;
    // Add Atom to Molecule
    virtual void addAtom(ConfigurationAtom *i);
    // Return number of atoms in the molecule
    int nAtoms() const;
    // Return Atoms vector
    std::vector<ConfigurationAtom *> &atoms();
    const std::vector<ConfigurationAtom *> &atoms() const;
    // Return nth Atom pointer
    ConfigurationAtom *atom(int n) const;
    // Update local atom pointers from main vector
    virtual void updateAtoms(std::vector<ConfigurationAtom> &mainAtoms, int offset);
    // Return global atom offset of first atom
    int globalAtomOffset() const;
    // Return global index of supplied atom
    int globalAtomIndex(const ConfigurationAtom *i) const;
    // Sets the index of the object within the parent DynamicArray
    void setArrayIndex(int index);
    // Gets the index of the object within the parent DynamicArray
    int arrayIndex() const;

    /*
     * Manipulations
     */
    private:
    // Typedef for manipulation functions
    using ManipulationFunction = std::function<void(ConfigurationAtom *j, Vector3 rJ)>;
    using ConstManipulationFunction = std::function<void(const ConfigurationAtom *j, Vector3 rJ)>;
    // Recursive function for general manipulation
    void recurseLocal(std::vector<bool> &flags, const Box &box, int indexI, ManipulationFunction action);
    void recurseLocal(std::vector<bool> &flags, const Box &box, int indexI, ConstManipulationFunction action) const;
    // General manipulation function working on reassembled molecule
    void traverseLocal(const Box &box, ManipulationFunction action);
    void traverseLocal(const Box &box, ConstManipulationFunction action) const;

    public:
    // Un-fold molecule so it is not cut by box boundaries, returning the centre of geometry
    Vector3 unFold(const Box &box);
    // Set centre of geometry
    void setCentreOfGeometry(const Box &box, const Vector3 &newCentre);
    // Calculate and return centre of geometry
    Vector3 centreOfGeometry(const Box &box) const;
    // Calculate and return centre of geometry over supplied atom indices
    Vector3 centreOfGeometry(const Box &box, const std::vector<int> &indices) const;
    // Calculate and return centre of mass over supplied atom indices
    Vector3 centreOfMass(const Box &box, const std::vector<int> &indices) const;
    // Transform molecule with supplied matrix, using centre of geometry as the origin
    void transform(const Box &box, const Matrix3 &transformationMatrix);
    // Transform molecule with supplied matrix about specified origin
    void transform(const Box &box, const Matrix3 &transformationMatrix, const Vector3 &origin);
    // Transform selected atoms with supplied matrix, around specified origin
    void transform(const Box &box, const Matrix3 &transformationMatrix, const Vector3 &origin,
                   const std::vector<int> &targetAtoms);
    // Translate whole molecule by the delta specified
    void translate(const Vector3 &delta);
    // Translate specified atoms by the delta specified
    void translate(const Vector3 &delta, const std::vector<int> &targetAtoms);
};
