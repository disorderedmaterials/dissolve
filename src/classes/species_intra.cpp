// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "data/atomicRadii.h"
#include "templates/algorithms.h"
#include <algorithm>

/*
 * Public
 */

// Add new SpeciesBond definition
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

    ++version_;

    return bonds_.back();
}
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
void Species::removeBond(int i, int j) { removeBond(&atoms_[i], &atoms_[j]); }

// Return number of SpeciesBonds defined
int Species::nBonds() const { return bonds_.size(); }

// Return vector of SpeciesBond
std::vector<SpeciesBond> &Species::bonds() { return bonds_; }

const std::vector<SpeciesBond> &Species::bonds() const { return bonds_; }

// Return whether SpeciesBond between specified SpeciesAtoms exists
bool Species::hasBond(const SpeciesAtom *i, const SpeciesAtom *j) const
{
    return std::any_of(bonds_.cbegin(), bonds_.cend(), [i, j](const auto &bond) { return bond.matches(i, j); });
}
bool Species::hasBond(int i, int j) const { return hasBond(&atom(i), &atom(j)); }

// Return the SpeciesBond between the specified SpeciesAtoms
OptionalReferenceWrapper<SpeciesBond> Species::getBond(SpeciesAtom *i, SpeciesAtom *j)
{
    auto it = std::find_if(bonds_.begin(), bonds_.end(), [i, j](auto &bond) { return bond.matches(i, j); });
    if (it == bonds_.end())
        return {};

    return *it;
}
OptionalReferenceWrapper<const SpeciesBond> Species::getBond(const SpeciesAtom *i, const SpeciesAtom *j) const
{
    auto it = std::find_if(bonds_.cbegin(), bonds_.cend(), [i, j](const auto &bond) { return bond.matches(i, j); });
    if (it == bonds_.end())
        return {};

    return *it;
}

// Return the SpeciesBond between the specified SpeciesAtom indices
OptionalReferenceWrapper<SpeciesBond> Species::getBond(int i, int j) { return getBond(&atom(i), &atom(j)); }
OptionalReferenceWrapper<const SpeciesBond> Species::getBond(int i, int j) const { return getBond(&atom(i), &atom(j)); }

// Add missing bonds
void Species::addMissingBonds(double tolerance, bool preventMetallic)
{
    double r, radiusI;
    for (auto indexI = 0; indexI < nAtoms() - 1; ++indexI)
    {
        // Get SpeciesAtom 'i' and its radius
        auto &i = atom(indexI);
        radiusI = AtomicRadii::radius(i.Z());
        for (auto indexJ = indexI + 1; indexJ < nAtoms(); ++indexJ)
        {
            // Get SpeciesAtom 'j'
            auto &j = atom(indexJ);

            // If the two atoms are both metal ions and preventMetallic = true, continue
            if (preventMetallic && Elements::isMetallic(i.Z()) && Elements::isMetallic(j.Z()))
                continue;

            // If the two atoms are already bound, continue
            if (i.getBond(&j))
                continue;

            // Calculate distance between atoms
            r = box_ ? box_->minimumDistance(j.r(), i.r()) : (j.r() - i.r()).magnitude();

            // Compare distance to sum of atomic radii (multiplied by tolerance factor)
            if (r <= (radiusI + AtomicRadii::radius(j.Z())) * tolerance)
                addBond(&i, &j);
        }
    }
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

// Remove all higher order intramolecular terms
void Species::removeHigherOrderIntramolecularTerms()
{
    angles_.clear();
    torsions_.clear();
    impropers_.clear();

    ++version_;
}

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

        // Swap j and k over if j is terminal and has only a single bond (i.e. jk)
        if (j->nBonds() == 1)
            std::swap(j, k);

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
    auto atomsContains = [this](const auto *x)
    { return std::find_if(atoms_.begin(), atoms_.end(), [&x](const auto &p) { return &p == x; }) != atoms_.end(); };

    // Check existing angle terms for any that are invalid
    angles_.erase(std::remove_if(angles_.begin(), angles_.end(),
                                 [this, &atomsContains](const auto &angle)
                                 {
                                     return ((!atomsContains(angle.i())) || (!atomsContains(angle.j())) ||
                                             (!atomsContains(angle.k()))) ||
                                            ((!hasBond(angle.i(), angle.j())) || (!hasBond(angle.j(), angle.k())));
                                 }),
                  angles_.end());

    // Remove torsions with invalid atoms or bonds
    torsions_.erase(std::remove_if(torsions_.begin(), torsions_.end(),
                                   [this, &atomsContains](const auto &torsion)
                                   {
                                       return ((!atomsContains(torsion.i())) || (!atomsContains(torsion.j())) ||
                                               (!atomsContains(torsion.k())) || (!atomsContains(torsion.l()))) ||
                                              ((!hasBond(torsion.i(), torsion.j())) || (!hasBond(torsion.j(), torsion.k())) ||
                                               (!hasBond(torsion.k(), torsion.l())));
                                   }),
                    torsions_.end());

    // Remove impropers with invalid atoms or bonds
    impropers_.erase(std::remove_if(impropers_.begin(), impropers_.end(),
                                    [this, &atomsContains](const auto &improper)
                                    {
                                        return ((!atomsContains(improper.i())) || (!atomsContains(improper.j())) ||
                                                (!atomsContains(improper.k())) || (!atomsContains(improper.l()))) ||
                                               ((!hasBond(improper.i(), improper.j())) ||
                                                (!hasBond(improper.j(), improper.k())) ||
                                                (!hasBond(improper.k(), improper.l())));
                                    }),
                     impropers_.end());

    ++version_;
}

