// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/species.h"
#include "classes/box.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "math/combinations.h"
#include "templates/algorithms.h"
#include <numeric>

SpeciesKernel::SpeciesKernel(const Species *sp, const PotentialMap &potentialMap)
    : GeometryKernel(species_->box(), potentialMap), species_(sp)
{
}

// Return pair potential energy of Species
Kernel::PairPotentialEnergyValue SpeciesKernel::pairPotentialEnergy() const
{
    const auto cutoff = PairPotential::range();

    Combinations comb(species_->nAtoms());
    return {0.0, dissolve::transform_reduce(ParallelPolicies::par, dissolve::counting_iterator<int>(0),
                                            dissolve::counting_iterator<int>(comb.getNumCombinations()), 0.0, std::plus<>(),
                                            [&](const auto idx)
                                            {
                                                auto [n, m] = comb.nthCombination(idx);
                                                auto &i = species_->atom(n);
                                                auto &j = species_->atom(m);
                                                auto &rI = i.r();
                                                auto &rJ = j.r();

                                                // Get interatomic distance
                                                double r = (rJ - rI).magnitude();
                                                if (r > cutoff)
                                                    return 0.0;

                                                // Get intramolecular scaling of atom pair
                                                auto &&[scalingType, elec14, vdw14] = i.scaling(&j);
                                                switch (scalingType)
                                                {
                                                    case (SpeciesAtom::ScaledInteraction::NotScaled):
                                                        return potentialMap_.energy(&i, &j, r);
                                                    case (SpeciesAtom::ScaledInteraction::Scaled):
                                                        return potentialMap_.energy(&i, &j, r, elec14, vdw14);
                                                    default:
                                                        return 0.0;
                                                }
                                            })};
}

// Return geometric energy of Species
Kernel::GeometryEnergyValue SpeciesKernel::geometricEnergy() const
{
    Kernel::GeometryEnergyValue energy;

    // Loop over bonds
    energy.bondEnergy =
        std::accumulate(species_->bonds().begin(), species_->bonds().end(), 0.0,
                        [&](const auto acc, const auto &b) { return acc + bondEnergy(b, b.j()->r(), b.i()->r()); });

    // Loop over angles
    energy.angleEnergy =
        std::accumulate(species_->angles().begin(), species_->angles().end(), 0.0, [&](const auto acc, const auto &a)
                        { return acc + angleEnergy(a, a.i()->r(), a.j()->r(), a.k()->r()); });

    // Loop over torsions
    energy.torsionEnergy =
        std::accumulate(species_->torsions().begin(), species_->torsions().end(), 0.0, [&](const auto acc, const auto &t)
                        { return acc + torsionEnergy(t, t.i()->r(), t.j()->r(), t.k()->r(), t.l()->r()); });

    // Loop over impropers
    energy.improperEnergy =
        std::accumulate(species_->impropers().begin(), species_->impropers().end(), 0.0, [&](const auto acc, const auto &imp)
                        { return acc + improperEnergy(imp, imp.i()->r(), imp.j()->r(), imp.k()->r(), imp.l()->r()); });

    return energy;
}

// Calculate pair potential forces within of Species
void SpeciesKernel::pairPotentialForces(std::vector<Vector3> &forces) const
{
    auto combinableUnbound = Kernel::createCombinableVector3(forces);

    auto pairwiseForceOperator = [&](int indexI, const auto &i, int indexJ, const auto &j)
    {
        if (indexI == indexJ)
            return;

        auto &&[scalingType, elec14, vdw14] = i.scaling(&j);
        if (scalingType == SpeciesAtom::ScaledInteraction::Excluded)
            return;

        // Determine final forces
        auto vecij = box_->minimumVector(i.r(), j.r());
        auto magSq = vecij.magnitudeSq();
        if (magSq > cutoffDistanceSquared_)
            return;

        auto r = sqrt(magSq);
        vecij /= r;

        if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
            vecij *= potentialMap_.force(&i, &j, r);
        else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
            vecij *= potentialMap_.force(&i, &j, r, elec14, vdw14);

        auto &fLocal = combinableUnbound.local();
        fLocal[indexI] -= vecij;
        fLocal[indexJ] += vecij;
    };

    if (species_->nAtoms() < 100)
        dissolve::for_each_pair(ParallelPolicies::seq, std::span(species_->atoms().begin(), species_->nAtoms()),
                                pairwiseForceOperator);
    else
        dissolve::for_each_pair(ParallelPolicies::par, std::span(species_->atoms().begin(), species_->nAtoms()),
                                pairwiseForceOperator);
    combinableUnbound.finalize();
}

// Calculate pair potential forces within of Species at the specified coordinates
void SpeciesKernel::pairPotentialForces(std::vector<Vector3> &forces, const std::vector<Vector3> &r) const
{
    auto combinableUnbound = Kernel::createCombinableVector3(forces);

    auto pairwiseForceOperator = [&](int indexI, const auto &i, int indexJ, const auto &j)
    {
        if (indexI == indexJ)
            return;

        auto &&[scalingType, elec14, vdw14] = i.scaling(&j);
        if (scalingType == SpeciesAtom::ScaledInteraction::Excluded)
            return;

        // Determine final forces
        auto vecij = box_->minimumVector(r[indexI], r[indexJ]);
        auto magSq = vecij.magnitudeSq();
        if (magSq > cutoffDistanceSquared_)
            return;

        auto r = sqrt(magSq);
        vecij /= r;

        if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
            vecij *= potentialMap_.force(&i, &j, r);
        else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
            vecij *= potentialMap_.force(&i, &j, r, elec14, vdw14);

        auto &fLocal = combinableUnbound.local();
        fLocal[indexI] -= vecij;
        fLocal[indexJ] += vecij;
    };

    if (species_->nAtoms() < 100)
        dissolve::for_each_pair(ParallelPolicies::seq, std::span(species_->atoms().begin(), species_->nAtoms()),
                                pairwiseForceOperator);
    else
        dissolve::for_each_pair(ParallelPolicies::par, std::span(species_->atoms().begin(), species_->nAtoms()),
                                pairwiseForceOperator);
    combinableUnbound.finalize();
}

