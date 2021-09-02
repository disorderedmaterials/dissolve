// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/scaledenergykernel.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "classes/molecule.h"
#include "classes/potentialmap.h"

ScaledEnergyKernel::ScaledEnergyKernel(double interMoleculeRScale, double intraMoleculeEScale, ProcessPool &procPool,
                                       const Box *box, const CellArray &cells, const PotentialMap &potentialMap,
                                       double energyCutoff)
    : EnergyKernel(procPool, box, cells, potentialMap, energyCutoff)
{
    interMoleculeRScale_ = interMoleculeRScale;
    intraMoleculeEScale_ = intraMoleculeEScale;
}

ScaledEnergyKernel::~ScaledEnergyKernel() = default;

/*
 * Internal Routines
 */

// Return PairPotential energy between atoms provided as pointers, at the distance specified
double ScaledEnergyKernel::pairPotentialEnergy(const std::shared_ptr<Atom> &i, const std::shared_ptr<Atom> &j, double r)
{
    /*
     * Check the Molecules of the supplied Atoms - if they exist within different Molecules we scale the distance
     * between the Atoms, effectively reproducing a scaling of the positions of all Molecular centres in the Box.
     */
    if (i->molecule() != j->molecule())
    {
        // Get COG of Molecules
        const auto cogI = i->molecule()->centreOfGeometry(box_);
        const auto cogJ = j->molecule()->centreOfGeometry(box_);
        double rIJ = box_->minimumDistance(cogI, cogJ);

        return potentialMap_.energy(*i, *j, r + (rIJ * interMoleculeRScale_ - rIJ));
    }

    return potentialMap_.energy(*i, *j, r) * intraMoleculeEScale_;
}
