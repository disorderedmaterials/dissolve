/*
    *** Calculate Distance-Angle Module - Core
    *** src/modules/calculate_dangle/core.cpp
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

#include "modules/calculate_dangle/dangle.h"

/*
 * Constructor / Destructor
 */

CalculateDAngleModule::CalculateDAngleModule() : Module(), analyser_(ProcedureNode::AnalysisContext)
{
    // Set unique name for this instance of the Module
    static int instanceId = 0;
    uniqueName_ = fmt::format("{}{:02d}", type(), instanceId++);

    // Initialise Module - set up keywords etc.
    initialise();
}

CalculateDAngleModule::~CalculateDAngleModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *CalculateDAngleModule::createInstance() const { return new CalculateDAngleModule; }