// Add new SpeciesAngle definition
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
SpeciesAngle &Species::addAngle(int i, int j, int k) { return addAngle(&atom(i), &atom(j), &atom(k)); }

// Return number of SpeciesAngles defined
int Species::nAngles() const { return angles_.size(); }

// Return vector of SpeciesAngle
std::vector<SpeciesAngle> &Species::angles() { return angles_; }

const std::vector<SpeciesAngle> &Species::angles() const { return angles_; }

// Return whether SpeciesAngle between SpeciesAtoms exists
bool Species::hasAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k) const
{
    return std::any_of(angles_.cbegin(), angles_.cend(), [i, j, k](const auto &angle) { return angle.matches(i, j, k); });
}

// Return the SpeciesAngle between the specified SpeciesAtoms
OptionalReferenceWrapper<SpeciesAngle> Species::getAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k)
{
    auto it = std::find_if(angles_.begin(), angles_.end(), [i, j, k](auto &angle) { return angle.matches(i, j, k); });
    if (it == angles_.end())
        return {};

    return *it;
}
OptionalReferenceWrapper<const SpeciesAngle> Species::getAngle(const SpeciesAtom *i, const SpeciesAtom *j,
                                                               const SpeciesAtom *k) const
{
    auto it = std::find_if(angles_.begin(), angles_.end(), [i, j, k](auto &angle) { return angle.matches(i, j, k); });
    if (it == angles_.end())
        return {};

    return *it;
}

// Return the SpeciesAngle between the specified SpeciesAtom indic
OptionalReferenceWrapper<SpeciesAngle> Species::getAngle(int i, int j, int k) { return getAngle(&atom(i), &atom(j), &atom(k)); }
OptionalReferenceWrapper<const SpeciesAngle> Species::getAngle(int i, int j, int k) const
{
    return getAngle(&atom(i), &atom(j), &atom(k));
}

// Add new SpeciesTorsion definition
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
SpeciesTorsion &Species::addTorsion(int i, int j, int k, int l) { return addTorsion(&atom(i), &atom(j), &atom(k), &atom(l)); }

// Return number of SpeciesTorsions defined
int Species::nTorsions() const { return torsions_.size(); }

// Return vector of SpeciesTorsions
std::vector<SpeciesTorsion> &Species::torsions() { return torsions_; }

const std::vector<SpeciesTorsion> &Species::torsions() const { return torsions_; }

