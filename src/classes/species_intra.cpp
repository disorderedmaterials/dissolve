// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/coreData.h"
#include "classes/fragment.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <algorithm>

// Return vector of SpeciesBond
std::vector<SpeciesBond> &Species::bonds() { return bonds_; }

const std::vector<SpeciesBond> &Species::bonds() const { return bonds_; }

// Return whether SpeciesBond between specified SpeciesAtoms exists
bool Species::hasBond(const SpeciesAtom *i, const SpeciesAtom *j) const
{
    return std::any_of(bonds_.cbegin(), bonds_.cend(), [i, j](const auto &bond) { return bond.matches(i, j); });
}
bool Species::hasBond(int i, int j) const { return hasBond(&atom(i), &atom(j)); }

// Return the SpeciesBond between the specified SpeciesAtom indices
OptionalReferenceWrapper<SpeciesBond> Species::getBond(int indexI, int indexJ)
{
    auto *i = &atoms_[indexI];
    auto *j = &atoms_[indexJ];

    auto it = std::find_if(bonds_.begin(), bonds_.end(), [i, j](auto &bond) { return bond.matches(i, j); });
    if (it == bonds_.end())
        return {};

    return *it;
}

// Remove bonds crossing periodic boundaries
void Species::removePeriodicBonds()
{
    if (box_->type() == Box::BoxType::NonPeriodic)
        return;

    auto it = std::remove_if(bonds_.begin(), bonds_.end(),
                             [&](const auto &b)
                             {
                                 // Check the literal vs the minimum image distance between the involved atoms 'i' and 'j'
                                 return fabs(box_->minimumDistance(b.i()->r(), b.j()->r()) -
                                             (b.j()->r() - b.i()->r()).magnitude()) > 1.0e-3;
                             });
    if (it != bonds_.end())
        bonds_.erase(it, bonds_.end());
}

// Return vector of SpeciesAngle
std::vector<SpeciesAngle> &Species::angles() { return angles_; }

const std::vector<SpeciesAngle> &Species::angles() const { return angles_; }

// Return whether SpeciesAngle between SpeciesAtoms exists
bool Species::hasAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k) const
{
    return std::any_of(angles_.cbegin(), angles_.cend(), [i, j, k](const auto &angle) { return angle.matches(i, j, k); });
}

// Return the SpeciesAngle between the specified SpeciesAtom indic
OptionalReferenceWrapper<SpeciesAngle> Species::getAngle(int indexI, int indexJ, int indexK)
{
    auto *i = &atoms_[indexI];
    auto *j = &atoms_[indexJ];
    auto *k = &atoms_[indexK];

    auto it = std::find_if(angles_.begin(), angles_.end(), [i, j, k](auto &angle) { return angle.matches(i, j, k); });
    if (it == angles_.end())
        return {};

    return *it;
}

// Return vector of SpeciesTorsions
std::vector<SpeciesTorsion> &Species::torsions() { return torsions_; }

const std::vector<SpeciesTorsion> &Species::torsions() const { return torsions_; }

// Return whether SpeciesTorsion between SpeciesAtoms exists
bool Species::hasTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) const
{
    return std::any_of(torsions_.cbegin(), torsions_.cend(), [&](const auto &t) { return t.matches(i, j, k, l); });
}

// Return the SpeciesTorsion between the specified SpeciesAtom indices
OptionalReferenceWrapper<SpeciesTorsion> Species::getTorsion(int indexI, int indexJ, int indexK, int indexL)
{
    auto *i = &atoms_[indexI];
    auto *j = &atoms_[indexJ];
    auto *k = &atoms_[indexK];
    auto *l = &atoms_[indexL];

    auto it =
        std::find_if(torsions_.begin(), torsions_.end(), [i, j, k, l](auto &torsion) { return torsion.matches(i, j, k, l); });
    if (it == torsions_.end())
        return {};

    return *it;
}

// Return vector of SpeciesImproper
std::vector<SpeciesImproper> &Species::impropers() { return impropers_; }

const std::vector<SpeciesImproper> &Species::impropers() const { return impropers_; }

// Return whether SpeciesImproper between SpeciesAtoms exists
bool Species::hasImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) const
{
    return std::any_of(impropers_.cbegin(), impropers_.cend(),
                       [i, j, k, l](auto &improper) { return improper.matches(i, j, k, l); });
}

