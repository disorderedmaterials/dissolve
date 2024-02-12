// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/speciesAtom.h"
#include "templates/vector3.h"
#include <iterator>
#include <vector>

class Atom;
class AtomRef;
class Cell;
class ExternalPotential;
class Molecule;
class SpeciesAtom;

class AtomVector
{
    private:
    std::vector<Vec3<double>> rs_;
    // Assigned AtomType index, local to Configuration (for partial indexing etc.)
    std::vector<int> localTypeIndex_;
    // Assigned master AtomType index (for pair potential indexing)
    std::vector<int> masterTypeIndex_;
    // SpeciesAtom that this Atom represents
    std::vector<const SpeciesAtom *>speciesAtom_{nullptr};
    // Molecule in which this Atom exists
    std::vector<std::shared_ptr<Molecule>> molecule_{nullptr};
    // Cell in which the atom exists
    std::vector<Cell *>cell_{nullptr};
    // Vector of targeted potentials affecting this atom
    std::vector<std::vector<const ExternalPotential *>> targetedPotentials_;

    public:
    AtomVector(int size = 0);
    std::size_t size() const;

    // Iterators
    AtomRef begin();
    AtomRef end();
    const AtomRef begin() const;
    const AtomRef end() const;

    // Accessors
    AtomRef front();
    AtomRef back();
    AtomRef operator[](std::size_t);
    const AtomRef operator[](std::size_t) const;

    // Modifiers
    void clear();
    void reserve(std::size_t newCapacity);
    AtomRef erase(const AtomRef pos);
    AtomRef erase(const AtomRef first, const AtomRef last);
    AtomRef emplace_back();
    friend class AtomRef;
};

class AtomRef : public std::iterator<std::random_access_iterator_tag, Atom>
{
    private:
    std::size_t index_{0};
    AtomVector *origin_{nullptr};

    public:
    AtomRef();
    AtomRef(const AtomRef &ref);
    AtomRef(const std::size_t index, const AtomVector &source);
    AtomRef &operator*();
    const AtomRef &operator*() const;
    bool operator==(const AtomRef &other) const;
    bool operator!=(const AtomRef &other) const;
    AtomRef operator[](std::size_t);
    AtomRef &operator++();
    AtomRef operator++(int);
    std::size_t operator-(const AtomRef &other) const;
    int globalAtomIndex() const;

    // Return SpeciesAtom that this Atom represents
    const SpeciesAtom *speciesAtom() const;
    // Set SpeciesAtom that this Atom represents
    void setSpeciesAtom(const SpeciesAtom *spAtom);
    // Return Molecule in which this Atom exists
    const std::shared_ptr<Molecule> &molecule() const;
    // Set Molecule in which this Atom exists
    void setMolecule(std::shared_ptr<Molecule> mol);
    // Set local AtomType index
    void setLocalTypeIndex(int id);
    // Return local AtomType index
    int localTypeIndex() const;
    // Set master AtomType index
    void setMasterTypeIndex(int id);
    // Return master AtomType index
    int masterTypeIndex() const;
    // Add targeted potential to this atom
    void addTargetedPotential(const ExternalPotential *potential);
    // Clear all targeted potentials from this Atom
    void clearTargetedPotentials();
    // Return list of targeted potentials for this atom
    const std::vector<const ExternalPotential *> &targetedPotentials() const;
    // Return coordinates
    const Vec3<double> &r() const;

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
    // Set cell in which the atom exists
    void setCell(Cell *cell);
    // Return cell in which the atom exists
    Cell *cell() const;
    // Return scaling type and factors (electrostatic, van der Waals) to employ with specified Atom
    SpeciesAtom::ScaledInteractionDefinition scaling(const AtomRef j) const;
    // Set coordinates
    void set(const Vec3<double> r);
    // Set coordinates
    void set(double rx, double ry, double rz);
    // Return x-coordinate
    double x() const;
    // Return y-coordinate
    double y() const;
    // Return z-coordinate
    double z() const;
};
