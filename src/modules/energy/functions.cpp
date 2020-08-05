/*
    *** Energy Module - Functions
    *** src/modules/energy/functions.cpp
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

#include "classes/configuration.h"
#include "classes/energykernel.h"
#include "classes/potentialmap.h"
#include "classes/species.h"
#include "genericitems/listhelper.h"
#include "modules/energy/energy.h"

// Return total interatomic energy of Configuration
double EnergyModule::interAtomicEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap)
{
    /*
     * Calculates the total interatomic energy of the system, i.e. the energy contributions from PairPotential
     * interactions between individual Atoms, accounting for intramolecular terms
     *
     * This is a parallel routine, with processes operating as process groups.
     */

    // Create an EnergyKernel
    EnergyKernel kernel(procPool, cfg, potentialMap);

    // Set the strategy
    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

    // Grab the Cell array and calculate total energy
    const CellArray &cellArray = cfg->cells();
    double totalEnergy = kernel.energy(cellArray, false, strategy, false);

    // Print process-local energy
    Messenger::printVerbose("Interatomic Energy (Local) is %15.9e\n", totalEnergy);

    // Sum energy over all processes in the pool and print
    procPool.allSum(&totalEnergy, 1, strategy);
    Messenger::printVerbose("Interatomic Energy (World) is %15.9e\n", totalEnergy);

    return totalEnergy;
}

// Return total interatomic energy of Species
double EnergyModule::interAtomicEnergy(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap)
{
    double r, angle;
    SpeciesAtom *i, *j;
    Vec3<double> rI;
    double scale, energy = 0.0;
    const auto cutoff = potentialMap.range();

    // Get start/end for loop
    auto loopStart = procPool.twoBodyLoopStart(sp->nAtoms());
    auto loopEnd = procPool.twoBodyLoopEnd(sp->nAtoms());

    // Double loop over species atoms
    for (auto indexI = loopStart; indexI <= loopEnd; ++indexI)
    {
        i = sp->atom(indexI);
        rI = i->r();

        for (int indexJ = indexI + 1; indexJ < sp->nAtoms(); ++indexJ)
        {
            j = sp->atom(indexJ);

            // Get interatomic distance
            r = (j->r() - rI).magnitude();
            if (r > cutoff)
                continue;

            // Get intramolecular scaling of atom pair
            scale = i->scaling(j);
            if (scale < 1.0e-3)
                continue;

            energy += potentialMap.energy(i, j, r) * scale;
        }
    }

    return energy;
}

// Return total intermolecular energy of Configuration
double EnergyModule::interMolecularEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap)
{
    /*
     * Calculates the total intermolecular energy of the system, i.e. the energy contributions from PairPotential
     * interactions between individual Atoms of different Molecules, thus neglecting intramolecular terms
     *
     * This is a parallel routine, with processes operating as process groups.
     */

    // Create an EnergyKernel
    EnergyKernel kernel(procPool, cfg, potentialMap);

    // Set the strategy
    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

    // Grab the Cell array and calculate total energy
    const CellArray &cellArray = cfg->cells();
    double totalEnergy = kernel.energy(cellArray, true, strategy, false);

    // Print process-local energy
    Messenger::printVerbose("Intermolecular Energy (Local) is %15.9e\n", totalEnergy);

    // Sum energy over all processes in the pool and print
    procPool.allSum(&totalEnergy, 1, strategy);
    Messenger::printVerbose("Intermolecular Energy (World) is %15.9e\n", totalEnergy);

    return totalEnergy;
}

// Return total intramolecular energy of Configuration
double EnergyModule::intraMolecularEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap)
{
    double bondEnergy, angleEnergy, torsionEnergy;

    return intraMolecularEnergy(procPool, cfg, potentialMap, bondEnergy, angleEnergy, torsionEnergy);
}

