/*
	*** Calculate RDF Module - Processing
	*** src/modules/calculate/rdf/process.cpp
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

#include "modules/calculate/rdf/rdf.h"
#include "main/dissolve.h"
#include "procedure/nodes/calculate.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/select.h"
#include "base/sysfunc.h"

// Run set-up stage
bool CalculateRDFModule::setUp(Dissolve& dissolve, ProcessPool& procPool)
{
	// Reconstruct the Analyser
	analyser_.clear();

	// Get relevant Module options
	const double binWidth = keywords_.asDouble("BinWidth");
	const bool excludeSameMolecule = keywords_.asBool("ExcludeSameMolecule");
	const double rMax = keywords_.asDouble("RMax");
	const double rMin = keywords_.asDouble("RMin");
	const bool saveData = keywords_.asBool("Save");

	// Check for sites being defined
	if (originSites_.nItems() == 0) return Messenger::error("At least one origin Site must be defined.\n");
	if (otherSites_.nItems() == 0) return Messenger::error("At least one other Site must be defined.\n");

	/*
	 * Assemble the code below (@var indicates local variable 'var')
	 *
	 * Select  'A'
	 *   Site  @originSites_
	 *   ForEach
	 *     Select  'B'
	 *       Site  @otherSites_
	 *       ExcludeSameSite  'A'
	 *       ExcludeSameMolecule  (if @excludeSameMolecule then 'A')
	 *       ForEach
	 *         Calculate  'rAB'
	 *           Distance  'A'  'B'
	 *         EndCalculate
	 *         Collect1D  @resultName
	 *           QuantityX  'rAB'
	 *           RangeX  @rMin  @rMax  @binWidth
	 *         EndCollect1D
	 *       EndForEach  'B'
	 *     EndSelect  'B'
	 *   EndForEach  'A'
	 * EndSelect  'A'
	 * Process1D  @resultName
	 *   NSites  'A'
	 *   SphericalShellVolume  On
	 *   NumberDensity  'B'
	 *   LabelValue  'g(r)'
	 *   LabelX  'r, Angstroms'
	 * EndProcess1D
	 */

	// Select: Site 'A' (@originSites)
	SelectProcedureNode* originSelect = new SelectProcedureNode(originSites_);
	originSelect->setName("A");
	analyser_.addRootSequenceNode(originSelect);

	// -- Select: Site 'B' (@otherSite)
	SelectProcedureNode* otherSelect = new SelectProcedureNode(otherSites_);
	otherSelect->setName("B");
	otherSelect->addSameSiteExclusion(originSelect);
	if (excludeSameMolecule) otherSelect->addSameMoleculeExclusion(originSelect);
	originSelect->addToForEachBranch(otherSelect);

	// -- -- Calculate: 'rAB'
	CalculateProcedureNode* calcDistance = new CalculateProcedureNode(CalculateProcedureNode::DistanceObservable, originSelect, otherSelect);
	otherSelect->addToForEachBranch(calcDistance);

	// -- -- Collect1D: @dataName
	Collect1DProcedureNode* collect1D = new Collect1DProcedureNode(calcDistance, rMin, rMax, binWidth);
	collect1D->setName(resultName());
	otherSelect->addToForEachBranch(collect1D);

	// Process1D: @dataName
	Process1DProcedureNode* process1D = new Process1DProcedureNode(collect1D);
	process1D->setName(resultName());
	process1D->addSitePopulationNormaliser(originSelect);
	process1D->addNumberDensityNormaliser(otherSelect);
	process1D->setNormaliseBySphericalShellVolume(true);
	process1D->setSaveData(saveData);
	process1D->setValueLabel("g(r)");
	process1D->setXAxisLabel("r, \\symbol{Angstrom}");
	analyser_.addRootSequenceNode(process1D);

	return true;
}

// Run main processing
bool CalculateRDFModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0)
	{
		Messenger::warn("No Configuration targets for Module.\n");
		return true;
	}

	// Loop over target Configurations
	for (RefListItem<Configuration,bool>* ri = targetConfigurations_.first(); ri != NULL; ri = ri->next)
	{
		// Grab Configuration pointer
		Configuration* cfg = ri->item;

		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// Execute the analysis
		if (!analyser_.execute(procPool, cfg, uniqueName(), dissolve.processingModuleData())) return Messenger::error("CalculateRDF experience problems with its analysis.\n");
	}

	return true;
}

