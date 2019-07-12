/*
	*** Calculate Distance-Angle Module - Processing
	*** src/modules/calculate/dangle/process.cpp
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

#include "modules/calculate/dangle/dangle.h"
#include "main/dissolve.h"
#include "analyse/nodes/calculate.h"
#include "analyse/nodes/collect1d.h"
#include "analyse/nodes/collect2d.h"
#include "analyse/nodes/process1d.h"
#include "analyse/nodes/process2d.h"
#include "analyse/nodes/select.h"
#include "base/sysfunc.h"

// Run set-up stage
bool CalculateDAngleModule::setUp(Dissolve& dissolve, ProcessPool& procPool)
{
	// Reconstruct the Analyser
	analyser_.clear();

	// Get relevant Module options
	const double angleBin = keywords_.asDouble("AngleBin");
	const double angleMax = keywords_.asDouble("AngleMax");
	const double angleMin = keywords_.asDouble("AngleMin");
	const double distanceBin = keywords_.asDouble("DistanceBin");
	const double distanceMax = keywords_.asDouble("DistanceMax");
	const double distanceMin = keywords_.asDouble("DistanceMin");
	const bool excludeSameMolecule = keywords_.asBool("ExcludeSameMolecule");
	const bool saveData = keywords_.asBool("Save");

	SpeciesSite* siteA = KeywordListHelper<SpeciesSite*>::retrieve(keywords_, "SiteA", NULL);
	if (!siteA) return Messenger::error("Site 'A' (of A-B...C) is not defined.\n");
	SpeciesSite* siteB = KeywordListHelper<SpeciesSite*>::retrieve(keywords_, "SiteB", NULL);
	if (!siteB) return Messenger::error("Site 'B' (of A-B...C) is not defined.\n");
	SpeciesSite* siteC = KeywordListHelper<SpeciesSite*>::retrieve(keywords_, "SiteC", NULL);
	if (!siteC) return Messenger::error("Site 'C' (of A-B...C) is not defined.\n");

	/*
	 * Assemble the code below (@var indicates local variable 'var')
	 *
	 * Select  'A'
	 *   Site  @siteA
	 *   ForEach
	 *     Select  'B'
	 *       Site  @siteB
	 *       SameMoleculeAsSite  'A'
	 *       ForEach
	 *         Select  'C'
	 *           Site  @siteC
	 *           ExcludeSameMolecule  (if @excludeSameMolecule then 'A')
	 *           ForEach
	 *             Calculate  'rBC'
	 *               Distance  'B'  'C'
	 *             EndCalculate
	 *             Calculate  'aABC'
	 *               Angle  'A'  'B'  'C'
	 *             EndCalculate
	 *             Collect1D  'RDF(BC)'
	 *               QuantityX  'rBC'
	 *               RangeX  @distanceMin  @distanceMax  @distanceBin
	 *             EndCollect
	 *             Collect1D  'ANGLE(ABC)'
	 *               QuantityX  'aABC'
	 *               RangeX  @angleMin  @angleMax  @angleBin
	 *             EndCollect
	 *             Collect2D  @dataName
	 *               QuantityX  'rBC'
	 *               QuantityY  'aABC'
	 *               RangeX  @distanceMin  @distanceMax  @distanceBin
	 *               RangeY  @angleMin  @angleMax  @angleBin
	 *             EndCollect2D
	 *           EndForEach  'C'
	 *         EndSelect  'C'
	 *       EndForEach  'B'
	 *     EndSelect  'B'
	 *   EndForEach  'A'
	 * EndSelect  'A'
	 * Process2D  @dataName
	 *   NormaliseToOne  On
	 *   LabelValue  'g(r)'
	 *   LabelX  'r, Angstroms'
	 *   LabelY  'theta, Degrees'
	 *   LabelValue  'Probability'
	 * EndProcess2D
	 */

	// Select: Site 'A' (@siteA)
	AnalysisSelectNode* selectA = new AnalysisSelectNode(siteA);
	selectA->setName("A");
	analyser_.addRootSequenceNode(selectA);

	// -- Select: Site 'B' (@siteB)
	AnalysisSelectNode* selectB = new AnalysisSelectNode(siteB);
	selectB->setName("B");
	selectB->setSameMolecule(selectA);
	selectA->addToForEachBranch(selectB);

	// -- -- Select: Site 'C' (@siteC)
	AnalysisSelectNode* selectC = new AnalysisSelectNode(siteC);
	selectC->setName("C");
	if (excludeSameMolecule) selectC->addSameMoleculeExclusion(selectA);
	selectB->addToForEachBranch(selectC);

	// -- -- -- Calculate: 'rBC'
	AnalysisCalculateNode* calcDistance = new AnalysisCalculateNode(AnalysisCalculateNode::DistanceObservable, selectB, selectC);
	selectC->addToForEachBranch(calcDistance);

	// -- -- -- Calculate: 'aABC'
	AnalysisCalculateNode* calcAngle = new AnalysisCalculateNode(AnalysisCalculateNode::AngleObservable, selectA, selectB, selectC);
	selectC->addToForEachBranch(calcAngle);

	// -- -- -- Collect1D:  'RDF(BC)
	AnalysisCollect1DNode* collectDistance = new AnalysisCollect1DNode(calcDistance, distanceMin, distanceMax, distanceBin);
	collectDistance->setName(rdfBCResultName());
	selectC->addToForEachBranch(collectDistance);

	// -- -- -- Collect1D:  'ANGLE(ABC)
	AnalysisCollect1DNode* collectAngle = new AnalysisCollect1DNode(calcAngle, angleMin, angleMax, angleBin);
	collectAngle->setName(angleABCResultName());
	selectC->addToForEachBranch(collectAngle);

	// -- -- -- Collect2D:  'Distance-Angle(B...C vs A-B...C)
	AnalysisCollect2DNode* collectDAngle = new AnalysisCollect2DNode(calcDistance, calcAngle, distanceMin, distanceMax, distanceBin, angleMin, angleMax, angleBin);
	collectAngle->setName(resultName());
	selectC->addToForEachBranch(collectDAngle);

	// Process1D: 'RDF(BC)'
	AnalysisProcess1DNode* processDistance = new AnalysisProcess1DNode(collectDistance);
	processDistance->setName(rdfBCResultName());
	processDistance->addSitePopulationNormaliser(selectA);
	processDistance->addNumberDensityNormaliser(selectC);
	processDistance->setNormaliseBySphericalShellVolume(true);
	processDistance->setSaveData(saveData);
	processDistance->setValueLabel("g(r)");
	processDistance->setXAxisLabel("r, \\symbol{Angstrom}");
	analyser_.addRootSequenceNode(processDistance);

	// Process1D: 'RDF(BC)'
	AnalysisProcess1DNode* processAngle = new AnalysisProcess1DNode(collectAngle);
	processAngle->setName(angleABCResultName());
	processAngle->setNormaliseToOne(true);
	processAngle->setSaveData(saveData);
	processAngle->setValueLabel("Normalised Frequency");
	processAngle->setXAxisLabel("\\symbol{theta}, \\symbol{degrees}");
	analyser_.addRootSequenceNode(processAngle);

	// Process1D: @dataName
	AnalysisProcess2DNode* processDAngle = new AnalysisProcess2DNode(collectDAngle);
	processDAngle->setName(resultName());
	processDAngle->setNormaliseToOne(true);
	processDAngle->setSaveData(saveData);
	processDAngle->setValueLabel("g(r)");
	processDAngle->setXAxisLabel("r, \\symbol{Angstrom}");
	processDAngle->setYAxisLabel("\\symbol{theta}, \\symbol{degrees}");
	analyser_.addRootSequenceNode(processDAngle);

	return true;
}

// Run main processing
bool CalculateDAngleModule::process(Dissolve& dissolve, ProcessPool& procPool)
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
		if (!analyser_.execute(procPool, cfg, uniqueName(), dissolve.processingModuleData())) return Messenger::error("CalculateDAngle experience problems with its analysis.\n");
	}

	return true;
}

