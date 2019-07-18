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
#include "procedure/nodes/calculate.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/process2d.h"
#include "procedure/nodes/select.h"
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

	// Check for sites being defined
	if (aSites_.nItems() == 0) return Messenger::error("At least one 'A' Site (of A-B...C) must be defined.\n");
	if (bSites_.nItems() == 0) return Messenger::error("At least one 'B' Site (of A-B...C) must be defined.\n");
	if (cSites_.nItems() == 0) return Messenger::error("At least one 'C' Site (of A-B...C) must be defined.\n");

	/*
	 * Assemble the code below (@var indicates local variable 'var')
	 *
	 * Select  'A'
	 *   Site  @aSites_
	 *   ForEach
	 *     Select  'B'
	 *       Site  @bSites_
	 *       SameMoleculeAsSite  'A'
	 *       ForEach
	 *         Select  'C'
	 *           Site  @cSites_
	 *           ExcludeSameMolecule  (if @excludeSameMolecule then 'A')
	 *           ForEach
	 *             Calculate  'rBC'
	 *               Distance  'B'  'C'
	 *             EndCalculate
	 *             Calculate  'aABC'
	 *               Angle  'A'  'B'  'C'
	 *             EndCalculate
	 *             Collect2D  @dataName
	 *               QuantityX  'rBC'
	 *               QuantityY  'aABC'
	 *               RangeX  @distanceMin  @distanceMax  @distanceBin
	 *               RangeY  @angleMin  @angleMax  @angleBin
	 *               SubCollect
	 *                 Collect1D  'RDF(BC)'
	 *                   QuantityX  'rBC'
	 *                   RangeX  @distanceMin  @distanceMax  @distanceBin
	 *                 EndCollect
	 *                 Collect1D  'ANGLE(ABC)'
	 *                   QuantityX  'aABC'
	 *                   RangeX  @angleMin  @angleMax  @angleBin
	 *                 EndCollect
	 *               EndSubCollect
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
	SelectProcedureNode* selectA = new SelectProcedureNode(aSites_);
	selectA->setName("A");
	analyser_.addRootSequenceNode(selectA);

	// -- Select: Site 'B' (@siteB)
	SelectProcedureNode* selectB = new SelectProcedureNode(bSites_);
	selectB->setName("B");
	selectB->setSameMolecule(selectA);
	selectA->addToForEachBranch(selectB);

	// -- -- Select: Site 'C' (@siteC)
	SelectProcedureNode* selectC = new SelectProcedureNode(cSites_);
	selectC->setName("C");
	if (excludeSameMolecule) selectC->addSameMoleculeExclusion(selectA);
	selectB->addToForEachBranch(selectC);

	// -- -- -- Calculate: 'rBC'
	CalculateProcedureNode* calcDistance = new CalculateProcedureNode(CalculateProcedureNode::DistanceObservable, selectB, selectC);
	selectC->addToForEachBranch(calcDistance);

	// -- -- -- Calculate: 'aABC'
	CalculateProcedureNode* calcAngle = new CalculateProcedureNode(CalculateProcedureNode::AngleObservable, selectA, selectB, selectC);
	selectC->addToForEachBranch(calcAngle);

	// -- -- -- Collect2D:  'Distance-Angle(B...C vs A-B...C)
	Collect2DProcedureNode* collectDAngle = new Collect2DProcedureNode(calcDistance, calcAngle, distanceMin, distanceMax, distanceBin, angleMin, angleMax, angleBin);
	collectDAngle->setName(resultName());
	selectC->addToForEachBranch(collectDAngle);

	// -- -- -- -- Collect1D:  'RDF(BC)
	Collect1DProcedureNode* collectDistance = new Collect1DProcedureNode(calcDistance, distanceMin, distanceMax, distanceBin);
	collectDistance->setName(rdfBCResultName());
	collectDAngle->addToSubCollectBranch(collectDistance);

	// -- -- -- -- Collect1D:  'ANGLE(ABC)
	Collect1DProcedureNode* collectAngle = new Collect1DProcedureNode(calcAngle, angleMin, angleMax, angleBin);
	collectAngle->setName(angleABCResultName());
	collectDAngle->addToSubCollectBranch(collectAngle);

	// Process1D: 'RDF(BC)'
	Process1DProcedureNode* processDistance = new Process1DProcedureNode(collectDistance);
	processDistance->setName(rdfBCResultName());
	processDistance->addSitePopulationNormaliser(selectA);
	processDistance->addSitePopulationNormaliser(selectB);
	processDistance->addNumberDensityNormaliser(selectC);
	processDistance->setNormaliseBySphericalShellVolume(true);
	processDistance->setSaveData(saveData);
	processDistance->setValueLabel("g(r)");
	processDistance->setXAxisLabel("r, \\symbol{Angstrom}");
	analyser_.addRootSequenceNode(processDistance);

	// Process1D: 'ANGLE(ABC)'
	Process1DProcedureNode* processAngle = new Process1DProcedureNode(collectAngle);
	processAngle->setName(angleABCResultName());
	processAngle->setNormaliseToOne(true);
	processAngle->setSaveData(saveData);
	processAngle->setValueLabel("Normalised Frequency");
	processAngle->setXAxisLabel("\\symbol{theta}, \\symbol{degrees}");
	analyser_.addRootSequenceNode(processAngle);

	// Process1D: @dataName
	Process2DProcedureNode* processDAngle = new Process2DProcedureNode(collectDAngle);
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

