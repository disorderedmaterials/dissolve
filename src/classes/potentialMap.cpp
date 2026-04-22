// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/potentialMap.h"
#include "base/messenger.h"
#include "classes/atomType.h"
#include "classes/configurationAtom.h"
#include "classes/molecule.h"
#include "classes/pairPotential.h"
#include "classes/species.h"

PotentialMap::PotentialMap(const std::vector<const AtomType *> &atomTypes, const DoubleKeyedMap<PairPotential> &pairPotentials)
{
    // Create PairPotential matrix
    nTypes_ = atomTypes.size();
    potentialMatrix_.initialise(nTypes_, nTypes_, true);

    dissolve::for_each_pair(
        ParallelPolicies::seq, atomTypes,
        [&](int i, const auto &atI, int j, const auto &atJ)
        {
            // Store PairPotential pointer
            if (i == j)
            {
                Messenger::print("Linking self-interaction PairPotential for '{}' (index {},{} in matrix).\n", atI->name(), i,
                                 j);
                potentialMatrix_[{i, j}] = &pairPotentials.get({atI->name(), atJ->name()});
            }
            else
            {
                Messenger::print("Linking PairPotential between '{}' and '{}' (indices {},{} and {},{} in matrix).\n",
                                 atI->name(), atJ->name(), i, j, j, i);
                potentialMatrix_[{i, j}] = &pairPotentials.get({atI->name(), atJ->name()});
            }
        });
}

// Clear all data
void PotentialMap::clear() { potentialMatrix_.clear(); }

/*
 * Energy / Force
 */

// Return energy between Atoms at distance specified
double PotentialMap::energy(const ConfigurationAtom &i, const ConfigurationAtom &j, double r) const
{
    assert(r >= 0.0);
    assert(i.speciesAtom() && j.speciesAtom());

    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    auto *pp = potentialMatrix_[{i.configurationTypeIndex(), j.configurationTypeIndex()}];
    return pp->energy(r) + (PairPotential::includeCoulombPotential()
                                ? 0
                                : pp->analyticCoulombEnergy(i.speciesAtom()->charge() * j.speciesAtom()->charge(), r));
}

// Return energy between Atoms at distance specified, scaling electrostatic and short-range components
double PotentialMap::energy(const ConfigurationAtom &i, const ConfigurationAtom &j, double r, double elecScale,
                            double srScale) const
{
    assert(r >= 0.0);
    assert(i.speciesAtom() && j.speciesAtom());

    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    auto *pp = potentialMatrix_[{i.configurationTypeIndex(), j.configurationTypeIndex()}];
    return PairPotential::includeCoulombPotential()
               ? pp->energy(r, elecScale, srScale)
               : pp->energy(r) * srScale +
                     pp->analyticCoulombEnergy(i.speciesAtom()->charge() * j.speciesAtom()->charge(), r) * elecScale;
}

// Return energy between SpeciesAtoms at distance specified
double PotentialMap::energy(const SpeciesAtom *i, const SpeciesAtom *j, double r) const
{
    assert(r >= 0.0);
    assert(i && j);

    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    auto *pp = potentialMatrix_[{i->atomType()->index(), j->atomType()->index()}];
    return pp->energy(r) +
           (PairPotential::includeCoulombPotential() ? 0 : pp->analyticCoulombEnergy(i->charge() * j->charge(), r));
}

// Return energy between SpeciesAtoms at distance specified, scaling electrostatic and short-range components
double PotentialMap::energy(const SpeciesAtom *i, const SpeciesAtom *j, double r, double elecScale, double srScale) const
{
    assert(r >= 0.0);
    assert(i && j);

    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    auto *pp = potentialMatrix_[{i->atomType()->index(), j->atomType()->index()}];
    return PairPotential::includeCoulombPotential()
               ? pp->energy(r, elecScale, srScale)
               : pp->energy(r) * srScale + pp->analyticCoulombEnergy(i->charge() * j->charge(), r) * elecScale;
}

// Return analytic energy between Atom types at distance specified
double PotentialMap::analyticEnergy(const ConfigurationAtom &i, const ConfigurationAtom &j, double r) const
{
    assert(r >= 0.0);

    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being local to the atom
    // types
    auto *pp = potentialMatrix_[{i.configurationTypeIndex(), j.configurationTypeIndex()}];
    return PairPotential::includeCoulombPotential()
               ? pp->analyticEnergy(r, 1.0, 1.0)
               : pp->analyticEnergy(i.speciesAtom()->charge() * j.speciesAtom()->charge(), r, 1.0, 1.0);
}

