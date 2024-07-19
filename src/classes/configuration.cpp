// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "base/lineParser.h"
#include "base/processPool.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/cell.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"

Configuration::Configuration()
    : generator_("Generator"), globalPotentialsModel_(globalPotentials_), targetedPotentialsModel_(targetedPotentials_)
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
    requestedSizeFactor_ = std::nullopt;
    appliedSizeFactor_ = std::nullopt;

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
    auto result = generator_.execute({procedureContext, this});
    if (!result)
        return Messenger::error("Failed to generate Configuration '{}'.\n", niceName());
    Messenger::print("\n");

    // Set-up Cells for the Box
    cells_.generate(box_.get(), requestedCellDivisionLength_, procedureContext.potentialMap().range());

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

// Initialise (generate or load) the basic contents of the Configuration
bool Configuration::initialiseContent(const ProcedureContext &procedureContext)
{
    // Clear existing content
    empty();

    appliedSizeFactor_ = std::nullopt;

    // Run the generator Procedure
    if (!generate(procedureContext))
        return false;

    updateAtomLocations(true);

    // If there are still no atoms, complain.
    if (nAtoms() == 0)
        return false;

    // Create cell array
    updateCells(procedureContext.potentialMap().range());

    // Apply size factor scaling if required
    applySizeFactor(procedureContext.processPool(), procedureContext.potentialMap());

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
    if (requestedSizeFactor_.has_value())
        configuration["sizeFactor"] = requestedSizeFactor_.value();
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
