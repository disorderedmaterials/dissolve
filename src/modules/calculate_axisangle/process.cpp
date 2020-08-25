/*
    *** Calculate Axis Angle Module - Processing
    *** src/modules/calculate_axisangle/process.cpp
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

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "modules/calculate_axisangle/axisangle.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/select.h"

// Run set-up stage
bool CalculateAxisAngleModule::setUp(Dissolve &dissolve, ProcessPool &procPool) { return true; }

// Run main processing
bool CalculateAxisAngleModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (targetConfigurations_.nItems() == 0)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Ensure any parameters in our nodes are set correctly
    const auto distanceRange = keywords_.asVec3Double("DistanceRange");
    const auto angleRange = keywords_.asVec3Double("AngleRange");
    collectDistance_->setKeyword<Vec3<double>>("RangeX", distanceRange);
    collectAngle_->setKeyword<Vec3<double>>("RangeX", angleRange);
    collectDAngle_->setKeyword<Vec3<double>>("RangeX", distanceRange);
    collectDAngle_->setKeyword<Vec3<double>>("RangeY", angleRange);
    const bool excludeSameMolecule = keywords_.asBool("ExcludeSameMolecule");
    RefList<SelectProcedureNode> sameMoleculeExclusions;
    if (excludeSameMolecule)
        sameMoleculeExclusions.append(selectA_);
    selectB_->setKeyword<RefList<SelectProcedureNode> &>("ExcludeSameMolecule", sameMoleculeExclusions);

    // Grab Configuration pointer
    auto *cfg = targetConfigurations_.firstItem();

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(cfg->processPool());

    // Execute the analysis
    if (!analyser_.execute(procPool, cfg, uniqueName(), dissolve.processingModuleData()))
        return Messenger::error("CalculateDAngle experienced problems with its analysis.\n");

    return true;
}
