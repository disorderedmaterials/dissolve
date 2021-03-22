// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/species.h"
#include "data/atomicradii.h"
#include <algorithm>

/*
 * Private
 */

// Add missing higher order intramolecular terms from current bond connectivity, and prune any that are now invalid
void Species::updateIntramolecularTerms()
{
    SpeciesAtom *i, *j, *k, *l;

    // Loop over bonds 'jk'
    for (auto &jk : bonds_)
    {
        // Get atoms 'j' and 'k'
        j = jk.i();
        k = jk.j();

        // Loop over bonds 'ij'
        for (SpeciesBond &ij : j->bonds())
        {
            // Avoid 'ij' == 'jk'
            if (&ij == &jk)
                continue;

            // Get atom 'i'
            i = ij.partner(j);

            // Attempt to add angle term 'ijk' if 'i' > 'k'
            if (!hasAngle(i, j, k))
                addAngle(i, j, k);

            // Loop over bonds 'kl'
            for (SpeciesBond &kl : k->bonds())
            {
                // Avoid 'kl' == 'jk'
                if (&kl == &jk)
                    continue;

                // Get atom 'l'
                l = kl.partner(k);

                // Attempt to add angle term 'jkl'
                if (!hasAngle(j, k, l))
                    addAngle(j, k, l);

                // If the torsion i-j-k-l doesn't already exist, add it now.
                if (!hasTorsion(i, j, k, l))
                    addTorsion(i, j, k, l);
            }
        }
    }
    auto atomsContains = [this](const auto *x) {
        return std::find_if(atoms_.begin(), atoms_.end(), [&x](const auto &p) { return &p == x; }) != atoms_.end();
    };

    // Check existing angle terms for any that are invalid
    angles_.erase(std::remove_if(angles_.begin(), angles_.end(),
                                 [this, &atomsContains](const auto &angle) {
                                     return ((!atomsContains(angle.i())) || (!atomsContains(angle.j())) ||
                                             (!atomsContains(angle.k()))) ||
                                            ((!hasBond(angle.i(), angle.j())) || (!hasBond(angle.j(), angle.k())));
                                 }),
                  angles_.end());

    // remove torsions with invalid atoms or bonds
    torsions_.erase(std::remove_if(torsions_.begin(), torsions_.end(),
                                   [this, &atomsContains](const auto &torsion) {
                                       return ((!atomsContains(torsion.i())) || (!atomsContains(torsion.j())) ||
                                               (!atomsContains(torsion.k())) || (!atomsContains(torsion.l()))) ||
                                              ((!hasBond(torsion.i(), torsion.j())) || (!hasBond(torsion.j(), torsion.k())) ||
                                               (!hasBond(torsion.k(), torsion.l())));
                                   }),
                    torsions_.end());
}

/*
 * Public
 */

// Add new SpeciesBond definition (from SpeciesAtoms*)
SpeciesBond &Species::addBond(SpeciesAtom *i, SpeciesAtom *j)
{
    // Check for existence of Bond already
    auto bondRef = getBond(i, j);
    if (bondRef)
    {
        Messenger::warn("Refused to add a new SpeciesBond between atoms {} and {} in Species '{}' since it already exists.\n",
                        i->userIndex(), j->userIndex(), name_);
        return *bondRef;
    }

    // OK to add new Bond
    bonds_.emplace_back(i, j);

    // Update higher-order connectivity?
    if (autoUpdateIntramolecularTerms_)
        updateIntramolecularTerms();

    ++version_;

    return bonds_.back();
}

// Add new SpeciesBond definition
SpeciesBond &Species::addBond(int i, int j) { return addBond(&atom(i), &atom(j)); }

// Remove bond between specified SpeciesAtoms
void Species::removeBond(SpeciesAtom *j, SpeciesAtom *k)
{
    // Find the bond
    auto it = std::remove_if(bonds_.begin(), bonds_.end(), [j, k](const auto &bond) { return bond.matches(j, k); });
    if (it == bonds_.end())
        return;

    // Clear higher-order terms
    angles_.clear();
    torsions_.clear();
    impropers_.clear();

    // Detach the bond from its atoms
    it->detach();

    // Erase the bond
    bonds_.erase(it);

    ++version_;
}

// Return number of SpeciesBonds defined
int Species::nBonds() const { return bonds_.size(); }

// Return array of SpeciesBond
std::vector<SpeciesBond> &Species::bonds() { return bonds_; }

const std::vector<SpeciesBond> &Species::bonds() const { return bonds_; }

