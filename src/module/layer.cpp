/*
    *** Module Layer
    *** src/module/layer.cpp
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

#include "module/layer.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "module/module.h"

ModuleLayer::ModuleLayer() : ModuleList(), ListItem<ModuleLayer>()
{
    enabled_ = true;
    frequency_ = 1;
    name_ = "Untitled Layer";
}

ModuleLayer::~ModuleLayer() {}

/*
 * Layer Definition
 */

// Set name of layer
void ModuleLayer::setName(std::string_view name) { name_ = name; }

// Return name of layer
std::string_view ModuleLayer::name() const { return name_; }

// Set whether the layer is enabled
void ModuleLayer::setEnabled(bool enabled) { enabled_ = enabled; }

// Return whether the layer is enabled
bool ModuleLayer::enabled() const { return enabled_; }

// Frequency, relative to the main iteration counter, at which to execute the layer
void ModuleLayer::setFrequency(int frequency) { frequency_ = frequency; }

// Return frequency, relative to the main iteration counter, at which to execute the layer
int ModuleLayer::frequency() const { return frequency_; }

// Return short descriptive text relating frequency to supplied iteration number
std::string ModuleLayer::frequencyDetails(int iteration) const
{
    // Check edge cases
    if (frequency_ < 0)
        return "NEGATIVE?";
    else if ((!enabled_) || (frequency_ == 0))
        return "disabled";
    else if (frequency_ == 1)
        return "every time";
    else if ((iteration % frequency_) == 0)
        return "this iteration";

    // Calculate number of steps necessary to get to next multiple of the frequency_
    auto nToGo = frequency_ - (iteration - frequency_ * (iteration / frequency_));
    if (nToGo == 1)
        return "next iteration";

    return fmt::format("in {} steps time", nToGo);
}

// Return whether the layer should execute this iteration
bool ModuleLayer::runThisIteration(int iteration) const
{
    if ((frequency_ < 1) || (!enabled_))
        return false;
    else if ((iteration % frequency_) == 0)
        return true;
    else
        return false;
}
