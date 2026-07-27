// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/speciesAtom.h"
#include "base/serialiserLibrary.h"
#include "classes/atomType.h"
#include "classes/box.h"
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
 * Serialisation
 */

// Express as a serialisable value
void SpeciesAtom::serialise(std::string tag, SerialisedValue &target) const
{
    using namespace Serialisable;
    target[tag] = {{"index", index_}, {"z", ser(Z_)}, {"r", ser(r_)}, {"q", q_}};
    if (atomType_)
        target[tag]["type"] = atomType_->name().data();
}
void SpeciesAtom::deserialise(const SerialisedValue &node)
{
    using namespace Deserialisable;
    index_ = de<int>(node.at("index"));

    set(de<Elements::Element>(node.at("z")), de<Vector3>(node.at("r")), de_or<double>(node, "q", 0));

    optionalOn(node, "type",
               [&](const auto innerNode)
               {
                   if (Z_ == Elements::Unknown)
                       return;
                   std::string name = de<std::string>(innerNode);
                   atomType_ = parent_->findAtomType(name);
                   if (atomType_ == nullptr)
                       atomType_ = parent_->addAtomType(Z_, name);
               });
}
