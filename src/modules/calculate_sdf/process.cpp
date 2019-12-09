/*
	*** CalculateSDF Module - Processing
	*** src/modules/calculate_sdf/process.cpp
	Copyright T. Youngs 2012-2019

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

#include "modules/calculate_sdf/sdf.h"
#include "main/dissolve.h"
#include "procedure/nodes/collect3d.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"
#include "base/sysfunc.h"

// Run main processing
bool CalculateSDFModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0)
	{
		Messenger::warn("No Configuration targets for Module.\n");
		return true;
	}

	// Ensure any parameters in our nodes are set correctly
	const Vec3<double> rangeX = keywords_.asVec3Double("RangeX");
	const Vec3<double> rangeY = keywords_.asVec3Double("RangeY");
	const Vec3<double> rangeZ = keywords_.asVec3Double("RangeZ");
	collectVector_->setKeyword< Vec3<double> >("RangeX", rangeX);
	collectVector_->setKeyword< Vec3<double> >("RangeY", rangeY);
	collectVector_->setKeyword< Vec3<double> >("RangeZ", rangeZ);
	const bool excludeSameMolecule = keywords_.asBool("ExcludeSameMolecule");
	RefList<SelectProcedureNode> sameMoleculeExclusions;
	if (excludeSameMolecule) sameMoleculeExclusions.append(selectA_);
	selectB_->setKeyword< RefList<SelectProcedureNode>& >("ExcludeSameMolecule", sameMoleculeExclusions);

	// Loop over target Configurations
	for (RefListItem<Configuration>* ri = targetConfigurations_.first(); ri != NULL; ri = ri->next())
	{
		// Grab Configuration pointer
		Configuration* cfg = ri->item();

		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// Execute the analysis
		if (!analyser_.execute(procPool, cfg, uniqueName(), dissolve.processingModuleData())) return Messenger::error("CalculateSDF experienced problems with its analysis.\n");
	}

	return true;
}

