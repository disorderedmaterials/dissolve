// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/speciesAtom.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "classes/speciesBond.h"
#include <algorithm>

SpeciesAtom::SpeciesAtom(Species *parent) : parent_(parent) {}

/*
 * Properties
 */

// Return parent Species
Species *SpeciesAtom::parent() const { return parent_; }

// Set AtomType of SpeciesAtom
void SpeciesAtom::setAtomType(const AtomType *at)
{
    // Check elements
    if (at && (at->Z() != Z_))
    {
        Messenger::warn("Refused to assign AtomType '{}' to an atom of element {}, since the element of the AtomType is {}.\n",
                        at->name(), Elements::symbol(Z_), Elements::symbol(at->Z()));
        return;
    }

    atomType_ = at;
}

// Return SpeciesAtomType of SpeciesAtom
const AtomType *SpeciesAtom::atomType() const { return atomType_; }

/*
 * Bond Information
 */
//
// // Add Bond reference
// void SpeciesAtom::addBond(SpeciesBond &bond)
// {
//     if (find_if(bonds_.begin(), bonds_.end(), [&bond](const SpeciesBond &b) { return &b == &bond; }) == bonds_.end())
//         bonds_.emplace_back(bond);
// }
//
// // Remove Bond reference
// void SpeciesAtom::removeBond(SpeciesBond &b)
// {
//     bonds_.erase(find_if(bonds_.begin(), bonds_.end(), [&b](const SpeciesBond &bond) { return &b == &bond; }));
// }
//
// // Return number of Bond references
// int SpeciesAtom::nBonds() const { return bonds_.size(); }
//
// // Return specified bond
// SpeciesBond &SpeciesAtom::bond(int index) { return bonds_.at(index); }
//
// // Return bonds list
// const std::vector<std::reference_wrapper<SpeciesBond>> &SpeciesAtom::bonds() const { return bonds_; }
//
// // Return whether Bond to specified Atom exists
// OptionalReferenceWrapper<SpeciesBond> SpeciesAtom::getBond(const SpeciesAtom *partner)
// {
//     auto result = find_if(bonds_.begin(), bonds_.end(), [&](const SpeciesBond &bond) { return bond.partner(this) == partner;
//     }); if (result == bonds_.end())
//         return std::nullopt;
//     return *result;
// }

// Add specified SpeciesAngle to Atom
void SpeciesAtom::addAngle(const SpeciesAngle *angle) { angles_.emplace_back(angle); }

// Return array of Angles in which the Atom is involved
const std::vector<const SpeciesAngle *> &SpeciesAtom::angles() const { return angles_; }

// Add specified SpeciesTorsion to Atom
void SpeciesAtom::addTorsion(const SpeciesTorsion *torsion) { torsions_.emplace_back(torsion); }

// Return array of Torsions in which the Atom is involved
const std::vector<const SpeciesTorsion *> &SpeciesAtom::torsions() const { return torsions_; }

// Add specified SpeciesImproper to Atom
void SpeciesAtom::addImproper(const SpeciesImproper *improper) { impropers_.emplace_back(improper); }

// Return array of Impropers in which the Atom is involved
const std::vector<const SpeciesImproper *> &SpeciesAtom::impropers() const { return impropers_; }

// Set all scaled intramolecular interactions
void SpeciesAtom::setScaledInteractions()
{
    scaledInteractions_.clear();

    std::function<void(SpeciesAtom *, SpeciesAtom::ScaledInteraction, double, double)> addInteractionFunction =
        [&](SpeciesAtom *j, SpeciesAtom::ScaledInteraction scaledType, double elecScale, double srScale)
    {
        auto it =
            std::find_if(scaledInteractions_.begin(), scaledInteractions_.end(), [j](const auto &p) { return p.first == j; });
        if (it == scaledInteractions_.end())
            scaledInteractions_.emplace_back(j, ScaledInteractionDefinition{scaledType, elecScale, srScale});
    };

    /*
     * Add atoms to our scaledInteractions_ vector with appropriate scaling factors based on the intramolecular term in which
     * the atoms exist. We never overwrite a scaling factor from a "lower order" interaction (i.e. a torsion 1-4 can never
     * override a bond or an angle exclusion) so need to search the list for each partner atom every time.
     */

    // Bonds
    for (const auto b : bonds_)
        addInteractionFunction(b->partner(this), SpeciesAtom::ScaledInteraction::Excluded, 0.0, 0.0);

    // Angles
    for (const auto angle : angles_)
    {
        if (angle->i() != this)
            addInteractionFunction(angle->i(), ScaledInteraction::Excluded, 0.0, 0.0);
        if (angle->j() != this)
            addInteractionFunction(angle->j(), ScaledInteraction::Excluded, 0.0, 0.0);
        if (angle->k() != this)
            addInteractionFunction(angle->k(), ScaledInteraction::Excluded, 0.0, 0.0);
    }

    // Torsions
    for (const auto torsion : torsions_)
    {
        if (torsion->i() == this)
        {
            addInteractionFunction(torsion->j(), ScaledInteraction::Excluded, 0.0, 0.0);
            addInteractionFunction(torsion->k(), ScaledInteraction::Excluded, 0.0, 0.0);
            addInteractionFunction(torsion->l(), ScaledInteraction::Scaled, torsion->electrostatic14Scaling(),
                                   torsion->vanDerWaals14Scaling());
        }
        else if (torsion->l() == this)
        {
            addInteractionFunction(torsion->i(), ScaledInteraction::Scaled, torsion->electrostatic14Scaling(),
                                   torsion->vanDerWaals14Scaling());
            addInteractionFunction(torsion->j(), ScaledInteraction::Excluded, 0.0, 0.0);
            addInteractionFunction(torsion->k(), ScaledInteraction::Excluded, 0.0, 0.0);
        }
        else
        {
            addInteractionFunction(torsion->i(), ScaledInteraction::Excluded, 0.0, 0.0);
            addInteractionFunction(torsion->l(), ScaledInteraction::Excluded, 0.0, 0.0);
            if (torsion->j() != this)
                addInteractionFunction(torsion->j(), ScaledInteraction::Excluded, 0.0, 0.0);
            if (torsion->k() != this)
                addInteractionFunction(torsion->k(), ScaledInteraction::Excluded, 0.0, 0.0);
        }
    }
}