// Return whether SpeciesTorsion between SpeciesAtoms exists
bool Species::hasTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) const
{
    return std::any_of(torsions_.cbegin(), torsions_.cend(), [&](const auto &t) { return t.matches(i, j, k, l); });
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
OptionalReferenceWrapper<const SpeciesTorsion> Species::getTorsion(const SpeciesAtom *i, const SpeciesAtom *j,
                                                                   const SpeciesAtom *k, const SpeciesAtom *l) const
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
OptionalReferenceWrapper<const SpeciesTorsion> Species::getTorsion(int i, int j, int k, int l) const
{
    return getTorsion(&atom(i), &atom(j), &atom(k), &atom(l));
}

// Add new SpeciesImproper definition
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
SpeciesImproper &Species::addImproper(int i, int j, int k, int l)
{
    return addImproper(&atom(i), &atom(j), &atom(k), &atom(l));
}

// Return number of SpeciesImproper defined
int Species::nImpropers() const { return impropers_.size(); }

// Return vector of SpeciesImproper
std::vector<SpeciesImproper> &Species::impropers() { return impropers_; }

const std::vector<SpeciesImproper> &Species::impropers() const { return impropers_; }

// Return whether SpeciesImproper between SpeciesAtoms exists
bool Species::hasImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) const
{
    return std::any_of(impropers_.cbegin(), impropers_.cend(),
                       [i, j, k, l](auto &improper) { return improper.matches(i, j, k, l); });
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
OptionalReferenceWrapper<const SpeciesImproper> Species::getImproper(const SpeciesAtom *i, const SpeciesAtom *j,
                                                                     const SpeciesAtom *k, const SpeciesAtom *l) const
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
OptionalReferenceWrapper<const SpeciesImproper> Species::getImproper(int i, int j, int k, int l) const
{
    return getImproper(&atom(i), &atom(j), &atom(k), &atom(l));
}

// Set-up excluded / scaled interactions on atoms
void Species::setUpScaledInteractions()
{
    for (auto &i : atoms_)
        i.setScaledInteractions();
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
        auto selection = fragment(bond.i()->index(), bond);

        // If the list now contains Atom j, the two atoms are present in a cycle of some sort, and we can only add the
        // Atom 'i' itself In that case we can also finish the list for Atom 'j', and continue the loop.
        if (std::find(selection.begin(), selection.end(), bond.j()->index()) != selection.end())
        {
            Messenger::printVerbose("Bond between Atoms {}-{} is present in a cycle, so a minimal set of attached "
                                    "atoms will be used.\n",
                                    bond.i()->userIndex(), bond.j()->userIndex());
            bond.setAttachedAtoms(0, bond.i()->index());
            bond.setAttachedAtoms(1, bond.j()->index());
            bond.setInCycle(true);
            continue;
        }
        else
            bond.setAttachedAtoms(0, selection);

        // Select all Atoms attached to Atom 'i', excluding the Bond as a path
        clearAtomSelection();
        selection = fragment(bond.j()->index(), bond);
        bond.setAttachedAtoms(1, selection);
    }

    // Angles - termini are 'i' and 'k'
    for (auto &angle : angles_)
    {
        // Grab relevant Bonds (if they exist)
        auto ji = angle.j()->getBond(angle.i());
        auto jk = angle.j()->getBond(angle.k());

        // Select all Atoms attached to Atom 'i', excluding the Bond ji as a path
        auto selection = fragment(angle.i()->index(), *ji, *jk);

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
                                    angle.i()->userIndex(), angle.j()->userIndex(), angle.k()->userIndex());
            angle.setAttachedAtoms(0, angle.i()->index());
            angle.setAttachedAtoms(1, angle.k()->index());
            angle.setInCycle(true);
            continue;
        }
        else
            angle.setAttachedAtoms(0, selection);

        // Select all Atoms attached to Atom 'k', excluding the Bond jk as a path
        clearAtomSelection();
        selection = fragment(angle.k()->index(), *ji, jk);

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
        auto jk = torsion.j()->getBond(torsion.k());

        // Select all Atoms attached to Atom 'j', excluding the Bond ji as a path
        auto selection = fragment(torsion.j()->index(), *jk);

        // Remove Atom 'j' from the list
        selection.erase(std::remove(selection.begin(), selection.end(), torsion.j()->index()));

        // If the list now contains Atom k, the two atoms are present in a cycle of some sort, and we can only add the
        // Atom 'i'
        if (std::find(selection.begin(), selection.end(), torsion.k()->index()) != selection.end())
        {
            Messenger::printVerbose("Torsion between Atoms {}-{}-{}-{} is present in a cycle, so a minimal set of "
                                    "attached atoms will be used.\n",
                                    torsion.i()->userIndex(), torsion.j()->userIndex(), torsion.k()->userIndex(),
                                    torsion.l()->userIndex());
            torsion.setAttachedAtoms(0, torsion.i()->index());
            torsion.setAttachedAtoms(1, torsion.l()->index());
            torsion.setInCycle(true);
            continue;
        }
        else
            torsion.setAttachedAtoms(0, selection);

        // Select all Atoms attached to Atom 'k', excluding the Bond jk as a path
        clearAtomSelection();
        selection = fragment(torsion.k()->index(), *jk);

        // Remove Atom 'k' from the list
        selection.erase(std::remove(selection.begin(), selection.end(), torsion.k()->index()));

        torsion.setAttachedAtoms(1, selection);
    }

    attachedAtomListsGenerated_ = true;
}

