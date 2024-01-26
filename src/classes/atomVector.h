// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/vector3.h"
#include <iterator>
#include <vector>

class Atom;
class AtomRef;
class ExternalPotential;
class Molecule;
class SpeciesAtom;

class AtomVector
{
    private:
    std::vector<Atom> atoms_;

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
    Atom *ref_{nullptr};
    AtomVector *origin_{nullptr};

    public:
    AtomRef();
    AtomRef(const AtomRef &ref);
    AtomRef(const Atom &ref, const AtomVector &source);
    Atom &operator*();
    const Atom &operator*() const;
    Atom *operator->();
    const Atom *operator->() const;
    bool operator==(const AtomRef &other) const;
    bool operator!=(const AtomRef &other) const;
    bool operator==(Atom *other) const;
    AtomRef operator[](std::size_t);
    AtomRef &operator++();
    AtomRef operator++(int);
    std::size_t operator-(const AtomRef &other) const;
    int globalAtomIndex() const;

    // Set SpeciesAtom that this Atom represents
    void setSpeciesAtom(const SpeciesAtom *spAtom);
    // Set Molecule in which this Atom exists
    void setMolecule(std::shared_ptr<Molecule> mol);
    // Set local AtomType index
    void setLocalTypeIndex(int id);
    // Return local AtomType index
    int localTypeIndex() const;
    // Set master AtomType index
    void setMasterTypeIndex(int id);
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
};
