/*
    *** Procedure Node - AddSpecies
    *** src/procedure/nodes/addspecies.cpp
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
    : ProcedureNode(ProcedureNode::AddSpeciesNode)
{
    // Set up keywords
    keywords_.add("Target", new SpeciesKeyword(sp), "Species", "Target species to add");
    keywords_.add("Target", new NodeValueKeyword(this, population), "Population", "Population of the target species to add");
    keywords_.add(
        "Target",
        new EnumOptionsKeyword<AddSpeciesProcedureNode::BoxActionStyle>(boxActionStyles() = AddSpeciesProcedureNode::AddVolume),
        "BoxAction", "Action to take on the Box geometry / volume on addition of the species");
    keywords_.add("Target",
                  new NodeValueEnumOptionsKeyword<Units::DensityUnits>(this, density, Units::densityUnits() = densityUnits),
                  "Density", "Density at which to add the target species");
    keywords_.add("Positioning", new BoolKeyword(true), "Rotate", "Whether to rotate molecules on insertion");
    keywords_.add("Positioning",
                  new EnumOptionsKeyword<AddSpeciesProcedureNode::PositioningType>(
                      positioningTypes() = AddSpeciesProcedureNode::RandomPositioning),
                  "Positioning", "Positioning type for individual molecules");
}

AddSpeciesProcedureNode::~AddSpeciesProcedureNode() {}

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
    static EnumOptionsList BoxActionStyleKeywords = EnumOptionsList()
                                                    << EnumOption(AddSpeciesProcedureNode::None, "None")
                                                    << EnumOption(AddSpeciesProcedureNode::AddVolume, "AddVolume")
                                                    << EnumOption(AddSpeciesProcedureNode::ScaleVolume, "ScaleVolume");

    static EnumOptions<AddSpeciesProcedureNode::BoxActionStyle> options("BoxAction", BoxActionStyleKeywords);

    return options;
}

// Return enum option info for PositioningType
EnumOptions<AddSpeciesProcedureNode::PositioningType> AddSpeciesProcedureNode::positioningTypes()
{
    static EnumOptionsList PositioningTypeKeywords = EnumOptionsList()
                                                     << EnumOption(AddSpeciesProcedureNode::CentralPositioning, "Central")
                                                     << EnumOption(AddSpeciesProcedureNode::CurrentPositioning, "Current")
                                                     << EnumOption(AddSpeciesProcedureNode::RandomPositioning, "Random");

    static EnumOptions<AddSpeciesProcedureNode::PositioningType> options("PositioningType", PositioningTypeKeywords);

    return options;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AddSpeciesProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) { return true; }

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult AddSpeciesProcedureNode::execute(ProcessPool &procPool, Configuration *cfg,
                                                                    std::string_view prefix, GenericList &targetList)
{
    const auto requestedPopulation = keywords_.asInt("Population");
    auto *sp = keywords_.retrieve<Species *>("Species");
    if (!sp)
    {
        Messenger::error("No Species set in AddSpecies node.\n");
        return ProcedureNode::Failure;
    }
    const auto nAtomsToAdd = requestedPopulation * sp->nAtoms();

    // Can't add the Species if it has any missing core information
    if (!sp->checkSetUp())
    {
        Messenger::error("Can't add Species '{}' because it is not set up correctly.\n", sp->name());
        return ProcedureNode::Failure;
    }

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
    const Box *box = cfg->box();
    for (int n = 0; n < requestedPopulation; ++n)
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

    return ProcedureNode::Success;
}