// Return whether SpeciesBond between specified SpeciesAtoms exists
bool Species::hasBond(SpeciesAtom *i, SpeciesAtom *j) const
{
    auto it = std::find_if(bonds_.cbegin(), bonds_.cend(), [i, j](const auto &bond) { return bond.matches(i, j); });

    return it != bonds_.cend();
}

// Return the SpeciesBond between the specified SpeciesAtoms
OptionalReferenceWrapper<SpeciesBond> Species::getBond(SpeciesAtom *i, SpeciesAtom *j)
{
    auto it = std::find_if(bonds_.begin(), bonds_.end(), [i, j](auto &bond) { return bond.matches(i, j); });
    if (it == bonds_.end())
        return {};

    return *it;
}

OptionalReferenceWrapper<const SpeciesBond> Species::getBond(SpeciesAtom *i, SpeciesAtom *j) const
{
    auto it = std::find_if(bonds_.cbegin(), bonds_.cend(), [i, j](const auto &bond) { return bond.matches(i, j); });
    if (it == bonds_.end())
        return {};

    return *it;
}

// Return the SpeciesBond between the specified SpeciesAtom indices
OptionalReferenceWrapper<SpeciesBond> Species::getBond(int i, int j)
{
    assert(i >= 0 || i < nAtoms());
    assert(j >= 0 || j < nAtoms());

    return getBond(&atom(i), &atom(j));
}

// Add missing bonds
void Species::addMissingBonds(double tolerance)
{
    Vec3<double> vij;
    double radiusI;
    for (auto indexI = 0; indexI < nAtoms() - 1; ++indexI)
    {
        // Get SpeciesAtom 'i' and its radius
        auto &i = atom(indexI);
        radiusI = AtomicRadii::radius(i.Z());
        for (auto indexJ = indexI + 1; indexJ < nAtoms(); ++indexJ)
        {
            // Get SpeciesAtom 'j'
            auto &j = atom(indexJ);

            // If the two atoms are already bound, continue
            if (i.hasBond(&j))
                continue;

            // Calculate distance between atoms
            vij = j.r() - i.r();

            // Compare distance to sum of atomic radii (multiplied by tolerance factor)
            if (vij.magnitude() <= (radiusI + AtomicRadii::radius(j.Z())) * tolerance)
                addBond(&i, &j);
        }
    }

    // May now require new higher-order terms
    if (autoUpdateIntramolecularTerms_)
        updateIntramolecularTerms();
}

// Add new SpeciesAngle definition (from supplied SpeciesAtom pointers)
SpeciesAngle &Species::addAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k)
{
    // Check for existence of Angle already
    auto angleRef = getAngle(i, j, k);
    if (angleRef)
    {
        Messenger::warn("Refused to add a new Angle between atoms {}, {} and {} in Species '{}' since it already exists.\n",
                        i->userIndex(), j->userIndex(), k->userIndex(), name_);
        return *angleRef;
    }

    // OK to add new angle
    angles_.emplace_back(i, j, k);

    ++version_;

    return angles_.back();
}

// Add new SpeciesAngle definition
SpeciesAngle &Species::addAngle(int i, int j, int k) { return addAngle(&atom(i), &atom(j), &atom(k)); }

// Return number of SpeciesAngles defined
int Species::nAngles() const { return angles_.size(); }

// Return array of SpeciesAngle
std::vector<SpeciesAngle> &Species::angles() { return angles_; }

const std::vector<SpeciesAngle> &Species::angles() const { return angles_; }

// Return whether SpeciesAngle between SpeciesAtoms exists
bool Species::hasAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k) const
{
    auto it = std::find_if(angles_.cbegin(), angles_.cend(), [i, j, k](const auto &angle) { return angle.matches(i, j, k); });

    return it != angles_.cend();
}

// Return the SpeciesAngle between the specified SpeciesAtoms
OptionalReferenceWrapper<SpeciesAngle> Species::getAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k)
{
    auto it = std::find_if(angles_.begin(), angles_.end(), [i, j, k](auto &angle) { return angle.matches(i, j, k); });
    if (it == angles_.end())
        return {};

    return *it;
}

// Return the SpeciesAngle between the specified SpeciesAtom indic
OptionalReferenceWrapper<SpeciesAngle> Species::getAngle(int i, int j, int k) { return getAngle(&atom(i), &atom(j), &atom(k)); }

