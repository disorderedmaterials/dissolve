// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "module/list.h"
#include "templates/listitem.h"

// Forward Declarations
class Module;
class GenericList;

// Module Layer - Named list of Modules with a defined processing frequency
class ModuleLayer : public ModuleList, public ListItem<ModuleLayer>
{
    public:
    ModuleLayer();
    ~ModuleLayer();

    /*
     * Layer Definition
     */
    private:
    // Name of layer
    std::string name_;
    // Whether the layer is enabled
    bool enabled_;
    // Frequency, relative to the main iteration counter, at which to execute the layer
    int frequency_;

    public:
    // Set name of layer
    void setName(std::string_view name);
    // Return name of layer
    std::string_view name() const;
    // Set whether the layer is enabled
    void setEnabled(bool enabled);
    // Return whether the layer is enabled
    bool enabled() const;
    // Frequency, relative to the main iteration counter, at which to execute the layer
    void setFrequency(int frequency);
    // Return frequency, relative to the main iteration counter, at which to execute the layer
    int frequency() const;
    // Return short descriptive text relating frequency to supplied iteration number
    std::string frequencyDetails(int iteration) const;
    // Return whether the layer should execute this iteration
    bool runThisIteration(int iteration) const;
};
