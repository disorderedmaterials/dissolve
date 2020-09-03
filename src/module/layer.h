/*
    *** Module Layer
    *** src/module/layer.h
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
