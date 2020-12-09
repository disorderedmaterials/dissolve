// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "classes/potentialmap.h"
#include "base/messenger.h"
#include "base/parameters.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/molecule.h"
#include "classes/pairpotential.h"
#include "classes/species.h"
#include "math/constants.h"
#include <math.h>
#include <new>
using namespace std;

PotentialMap::PotentialMap() {}

PotentialMap::~PotentialMap() {}

// Clear all data
void PotentialMap::clear() { potentialMatrix_.clear(); }

/*
 * Source Parameters
 */

// Initialise maps
bool PotentialMap::initialise(const std::vector<std::shared_ptr<AtomType>> &masterAtomTypes,
                              const List<PairPotential> &pairPotentials, double pairPotentialRange)
{
    // Clear old data first
    clear();

    // Create PairPotential matrix
    nTypes_ = masterAtomTypes.size();
    potentialMatrix_.initialise(nTypes_, nTypes_);

    // Loop over defined PairPotentials
    int indexI, indexJ;
    for (auto *pot = pairPotentials.first(); pot != nullptr; pot = pot->next())
    {
        indexI = pot->atomTypeI()->index();
        indexJ = pot->atomTypeJ()->index();
        if (indexI == -1)
            return Messenger::error("Couldn't find AtomType '{}' in typeIndex.\n", pot->atomTypeI()->name());
        if (indexJ == -1)
            return Messenger::error("Couldn't find AtomType '{}' in typeIndex.\n", pot->atomTypeJ()->name());

        // Store PairPotential pointer
        if (indexI == indexJ)
        {
            Messenger::print("Linking self-interaction PairPotential for '{}' (index {},{} in matrix).\n",
                             pot->atomTypeI()->name(), indexI, indexJ);
            potentialMatrix_[{indexI, indexI}] = pot;
        }
        else
        {
            Messenger::print("Linking PairPotential between '{}' and '{}' (indices {},{} and {},{} in matrix).\n",
                             pot->atomTypeI()->name(), pot->atomTypeJ()->name(), indexI, indexJ, indexJ, indexI);
            potentialMatrix_[{indexI, indexJ}] = pot;
            potentialMatrix_[{indexJ, indexI}] = pot;
        }
    }

    // Store potential range
    range_ = pairPotentialRange;

    return true;
}

// Return PairPotential range
double PotentialMap::range() const { return range_; }

/*
 * Energy / Force
 */

// Return energy between Atoms at distance specified
double PotentialMap::energy(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j, double r) const
{
#ifdef CHECKS
    if ((i->masterTypeIndex() < 0) || (i->masterTypeIndex() >= nTypes_))
    {
        Messenger::print("OUT_OF_RANGE - Type index of atom i ({}) passed to PotentialMap::energy() is out of range "
                         "(nTypes_ = {}).\n",
                         i->masterTypeIndex(), nTypes_);
        return 0.0;
    }
    if ((j->masterTypeIndex() < 0) || (j->masterTypeIndex() >= nTypes_))
    {
        Messenger::print("OUT_OF_RANGE - Type index of atom j ({}) passed to PotentialMap::energy() is out of range "
                         "(nTypes_ = {}).\n",
                         j->masterTypeIndex(), nTypes_);
        return 0.0;
    }
    if (r < 0.0)
    {
        Messenger::print("OUT_OF_RANGE - Distance passed to PotentialMap::energy() is negative ({}).\n", r);
        return 0.0;
    }
    if ((!i->speciesAtom()) || (!j->speciesAtom()))
    {
        Messenger::print("NULL_POINTER - One or both SpeciesAtoms in the Atoms passed to PotentialMap::energy() are "
                         "NULL.\n");
        return 0.0;
    }
#endif
    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    PairPotential *pp = potentialMatrix_[{i->masterTypeIndex(), j->masterTypeIndex()}];
    return pp->energy(r) +
           (pp->includeCoulomb() ? 0 : pp->analyticCoulombEnergy(i->speciesAtom()->charge() * j->speciesAtom()->charge(), r));
}

// Return energy between SpeciesAtoms at distance specified
double PotentialMap::energy(const SpeciesAtom *i, const SpeciesAtom *j, double r) const
{
    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    PairPotential *pp = potentialMatrix_[{i->atomType()->index(), j->atomType()->index()}];
    return pp->energy(r) + (pp->includeCoulomb() ? 0 : pp->analyticCoulombEnergy(i->charge() * j->charge(), r));
}

