// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atom.h"
#include "base/serialiserLibrary.h"
#include "classes/box.h"

/*
 * Properties
 */

// Set basic properties
void AtomBase::set(Elements::Element Z, const Vector3 &r, double q)
{
    r_ = r;
    Z_ = Z;
    q_ = q;
}

// Set coordinates
void AtomBase::setR(const Vector3 &r) { r_ = r; }

// Return coordinates
const Vector3 &AtomBase::r() const { return r_; }

// Set atomic element
void AtomBase::setZ(Elements::Element Z) { Z_ = Z; }

// Return atomic element
Elements::Element AtomBase::Z() const { return Z_; }

// Return presence of atom
bool AtomBase::isPresence(AtomConstants::Presence presence) const
{
    return presence == AtomConstants::Presence::Any ||
           (Z_ == Elements::Phantom ? AtomConstants::Presence::Phantom : AtomConstants::Presence::Physical) == presence;
}

// Set atomic charge
void AtomBase::setQ(double q) { q_ = q; }

// Return atomic charge
double AtomBase::q() const { return q_; }

// Return index (0->[N-1])
int AtomBase::index() const { return index_; }

// Set index
void AtomBase::setIndex(int index) { index_ = index; }

// Set index of associated atom type in parent object
void AtomBase::setAtomTypeIndex(int id) { atomTypeIndex_ = id; }

// Return associated atom type index
int AtomBase::atomTypeIndex() const { return atomTypeIndex_; }

/*
 * Atom Geometry
 */

EnumOptions<AtomBase::AtomGeometry> AtomBase::geometries()
{
    return EnumOptions<AtomBase::AtomGeometry>("AtomGeometry", {
                                                                   {AtomGeometry::Unknown, "Unknown"},
                                                                   {AtomGeometry::Unbound, "Unbound"},
                                                                   {AtomGeometry::Terminal, "Terminal"},
                                                                   {AtomGeometry::Linear, "Linear"},
                                                                   {AtomGeometry::TShape, "TS"},
                                                                   {AtomGeometry::TrigonalPlanar, "TP"},
                                                                   {AtomGeometry::Tetrahedral, "Tet"},
                                                                   {AtomGeometry::SquarePlanar, "SqP"},
                                                                   {AtomGeometry::TrigonalBipyramidal, "TBP"},
                                                                   {AtomGeometry::Octahedral, "Oct"},
                                                               });
}

// Calculate and return the geometry of this atom
AtomBase::AtomGeometry AtomBase::geometry() const
{
    auto angle = 0.0;

    // Get connected neighbour atoms
    const auto neighbours = connectedAtoms();

    // Work based on the number of bound atoms
    switch (neighbours.size())
    {
        // 'Simple' cases first
        case (0):
            return AtomGeometry::Unbound;
        case (1):
            return AtomGeometry::Terminal;
        case (5):
            return AtomGeometry::TrigonalBipyramidal;
        case (6):
            return AtomGeometry::Octahedral;
            // For the remaining types, take averages of bond angles about the atom
        case (2):
            angle = Box::literalAngleInDegrees(neighbours[0]->r(), r_, neighbours[1]->r());
            if (angle > 150.0)
                return AtomGeometry::Linear;
            else
                return AtomGeometry::Tetrahedral;
            break;
        case (3):
            angle = Box::literalAngleInDegrees(neighbours[0]->r(), r_, neighbours[1]->r());
            angle = std::max(angle, Box::literalAngleInDegrees(neighbours[0]->r(), r_, neighbours[2]->r()));
            angle = std::max(angle, Box::literalAngleInDegrees(neighbours[1]->r(), r_, neighbours[2]->r()));
            if (angle > 150.0)
                return AtomGeometry::TShape;
            else if (angle > 115.0 && angle < 125.0)
                return AtomGeometry::TrigonalPlanar;
            else
                return AtomGeometry::Tetrahedral;
            break;
        case (4):
            // Two possibilities - tetrahedral or square planar. Tetrahedral will have an
            // average of all angles of ~ 109.5, for square planar (1/6) * (4*90 + 2*180) = 120
            angle = 0.0;
            for (auto n = 0; n < neighbours.size(); ++n)
            {
                for (auto m = n + 1; m < neighbours.size(); ++m)
                {
                    angle += Box::literalAngleInDegrees(neighbours[n]->r(), r_, neighbours[m]->r());
                }
            }
            angle /= 6.0;
            if (angle > 100.0 && angle < 115.0)
                return AtomGeometry::Tetrahedral;
            else
                return AtomGeometry::SquarePlanar;
        default:
            return AtomGeometry::Unknown;
    }
}

// Return whether the geometry of this atom matches that specified
bool AtomBase::isGeometry(AtomGeometry geom) const { return geometry() == geom; }

// Express as a serialisable value
void AtomBase::serialise(std::string tag, SerialisedValue &target) const
{
    target[tag] = Serialisable::table{{"index", index_}, {"z", Serialisable::ser(Z_)}, {"r", Serialisable::ser(r_)}, {"q", q_}};
}
// Read values from a serialisable value
void AtomBase::deserialise(const SerialisedValue &node)
{
    index_ = Deserialisable::deser<int>(node.at("index"));

    set(Deserialisable::deser<Elements::Element>(node.at("z")), Deserialisable::deser<Vector3>(node.at("r")),
        Deserialisable::deser_or<double>(node, "q", 0));
}
