// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <algorithm>

// Return vector of SpeciesBond
std::vector<SpeciesBond> &Species::bonds() { return bonds_; }

const std::vector<SpeciesBond> &Species::bonds() const { return bonds_; }

// Return the SpeciesBond between the specified SpeciesAtoms, if it exists
OptionalReferenceWrapper<const SpeciesBond> Species::getBond(const SpeciesAtom *i, const SpeciesAtom *j) const
{
    auto it = std::find_if(bonds_.begin(), bonds_.end(), [i, j](auto &bond) { return bond.matches(i, j); });
    if (it == bonds_.end())
        return {};

    return *it;
}
OptionalReferenceWrapper<const SpeciesBond> Species::getBond(int i, int j) const
{
    return getBond(&atoms_.at(i), &atoms_.at(j));
}

// Remove bonds crossing periodic boundaries
void Species::removePeriodicBonds()
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
std::vector<SpeciesAngle> &Species::angles() { return angles_; }

const std::vector<SpeciesAngle> &Species::angles() const { return angles_; }

// Return the SpeciesAngle between the specified SpeciesAtoms, if it exists
OptionalReferenceWrapper<const SpeciesAngle> Species::getAngle(const SpeciesAtom *i, const SpeciesAtom *j,
                                                               const SpeciesAtom *k) const
{
    auto it = std::find_if(angles_.begin(), angles_.end(), [i, j, k](auto &angle) { return angle.matches(i, j, k); });
    if (it == angles_.end())
        return {};

    return *it;
}
OptionalReferenceWrapper<const SpeciesAngle> Species::getAngle(int i, int j, int k) const
{
    return getAngle(&atoms_.at(i), &atoms_.at(j), &atoms_.at(k));
}

// Return vector of SpeciesTorsions
std::vector<SpeciesTorsion> &Species::torsions() { return torsions_; }

const std::vector<SpeciesTorsion> &Species::torsions() const { return torsions_; }

// Return the SpeciesTorsion between the specified SpeciesAtoms, if it exists
OptionalReferenceWrapper<const SpeciesTorsion> Species::getTorsion(const SpeciesAtom *i, const SpeciesAtom *j,
                                                                   const SpeciesAtom *k, const SpeciesAtom *l) const
{
    auto it =
        std::find_if(torsions_.begin(), torsions_.end(), [i, j, k, l](auto &torsion) { return torsion.matches(i, j, k, l); });
    if (it == torsions_.end())
        return {};

    return *it;
}
OptionalReferenceWrapper<const SpeciesTorsion> Species::getTorsion(int i, int j, int k, int l) const
{
    return getTorsion(&atoms_.at(i), &atoms_.at(j), &atoms_.at(k), &atoms_.at(l));
}

// Return vector of SpeciesImproper
std::vector<SpeciesImproper> &Species::impropers() { return impropers_; }

const std::vector<SpeciesImproper> &Species::impropers() const { return impropers_; }

// Add a new improper term between the specified atoms
SpeciesImproper &Species::addImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
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
OptionalReferenceWrapper<const SpeciesImproper> Species::getImproper(const SpeciesAtom *i, const SpeciesAtom *j,
                                                                     const SpeciesAtom *k, const SpeciesAtom *l) const
{
    auto it = std::find_if(impropers_.begin(), impropers_.end(),
                           [i, j, k, l](auto &improper) { return improper.matches(i, j, k, l); });
    if (it == impropers_.end())
        return {};

    return *it;
}
OptionalReferenceWrapper<const SpeciesImproper> Species::getImproper(int i, int j, int k, int l) const
{
    return getImproper(&atoms_.at(i), &atoms_.at(j), &atoms_.at(k), &atoms_.at(l));
}

// Clear intramolecular forcefield terms
void Species::clearIntramolecularForcefieldTerms()
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
const Box &Species::box() const { return box_; }

// Remove Box definition and revert to single image
void Species::removeBox() { box_ = Box::none(); }

// Create Box definition with specified lengths and angles
void Species::createBox(const Vector3 lengths, const Vector3 angles, bool nonPeriodic)
{
    box_ = Box::generate(lengths, angles, nonPeriodic);
}