// Return scaling type and factors (electrostatic, van der Waals) to employ with specified Atom
SpeciesAtom::ScaledInteractionDefinition SpeciesAtom::scaling(const SpeciesAtom *j) const
{
    auto it = std::find_if(scaledInteractions_.begin(), scaledInteractions_.end(), [j](const auto &p) { return p.first == j; });
    if (it != scaledInteractions_.end())
        return it->second;
    return {SpeciesAtom::ScaledInteraction::NotScaled, 1.0, 1.0};
}
/*
 * Atom Environment Helpers
 */

EnumOptions<SpeciesAtom::AtomGeometry> SpeciesAtom::geometries()
{
    return EnumOptions<SpeciesAtom::AtomGeometry>("AtomGeometry", {
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
SpeciesAtom::AtomGeometry SpeciesAtom::geometry() const { return geometry(this); }

// Return whether the geometry of this atom matches that specified
bool SpeciesAtom::isGeometry(SpeciesAtom::AtomGeometry geom) const { return geometry(this) == geom; }

// Calculate and return the geometry of the specified SpeciesAtom
SpeciesAtom::AtomGeometry SpeciesAtom::geometry(const SpeciesAtom *i)
{
    double angle, largest;
    SpeciesAtom *h, *j;
    const auto &bonds = i->bonds();

    // Work based on the number of bound atoms
    switch (bonds.size())
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
            h = bonds[0]->partner(i);
            j = bonds[1]->partner(i);
            angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
            if (angle > 150.0)
                return AtomGeometry::Linear;
            else
                return AtomGeometry::Tetrahedral;
            break;
        case (3):
            h = bonds[0]->partner(i);
            j = bonds[1]->partner(i);
            angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
            largest = angle;
            j = bonds[2]->partner(i);
            angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
            if (angle > largest)
                largest = angle;
            h = bonds[1]->partner(i);
            angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
            if (angle > largest)
                largest = angle;
            if (largest > 150.0)
                return AtomGeometry::TShape;
            else if ((largest > 115.0) && (largest < 125.0))
                return AtomGeometry::TrigonalPlanar;
            else
                return AtomGeometry::Tetrahedral;
            break;
        case (4):
            // Two possibilities - tetrahedral or square planar. Tetrahedral will have an
            // average of all angles of ~ 109.5, for square planar (1/6) * (4*90 + 2*180) = 120
            angle = 0.0;
            for (auto n = 0; n < i->bonds().size(); ++n)
            {
                h = bonds[n]->partner(i);
                for (auto m = n + 1; m < i->bonds().size(); ++m)
                {
                    j = bonds[m]->partner(i);
                    angle += NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
                }
            }
            angle /= 6.0;
            if ((angle > 100.0) && (angle < 115.0))
                return AtomGeometry::Tetrahedral;
            else
                return AtomGeometry::SquarePlanar;
    }

    return AtomGeometry::Unknown;
}

// Return whether the specified SpeciesAtom exists in the specified geometry
bool SpeciesAtom::isGeometry(const SpeciesAtom *i, AtomGeometry geom) { return geometry(i) == geom; }

/*
 * Serialisation
 */

// Express as a serialisable value
void SpeciesAtom::serialise(std::string tag, SerialisedValue &target) const
{
    target[tag] = {{"index", index_}, {"z", Z_}, {"r", r_}, {"q", q_}};
    if (atomType_)
        target[tag]["type"] = atomType_->name().data();
}
void SpeciesAtom::deserialise(const SerialisedValue &node)
{
    index_ = toml::find<int>(node, "index");

    set(toml::find<Elements::Element>(node, "z"), toml::find<Vector3>(node, "r"), toml::find_or<double>(node, "q", 0));

    Serialisable::optionalOn(node, "type",
                             [&](const auto innerNode)
                             {
                                 if (Z_ == Elements::Unknown)
                                     return;
                                 std::string name = toml::get<std::string>(innerNode);
                                 atomType_ = parent_->findAtomType(name);
                                 if (atomType_ == nullptr)
                                     atomType_ = parent_->addAtomType(Z_, name);
                             });
}
