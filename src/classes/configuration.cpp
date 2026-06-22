// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/configuration.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/cell.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "main/dissolve.h"

Configuration::Configuration() : box_(Box::BoxType::Cubic, {1.0, 1.0, 1.0}, {90.0, 90.0, 90.0}) {}

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
    version_.zero();

    // Reset definition
    temperature_ = 300.0;
}

/*
 * Definition
 */

// Set name of the Configuration
void Configuration::setName(std::string_view name) { name_ = DissolveSys::niceName(name); }

// Return name of the Configuration
std::string_view Configuration::name() const { return name_; }

// Set configuration temperature
void Configuration::setTemperature(double t) { temperature_ = t; }

// Return configuration temperature
double Configuration::temperature() const { return temperature_; }

// Express as a serialisable value
void Configuration::serialise(std::string tag, SerialisedValue &target) const
{
    auto &configuration = target[tag];

    if (requestedCellDivisionLength_ != defaultCellDivisionLength_)
        configuration["cellDivisionLength"] = requestedCellDivisionLength_;
    if (requestedSizeFactor_.has_value())
        configuration["sizeFactor"] = requestedSizeFactor_.value();
    if (temperature_ != defaultTemperature_)
        configuration["temperature"] = temperature_;
}

// Read values from a serialisable value
void Configuration::deserialise(const SerialisedValue &node, const CoreData &data)
{
    setTemperature(toml::find_or<double>(node, "temperature", defaultTemperature_));
    requestedSizeFactor_ = toml::find_or<double>(node, "sizeFactor", defaultSizeFactor_);
    requestedCellDivisionLength_ = toml::find_or<double>(node, "cellDivisionLength", defaultCellDivisionLength_);
}