// Calculate geometric forces within Species
void SpeciesKernel::geometricForces(std::vector<Vector3> &forces) const
{
    // Loop over bonds
    for (const auto &b : species_->bonds())
        bondForces(b, b.i()->r(), b.j()->r(), forces);

    // Loop over angles
    for (const auto &a : species_->angles())
        angleForces(a, a.i()->r(), a.j()->r(), a.k()->r(), forces);

    // Loop over torsions
    for (const auto &t : species_->torsions())
        torsionForces(t, t.i()->r(), t.j()->r(), t.k()->r(), t.l()->r(), forces);

    // Loop over impropers
    for (const auto &imp : species_->impropers())
        improperForces(imp, imp.i()->r(), imp.j()->r(), imp.k()->r(), imp.l()->r(), forces);
}

// Calculate geometric forces within Species at the specified coordinates
void SpeciesKernel::geometricForces(std::vector<Vector3> &forces, const std::vector<Vector3> &r) const
{
    // Loop over bonds
    for (const auto &b : species_->bonds())
        bondForces(b, r[b.i()->index()], r[b.j()->index()], forces);

    // Loop over angles
    for (const auto &a : species_->angles())
        angleForces(a, r[a.i()->index()], r[a.j()->index()], r[a.k()->index()], forces);

    // Loop over torsions
    for (const auto &t : species_->torsions())
        torsionForces(t, r[t.i()->index()], r[t.j()->index()], r[t.k()->index()], r[t.l()->index()], forces);

    // Loop over impropers
    for (const auto &imp : species_->impropers())
        improperForces(imp, r[imp.i()->index()], r[imp.j()->index()], r[imp.k()->index()], r[imp.l()->index()], forces);
}

/*
 *Totals
 */

// Return total energy (interatomic and intramolecular) of Species
Kernel::EnergyResult SpeciesKernel::totalEnergy(Flags<Kernel::CalculationFlags> flags)
{
    return {flags.isSet(Kernel::ExcludeIntraMolecularPairPotential) ? 0.0 : pairPotentialEnergy(),
            flags.isSet(Kernel::ExcludeGeometric) ? 0.0 : geometricEnergy()};
}

// Calculate total forces within the specified Species
void SpeciesKernel::totalForces(std::vector<Vector3> &ppForceVector, std::vector<Vector3> &geometricForceVector,
                                Flags<Kernel::CalculationFlags> flags)
{
    // Resize and zero force arrays
    ppForceVector.resize(species_->nAtoms());
    geometricForceVector.resize(species_->nAtoms());
    std::fill(ppForceVector.begin(), ppForceVector.end(), Vector3());
    std::fill(geometricForceVector.begin(), geometricForceVector.end(), Vector3());

    // Calculate pairwise forces between atoms
    if (flags.isNotSet(Kernel::ExcludeIntraMolecularPairPotential))
        pairPotentialForces(ppForceVector);

    // Calculate geometric forces
    if (flags.isNotSet(Kernel::ExcludeGeometric))
        geometricForces(geometricForceVector);

    // Must multiply by 100.0 to convert from kJ/mol to 10J/mol (our internal units)
    std::transform(ppForceVector.begin(), ppForceVector.end(), ppForceVector.begin(), [](auto f) { return f * 100.0; });
    std::transform(geometricForceVector.begin(), geometricForceVector.end(), geometricForceVector.begin(),
                   [](auto f) { return f * 100.0; });
}

// Calculate total forces within the specified Species
void SpeciesKernel::totalForces(std::vector<Vector3> &ppForceVector, std::vector<Vector3> &geometricForceVector,
                                const std::vector<Vector3> &r, Flags<Kernel::CalculationFlags> flags)
{
    assert(species_->nAtoms() == r.size());

    // Resize and zero force arrays
    ppForceVector.resize(species_->nAtoms());
    geometricForceVector.resize(species_->nAtoms());
    std::fill(ppForceVector.begin(), ppForceVector.end(), Vector3());
    std::fill(geometricForceVector.begin(), geometricForceVector.end(), Vector3());

    // Calculate pairwise forces between atoms
    if (flags.isNotSet(Kernel::ExcludeIntraMolecularPairPotential))
        pairPotentialForces(ppForceVector, r);

    // Calculate geometric forces
    if (flags.isNotSet(Kernel::ExcludeGeometric))
        geometricForces(geometricForceVector, r);

    // Must multiply by 100.0 to convert from kJ/mol to 10J/mol (our internal units)
    std::transform(ppForceVector.begin(), ppForceVector.end(), ppForceVector.begin(), [](auto f) { return f * 100.0; });
    std::transform(geometricForceVector.begin(), geometricForceVector.end(), geometricForceVector.begin(),
                   [](auto f) { return f * 100.0; });
}
