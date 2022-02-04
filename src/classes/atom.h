// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "templates/vector3.h"
#include <memory>
#include <vector>

// Forward Declarations
class Cell;
class Molecule;
class SpeciesAtom;

// Atom Definition
class Atom
{
    /*
     * Properties
     */
    private:
    // Coordinates
    Vec3<double> r_;
    // Assigned AtomType index, local to Configuration (for partial indexing etc.)
    int localTypeIndex_ {-1};
    // Assigned master AtomType index (for pair potential indexing)
    int masterTypeIndex_ {-1};
    // Object's index within the parent DynamicArray
    int arrayIndex_ {-1};

    public:
    // Set coordinates
    void set(const Vec3<double> r);
    // Set coordinates
    void set(double rx, double ry, double rz);
    // Return coordinates
    const Vec3<double> &r() const;
    // Return x-coordinate
    double x() const;
    // Return y-coordinate
    double y() const;
    // Return z-coordinate
    double z() const;
    // Set local AtomType index
    void setLocalTypeIndex(int id);
    // Return local AtomType index
    int localTypeIndex() const;
    // Set master AtomType index
    void setMasterTypeIndex(int id);
    // Return master AtomType index
    int masterTypeIndex() const;
    // Sets the index of the object within the parent DynamicArray
    void setArrayIndex(int index);
    // Gets the index of the object within the parent DynamicArray
    int getArrayIndex() const;

    /*
     * Location
     */
    private:
    // SpeciesAtom that this Atom represents
    const SpeciesAtom *speciesAtom_{nullptr};
    // Molecule in which this Atom exists
    std::shared_ptr<Molecule> molecule_{nullptr};
    // Cell in which the atom exists
    Cell *cell_ {nullptr};

    public:
    // Set SpeciesAtom that this Atom represents
    void setSpeciesAtom(const SpeciesAtom *spAtom);
    // Return SpeciesAtom that this Atom represents
    const SpeciesAtom *speciesAtom() const;
    // Set Molecule in which this Atom exists
    void setMolecule(std::shared_ptr<Molecule> mol);
    // Return Molecule in which this Atom exists
    const std::shared_ptr<Molecule> &molecule() const;
    // Set cell in which the atom exists
    void setCell(Cell *cell);
    // Return cell in which the atom exists
    Cell *cell() const;

    /*
     * Coordinate Manipulation
     */
    public:
    // Set coordinates
    void setCoordinates(const Vec3<double> &newr);
    // Set coordinates
    void setCoordinates(double dx, double dy, double dz);
    // Translate coordinates
    void translateCoordinates(const Vec3<double> &delta);
    // Translate coordinates
    void translateCoordinates(double dx, double dy, double dz);

    /*
     * Intramolecular Information
     */
    public:
    // Return scaling factor to employ with specified Atom
    double scaling(const Atom *j) const;
};