// Return the SpeciesImproper between the specified SpeciesAtom indices
OptionalReferenceWrapper<SpeciesImproper> Species::getImproper(int indexI, int indexJ, int indexK, int indexL)
{
    auto *i = &atoms_[indexI];
    auto *j = &atoms_[indexJ];
    auto *k = &atoms_[indexK];
    auto *l = &atoms_[indexL];

    auto it = std::find_if(impropers_.begin(), impropers_.end(),
                           [i, j, k, l](auto &improper) { return improper.matches(i, j, k, l); });
    if (it == impropers_.end())
        return {};

    return *it;
}

// Return whether the attached atoms lists have been created
bool Species::attachedAtomListsGenerated() const { return attachedAtomListsGenerated_; }

// Determine angles and torsions from bond connectivity
void Species::determineAnglesAndTorsions()
{
    for (auto &jk : bonds_)
    {
        // Get atoms 'j' and 'k'
        auto *j = jk.i();
        auto *k = jk.j();

        // Swap j and k over if j is terminal and has only a single bond (i.e. jk)
        if (j->bonds().size() == 1)
            std::swap(j, k);

        // Loop over bonds 'ij'
        for (auto *ij : j->bonds())
        {
            // Avoid 'ij' == 'jk'
            if (ij == &jk)
                continue;

            // Get atom 'i'
            auto *i = ij->partner(j);

            // ADd angle i-j-k
            angles_.emplace_back(this, i, j, k);

            // Loop over bonds 'kl'
            for (auto kl : k->bonds())
            {
                // Avoid 'kl' == 'jk'
                if (kl == &jk)
                    continue;

                // Get atom 'l'
                auto *l = kl->partner(k);

                // Add angle j-k-l
                angles_.emplace_back(this, j, k, l);

                // Add torsion i-j-k-l
                torsions_.emplace_back(this, i, j, k, l);
            }
        }
    }
}
// Finalise internal relationships related to geometry once it is defined
void Species::finaliseGeometry()
{
    // From the lists of intramolecular terms, let our atoms know that they are present in them
    for (const auto &bond : bonds_)
    {
        bond.i()->addBond(&bond);
        bond.j()->addBond(&bond);
    }
    for (const auto &angle : angles_)
    {
        angle.i()->addAngle(&angle);
        angle.j()->addAngle(&angle);
        angle.k()->addAngle(&angle);
    }
    for (const auto &torsion : torsions_)
    {
        torsion.i()->addTorsion(&torsion);
        torsion.j()->addTorsion(&torsion);
        torsion.k()->addTorsion(&torsion);
        torsion.l()->addTorsion(&torsion);
    }
    for (const auto &improper : impropers_)
    {
        improper.i()->addImproper(&improper);
        improper.j()->addImproper(&improper);
        improper.k()->addImproper(&improper);
        improper.l()->addImproper(&improper);
    }

    // Set-up excluded / scaled interactions on atoms arising from bonds, angles, and torsions
    auto index = 0;
    for (auto &i : atoms_)
    {
        i.setScaledInteractions();
        i.setIndex(index++);
    }

    // If this is a periodic species, we're done
    if (box_->type() != Box::BoxType::NonPeriodic)
    {
        attachedAtomListsGenerated_ = false;
        return;
    }

    // Generate attached atom lists for all intramolecular terms
    // Bonds
    for (auto &bond : bonds_)
    {
        // Select all Atoms attached to Atom 'i', excluding the Bond as a path
        auto selection = Fragment<SpeciesAtom, SpeciesBond>::get(atoms_, bond.i()->index(), {&bond});

        // If the list now contains Atom j, the two atoms are present in a cycle of some sort, and we can only add the
        // Atom 'i' itself In that case we can also finish the list for Atom 'j', and continue the loop.
        if (std::find(selection.begin(), selection.end(), bond.j()->index()) != selection.end())
        {
            Messenger::printVerbose("Bond between Atoms {}-{} is present in a cycle, so a minimal set of attached "
                                    "atoms will be used.\n",
                                    bond.i()->index(), bond.j()->index());
            bond.setAttachedAtoms(0, bond.i()->index());
            bond.setAttachedAtoms(1, bond.j()->index());
            bond.setInCycle(true);
            continue;
        }
        else
            bond.setAttachedAtoms(0, selection);

        // Select all Atoms attached to Atom 'i', excluding the Bond as a path
        selection = Fragment<SpeciesAtom, SpeciesBond>::get(atoms_, bond.j()->index(), {&bond});
        bond.setAttachedAtoms(1, selection);
    }

    // Angles - termini are 'i' and 'k'
    for (auto &angle : angles_)
    {
        // Grab relevant bonds
        auto ji = angle.j()->getBondWith(angle.i());
        auto jk = angle.j()->getBondWith(angle.k());

        // Select all Atoms attached to Atom 'i', excluding the Bond ji as a path
        auto selection = Fragment<SpeciesAtom, SpeciesBond>::get(atoms_, angle.i()->index(), {ji, jk});

        // Remove Atom 'j' from the list if it's there
        auto jit = std::find(selection.begin(), selection.end(), angle.j()->index());
        if (jit != selection.end())
            selection.erase(jit);

        // If the list now contains Atom k, the two atoms are present in a cycle of some sort, and we can only add the
        // Atom 'i' itself In that case we can also finish the list for Atom 'k', and continue the loop.
        if (std::find(selection.begin(), selection.end(), angle.k()->index()) != selection.end())
        {
            Messenger::printVerbose("Angle between Atoms {}-{}-{} is present in a cycle, so a minimal set of "
                                    "attached atoms will be used.\n",
                                    angle.i()->index(), angle.j()->index(), angle.k()->index());
            angle.setAttachedAtoms(0, angle.i()->index());
            angle.setAttachedAtoms(1, angle.k()->index());
            angle.setInCycle(true);
            continue;
        }
        else
            angle.setAttachedAtoms(0, selection);

        // Select all Atoms attached to Atom 'k', excluding the Bond jk as a path
        selection = Fragment<SpeciesAtom, SpeciesBond>::get(atoms_, angle.k()->index(), {ji, jk});

        // Remove Atom 'j' from the list if it's there
        jit = std::find(selection.begin(), selection.end(), angle.j()->index());
        if (jit != selection.end())
            selection.erase(jit);

        angle.setAttachedAtoms(1, selection);
    }

    // Torsions - termini are 'j' and 'k'
    for (auto &torsion : torsions_)
    {
        // Grab relevant Bond (if it exists)
        auto jk = torsion.j()->getBondWith(torsion.k());

        // Select all Atoms attached to Atom 'j', excluding the Bond ji as a path
        auto selection = Fragment<SpeciesAtom, SpeciesBond>::get(atoms_, torsion.j()->index(), {jk});

        // Remove Atom 'j' from the list
        selection.erase(std::remove(selection.begin(), selection.end(), torsion.j()->index()));

        // If the list now contains Atom k, the two atoms are present in a cycle of some sort, and we can only add the
        // Atom 'i'
        if (std::find(selection.begin(), selection.end(), torsion.k()->index()) != selection.end())
        {
            Messenger::printVerbose("Torsion between Atoms {}-{}-{}-{} is present in a cycle, so a minimal set of "
                                    "attached atoms will be used.\n",
                                    torsion.i()->index(), torsion.j()->index(), torsion.k()->index(), torsion.l()->index());
            torsion.setAttachedAtoms(0, torsion.i()->index());
            torsion.setAttachedAtoms(1, torsion.l()->index());
            torsion.setInCycle(true);
            continue;
        }
        else
            torsion.setAttachedAtoms(0, selection);

        // Select all Atoms attached to Atom 'k', excluding the Bond jk as a path
        selection = Fragment<SpeciesAtom, SpeciesBond>::get(atoms_, torsion.k()->index(), {jk});

        // Remove Atom 'k' from the list
        selection.erase(std::remove(selection.begin(), selection.end(), torsion.k()->index()));

        torsion.setAttachedAtoms(1, selection);
    }

    attachedAtomListsGenerated_ = true;
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
const Box *Species::box() const { return box_.get(); }

// Remove Box definition and revert to single image
void Species::removeBox() { box_ = std::make_unique<SingleImageBox>(); }

// Create Box definition with specified lengths and angles
void Species::createBox(const Vector3 lengths, const Vector3 angles, bool nonPeriodic)
{
    box_ = nonPeriodic ? std::make_unique<NonPeriodicBox>() : Box::generate(lengths, angles);
}
