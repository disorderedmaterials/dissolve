// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/addspecies.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "keywords/types.h"

AddSpeciesProcedureNode::AddSpeciesProcedureNode(Species *sp, NodeValue population, NodeValue density,
                                                 Units::DensityUnits densityUnits)
    : ProcedureNode(ProcedureNode::NodeType::AddSpecies)
{
    // Set up keywords
    keywords_.add("Control", new SpeciesKeyword(sp), "Species", "Target species to add");
    keywords_.add("Control", new NodeValueKeyword(this, population), "Population", "Population of the target species to add");
    keywords_.add(
        "Control",
        new EnumOptionsKeyword<AddSpeciesProcedureNode::BoxActionStyle>(boxActionStyles() = AddSpeciesProcedureNode::AddVolume),
        "BoxAction", "Action to take on the Box geometry / volume on addition of the species");
    keywords_.add("Control",
                  new NodeValueEnumOptionsKeyword<Units::DensityUnits>(this, density, Units::densityUnits() = densityUnits),
                  "Density", "Density at which to add the target species");
    keywords_.add("Control", new BoolKeyword(true), "Rotate", "Whether to randomly rotate molecules on insertion");
    keywords_.add("Control",
                  new EnumOptionsKeyword<AddSpeciesProcedureNode::PositioningType>(
                      positioningTypes() = AddSpeciesProcedureNode::RandomPositioning),
                  "Positioning", "Positioning type for individual molecules");
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool AddSpeciesProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::GenerationContext);
}

// Return whether a name for the node must be provided
bool AddSpeciesProcedureNode::mustBeNamed() const { return false; }

/*
 * Node Data
 */

// Return enum option info for PositioningType
EnumOptions<AddSpeciesProcedureNode::BoxActionStyle> AddSpeciesProcedureNode::boxActionStyles()
{
    return EnumOptions<AddSpeciesProcedureNode::BoxActionStyle>("BoxAction",
                                                                {{AddSpeciesProcedureNode::None, "None"},
                                                                 {AddSpeciesProcedureNode::AddVolume, "AddVolume"},
                                                                 {AddSpeciesProcedureNode::ScaleVolume, "ScaleVolume"}});
}

// Return enum option info for PositioningType
EnumOptions<AddSpeciesProcedureNode::PositioningType> AddSpeciesProcedureNode::positioningTypes()
{
    return EnumOptions<AddSpeciesProcedureNode::PositioningType>("PositioningType",
                                                                 {{AddSpeciesProcedureNode::CentralPositioning, "Central"},
                                                                  {AddSpeciesProcedureNode::CurrentPositioning, "Current"},
                                                                  {AddSpeciesProcedureNode::RandomPositioning, "Random"}});
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AddSpeciesProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) { return true; }

