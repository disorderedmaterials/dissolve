// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/atomConstants.h"
#include "classes/atomType.h"
#include "math/vector3.h"

// Forward Declarations
class Bond;

// Basic Atom
class Atom : public Serialisable<>
{
    public:
    Atom() = default;
    virtual ~Atom() = default;

    /*
     * Properties
     */
    public:
    protected:
    // Coordinates
    Vector3 r_;
    // Atomic element
    Elements::Element Z_{Elements::Unknown};
    // Charge
    double q_{0.0};
    // Index in parent
    int index_{-1};
    // Atom type index in parent object
    int atomTypeIndex_{AtomConstants::TypeIndex::Ignore};

    public:
    // Set basic properties
    void set(Elements::Element Z, const Vector3 &r, double q = 0.0);
    // Set coordinates
    void setR(const Vector3 &r);
    // Return coordinates
    const Vector3 &r() const;
    // Set atomic element
    void setZ(Elements::Element z);
    // Return atomic element
    Elements::Element Z() const;
    // Return presence of atom
    AtomConstants::Presence presence() const;
    // Return whether the atom is of the presence specified
    bool isPresence(AtomConstants::Presence presence) const;
    // Set atomic charge
    void setQ(double q);
    // Return atomic charge
    double q() const;
    // Set index
    void setIndex(int index);
    // Return index
    int index() const;
    // Set index of associated atom type in parent object
    void setAtomTypeIndex(int id);
    // Return associated atom type index
    int atomTypeIndex() const;

    /*
     * Coordinate Manipulation Operators
     */
    public:
    void operator+=(const Vector3 &delta);
    void operator-=(const Vector3 &delta);

    /*
     * Connectivity
     */
    private:
    // Bonds to this atom
    std::vector<Bond *> bonds_;

    public:
    // Return bonds to this atom
    const std::vector<Bond *> &bonds() const;
    // Add a bond to this atom
    void addBond(Bond *bond);
    // Remove bond from atom
    void removeBond(Bond *bond);

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
