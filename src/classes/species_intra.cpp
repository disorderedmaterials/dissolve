/*
    *** Species Definition - Intramolecular Terms
    *** src/classes/species_intra.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/species.h"
#include "data/atomicradius.h"
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

    // Check existing angle terms for any that are invalid
    angles_.erase(std::remove_if(angles_.begin(), angles_.end(),
                                 [this](const auto &angle) {
                                     return ((!atoms_.contains(angle.i())) || (!atoms_.contains(angle.j())) ||
                                             (!atoms_.contains(angle.k()))) ||
                                            ((!hasBond(angle.i(), angle.j())) || (!hasBond(angle.j(), angle.k())));
                                 }),
                  angles_.end());

    // remove torsions with invalid atoms or bonds
    torsions_.erase(std::remove_if(torsions_.begin(), torsions_.end(),
                                   [this](const auto &torsion) {
                                       return ((!atoms_.contains(torsion.i())) || (!atoms_.contains(torsion.j())) ||
                                               (!atoms_.contains(torsion.k())) || (!atoms_.contains(torsion.l()))) ||
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
    bonds_.emplace_back(i, j).setParent(this);

    // Update higher-order connectivity?
    if (autoUpdateIntramolecularTerms_)
        updateIntramolecularTerms();

    ++version_;

    return bonds_.back();
}

// Add new SpeciesBond definition
SpeciesBond &Species::addBond(int i, int j) { return addBond(atoms_[i], atoms_[j]); }

// Remove bond between specified SpeciesAtoms
void Species::removeBond(SpeciesAtom *i, SpeciesAtom *j)
{
    auto it = std::remove_if(bonds_.begin(), bonds_.end(), [i, j](const auto &bond) { return bond.matches(i, j); });
    std::for_each(it, bonds_.end(), [](auto &bond) { bond.detach(); });
    bonds_.erase(it);
}

// Return number of SpeciesBonds defined
int Species::nBonds() const { return bonds_.size(); }

// Return array of SpeciesBond
std::vector<SpeciesBond> &Species::bonds() { return bonds_; }

// Return array of SpeciesBond (const)
const std::vector<SpeciesBond> &Species::constBonds() const { return bonds_; }

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

// Return the SpeciesBond between the specified SpeciesAtom indices
OptionalReferenceWrapper<SpeciesBond> Species::getBond(int i, int j)
{
    if ((i < 0) || (i >= nAtoms()))
    {
        Messenger::print("OUT_OF_RANGE - Internal index 'i' supplied to Species::hasBond() is out of range ({}) for "
                         "Species '{}'\n",
                         i, name_);
        return {};
    }
    if ((j < 0) || (j >= nAtoms()))
    {
        Messenger::print("OUT_OF_RANGE - Internal index 'j' supplied to Species::hasBond() is out of range ({}) for "
                         "Species '{}'\n",
                         j, name_);
        return {};
    }

    return getBond(atoms_[i], atoms_[j]);
}

// Return the SpeciesBond between the specified SpeciesAtoms (const)
OptionalReferenceWrapper<const SpeciesBond> Species::getConstBond(SpeciesAtom *i, SpeciesAtom *j) const
{
    auto it = std::find_if(bonds_.cbegin(), bonds_.cend(), [i, j](const auto &bond) { return bond.matches(i, j); });
    if (it == bonds_.end())
        return {};

    return *it;
}

// Add missing bonds
void Species::addMissingBonds(double tolerance)
{
    Vec3<double> vij;
    double radiusI;
    SpeciesAtom **atoms = atoms_.array();
    for (int indexI = 0; indexI < nAtoms() - 1; ++indexI)
    {
        // Get SpeciesAtom 'i' and its radius
        SpeciesAtom *i = atoms[indexI];
        radiusI = AtomicRadius::radius(i->element());
        for (int indexJ = indexI + 1; indexJ < nAtoms(); ++indexJ)
        {
            // Get SpeciesAtom 'j'
            SpeciesAtom *j = atoms[indexJ];

            // If the two atoms are already bound, continue
            if (i->hasBond(j))
                continue;

            // Calculate distance between atoms
            vij = j->r() - i->r();

            // Compare distance to sum of atomic radii (multiplied by tolerance factor)
            if (vij.magnitude() <= (radiusI + AtomicRadius::radius(j->element())) * tolerance)
                addBond(i, j);
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

    // We can't use emplace_back since SpeciesAngle needs
    // to dereference its `this` pointer in the constructor
    // to update the SpeciesAtoms.
    angles_.push_back(std::move(SpeciesAngle(i, j, k)));
    angles_.back().setParent(this);

    ++version_;

    return angles_.back();
}

// Add new SpeciesAngle definition
SpeciesAngle &Species::addAngle(int i, int j, int k) { return addAngle(atoms_[i], atoms_[j], atoms_[k]); }

// Return number of SpeciesAngles defined
int Species::nAngles() const { return angles_.size(); }

// Return array of SpeciesAngle
std::vector<SpeciesAngle> &Species::angles() { return angles_; }

// Return array of SpeciesAngle (const)
const std::vector<SpeciesAngle> &Species::constAngles() const { return angles_; }

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
OptionalReferenceWrapper<SpeciesAngle> Species::getAngle(int i, int j, int k)
{
    return getAngle(atoms_[i], atoms_[j], atoms_[k]);
}

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
    torsions_.back().setParent(this);

    ++version_;

    return torsions_.back();
}

// Add new SpeciesTorsion definition
SpeciesTorsion &Species::addTorsion(int i, int j, int k, int l)
{
    return addTorsion(atoms_[i], atoms_[j], atoms_[k], atoms_[l]);
}

// Return number of SpeciesTorsions defined
int Species::nTorsions() const { return torsions_.size(); }

// Return array of SpeciesTorsions
std::vector<SpeciesTorsion> &Species::torsions() { return torsions_; }

// Return array of SpeciesTorsions (const)
const std::vector<SpeciesTorsion> &Species::constTorsions() const { return torsions_; }

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
    return getTorsion(atoms_[i], atoms_[j], atoms_[k], atoms_[l]);
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
    impropers_.emplace_back(i, j, k, l).setParent(this);

    ++version_;

    return impropers_.back();
}

// Add new SpeciesImproper definition
SpeciesImproper &Species::addImproper(int i, int j, int k, int l)
{
    return addImproper(atoms_[i], atoms_[j], atoms_[k], atoms_[l]);
}

// Return number of SpeciesImproper defined
int Species::nImpropers() const { return impropers_.size(); }

// Return array of SpeciesImproper
std::vector<SpeciesImproper> &Species::impropers() { return impropers_; }

// Return array of SpeciesImproper (const)
const std::vector<SpeciesImproper> &Species::constImpropers() const { return impropers_; }

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
    return getImproper(atoms_[i], atoms_[j], atoms_[k], atoms_[l]);
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
}
