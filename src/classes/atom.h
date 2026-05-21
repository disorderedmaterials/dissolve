// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "classes/atomConstants.h"
#include "classes/bond.h"
#include "data/elements.h"
#include "math/vector3.h"

// Base Atom
class BaseAtom
{
    /*
     * Properties
     */
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
    bool isPresence(AtomConstants::Presence presence) const;
    // Set atomic charge
    void setQ(double q);
    // Return atomic charge
    double q() const;
    // Return index (0->[N-1])
    int index() const;
    // Set index
    void setIndex(int index);
    // Set index of associated atom type in parent object
    void setAtomTypeIndex(int id);
    // Return associated atom type index
    int atomTypeIndex() const;

    /*
     * General Connectivity
     */
    public:
    // Return number of bonds
    virtual int nBonds() const = 0;
    // Return other BaseAtoms connected to this one via bonds
    virtual std::vector<BaseAtom *> connectedAtoms() const = 0;

    /*
     * Atom Geometry
     */
    public:
    // Atom Geometry enum
    enum class AtomGeometry
    {
        Unknown,
        Unbound,
        Terminal,
        Linear,
        TShape,
        TrigonalPlanar,
        Tetrahedral,
        SquarePlanar,
        TrigonalBipyramidal,
        Octahedral
    };
    // Return EnumOptions for AtomGeometry
    static EnumOptions<AtomGeometry> geometries();
    // Calculate and return the geometry of this atom
    AtomGeometry geometry() const;
    // Return whether the geometry of this atom matches that specified
    bool isGeometry(AtomGeometry geom) const;
};

// Atom
template <typename BondClass> class Atom : public BaseAtom, public Serialisable<>
{
    public:
    Atom() = default;
    virtual ~Atom() = default;

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
    // Return number of bonds
    int nBonds() const { return bonds_.size(); }
    // Return indices of other BaseAtoms to which this one is connected
    std::vector<BaseAtom *> connectedAtoms() const
    {
        std::vector<BaseAtom *> connections;
        for (const auto *bond : bonds_)
            connections.emplace_back(bond->partner(this));
        return connections;
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
