// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/insertRandom.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "kernels/energy.h"
#include "math/mathFunc.h"
#include "nodes/dissolve.h"
#include <random>

InsertRandomNode::InsertRandomNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Species", "Source species to add", species_);
    addInput("Configuration", "Target configuration to insert into", configuration_);
    addInput("Population", "Population of the target to add", population_);
    addInput("Density", "Density at which to add the target", density_);

    // Options
    addOption("DensityUnits", "Units of target density", densityUnits_);
    addOption("BoxScaling", "Action to take on the box volume on addition of the species", boxScaling_);
    addOption("ScaleA", "Scale box length A when modifying volume", scaleA_);
    addOption("ScaleB", "Scale box length B when modifying volume", scaleB_);
    addOption("ScaleC", "Scale box length C when modifying volume", scaleC_);
    addOption("Rotate", "Whether to randomly rotate molecules on insertion", rotate_);

    // Outputs
    addOutput("Configuration", "Modified configuration", configuration_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view InsertRandomNode::type() const { return "InsertRandom"; };

// Return short summary of the node's purpose
std::string_view InsertRandomNode::summary() const { return "Insert molecules randomly into a configuration"; };

/*
 * Data
 */

// Return enum option info for BoxScalingType
EnumOptions<InsertRandomNode::BoxScalingType> InsertRandomNode::boxScalingType()
{
    return EnumOptions<InsertRandomNode::BoxScalingType>("BoxScalingType",
                                                         {{InsertRandomNode::BoxScalingType::None, "None"},
                                                          {InsertRandomNode::BoxScalingType::AddVolume, "AddVolume"},
                                                          {InsertRandomNode::BoxScalingType::FixedDensity, "FixedDensity"}});
}
EnumOptions<InsertRandomNode::BoxScalingType> getEnumOptions(InsertRandomNode::BoxScalingType)
{
    return InsertRandomNode::boxScalingType();
}

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult InsertRandomNode::process()
{
    // Check population to add
    auto ipop = population_.asInteger();
    if (ipop <= 0)
    {
        warn("Population is zero so nothing will be added.\n");
        return NodeConstants::ProcessResult::Unchanged;
    }

    // Determine total number of atoms and mass to be added
    auto massToBeAdded = species_->mass() * ipop;
    auto nPhysicalToBeAdded = species_->nAtoms(AtomConstants::Presence::Physical) * ipop;
    auto nTotalAtomsToBeAdded = species_->nAtoms() * ipop;

    // Get scale factors to apply to current configuration
    Vector3 scaleFactors(1.0, 1.0, 1.0);
    switch (boxScaling_)
    {
        case (InsertRandomNode::BoxScalingType::None):
            message("No scaling will be applied to the current box.\n");
            break;
        case (InsertRandomNode::BoxScalingType::AddVolume):
            message("Volume required for {} copies of the species at a density of {} {} will be added.\n", ipop,
                    density_.asDouble(), Units::densityUnits().keyword(densityUnits_));
            scaleFactors = configuration_->getScaleFactors(nPhysicalToBeAdded, massToBeAdded, density_.asDouble(),
                                                           densityUnits_, {scaleA_, scaleB_, scaleC_});
            break;
        case (InsertRandomNode::BoxScalingType::FixedDensity):
            message("Box will be scaled to give a density of {} {} after addition of {} copies of the species.\n",
                    density_.asDouble(), Units::densityUnits().keyword(densityUnits_), ipop);
            scaleFactors = configuration_->getScaleFactors(nPhysicalToBeAdded, massToBeAdded, density_.asDouble(),
                                                           densityUnits_, {scaleA_, scaleB_, scaleC_}, true);
            break;
    }

    // Scale existing box and contents
    configuration_->scaleContents(scaleFactors);
    configuration_->scaleBox(scaleFactors);

    message("New box volume is {:e} cubic Angstroms - scale factors were ({},{},{}).\n", configuration_->box().volume(),
            scaleFactors.x, scaleFactors.y, scaleFactors.z);

    // Reserve space in the configuration for our new atoms
    const auto &box = configuration_->box();
    configuration_->atoms().reserve(configuration_->atoms().size() + nTotalAtomsToBeAdded);

    // Perform the insertion
    for (auto n = 0; n < ipop; ++n)
    {
        // Add the Molecule
        auto mol = configuration_->addMolecule(species_);

        // Randomise position of Molecule over the whole box
        auto newCentre = box.getReal({DissolveMath::random(), DissolveMath::random(), DissolveMath::random()});
        mol->setCentreOfGeometry(box, newCentre);

        // Generate and apply a random rotation matrix
        if (rotate_)
            mol->transform(box, Matrix3::createRotationXY(DissolveMath::randomPlusMinusOne() * 180.0,
                                                          DissolveMath::randomPlusMinusOne() * 180.0));
    }

    message("New box density is {:e} atoms/Angstrom**3 ({} g/cm3).\n", configuration_->atomicDensity().value_or(0.0),
            configuration_->chemicalDensity().value_or(0.0));

    // We've added new content to the box, so Need to update our object relationships
    configuration_->updateObjectRelationships();

    return NodeConstants::ProcessResult::Success;
}
