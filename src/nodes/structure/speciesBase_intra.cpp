// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "nodes/structure/speciesBase.h"
#include "templates/algorithms.h"
#include <algorithm>

// Return vector of SpeciesBond
std::vector<SpeciesBond> &SpeciesBaseNode::bonds() { return bonds_; }

const std::vector<SpeciesBond> &SpeciesBaseNode::bonds() const { return bonds_; }

// Return the SpeciesBond between the specified SpeciesAtoms, if it exists
OptionalReferenceWrapper<const SpeciesBond> SpeciesBaseNode::getBond(const SpeciesAtom *i, const SpeciesAtom *j) const
{
    auto it = std::find_if(bonds_.begin(), bonds_.end(), [i, j](auto &bond) { return bond.matches(i, j); });
    if (it == bonds_.end())
        return {};

    return *it;
}
OptionalReferenceWrapper<const SpeciesBond> SpeciesBaseNode::getBond(int i, int j) const
{
    return getBond(&atoms_.at(i), &atoms_.at(j));
}

// Remove bonds crossing periodic boundaries
void SpeciesBaseNode::removePeriodicBonds()
{
    if (box_.type() == Box::BoxType::None)
        return;

    auto it = std::remove_if(bonds_.begin(), bonds_.end(),
                             [&](const auto &b)
                             {
                                 // Check the literal vs the minimum image distance between the involved atoms 'i' and 'j'
                                 return fabs(box_.minimumDistance(b.i()->r(), b.j()->r()) -
                                             (b.j()->r() - b.i()->r()).magnitude()) > 1.0e-3;
                             });
    if (it != bonds_.end())
        bonds_.erase(it, bonds_.end());
}

// Return vector of SpeciesAngle
std::vector<SpeciesAngle> &SpeciesBaseNode::angles() { return angles_; }

const std::vector<SpeciesAngle> &SpeciesBaseNode::angles() const { return angles_; }

// Return the SpeciesAngle between the specified SpeciesAtoms, if it exists
OptionalReferenceWrapper<const SpeciesAngle> SpeciesBaseNode::getAngle(const SpeciesAtom *i, const SpeciesAtom *j,
                                                                       const SpeciesAtom *k) const
{
    auto it = std::find_if(angles_.begin(), angles_.end(), [i, j, k](auto &angle) { return angle.matches(i, j, k); });
    if (it == angles_.end())
        return {};

    return *it;
}
OptionalReferenceWrapper<const SpeciesAngle> SpeciesBaseNode::getAngle(int i, int j, int k) const
{
    return getAngle(&atoms_.at(i), &atoms_.at(j), &atoms_.at(k));
}

// Return vector of SpeciesTorsions
std::vector<SpeciesTorsion> &SpeciesBaseNode::torsions() { return torsions_; }

const std::vector<SpeciesTorsion> &SpeciesBaseNode::torsions() const { return torsions_; }

// Return the SpeciesTorsion between the specified SpeciesAtoms, if it exists
OptionalReferenceWrapper<const SpeciesTorsion> SpeciesBaseNode::getTorsion(const SpeciesAtom *i, const SpeciesAtom *j,
                                                                           const SpeciesAtom *k, const SpeciesAtom *l) const
{
    auto it =
        std::find_if(torsions_.begin(), torsions_.end(), [i, j, k, l](auto &torsion) { return torsion.matches(i, j, k, l); });
    if (it == torsions_.end())
        return {};

    return *it;
}
OptionalReferenceWrapper<const SpeciesTorsion> SpeciesBaseNode::getTorsion(int i, int j, int k, int l) const
{
    return getTorsion(&atoms_.at(i), &atoms_.at(j), &atoms_.at(k), &atoms_.at(l));
}

// Return vector of SpeciesImproper
std::vector<SpeciesImproper> &SpeciesBaseNode::impropers() { return impropers_; }

const std::vector<SpeciesImproper> &SpeciesBaseNode::impropers() const { return impropers_; }

// Add a new improper term between the specified atoms
SpeciesImproper &SpeciesBaseNode::addImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
{
    auto &improper = impropers_.emplace_back(this, i, j, k, l);

    // Must inform the atoms now that they are involved in a new improper
    improper.i()->addImproper(&improper);
    improper.j()->addImproper(&improper);
    improper.k()->addImproper(&improper);
    improper.l()->addImproper(&improper);

    return improper;
}

// Return the SpeciesImproper between the specified SpeciesAtoms, if it exists
OptionalReferenceWrapper<const SpeciesImproper> SpeciesBaseNode::getImproper(const SpeciesAtom *i, const SpeciesAtom *j,
                                                                             const SpeciesAtom *k, const SpeciesAtom *l) const
{
    auto it = std::find_if(impropers_.begin(), impropers_.end(),
                           [i, j, k, l](auto &improper) { return improper.matches(i, j, k, l); });
    if (it == impropers_.end())
        return {};

    return *it;
}
OptionalReferenceWrapper<const SpeciesImproper> SpeciesBaseNode::getImproper(int i, int j, int k, int l) const
{
    return getImproper(&atoms_.at(i), &atoms_.at(j), &atoms_.at(k), &atoms_.at(l));
}

// Clear intramolecular forcefield terms
void SpeciesBaseNode::clearIntramolecularForcefieldTerms()
{
    for (auto &b : bonds_)
        b.setInteractionFormAndParameters(BondFunctions::Form::None, std::vector<double>());

    for (auto &a : angles_)
        a.setInteractionFormAndParameters(AngleFunctions::Form::None, std::vector<double>());

    for (auto &t : torsions_)
        t.setInteractionFormAndParameters(TorsionFunctions::Form::None, std::vector<double>());

    impropers_.clear();
}

// Return periodic box
Box &SpeciesBaseNode::box() { return box_; }
const Box &SpeciesBaseNode::box() const { return box_; }

// Remove Box definition and revert to nonperiodic
void SpeciesBaseNode::removeBox() { box_ = Box(); }
