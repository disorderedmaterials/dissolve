/*
	*** Calculate Distance-Angle Module - Initialisation
	*** src/modules/calculate/dangle/init.cpp
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
#include "keywords/types.h"
#include "procedure/nodes/calculate.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/process2d.h"
#include "procedure/nodes/select.h"

// Perform any necessary initialisation for the Module
void CalculateDAngleModule::initialise()
{
	/*
	 * Assemble the following Procedure:
	 *
	 * Select  'A'
	 *   Site  ...
	 *   ForEach
	 *     Select  'B'
	 *       Site  ...
	 *       SameMoleculeAsSite  'A'
	 *       ForEach
	 *         Select  'C'
	 *           Site  ...
	 *           ExcludeSameMolecule  'A'
	 *           ForEach
	 *             Calculate  'rBC'
	 *               Distance  'B'  'C'
	 *             EndCalculate
	 *             Calculate  'aABC'
	 *               Angle  'A'  'B'  'C'
	 *             EndCalculate
	 *             Collect2D  DAngle
	 *               QuantityX  'rBC'
	 *               QuantityY  'aABC'
	 *               RangeX  0.0  10.0  0.05
	 *               RangeY  0.0  180.0  1.0
	 *               SubCollect
	 *                 Collect1D  'RDF(BC)'
	 *                   QuantityX  'rBC'
	 *                   RangeX  0.0  10.0  0.05
	 *                 EndCollect
	 *                 Collect1D  'ANGLE(ABC)'
	 *                   QuantityX  'aABC'
	 *                   RangeX  0.0  180.0  1.0
	 *                 EndCollect
	 *               EndSubCollect
	 *             EndCollect2D
	 *           EndForEach  'C'
	 *         EndSelect  'C'
	 *       EndForEach  'B'
	 *     EndSelect  'B'
	 *   EndForEach  'A'
	 * EndSelect  'A'
	 * Process2D  DAngle
	 *   NormaliseToOne  On
	 *   LabelValue  'g(r)'
	 *   LabelX  'r, Angstroms'
	 *   LabelY  'theta, Degrees'
	 *   LabelValue  'Probability'
	 * EndProcess2D
	 */

	// Select: Site 'A'
	selectA_ = new SelectProcedureNode;
	selectA_->setName("A");
	SequenceProcedureNode* forEachA = selectA_->addForEachBranch(ProcedureNode::AnalysisContext);
	analyser_.addRootSequenceNode(selectA_);

	// -- Select: Site 'B'
	selectB_ = new SelectProcedureNode;
	selectB_->setName("B");
	selectB_->setKeyword<SelectProcedureNode*>("SameMoleculeAsSite", selectA_);
	SequenceProcedureNode* forEachB = selectB_->addForEachBranch(ProcedureNode::AnalysisContext);
	forEachA->addNode(selectB_);

	// -- -- Select: Site 'C'
	selectC_ = new SelectProcedureNode;
	selectC_->setName("C");
	RefList<SelectProcedureNode> sameMoleculeExclusions(selectA_);
	selectC_->setKeyword< RefList<SelectProcedureNode>& >("ExcludeSameMolecule", sameMoleculeExclusions);
	SequenceProcedureNode* forEachC = selectC_->addForEachBranch(ProcedureNode::AnalysisContext);
	forEachB->addNode(selectC_);

	// -- -- -- Calculate: 'rBC'
	CalculateProcedureNode* calcDistance = new CalculateProcedureNode(CalculateProcedureNode::DistanceObservable, selectB_, selectC_);
	forEachC->addNode(calcDistance);

	// -- -- -- Calculate: 'aABC'
	CalculateProcedureNode* calcAngle = new CalculateProcedureNode(CalculateProcedureNode::AngleObservable, selectA_, selectB_, selectC_);
	forEachC->addNode(calcAngle);

	// -- -- -- Collect2D:  'Distance-Angle(B...C vs A-B...C)'
	collectDAngle_ = new Collect2DProcedureNode(calcDistance, calcAngle, 0.0, 10.0, 0.05, 0.0, 180.0, 1.0);
	SequenceProcedureNode* subCollection = collectDAngle_->addSubCollectBranch(ProcedureNode::AnalysisContext);
	forEachC->addNode(collectDAngle_);

	// -- -- -- -- Collect1D:  'RDF(BC)'
	collectDistance_ = new Collect1DProcedureNode(calcDistance, 0.0, 10.0, 0.05);
	subCollection->addNode(collectDistance_);

	// -- -- -- -- Collect1D:  'ANGLE(ABC)'
	collectAngle_ = new Collect1DProcedureNode(calcAngle, 0.0, 180.0, 1.0);
	subCollection->addNode(collectAngle_);

	// Process1D: 'RDF(BC)'
	processDistance_ = new Process1DProcedureNode(collectDistance_);
	processDistance_->addSitePopulationNormaliser(selectA_);
	processDistance_->addSitePopulationNormaliser(selectB_);
	processDistance_->addNumberDensityNormaliser(selectC_);
	processDistance_->setNormaliseBySphericalShellVolume(true);
	processDistance_->setValueLabel("g(r)");
	processDistance_->setXAxisLabel("r, \\symbol{Angstrom}");
	analyser_.addRootSequenceNode(processDistance_);

	// Process1D: 'ANGLE(ABC)'
	processAngle_ = new Process1DProcedureNode(collectAngle_);
	processAngle_->setNormaliseToOne(true);
	processAngle_->setValueLabel("Normalised Frequency");
	processAngle_->setXAxisLabel("\\symbol{theta}, \\symbol{degrees}");
	analyser_.addRootSequenceNode(processAngle_);

	// Process2D: 'DAngle'
	processDAngle_ = new Process2DProcedureNode(collectDAngle_);
	processDAngle_->setNormaliseToOne(true);
	processDAngle_->setValueLabel("g(r)");
	processDAngle_->setXAxisLabel("r, \\symbol{Angstrom}");
	processDAngle_->setYAxisLabel("\\symbol{theta}, \\symbol{degrees}");
	analyser_.addRootSequenceNode(processDAngle_);

	// Update names for the nodes
	updateNodeNames();

	/*
	 * Keywords (including those exposed from the ProcedureNodes)
	 */

	// Calculation
	keywords_.add("Calculation", new Vec3DoubleKeyword(Vec3<double>(0.0, 10.0, 0.05), Vec3<double>(0.0, 0.0, 1.0e-5)), "DistanceRange", "Range (min, max, delta) of distance axis", "<min> <max> <delta> (Angstroms)");
	keywords_.add("Calculation", new Vec3DoubleKeyword(Vec3<double>(0.0, 180.0, 1.0), Vec3<double>(0.0, 0.0, 1.0e-5)), "AngleRange", "Range (min, max, delta) of angle axis", "<min> <max> <delta> (degrees)");

	// Sites
	keywords_.link("Sites", selectA_->keywords().find("Site"), "SiteA", "Add site(s) which represent 'A' in the interaction A-B...C", "<Species> <Site> [<Species> <Site> ... ]");
	keywords_.link("Sites", selectB_->keywords().find("Site"), "SiteB", "Add site(s) which represent 'B' in the interaction A-B...C", "<Species> <Site> [<Species> <Site> ... ]");
	keywords_.link("Sites", selectC_->keywords().find("Site"), "SiteC", "Add site(s) which represent 'C' in the interaction A-B...C", "<Species> <Site> [<Species> <Site> ... ]");
	keywords_.add("Sites", new BoolKeyword(false), "ExcludeSameMolecule", "Whether to exclude correlations between B and C sites on the same molecule", "<True|False>");

	// Export
	keywords_.add("Export", new BoolKeyword(false), "Save", "Save calculated data to disk", "<True|False>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
KeywordBase::ParseResult CalculateDAngleModule::parseComplexKeyword(KeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return KeywordBase::Unrecognised;
}