// Detach master term links for all interaction types, copying parameters to local SpeciesIntra
void Species::detachFromMasterTerms()
{
    for (auto &bond : bonds_)
        bond.detachFromMasterTerm();

    for (auto &angle : angles_)
        angle.detachFromMasterTerm();

    for (auto &torsion : torsions_)
        torsion.detachFromMasterTerm();

    for (auto &improper : impropers_)
        improper.detachFromMasterTerm();
}

// Detach links to specified master term, copying parameters to local SpeciesIntra
void Species::detachFromMasterTerm(MasterBond *master)
{
    for (auto &bond : bonds_)
        if (bond.masterTerm() == master)
            bond.detachFromMasterTerm();
}

void Species::detachFromMasterTerm(MasterAngle *master)
{
    for (auto &angle : angles_)
        if (angle.masterTerm() == master)
            angle.detachFromMasterTerm();
}

void Species::detachFromMasterTerm(MasterTorsion *master)
{
    for (auto &torsion : torsions_)
        if (torsion.masterTerm() == master)
            torsion.detachFromMasterTerm();
}

void Species::detachFromMasterTerm(MasterImproper *master)
{
    for (auto &improper : impropers_)
        if (improper.masterTerm() == master)
            improper.detachFromMasterTerm();
}

template <class Master, class Intra>
void generateMasterTerm(Intra &term, std::string_view termName,
                        std::function<OptionalReferenceWrapper<Master>(std::string_view termName)> termGetter,
                        std::function<Master &(std::string_view termName)> termCreator)
{
    // Search for an existing master term by this name, or a related one suffixed with a number
    OptionalReferenceWrapper<Master> optMaster;
    auto index = 0;
    while ((optMaster = termGetter(index == 0 ? termName : fmt::format("{}_{}", termName, index))))
    {
        // Are the parameters the same as our local term?
        const Master &master = optMaster->get();
        if (master.interactionForm() != term.interactionForm() ||
            master.interactionParameters().size() != term.interactionParameters().size())
            continue;
        else
            for (auto &&[localValue, masterValue] : zip(term.interactionParameters(), master.interactionParameters()))
                if (fabs(localValue - masterValue) > 1.0e-8)
                    optMaster = std::nullopt;

        // If the terms are identical, use this one (break out now)
        if (optMaster)
            break;

        ++index;
    }

    // If we no longer have a valid reference, need to create a new term
    if (!optMaster)
    {
        optMaster = termCreator(index == 0 ? termName : fmt::format("{}_{}", termName, index));
        optMaster->get().setInteractionFormAndParameters(term.interactionForm(), term.interactionParameters());
    }

    term.setMasterTerm(&optMaster->get());
}