// Execute node, targetting the supplied Configuration
bool AddSpeciesProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                      GenericList &targetList)
{
    const auto requestedPopulation = keywords_.asInt("Population");
    auto *sp = keywords_.retrieve<Species *>("Species");
    if (!sp)
        return Messenger::error("No Species set in AddSpecies node.\n");
    const auto nAtomsToAdd = requestedPopulation * sp->nAtoms();

    // Can't add the Species if it has any missing core information
    if (!sp->checkSetUp())
        return Messenger::error("Can't add Species '{}' because it is not set up correctly.\n", sp->name());

    Messenger::print("[AddSpecies] Adding species '{}' - population is {}.\n", sp->name(), requestedPopulation);

    // If a density was not given, just add new molecules to the current box without adjusting its size
    auto &densityAndUnits = keywords_.retrieve<Venum<NodeValue, Units::DensityUnits>>("Density");
    double density = densityAndUnits.value().asDouble();
    auto boxAction = keywords_.enumeration<AddSpeciesProcedureNode::BoxActionStyle>("BoxAction");
    if (boxAction == AddSpeciesProcedureNode::None)
        Messenger::print("[AddSpecies] Current box geometry / volume will remain as-is.\n");
    else if (boxAction == AddSpeciesProcedureNode::AddVolume)
    {
        Messenger::print("[AddSpecies] Current box volume will be increased to accommodate volume of new species.\n");

        // Get current cell volume
        double currentVolume = cfg->box()->volume();

        // Determine volume required to contain the population of the specified Species at the requested density
        double requiredVolume = 0.0;
        if (densityAndUnits.enumeration() == Units::AtomsPerAngstromUnits)
            requiredVolume = nAtomsToAdd / density;
        else
            requiredVolume = ((sp->mass() * requestedPopulation) / AVOGADRO) / (density / 1.0E24);

        Messenger::print("[AddSpecies] Density for new species is {} {}.\n", density,
                         Units::densityUnits().keyword(densityAndUnits.enumeration()));
        Messenger::print("[AddSpecies] Required volume for new species is {} cubic Angstroms.\n", requiredVolume);

        // If the current box has no atoms in it, absorb the current volume rather than adding to it
        if (cfg->nAtoms() > 0)
            requiredVolume += currentVolume;
        else
            Messenger::print("[AddSpecies] Current box is empty, so new volume will be set to exactly {} cubic Angstroms.\n",
                             requiredVolume);

        double scaleFactor = pow(requiredVolume / currentVolume, 1.0 / 3.0);

        // Scale centres of geometry of existing contents
        cfg->scaleMoleculeCentres(scaleFactor);

        // Scale the current Box so there is enough space for our new species
        cfg->scaleBox(scaleFactor);

        Messenger::print("[AddSpecies] New box volume is {:e} cubic Angstroms (scale factor was {}).\n", cfg->box()->volume(),
                         scaleFactor);
    }
    else if (boxAction == AddSpeciesProcedureNode::ScaleVolume)
    {
        Messenger::print("[AddSpecies] Box volume will be set to give supplied density.\n");

        // Get volume required to hold current cell contents at the requested density
        double existingRequiredVolume = 0.0;
        if (densityAndUnits.enumeration() == Units::AtomsPerAngstromUnits)
            existingRequiredVolume = cfg->nAtoms() / density;
        else
            existingRequiredVolume = cfg->atomicMass() / (density / 1.0E24);
        Messenger::print("[AddSpecies] Existing contents requires volume of {} cubic Angstroms at specified density.\n",
                         existingRequiredVolume);

        // Determine volume required to contain the population of the specified Species at the requested density
        double requiredVolume = 0.0;
        if (densityAndUnits.enumeration() == Units::AtomsPerAngstromUnits)
            requiredVolume = nAtomsToAdd / density;
        else
            requiredVolume = ((sp->mass() * requestedPopulation) / AVOGADRO) / (density / 1.0E24);

        Messenger::print("[AddSpecies] Required volume for new species is {} cubic Angstroms.\n", requiredVolume);

        // Add on required volume for existing box contents
        if (cfg->nAtoms() > 0)
            requiredVolume += existingRequiredVolume;

        double scaleFactor = pow(requiredVolume / cfg->box()->volume(), 1.0 / 3.0);

        // Scale centres of geometry of existing contents
        cfg->scaleMoleculeCentres(scaleFactor);

        // Scale the current Box so there is enough space for our new species
        cfg->scaleBox(scaleFactor);

        Messenger::print("[AddSpecies] Current box scaled by {} - new volume is {:e} cubic Angstroms.\n", scaleFactor,
                         cfg->box()->volume());
    }

    // Get the positioning type and rotation flag
    auto positioning = keywords_.enumeration<AddSpeciesProcedureNode::PositioningType>("Positioning");
    auto rotate = keywords_.asBool("Rotate");

    Messenger::print("[AddSpecies] Positioning type is '{}' and rotation is {}.\n",
                     AddSpeciesProcedureNode::positioningTypes().keyword(positioning), rotate ? "on" : "off");

    // Now we add the molecules
    procPool.initialiseRandomBuffer(ProcessPool::PoolProcessesCommunicator);
    Vec3<double> r, cog, newCentre, fr;
    CoordinateSet *coordSet = sp->coordinateSets().first();
    Matrix3 transform;
    const auto *box = cfg->box();
    for (auto n = 0; n < requestedPopulation; ++n)
    {
        // Add the Molecule
        std::shared_ptr<Molecule> mol = cfg->addMolecule(sp, coordSet);

        // Move to next coordinate set if available
        if (coordSet)
        {
            coordSet = coordSet->next();
            if (!coordSet)
                coordSet = sp->coordinateSets().first();
        }

        // Set / generate position of Molecule
        switch (positioning)
        {
            case (AddSpeciesProcedureNode::RandomPositioning):
                fr.set(procPool.random(), procPool.random(), procPool.random());
                newCentre = box->fracToReal(fr);
                mol->setCentreOfGeometry(box, newCentre);
                break;
            case (AddSpeciesProcedureNode::CentralPositioning):
                fr.set(0.5, 0.5, 0.5);
                newCentre = box->fracToReal(fr);
                mol->setCentreOfGeometry(box, newCentre);
                break;
            case (AddSpeciesProcedureNode::CurrentPositioning):
                break;
            default:
                Messenger::error("Unrecognised positioning type.\n");
                break;
        }

        // Generate and apply a random rotation matrix
        if (rotate)
        {
            transform.createRotationXY(procPool.randomPlusMinusOne() * 180.0, procPool.randomPlusMinusOne() * 180.0);
            mol->transform(box, transform);
        }
    }

    Messenger::print("[AddSpecies] New box density is {:e} cubic Angstroms ({} g/cm3).\n", cfg->atomicDensity(),
                     cfg->chemicalDensity());

    return true;
}