// Return analytic energy between Atom types at distance specified, scaling electrostatic and short-range components
double PotentialMap::analyticEnergy(const ConfigurationAtom &i, const ConfigurationAtom &j, double r, double elecScale,
                                    double srScale) const
{
    assert(r >= 0.0);

    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being local to the atom
    // types
    auto *pp = potentialMatrix_[{i.configurationTypeIndex(), j.configurationTypeIndex()}];
    return PairPotential::includeCoulombPotential()
               ? pp->analyticEnergy(r, elecScale, srScale)
               : pp->analyticEnergy(i.speciesAtom()->charge() * j.speciesAtom()->charge(), r, elecScale, srScale);
}

// Return force between Atoms at distance specified
double PotentialMap::force(const ConfigurationAtom &i, const ConfigurationAtom &j, double r) const
{
    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    auto *pp = potentialMatrix_[{i.configurationTypeIndex(), j.configurationTypeIndex()}];
    return PairPotential::includeCoulombPotential()
               ? pp->force(r)
               : pp->force(r) + pp->analyticCoulombForce(i.speciesAtom()->charge() * j.speciesAtom()->charge(), r);
}

// Return force between Atoms at distance specified, scaling electrostatic and short-range components
double PotentialMap::force(const ConfigurationAtom &i, const ConfigurationAtom &j, double r, double elecScale,
                           double srScale) const
{
    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    auto *pp = potentialMatrix_[{i.configurationTypeIndex(), j.configurationTypeIndex()}];
    return PairPotential::includeCoulombPotential()
               ? pp->force(r, elecScale, srScale)
               : pp->force(r) * srScale +
                     pp->analyticCoulombForce(i.speciesAtom()->charge() * j.speciesAtom()->charge(), r) * elecScale;
}

// Return force between SpeciesAtoms at distance specified
double PotentialMap::force(const SpeciesAtom *i, const SpeciesAtom *j, double r) const
{
    assert(r >= 0.0);
    assert(i && j);

    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    auto *pp = potentialMatrix_[{i->atomType()->index(), j->atomType()->index()}];
    return PairPotential::includeCoulombPotential() ? pp->force(r)
                                                    : pp->force(r) + pp->analyticCoulombForce(i->charge() * j->charge(), r);
}

// Return force between SpeciesAtoms at distance specified, scaling electrostatic and short-range components
double PotentialMap::force(const SpeciesAtom *i, const SpeciesAtom *j, double r, double elecScale, double srScale) const
{
    assert(r >= 0.0);
    assert(i && j);

    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    auto *pp = potentialMatrix_[{i->atomType()->index(), j->atomType()->index()}];
    return PairPotential::includeCoulombPotential()
               ? pp->force(r, elecScale, srScale)
               : pp->force(r) * srScale + pp->analyticCoulombForce(i->charge() * j->charge(), r) * elecScale;
}

// Return analytic force between Atom types at distance specified
double PotentialMap::analyticForce(const ConfigurationAtom &i, const ConfigurationAtom &j, double r) const
{
    assert(r >= 0.0);
    assert(i.speciesAtom() && j.speciesAtom());

    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    auto *pp = potentialMatrix_[{i.configurationTypeIndex(), j.configurationTypeIndex()}];
    return PairPotential::includeCoulombPotential()
               ? pp->analyticForce(r, 1.0, 1.0)
               : pp->analyticForce(i.speciesAtom()->charge() * j.speciesAtom()->charge(), r, 1.0, 1.0);
}

// Return analytic force between Atom types at distance specified, scaling electrostatic and short-range components
double PotentialMap::analyticForce(const ConfigurationAtom &i, const ConfigurationAtom &j, double r, double elecScale,
                                   double srScale) const
{
    assert(r >= 0.0);
    assert(i.speciesAtom() && j.speciesAtom());

    // Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the
    // interpolated potential
    auto *pp = potentialMatrix_[{i.configurationTypeIndex(), j.configurationTypeIndex()}];
    return PairPotential::includeCoulombPotential()
               ? pp->analyticForce(r, elecScale, srScale)
               : pp->analyticForce(i.speciesAtom()->charge() * j.speciesAtom()->charge(), r, elecScale, srScale);
}
