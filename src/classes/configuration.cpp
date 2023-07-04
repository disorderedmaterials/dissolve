// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/configuration.h"
#include "base/lineParser.h"
#include "base/processPool.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/cell.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "modules/energy/energy.h"

Configuration::Configuration() : generator_(ProcedureNode::GenerationContext, "Generator")
{
    createBox({1.0, 1.0, 1.0}, {90, 90, 90}, false);
}

Configuration::~Configuration() { clear(); }

// Clear all data
void Configuration::clear()
{
    // Clear generated content
    empty();

    // Reset size factors
    requestedSizeFactor_ = 1.0;
    appliedSizeFactor_ = 1.0;

    // Reset box / Cells
    requestedCellDivisionLength_ = 7.0;
    contentsVersion_.zero();

    // Reset definition
    temperature_ = 300.0;
    generator_.clear();
}

/*
 * Definition
 */

// Set name of the Configuration
void Configuration::setName(std::string_view name)
{
    name_ = name;

    // Generate a nice name (i.e. no spaces, slashes etc.)
    niceName_ = DissolveSys::niceName(name_);
}

// Return name of the Configuration
std::string_view Configuration::name() const { return name_; }

// Return nice name of the Configuration
std::string_view Configuration::niceName() const { return niceName_; }

// Return the current generator
Procedure &Configuration::generator() { return generator_; }

// Create the Configuration according to its generator Procedure
bool Configuration::generate(const ProcedureContext &procedureContext)
{
    // Empty the current contents
    empty();

    // Generate the contents
    Messenger::print("\nExecuting generator procedure for Configuration '{}'...\n\n", niceName());
    auto context = procedureContext;
    context.setConfiguration(this);
    auto result = generator_.execute(context);
    if (!result)
        return Messenger::error("Failed to generate Configuration '{}'.\n", niceName());
    Messenger::print("\n");

    // Set-up Cells for the Box
    cells_.generate(box_.get(), requestedCellDivisionLength_, context.potentialMap().range());

    // Make sure all objects know about each other
    updateObjectRelationships();

    // Finalise used AtomType list
    atomTypes_.finalise();

    // Link targeted potentials to atoms
    linkTargetedPotentials();

    ++contentsVersion_;

    // Sanity check the contents - if we have zero atoms then there's a problem!
    if (nAtoms() == 0)
        return Messenger::error("Generated contents for Configuration '{}' contains no atoms!\n", name());

    return true;
}

// Return import coordinates file / format
CoordinateImportFileFormat &Configuration::inputCoordinates() { return inputCoordinates_; }

// Initialise (generate or load) the basic contents of the Configuration
bool Configuration::initialiseContent(const ProcedureContext &procedureContext)
{
    // Clear existing content
    empty();

    appliedSizeFactor_ = 1.0;

    // Run the generator Procedure and potentially load coordinates from file
    // Run the generator procedure (we will need species / atom info to load any coordinates in)
    if (!generate(procedureContext))
        return false;

    // If there are still no atoms, complain.
    if (nAtoms() == 0)
        return false;

    // If an input file was specified, try to load it
    if (inputCoordinates_.hasFilename())
    {
        if (DissolveSys::fileExists(inputCoordinates_.filename()))
        {
            Messenger::print("Loading initial coordinates from file '{}'...\n", inputCoordinates_.filename());
            if (!inputCoordinates_.importData(this, &procedureContext.processPool()))
                return false;

            // Need to update cell locations now, as we have new atom positions
            updateAtomLocations(true);
        }
        else
            return Messenger::error("Input coordinates file '{}' specified for Configuration '{}', but the "
                                    "file doesn't exist.\n",
                                    name(), inputCoordinates_.filename());
    }

    // Create cell array
    updateCells(procedureContext.potentialMap().range());

    return true;
}

// Set configuration temperature
void Configuration::setTemperature(double t) { temperature_ = t; }

// Return configuration temperature
double Configuration::temperature() const { return temperature_; }

// Express as a serialisable value
SerialisedValue Configuration::serialise() const
{
    SerialisedValue configuration;

    if (requestedCellDivisionLength_ != defaultCellDivisionLength_)
        configuration["cellDivisionLength"] = requestedCellDivisionLength_;
    if (requestedSizeFactor_ != defaultSizeFactor_)
        configuration["sizeFactor"] = requestedSizeFactor_;
    if (temperature_ != defaultTemperature_)
        configuration["temperature"] = temperature_;

    configuration["generator"] = generator_;

    return configuration;
}

// Read values from a serialisable value
void Configuration::deserialise(const SerialisedValue &node, const CoreData &data)
{
    setTemperature(toml::find_or<double>(node, "temperature", defaultTemperature_));
    requestedSizeFactor_ = toml::find_or<double>(node, "sizeFactor", defaultSizeFactor_);
    requestedCellDivisionLength_ = toml::find_or<double>(node, "cellDivisionLength", defaultCellDivisionLength_);
    generator_.deserialise(node.at("generator"), data);
}