// Return total intramolecular energy of Configuration, storing components in provided variables
double EnergyModule::intraMolecularEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                          double &bondEnergy, double &angleEnergy, double &torsionEnergy)
{
    /*
     * Calculate the total intramolecular energy of the system, arising from Bond, Angle, and Torsion
     * terms in all Molecules.
     *
     * This is a parallel routine, with processes operating as a standard world group.
     */

    // Create an EnergyKernel
    EnergyKernel kernel(procPool, cfg, potentialMap);

    bondEnergy = 0;
    angleEnergy = 0;
    torsionEnergy = 0;

    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

    // Set start/stride for parallel loop
    auto start = procPool.interleavedLoopStart(strategy);
    auto stride = procPool.interleavedLoopStride(strategy);

    std::deque<std::shared_ptr<Molecule>> molecules = cfg->molecules();
    std::shared_ptr<const Molecule> mol;
    for (int m = start; m < cfg->nMolecules(); m += stride)
    {
        // Get Molecule pointer
        mol = molecules[m];

        // Loop over Bonds
        DynamicArrayConstIterator<SpeciesBond> bondIterator(mol->species()->constBonds());
        while (const SpeciesBond *b = bondIterator.iterate())
            bondEnergy += kernel.energy(b, mol->atom(b->indexI()), mol->atom(b->indexJ()));

        // Loop over Angles
        DynamicArrayConstIterator<SpeciesAngle> angleIterator(mol->species()->constAngles());
        while (const SpeciesAngle *a = angleIterator.iterate())
            angleEnergy += kernel.energy(a, mol->atom(a->indexI()), mol->atom(a->indexJ()), mol->atom(a->indexK()));

        // Loop over Torsions
        DynamicArrayConstIterator<SpeciesTorsion> torsionIterator(mol->species()->constTorsions());
        while (const SpeciesTorsion *t = torsionIterator.iterate())
            torsionEnergy += kernel.energy(t, mol->atom(t->indexI()), mol->atom(t->indexJ()), mol->atom(t->indexK()),
                                           mol->atom(t->indexL()));
    }

    double totalIntra = bondEnergy + angleEnergy + torsionEnergy;

    Messenger::printVerbose("Intramolecular Energy (Local) is %15.9e kJ/mol (%15.9e bond + %15.9e angle + %15.9e torsion)\n",
                            totalIntra, bondEnergy, angleEnergy, torsionEnergy);

    // Sum energy and print
    double values[3];
    values[0] = bondEnergy;
    values[1] = angleEnergy;
    values[2] = torsionEnergy;
    procPool.allSum(values, 3, strategy);
    bondEnergy = values[0];
    angleEnergy = values[1];
    torsionEnergy = values[2];

    totalIntra = bondEnergy + angleEnergy + torsionEnergy;

    Messenger::printVerbose("Intramolecular Energy (World) is %15.9e kJ/mol (%15.9e bond + %15.9e angle + %15.9e torsion)\n",
                            totalIntra, bondEnergy, angleEnergy, torsionEnergy);

    return totalIntra;
}

// Return total intramolecular energy of Species
double EnergyModule::intraMolecularEnergy(ProcessPool &procPool, Species *sp)
{
    auto energy = 0.0;

    // Loop over bonds
    DynamicArrayConstIterator<SpeciesBond> bondIterator(sp->constBonds());
    while (const SpeciesBond *b = bondIterator.iterate())
        energy += EnergyKernel::energy(b);

    // Loop over angles
    DynamicArrayConstIterator<SpeciesAngle> angleIterator(sp->constAngles());
    while (const SpeciesAngle *a = angleIterator.iterate())
        energy += EnergyKernel::energy(a);

    // Loop over torsions
    DynamicArrayConstIterator<SpeciesTorsion> torsionIterator(sp->constTorsions());
    while (const SpeciesTorsion *t = torsionIterator.iterate())
        energy += EnergyKernel::energy(t);

    return energy;
}

// Return total energy (interatomic and intramolecular) of Configuration
double EnergyModule::totalEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap)
{
    return (interAtomicEnergy(procPool, cfg, potentialMap) + intraMolecularEnergy(procPool, cfg, potentialMap));
}

// Return total energy (interatomic and intramolecular) of Configuration, storing components in provided variables
double EnergyModule::totalEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                 double &interEnergy, double &bondEnergy, double &angleEnergy, double &torsionEnergy)
{
    interEnergy = interAtomicEnergy(procPool, cfg, potentialMap);
    intraMolecularEnergy(procPool, cfg, potentialMap, bondEnergy, angleEnergy, torsionEnergy);

    return interEnergy + bondEnergy + angleEnergy + torsionEnergy;
}

// Return total energy (interatomic and intramolecular) of Species
double EnergyModule::totalEnergy(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap)
{
    return (interAtomicEnergy(procPool, sp, potentialMap) + intraMolecularEnergy(procPool, sp));
}

// Check energy stability of specified Configuration
EnergyModule::EnergyStability EnergyModule::checkStability(Configuration *cfg)
{
    // First, check if the Configuration is targetted by an EnergyModule
    if (!GenericListHelper<bool>::value(cfg->moduleData(), "_IsEnergyModuleTarget", NULL, false))
    {
        Messenger::error("Configuration '%s' is not targeted by any EnergyModule, so stability cannot be assessed. "
                         "Check your setup!\n",
                         cfg->name());
        return EnergyModule::NotAssessable;
    }

    // Retrieve the EnergyStable flag from the Configuration's module data
    if (cfg->moduleData().contains("EnergyStable"))
    {
        auto stable = GenericListHelper<bool>::value(cfg->moduleData(), "EnergyStable");
        if (!stable)
        {
            Messenger::print("Energy for Configuration '%s' is not yet stable.\n", cfg->name());
            return EnergyModule::EnergyUnstable;
        }
    }
    else
    {
        Messenger::warn("No energy stability information is present in Configuration '%s' (yet) - check your setup.\n",
                        cfg->name());
        return EnergyModule::NotAssessable;
    }

    return EnergyModule::EnergyStable;
}

// Check energy stability of specified Configurations, returning the number that failed
int EnergyModule::nUnstable(const RefList<Configuration> &configurations)
{
    auto nFailed = 0;

    for (Configuration *cfg : configurations)
    {
        // Check the stability of this Configuration
        auto result = checkStability(cfg);

        if (result == EnergyModule::EnergyStable)
            ++nFailed;
        else if (result == EnergyModule::NotAssessable)
            return EnergyModule::NotAssessable;
    }

    return nFailed;
}