// Add new SpeciesTorsion definition (from supplied SpeciesAtom pointers)
SpeciesTorsion &Species::addTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
{
    // Check for existence of Torsion already
    if (hasTorsion(i, j, k, l))
    {
        Messenger::warn("Refused to add a new Torsion between atoms {}, {}, {} and {} in Species '{}' since it already "
                        "exists.\n",
                        i->userIndex(), j->userIndex(), k->userIndex(), l->userIndex(), name_);
        return *getTorsion(i, j, k, l);
    }

    // OK to add new torsion
    torsions_.emplace_back(i, j, k, l);

    ++version_;

    return torsions_.back();
}

// Add new SpeciesTorsion definition
SpeciesTorsion &Species::addTorsion(int i, int j, int k, int l) { return addTorsion(&atom(i), &atom(j), &atom(k), &atom(l)); }

// Return number of SpeciesTorsions defined
int Species::nTorsions() const { return torsions_.size(); }

// Return array of SpeciesTorsions
std::vector<SpeciesTorsion> &Species::torsions() { return torsions_; }

const std::vector<SpeciesTorsion> &Species::torsions() const { return torsions_; }

// Return whether SpeciesTorsion between SpeciesAtoms exists
bool Species::hasTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) const
{
    auto it = std::find_if(torsions_.cbegin(), torsions_.cend(), [&](const auto &t) { return t.matches(i, j, k, l); });
    return it != torsions_.end();
}

// Return the SpeciesTorsion between the specified SpeciesAtoms
OptionalReferenceWrapper<SpeciesTorsion> Species::getTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
{
    auto it =
        std::find_if(torsions_.begin(), torsions_.end(), [i, j, k, l](auto &torsion) { return torsion.matches(i, j, k, l); });
    if (it == torsions_.end())
        return {};

    return *it;
}

// Return the SpeciesTorsion between the specified SpeciesAtom indices
OptionalReferenceWrapper<SpeciesTorsion> Species::getTorsion(int i, int j, int k, int l)
{
    return getTorsion(&atom(i), &atom(j), &atom(k), &atom(l));
}

// Add new SpeciesImproper definition (from SpeciesAtom*)
SpeciesImproper &Species::addImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
{
    // Check for existence of Improper already
    if (hasImproper(i, j, k, l))
    {
        Messenger::warn("Refused to add a new Improper between atoms {}, {}, {} and {} in Species '{}' since it "
                        "already exists.\n",
                        i->userIndex(), j->userIndex(), k->userIndex(), l->userIndex(), name_);
        return *getImproper(i, j, k, l);
    }

    // OK to add new improper
    impropers_.emplace_back(i, j, k, l);

    ++version_;

    return impropers_.back();
}

// Add new SpeciesImproper definition
SpeciesImproper &Species::addImproper(int i, int j, int k, int l)
{
    return addImproper(&atom(i), &atom(j), &atom(k), &atom(l));
}

// Return number of SpeciesImproper defined
int Species::nImpropers() const { return impropers_.size(); }

// Return array of SpeciesImproper
std::vector<SpeciesImproper> &Species::impropers() { return impropers_; }

const std::vector<SpeciesImproper> &Species::impropers() const { return impropers_; }

// Return whether SpeciesImproper between SpeciesAtoms exists
bool Species::hasImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) const
{
    auto it = std::find_if(impropers_.begin(), impropers_.end(),
                           [i, j, k, l](auto &improper) { return improper.matches(i, j, k, l); });
    if (it == impropers_.end())
        return false;

    return true;
}

// Return the SpeciesImproper between the specified SpeciesAtoms (if it exists)
OptionalReferenceWrapper<SpeciesImproper> Species::getImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
{
    auto it = std::find_if(impropers_.begin(), impropers_.end(),
                           [i, j, k, l](auto &improper) { return improper.matches(i, j, k, l); });
    if (it == impropers_.end())
        return {};

    return *it;
}

// Return the SpeciesImproper between the specified SpeciesAtom indices
OptionalReferenceWrapper<SpeciesImproper> Species::getImproper(int i, int j, int k, int l)
{
    return getImproper(&atom(i), &atom(j), &atom(k), &atom(l));
}

// Return whether the attached atoms lists have been created
bool Species::attachedAtomListsGenerated() const { return attachedAtomListsGenerated_; }

