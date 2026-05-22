// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/atomConstants.h"
#include "classes/atomType.h"
#include "math/vector3.h"

// Basic Atom
template <typename BondClass> class Atom : public Serialisable<>
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
    void set(Elements::Element Z, const Vector3 &r, double q = 0.0)
    {
        r_ = r;
        Z_ = Z;
        q_ = q;
    }
    // Set coordinates
    void setR(const Vector3 &r) { r_ = r; }
    // Return coordinates
    const Vector3 &r() const { return r_; }
    // Set atomic element
    void setZ(Elements::Element z) { Z_ = z; }
    // Return atomic element
    Elements::Element Z() const { return Z_; }
    // Return presence of atom
    bool isPresence(AtomConstants::Presence presence) const
    {
        return presence == AtomConstants::Presence::Any ||
               (Z_ == Elements::Phantom ? AtomConstants::Presence::Phantom : AtomConstants::Presence::Physical) == presence;
    }
    // Set atomic charge
    void setQ(double q) { q_ = q; }
    // Return atomic charge
    double q() const { return q_; }
    // Return index (0->[N-1])
    int index() const { return index_; };
    // Set index
    void setIndex(int index) { index_ = index; }
    // Set index of associated atom type in parent object
    void setAtomTypeIndex(int id) { atomTypeIndex_ = id; }
    // Return associated atom type index
    int atomTypeIndex() const { return atomTypeIndex_; }

    /*
     * Coordinate Manipulation Operators
     */
    public:
    void operator+=(const Vector3 &delta) { r_ += delta; }
    void operator-=(const Vector3 &delta) { r_ -= delta; }

    /*
     * Connectivity
     */
    protected:
    // Bonds to this atom
    std::vector<BondClass *> bonds_;

    public:
    // Return bonds to this atom
    const std::vector<BondClass *> &bonds() const { return bonds_; }
    // Add a bond to this atom
    void addBond(BondClass *bond) { bonds_.push_back(bond); }
    // Remove bond from atom
    void removeBond(BondClass *bond) { bonds_.erase(std::remove(bonds_.begin(), bonds_.end(), bond)); }
    // Get bond with the supplied atom (if it exists)
    BondClass *getBondWith(const Atom *other)
    {
        auto it = std::ranges::find_if(bonds_, [other](const auto *bond) { return bond->i() == other || bond->j() == other; });
        if (it != bonds_.end())
            return *it;
        return nullptr;
    }

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const
    {
        target[tag] = {{"index", index_}, {"z", Z_}, {"r", r_}, {"q", q_}};
    }
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node)
    {
        index_ = toml::find<int>(node, "index");

        set(toml::find<Elements::Element>(node, "z"), toml::find<Vector3>(node, "r"), toml::find_or<double>(node, "q", 0));
    }
};
