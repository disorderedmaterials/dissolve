/*
    *** CalculateSDF Module - Processing
    *** src/modules/calculate_sdf/process.cpp
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
#include "modules/calculate_sdf/sdf.h"
#include "procedure/nodes/collect3d.h"
#include "procedure/nodes/process3d.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"

// Run main processing
bool CalculateSDFModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (targetConfigurations_.nItems() == 0)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Ensure any parameters in our nodes are set correctly
    const auto rangeX = keywords_.asVec3Double("RangeX");
    const auto rangeY = keywords_.asVec3Double("RangeY");
    const auto rangeZ = keywords_.asVec3Double("RangeZ");
    collectVector_->setKeyword<Vec3<double>>("RangeX", rangeX);
    collectVector_->setKeyword<Vec3<double>>("RangeY", rangeY);
    collectVector_->setKeyword<Vec3<double>>("RangeZ", rangeZ);
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
        return Messenger::error("CalculateSDF experienced problems with its analysis.\n");

    // Save data?
    if (sdfFileAndFormat_.hasValidFileAndFormat())
    {
        if (procPool.isMaster())
        {
            if (sdfFileAndFormat_.exportData(processPosition_->processedData()))
                procPool.decideTrue();
            else
            {
                procPool.decideFalse();
                return false;
            }
        }
        else if (!procPool.decision())
            return false;
    }

    return true;
}