// Generate attached SpeciesAtom lists for all intramolecular terms
void Species::generateAttachedAtomLists()
{
    // Bonds
    for (auto &bond : bonds_)
    {
        // Select all Atoms attached to Atom 'i', excluding the Bond as a path
        clearAtomSelection();
        selectFromAtom(bond.i(), bond);

        // If the list now contains Atom j, the two atoms are present in a cycle of some sort, and we can only add the
        // Atom 'i' itself In that case we can also finish the list for Atom 'j', and continue the loop.
        if (selectedAtoms_.contains(bond.j()))
        {
            Messenger::printVerbose("Bond between Atoms {}-{} is present in a cycle, so a minimal set of attached "
                                    "atoms will be used.\n",
                                    bond.i()->userIndex(), bond.j()->userIndex());
            bond.setAttachedAtoms(0, bond.i());
            bond.setAttachedAtoms(1, bond.j());
            bond.setInCycle(true);
            continue;
        }
        else
            bond.setAttachedAtoms(0, selectedAtoms_);

        // Select all Atoms attached to Atom 'i', excluding the Bond as a path
        clearAtomSelection();
        selectFromAtom(bond.j(), bond);
        bond.setAttachedAtoms(1, selectedAtoms_);
    }

    // Angles - termini are 'i' and 'k'
    for (auto &angle : angles_)
    {
        // Grab relevant Bonds (if they exist)
        auto ji = angle.j()->hasBond(angle.i());
        auto jk = angle.j()->hasBond(angle.k());

        // Select all Atoms attached to Atom 'i', excluding the Bond ji as a path
        clearAtomSelection();
        selectFromAtom(angle.i(), *ji, *jk);

        // Remove Atom 'j' from the list if it's there
        selectedAtoms_.remove(angle.j());

        // If the list now contains Atom k, the two atoms are present in a cycle of some sort, and we can only add the
        // Atom 'i' itself In that case we can also finish the list for Atom 'k', and continue the loop.
        if (selectedAtoms_.contains(angle.k()))
        {
            Messenger::printVerbose("Angle between Atoms {}-{}-{} is present in a cycle, so a minimal set of "
                                    "attached atoms will be used.\n",
                                    angle.i()->userIndex(), angle.j()->userIndex(), angle.k()->userIndex());
            angle.setAttachedAtoms(0, angle.i());
            angle.setAttachedAtoms(1, angle.k());
            angle.setInCycle(true);
            continue;
        }
        else
            angle.setAttachedAtoms(0, selectedAtoms_);

        // Select all Atoms attached to Atom 'k', excluding the Bond jk as a path
        clearAtomSelection();
        selectFromAtom(angle.k(), *ji, jk);

        // Remove Atom 'j' from the list if it's there
        selectedAtoms_.remove(angle.j());

        angle.setAttachedAtoms(1, selectedAtoms_);
    }

    // Torsions - termini are 'j' and 'k'
    for (auto &torsion : torsions_)
    {
        // Grab relevant Bond (if it exists)
        auto jk = torsion.j()->hasBond(torsion.k());

        // Select all Atoms attached to Atom 'j', excluding the Bond ji as a path
        clearAtomSelection();
        selectFromAtom(torsion.j(), *jk);

        // Remove Atom 'j' from the list
        selectedAtoms_.remove(torsion.j());

        // If the list now contains Atom k, the two atoms are present in a cycle of some sort, and we can only add the
        // Atom 'i'
        if (selectedAtoms_.contains(torsion.k()))
        {
            Messenger::printVerbose("Torsion between Atoms {}-{}-{}-{} is present in a cycle, so a minimal set of "
                                    "attached atoms will be used.\n",
                                    torsion.i()->userIndex(), torsion.j()->userIndex(), torsion.k()->userIndex(),
                                    torsion.l()->userIndex());
            torsion.setAttachedAtoms(0, torsion.i());
            torsion.setAttachedAtoms(1, torsion.l());
            torsion.setInCycle(true);
            continue;
        }
        else
            torsion.setAttachedAtoms(0, selectedAtoms_);

        // Select all Atoms attached to Atom 'k', excluding the Bond jk as a path
        clearAtomSelection();
        selectFromAtom(torsion.k(), *jk);

        // Remove Atom 'k' from the list
        selectedAtoms_.remove(torsion.k());

        torsion.setAttachedAtoms(1, selectedAtoms_);
    }

    attachedAtomListsGenerated_ = true;
}

// Detach master term links for all interaction types, copying parameters to local SpeciesIntra
void Species::detachFromMasterTerms()
{
    for (auto &bond : bonds_)
        bond.detachFromMasterIntra();

    for (auto &angle : angles_)
        angle.detachFromMasterIntra();

    for (auto &torsion : torsions_)
        torsion.detachFromMasterIntra();

    for (auto &improper : impropers_)
        improper.detachFromMasterIntra();
}
