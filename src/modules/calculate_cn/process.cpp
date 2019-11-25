/*
	*** CalculateCN Module - Processing
	*** src/modules/calculate_cn/process.cpp
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
	RefListIterator<Configuration> configIterator(rdfModule->targetConfigurations());
	while (Configuration* cfg = configIterator.iterate())
	{
		if (!analyser_.execute(procPool, cfg, CharString("%s//Analyser", uniqueName()), dissolve.processingModuleData())) return Messenger::error("CalculateCN experienced problems with its analysis.\n");
	}

	return true;
}