// Reduce intramolecular terms to master terms
void Species::reduceToMasterTerms(CoreData &coreData, bool selectionOnly)
{
    // Bonds
    for (auto &bond : bonds_)
    {
        // Selection only?
        if (selectionOnly && !bond.isSelected())
            continue;

        // Construct a name for the master term based on the atom types
        std::vector<std::string_view> names = {bond.i()->atomType()->name(), bond.j()->atomType()->name()};
        std::sort(names.begin(), names.end());
        generateMasterTerm<MasterBond>(
            bond, joinStrings(names, "-"), [&coreData](std::string_view name) { return coreData.getMasterBond(name); },
            [&coreData](auto name) -> MasterBond & { return coreData.addMasterBond(name); });
    }

    // Angles
    for (auto &angle : angles_)
    {
        // Selection only?
        if (selectionOnly && !angle.isSelected())
            continue;

        // Construct a name for the master term based on the atom types
        if (angle.i()->atomType()->name() < angle.k()->atomType()->name())
            generateMasterTerm<MasterAngle>(
                angle,
                fmt::format("{}-{}-{}", angle.i()->atomType()->name(), angle.j()->atomType()->name(),
                            angle.k()->atomType()->name()),
                [&coreData](std::string_view name) { return coreData.getMasterAngle(name); },
                [&coreData](auto name) -> MasterAngle & { return coreData.addMasterAngle(name); });
        else
            generateMasterTerm<MasterAngle>(
                angle,
                fmt::format("{}-{}-{}", angle.k()->atomType()->name(), angle.j()->atomType()->name(),
                            angle.i()->atomType()->name()),
                [&coreData](std::string_view name) { return coreData.getMasterAngle(name); },
                [&coreData](auto name) -> MasterAngle & { return coreData.addMasterAngle(name); });
    }

    // Torsions
    for (auto &torsion : torsions_)
    {
        // Selection only?
        if (selectionOnly && !torsion.isSelected())
            continue;

        // Construct a name for the master term based on the atom types
        if (torsion.i()->atomType()->name() < torsion.l()->atomType()->name())
            generateMasterTerm<MasterTorsion>(
                torsion,
                fmt::format("{}-{}-{}-{}", torsion.i()->atomType()->name(), torsion.j()->atomType()->name(),
                            torsion.k()->atomType()->name(), torsion.l()->atomType()->name()),
                [&coreData](std::string_view name) { return coreData.getMasterTorsion(name); },
                [&coreData](auto name) -> MasterTorsion & { return coreData.addMasterTorsion(name); });
        else
            generateMasterTerm<MasterTorsion>(
                torsion,
                fmt::format("{}-{}-{}-{}", torsion.l()->atomType()->name(), torsion.k()->atomType()->name(),
                            torsion.j()->atomType()->name(), torsion.i()->atomType()->name()),
                [&coreData](std::string_view name) { return coreData.getMasterTorsion(name); },
                [&coreData](auto name) -> MasterTorsion & { return coreData.addMasterTorsion(name); });
    }

    // Impropers
    for (auto &improper : impropers_)
    {
        // Selection only?
        if (selectionOnly && !improper.isSelected())
            continue;

        // Construct a name for the master term based on the atom types
        std::vector<std::string_view> jkl = {improper.j()->atomType()->name(), improper.k()->atomType()->name(),
                                             improper.l()->atomType()->name()};
        std::sort(jkl.begin(), jkl.end());
        generateMasterTerm<MasterImproper>(
            improper, fmt::format("{}-{}", improper.i()->atomType()->name(), joinStrings(jkl, "-")),
            [&coreData](std::string_view name) { return coreData.getMasterImproper(name); },
            [&coreData](auto name) -> MasterImproper & { return coreData.addMasterImproper(name); });
    }
}

// Return periodic box
const Box *Species::box() const { return box_.get(); }

// Remove Box definition and revert to single image
void Species::removeBox() { box_ = std::make_unique<SingleImageBox>(); }

// Create Box definition with specified lengths and angles
void Species::createBox(const Vec3<double> lengths, const Vec3<double> angles, bool nonPeriodic)
{
    box_ = nonPeriodic ? std::make_unique<NonPeriodicBox>() : Box::generate(lengths, angles);
}