// Return analytic energy between Atom types at distance specified
double PotentialMap::analyticEnergy(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j, double r) const
{
#ifdef CHECKS
    if ((i->masterTypeIndex() < 0) || (i->masterTypeIndex() >= nTypes_))
    {
        Messenger::print("OUT_OF_RANGE - Type index of atom i ({}) passed to PotentialMap::analyticEnergy() is out of "
                         "range (nTypes_ = {}).\n",
                         i->masterTypeIndex(), nTypes_);
        return 0.0;
    }
    if ((j->masterTypeIndex() < 0) || (j->masterTypeIndex() >= nTypes_))
    {
        Messenger::print("OUT_OF_RANGE - Type index of atom j ({}) passed to PotentialMap::analyticEnergy() is out of "
                         "range (nTypes_ = {}).\n",
                         j->masterTypeIndex(), nTypes_);
        return 0.0;
    }
    if (r < 0.0)
    {
        Messenger::print("OUT_OF_RANGE - Distance passed to PotentialMap::analyticEnergy() is negative ({}).\n", r);
        return 0.0;
    }
#endif
    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being local to the atom
    // types
    PairPotential *pp = potentialMatrix_[{i->masterTypeIndex(), j->masterTypeIndex()}];
    return pp->includeCoulomb() ? pp->analyticEnergy(r)
                                : pp->analyticEnergy(i->speciesAtom()->charge() * j->speciesAtom()->charge(), r);
}

// Return force between Atoms at distance specified
double PotentialMap::force(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j, double r) const
{
#ifdef CHECKS
    if ((i->masterTypeIndex() < 0) || (i->masterTypeIndex() >= nTypes_))
    {
        Messenger::print("OUT_OF_RANGE - Type index of atom i ({}) passed to PotentialMap::force() is out of range "
                         "(nTypes_ = {}).\n",
                         i->masterTypeIndex(), nTypes_);
        return 0.0;
    }
    if ((j->masterTypeIndex() < 0) || (j->masterTypeIndex() >= nTypes_))
    {
        Messenger::print("OUT_OF_RANGE - Type index of atom j ({}) passed to PotentialMap::force() is out of range "
                         "(nTypes_ = {}).\n",
                         j->masterTypeIndex(), nTypes_);
        return 0.0;
    }
    if (r < 0.0)
    {
        Messenger::print("OUT_OF_RANGE - Distance passed to PotentialMap::force() is negative ({}).\n", r);
        return 0.0;
    }
    if ((!i->speciesAtom()) || (!j->speciesAtom()))
    {
        Messenger::print("NULL_POINTER - One or both SpeciesAtoms in the Atoms passed to PotentialMap::force() are "
                         "NULL.\n");
        return 0.0;
    }
#endif
    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    PairPotential *pp = potentialMatrix_[{i->masterTypeIndex(), j->masterTypeIndex()}];
    return pp->includeCoulomb()
               ? pp->force(r)
               : pp->force(r) + pp->analyticCoulombForce(i->speciesAtom()->charge() * j->speciesAtom()->charge(), r);
}

// Return force between SpeciesAtoms at distance specified
double PotentialMap::force(const SpeciesAtom *i, const SpeciesAtom *j, double r) const
{
    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    PairPotential *pp = potentialMatrix_[{i->atomType()->index(), j->atomType()->index()}];
    return pp->includeCoulomb() ? pp->force(r) : pp->force(r) + pp->analyticCoulombForce(i->charge() * j->charge(), r);
}

// Return analytic force between Atom types at distance specified
double PotentialMap::analyticForce(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j, double r) const
{
#ifdef CHECKS
    if ((i->masterTypeIndex() < 0) || (i->masterTypeIndex() >= nTypes_))
    {
        Messenger::print("OUT_OF_RANGE - Type index of atom i ({}) passed to PotentialMap::analyticForce() is out of "
                         "range (nTypes_ = {}).\n",
                         i->masterTypeIndex(), nTypes_);
        return 0.0;
    }
    if ((j->masterTypeIndex() < 0) || (j->masterTypeIndex() >= nTypes_))
    {
        Messenger::print("OUT_OF_RANGE - Type index of atom j ({}) passed to PotentialMap::analyticForce() is out of "
                         "range (nTypes_ = {}).\n",
                         j->masterTypeIndex(), nTypes_);
        return 0.0;
    }
    if (r < 0.0)
    {
        Messenger::print("OUT_OF_RANGE - Distance passed to PotentialMap::analyticForce() is negative ({}).\n", r);
        return 0.0;
    }
#endif
    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    PairPotential *pp = potentialMatrix_[{i->masterTypeIndex(), j->masterTypeIndex()}];
    return pp->includeCoulomb() ? pp->analyticForce(r)
                                : pp->analyticForce(i->speciesAtom()->charge() * j->speciesAtom()->charge(), r);
}
