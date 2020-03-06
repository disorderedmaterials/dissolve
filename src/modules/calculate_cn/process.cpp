/*
	*** CalculateCN Module - Processing
	*** src/modules/calculate_cn/process.cpp
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

#include "modules/calculate_cn/cn.h"
#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/integrate1d.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "main/dissolve.h"
#include "base/sysfunc.h"

// Run main processing
bool CalculateCNModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	// Check for valid CalculateRDF pointer
	bool found = false;
	const CalculateRDFModule* rdfModule = keywords_.retrieve<const CalculateRDFModule*>("SourceRDF", NULL, &found);
	if ((!found) || (!rdfModule)) return Messenger::error("No suitable CalculateRDF target set for CalculateCN.\n");

	// Set the target Collect1D and normalisation nodes in the Process1D
	process1D_->setKeyword<const Collect1DProcedureNode*>("SourceData", rdfModule->collectDistanceNode());
	RefList<const SelectProcedureNode> siteNodes(rdfModule->selectANode());
	siteNormaliser_->setKeyword< RefList<const SelectProcedureNode>& >("Site", siteNodes);

	// Execute the analysis on the Configurations targeted by the RDF module
	for (Configuration* cfg : rdfModule->targetConfigurations())
	{
		if (!analyser_.execute(procPool, cfg, CharString("%s//Analyser", uniqueName()), dissolve.processingModuleData())) return Messenger::error("CalculateCN experienced problems with its analysis.\n");
	}

	// Test?
	const double testThreshold = keywords_.asDouble("TestThreshold");
	if (keywords_.isSet("TestRangeA"))
	{
		const double delta = keywords_.asDouble("TestRangeA") - coordinationNumber(0).value();

		Messenger::print("Reference coordination number delta with correct value for range A is %15.9e and is %s (threshold is %10.3e)\n", delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
		if (!procPool.allTrue(fabs(delta) < testThreshold)) return false;
	}
	if (keywords_.isSet("TestRangeB"))
	{
		// Is range B enabled?
		if (!isRangeBEnabled()) return Messenger::error("Test coordination number for range B supplied, but calculation for that range is not active.\n");

		const double delta = keywords_.asDouble("TestRangeB") - coordinationNumber(1).value();

		Messenger::print("Reference coordination number delta with correct value for range B is %15.9e and is %s (threshold is %10.3e)\n", delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
		if (!procPool.allTrue(fabs(delta) < testThreshold)) return false;
	}
	if (keywords_.isSet("TestRangeC"))
	{
		// Is range B enabled?
		if (!isRangeBEnabled()) return Messenger::error("Test coordination number for range C supplied, but calculation for that range is not active.\n");

		const double delta = keywords_.asDouble("TestRangeC") - coordinationNumber(2).value();

		Messenger::print("Reference coordination number delta with correct value for range C is %15.9e and is %s (threshold is %10.3e)\n", delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
		if (!procPool.allTrue(fabs(delta) < testThreshold)) return false;
	}

	return true;
}
