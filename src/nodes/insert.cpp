// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/insert.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "kernels/energy.h"
#include "math/mathFunc.h"
#include "nodes/dissolve.h"
#include <algorithm>
#include <random>
#include <variant>

InsertNode::InsertNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Target configuration to insert into", configuration_);
    addOutput("Configuration", "Modified configuration", configuration_);
    addInput("Population", "Population of the target to add", population_);
    addInput("Density", "Density at which to add the target", density_);
    addInput("Species", "Source species or molecule set to add", speciesVariant_);
    addInput("Instances", "Molecular fragment instances", instances_);

    // Options
    addOption("DensityUnits", "Units of target density", densityUnits_);
    addOption("BoxAction", "Action to take on the Box geometry / volume on addition of the species", boxAction_);
    addOption("InstantiationMethod", "Strategy for instantiation of species during insertion", instantiationMethod_);
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
std::string_view InsertNode::type() const { return "Insert"; };

// Return short summary of the node's purpose
std::string_view InsertNode::summary() const { return "Insert molecules randomly into a configuration"; };

/*
 * Data
 */

// Return enum option info for PositioningType
EnumOptions<InsertNode::BoxActionStyle> InsertNode::boxActionStyles()
{
    return EnumOptions<InsertNode::BoxActionStyle>("BoxAction", {{InsertNode::BoxActionStyle::None, "None"},
                                                                 {InsertNode::BoxActionStyle::AddVolume, "AddVolume"},
                                                                 {InsertNode::BoxActionStyle::ScaleVolume, "ScaleVolume"}});
}
EnumOptions<InsertNode::BoxActionStyle> getEnumOptions(InsertNode::BoxActionStyle) { return InsertNode::boxActionStyles(); }

// Return enum option info for InstantiationMethod
EnumOptions<InsertNode::InstantiationMethod> InsertNode::instantiationMethod()
{
    return EnumOptions<InsertNode::InstantiationMethod>("InstantiationMethod",
                                                        {{InsertNode::InstantiationMethod::Sample, "Sample"},
                                                         {InsertNode::InstantiationMethod::InstantiateAll, "InstantiateAll"}});
}
EnumOptions<InsertNode::InstantiationMethod> getEnumOptions(InsertNode::InstantiationMethod)
{
    return InsertNode::instantiationMethod();
}

/*
 * Processing
 */

// Get population totals to be added from specified MoleculeSet
std::tuple<int, int, double> InsertNode::getPopulationTotals(int population, const MoleculeSet &molecules) const
{
    auto massToBeAdded = 0.0;
    auto nPhysicalToBeAdded = 0, nTotalAtomsToBeAdded = 0;
    for (auto n = 0; n < population; ++n)
    {
        const auto *molSp = molecules.localMolecule(n).species();
        nPhysicalToBeAdded += molSp->nAtoms(AtomConstants::Presence::Physical);
        nTotalAtomsToBeAdded += molSp->nAtoms();
        massToBeAdded += molSp->mass();
    }

    return {nPhysicalToBeAdded, nTotalAtomsToBeAdded, massToBeAdded};
}

// Add volume the configuration's box to accommodate specified atoms / mass
void InsertNode::addVolume(int nAtomsToAdd, double massToAdd) const
{
    // Determine volume required
    auto requiredVolume = densityUnits_ == Units::AtomsPerAngstromUnits
                              ? nAtomsToAdd / density_.asDouble()
                              : (massToAdd / DissolveMath::Avogadro) / (density_.asDouble() / 1.0E24);

    Messenger::print("Current box volume will be increased to accommodate volume of new species.\n");

    // Get current cell volume
    auto currentVolume = configuration_->box().volume();

    Messenger::print("Density for new molecule(s) is {} {}.\n", density_.asDouble(),
                     Units::densityUnits().keyword(densityUnits_));
    Messenger::print("Required volume for new molecule(s) is {} cubic Angstroms.\n", requiredVolume);

    // If the current box has no atoms in it, absorb the current volume rather than adding to it
    if (configuration_->nAtoms() > 0)
        requiredVolume += currentVolume;
    else
        Messenger::print("Current box is empty, so new volume will be set to exactly {} cubic Angstroms.\n", requiredVolume);

    auto scaleFactors = configuration_->box().scaleFactors(requiredVolume, {scaleA_, scaleB_, scaleC_});

    // Scale existing contents
    configuration_->scaleContents(scaleFactors);

    // Scale the current Box so there is enough space for our new species
    configuration_->scaleBox(scaleFactors);

    Messenger::print("New box volume is {:e} cubic Angstroms - scale factors were ({},{},{}).\n",
                     configuration_->box().volume(), scaleFactors.x, scaleFactors.y, scaleFactors.z);
}

