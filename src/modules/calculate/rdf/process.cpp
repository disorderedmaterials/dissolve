/*
	*** CalculateRDF Module - Processing
	*** src/modules/calculate/rdf/process.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/calculate/rdf/calculaterdf.h"
#include "main/dissolve.h"
#include "analyse/nodes/calculate.h"
#include "analyse/nodes/collect1d.h"
#include "analyse/nodes/normalise1d.h"
#include "analyse/nodes/select.h"
#include "base/sysfunc.h"

// Run set-up stage
bool CalculateRDFModule::setUp(Dissolve& dissolve, ProcessPool& procPool)
{
	// Reconstruct the Analyser
	analyser_.clear();

	// Get relevant Module options
	const double binWidth = keywords_.asDouble("BinWidth");
	const bool excludeSameMolecule = keywords_.asBool("ExcludeSameMolecule");
	CharString rdfName = keywords_.asString("Name");
	if (rdfName.isEmpty()) rdfName = uniqueName();
	SpeciesSite* originSite = KeywordListHelper<SpeciesSite*>::retrieve(keywords_, "OriginSite", NULL);
	if (!originSite) return Messenger::error("Origin site is not defined.\n");
	SpeciesSite* otherSite = KeywordListHelper<SpeciesSite*>::retrieve(keywords_, "OtherSite", NULL);
	if (!otherSite) return Messenger::error("Other (surrounding) site is not defined.\n");
	const double rMax = keywords_.asDouble("RMax");
	const double rMin = keywords_.asDouble("RMin");

	/*
	 * Assemble the code below (@var indicates local variable 'var')
	 *
	 * Select  'A'
	 *   Site  @originSite
	 *   ForEach
	 *     Select  'B'
	 *       Site  @otherSite
	 *       ExcludeSameSite  'A'
	 *       ExcludeSameMolecule  (if @excludeSameMolecule then 'A')
	 *       ForEach
	 *         Calculate  'rAB'
	 *           Distance  'A'  'B'
	 *         EndCalculate
	 *         Collect1D  @rdfName
	 *           QuantityX  'rAB'
	 *           RangeX  @rMin  @rMax  @binWidth
	 *         EndCollect1D
	 *       EndForEach  'B'
	 *     EndSelect  'B'
	 *   EndForEach  'A'
	 * EndSelect  'A'
	 * Normalise1D  @rdfName
	 *   NSites  'A'
	 *   SphericalShellVolume  On
	 *   NumberDensity  'B'
	 *   LabelValue  'g(r)'
	 *   LabelX  'r, Angstroms'
	 * EndNormalise1D
	 */

	// Select: Site 'A' (@originSite)
	AnalysisSelectNode* originSelect = new AnalysisSelectNode(originSite);
	originSelect->setName("A");
	analyser_.addRootSequenceNode(originSelect);

	// -- Select: Site 'B' (@otherSite)
	AnalysisSelectNode* otherSelect = new AnalysisSelectNode(otherSite);
	otherSelect->setName("B");
	otherSelect->addSameSiteExclusion(originSelect);
	if (excludeSameMolecule) otherSelect->addSameMoleculeExclusion(originSelect);
	originSelect->addToForEachBranch(otherSelect);

	// -- -- Calculate: 'rAB'
	AnalysisCalculateNode* calcDistance = new AnalysisCalculateNode(AnalysisCalculateNode::DistanceObservable, originSelect, otherSelect);
	otherSelect->addToForEachBranch(calcDistance);

	// -- -- Collect1D: @rdfName
	AnalysisCollect1DNode* collect1D = new AnalysisCollect1DNode(calcDistance, rMin, rMax, binWidth);
	collect1D->setName(rdfName);
	otherSelect->addToForEachBranch(collect1D);

	// Normalise1D: @rdfName
	AnalysisNormalise1DNode* normalise1D = new AnalysisNormalise1DNode(collect1D);
	normalise1D->setName(rdfName);
	normalise1D->addSitePopulationNormaliser(originSelect);
	normalise1D->addNumberDensityNormaliser(otherSelect);
	normalise1D->setNormaliseBySphericalShellVolume(true);
	normalise1D->setSaveNormalisedData(true);
	normalise1D->setValueLabel("g(r)");
	normalise1D->setXAxisLabel("r, \\symbol{Angstrom}");
	analyser_.addRootSequenceNode(normalise1D);

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

