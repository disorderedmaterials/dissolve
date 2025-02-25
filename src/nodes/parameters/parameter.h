// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <string>
#include <vector>

class Parameter
{
public:
    Parameter();
    virtual ~Parameter() = default;

/*
 * Basic Definition
 */
public:
    enum ParameterDataType
    {
        None,
        Number,
        String,
        ThreeVector,
        ConfigurationPointer,
        ModulePointer
    };

protected:
    // Name of the parameter
    std::string name_;
    // Description of parameter (used as tooltip in the GUI)
    std::string description_;
    // Type of the parameter
    ParameterDataType type_{ParameterDataType::None};

    /*
     * Data
     */
    public:
    // Return whether the contained data represents the default value
    virtual bool isDefault() const = 0;

    /*
     * Node Interaction
     */
    public:
    // Get data from edge connection(s)?
    // void getDataFromEdges();
    // ...or...
    // Update value from any edge connection(s)?
    // void updateFromEdges();

    /*
     * I/O
     */
    public:
    // Express as a serialised value
    virtual SerialisedValue serialise() const = 0;
    // Read from a serialised value
    virtual void deserialise(const SerialisedValue &node) = 0;
};