// Scale the configuration's box volume to accommodate specified atoms / mass
void InsertNode::scaleVolume(int nAtomsToAdd, double massToAdd) const
{
    // Determine volume required
    auto requiredVolume = densityUnits_ == Units::AtomsPerAngstromUnits
                              ? nAtomsToAdd / density_.asDouble()
                              : (massToAdd / DissolveMath::Avogadro) / (density_.asDouble() / 1.0E24);

    Messenger::print("Box volume will be set to give supplied density.\n");

    // Get volume required to hold current cell contents at the requested density
    auto existingRequiredVolume = 0.0;
    if (densityUnits_ == Units::AtomsPerAngstromUnits)
        existingRequiredVolume = configuration_->nAtoms() / density_.asDouble();
    else
        existingRequiredVolume = configuration_->atomicMass() / (density_.asDouble() / 1.0E24);
    Messenger::print("Existing contents requires volume of {} cubic Angstroms at specified density.\n", existingRequiredVolume);

    Messenger::print("Required volume for new species is {} cubic Angstroms.\n", requiredVolume);

    // Add on required volume for existing box contents
    if (configuration_->nAtoms() > 0)
        requiredVolume += existingRequiredVolume;

    auto scaleFactors = configuration_->box().scaleFactors(requiredVolume, {scaleA_, scaleB_, scaleC_});

    // Scale existing contents
    configuration_->scaleContents(scaleFactors);

    // Scale the current Box so there is enough space for our new species
    configuration_->scaleBox(scaleFactors);

    Messenger::print("Current box scaled by ({},{},{}) - new volume is {:e} cubic Angstroms.\n", scaleFactors.x, scaleFactors.y,
                     scaleFactors.z, configuration_->box().volume());
}

// Perform processing
NodeConstants::ProcessResult InsertNode::process()
{
    // Get target MoleculeSet
    MoleculeSet speciesMoleculeSet;
    auto insertFromSpecies = speciesVariant_.isAlternative(std::type_index(typeid(const Species *)));
    if (insertFromSpecies)
        speciesMoleculeSet.addMolecule(std::get<const Species *>(speciesVariant_.data));
    const MoleculeSet &targetMoleculeSet =
        insertFromSpecies ? speciesMoleculeSet : *std::get<const MoleculeSet *>(speciesVariant_.data);

    // Bool flag - do we have instances for this species
    auto hasInstances = !instances_.instances().empty();

    auto ipop = hasInstances && instantiationMethod_ == InstantiationMethod::InstantiateAll ? instances_.instances().size()
                                                                                            : population_.asInteger();
    if (ipop <= 0)
    {
        warn("Population is zero so nothing will be added.\n");
        return NodeConstants::ProcessResult::Unchanged;
    }

    // Determine total number of atoms and mass to be added
    auto [nPhysicalAtoms, nAnyAtoms, massToBeAdded] = getPopulationTotals(ipop, targetMoleculeSet);

    // Set / adjust target box volume
    switch (boxAction_)
    {
        case (InsertNode::BoxActionStyle::None):
            message("Current box geometry / volume will remain as-is.\n");
            break;
        case (InsertNode::BoxActionStyle::AddVolume):
            addVolume(nPhysicalAtoms, massToBeAdded);
            break;
        case (InsertNode::BoxActionStyle::ScaleVolume):
            scaleVolume(nPhysicalAtoms, massToBeAdded);
            break;
    }

    // Prepare random number generation in case we are inserting via random sampling
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, ipop - 1);

    Matrix3 transform;
    const auto &box = configuration_->box();
    configuration_->atoms().reserve(configuration_->atoms().size() + nAnyAtoms);
    for (auto n = 0; n < ipop; ++n)
    {
        // Add the Molecule
        auto mol = configuration_->copyMolecule(targetMoleculeSet.localMolecule(n));

        auto insertionComplete = false;

        // If we have instances, either instantiate all from current positions, or sample from them randomly and/or randomise
        // position of Molecule over the whole box
        if (hasInstances)
        {
            std::vector<Vector3> atomicCoords;
            switch (instantiationMethod_)
            {
                case InstantiationMethod::InstantiateAll:
                {
                    atomicCoords = instances_.instances()[n];
                    insertionComplete = true;
                    break;
                }
                case InstantiationMethod::Sample:
                    atomicCoords = instances_.instances()[distr(gen)];
                    break;
                default:
                    return error("Invalid instantiation method found (must be one of 'InstantialAll' or 'Sample')");
            }

            // Update molecular atomic coordinates
            for (auto i = 0; i < mol->nAtoms(); ++i)
                mol->atom(i)->setR(atomicCoords[i]);

            if (insertionComplete)
                continue;
        }

        // Randomise position of Molecule over the whole box
        auto newCentre = box.getReal({DissolveMath::random(), DissolveMath::random(), DissolveMath::random()});
        mol->setCentreOfGeometry(box, newCentre);

        // Generate and apply a random rotation matrix
        if (rotate_)
        {
            transform.createRotationXY(DissolveMath::randomPlusMinusOne() * 180.0, DissolveMath::randomPlusMinusOne() * 180.0);
            mol->transform(box, transform);
        }
    }

    message("New box density is {:e} atoms/Angstrom**3 ({} g/cm3).\n", configuration_->atomicDensity().value_or(0.0),
            configuration_->chemicalDensity().value_or(0.0));

    // We've added new content to the box, so Need to update our object relationships
    configuration_->updateObjectRelationships();

    return NodeConstants::ProcessResult::Success;
}
