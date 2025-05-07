// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/insertBase.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/moleculeSet.h"
#include "math/mathFunc.h"

InsertNodeBase::InsertNodeBase(Graph *parentGraph) : Node(parentGraph)
{
    addInput<Configuration *>("Configuration", "Target configuration to insert into", configuration_);
    addOutput<Configuration *>("Configuration", "Modified configuration", configuration_);
    addInput<Number>("Population", "Population of the target to add", population_);
    addInput<Number>("Density", "Density at which to add the target", density_);
    addOption<Units::DensityUnits>("DensityUnits", "Units of target density", densityUnits_);

    addOption("BoxAction", "Action to take on the Box geometry / volume on addition of the species", boxAction_);
    addOption("ScaleA", "Scale box length A when modifying volume", scaleA_);
    addOption("ScaleB", "Scale box length B when modifying volume", scaleB_);
    addOption("ScaleC", "Scale box length C when modifying volume", scaleC_);
    addOption("Rotate", "Whether to randomly rotate molecules on insertion", rotate_);
}
EnumOptions<Units::DensityUnits> getEnumOptions(Units::DensityUnits) { return Units::densityUnits(); }

/*
 * Data
 */

// Return enum option info for PositioningType
EnumOptions<InsertNodeBase::BoxActionStyle> InsertNodeBase::boxActionStyles()
{
    return EnumOptions<InsertNodeBase::BoxActionStyle>("BoxAction",
                                                       {{InsertNodeBase::BoxActionStyle::None, "None"},
                                                        {InsertNodeBase::BoxActionStyle::AddVolume, "AddVolume"},
                                                        {InsertNodeBase::BoxActionStyle::ScaleVolume, "ScaleVolume"}});
}
EnumOptions<InsertNodeBase::BoxActionStyle> getEnumOptions(InsertNodeBase::BoxActionStyle)
{
    return InsertNodeBase::boxActionStyles();
}

/*
 * Common Functions
 */

// Get population totals to be added from specified MoleculeSet
std::tuple<int, int, double> InsertNodeBase::getPopulationTotals(int population, const MoleculeSet &molecules) const
{
    auto massToBeAdded = 0.0;
    auto nPhysicalToBeAdded = 0, nTotalAtomsToBeAdded = 0;
    for (auto n = 0; n < population; ++n)
    {
        const auto *molSp = molecules.localMolecule(n).species();
        nPhysicalToBeAdded += molSp->nAtoms(SpeciesAtom::Presence::Physical);
        nTotalAtomsToBeAdded += molSp->nAtoms();
        massToBeAdded += molSp->mass();
    }

    return {nPhysicalToBeAdded, nTotalAtomsToBeAdded, massToBeAdded};
}

// Adjust or set box volume ready for addition
void InsertNodeBase::adjustBoxVolume(Configuration *cfg, int nAtomsToAdd, double massToAdd) const
{
    // Determine volume required to contain the population of the specified Species at the requested density
    auto requiredVolume = 0.0;
    if (densityUnits_ == Units::AtomsPerAngstromUnits)
        requiredVolume = nAtomsToAdd / density_.asDouble();
    else
        requiredVolume = (massToAdd / DissolveMath::Avogadro) / (density_.asDouble() / 1.0E24);

    // If a density was not given, just add new molecules to the current box without adjusting its size
    if (boxAction_ == InsertNodeBase::BoxActionStyle::None)
        Messenger::print(" ... Current box geometry / volume will remain as-is.\n");
    else if (boxAction_ == InsertNodeBase::BoxActionStyle::AddVolume)
    {
        Messenger::print(" ... Current box volume will be increased to accommodate volume of new species.\n");

        // Get current cell volume
        auto currentVolume = configuration_->box()->volume();

        Messenger::print(" ... Density for new molecule(s) is {} {}.\n", density_.asDouble(),
                         Units::densityUnits().keyword(densityUnits_));
        Messenger::print(" ... Required volume for new molecule(s) is {} cubic Angstroms.\n", requiredVolume);

        // If the current box has no atoms in it, absorb the current volume rather than adding to it
        if (configuration_->nAtoms() > 0)
            requiredVolume += currentVolume;
        else
            Messenger::print(" ... Current box is empty, so new volume will be set to exact ly {} cubic Angstroms.\n",
                             requiredVolume);

        auto scaleFactors = configuration_->box()->scaleFactors(requiredVolume, {scaleA_, scaleB_, scaleC_});

        // Scale existing contents
        configuration_->scaleContents(scaleFactors);

        // Scale the current Box so there is enough space for our new species
        configuration_->scaleBox(scaleFactors);

        Messenger::print(" ... New box volume is {:e} cubic Angstroms - scale factors were ({},{},{}).\n",
                         configuration_->box()->volume(), scaleFactors.x, scaleFactors.y, scaleFactors.z);
    }
    else if (boxAction_ == InsertNodeBase::BoxActionStyle::ScaleVolume)
    {
        Messenger::print(" ... Box volume will be set to give supplied density.\n");

        // Get volume required to hold current cell contents at the requested density
        auto existingRequiredVolume = 0.0;
        if (densityUnits_ == Units::AtomsPerAngstromUnits)
            existingRequiredVolume = configuration_->nAtoms() / density_.asDouble();
        else
            existingRequiredVolume = configuration_->atomicMass() / (density_.asDouble() / 1.0E24);
        Messenger::print(" ... Existing contents requires volume of {} cubic Angstroms at specified density.\n",
                         existingRequiredVolume);

        Messenger::print(" ... Required volume for new species is {} cubic Angstroms.\n", requiredVolume);

        // Add on required volume for existing box contents
        if (configuration_->nAtoms() > 0)
            requiredVolume += existingRequiredVolume;

        auto scaleFactors = configuration_->box()->scaleFactors(requiredVolume, {scaleA_, scaleB_, scaleC_});

        // Scale existing contents
        configuration_->scaleContents(scaleFactors);

        // Scale the current Box so there is enough space for our new species
        configuration_->scaleBox(scaleFactors);

        Messenger::print(" ... Current box scaled by ({},{},{}) - new volume is {:e} cubic Angstroms.\n", scaleFactors.x,
                         scaleFactors.y, scaleFactors.z, configuration_->box()->volume());
    }
}
