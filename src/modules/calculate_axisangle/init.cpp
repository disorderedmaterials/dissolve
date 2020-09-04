/*
    *** Calculate Axis Angle Module - Initialisation
    *** src/modules/calculate_axisangle/init.cpp
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

#include "keywords/types.h"
#include "modules/calculate_axisangle/axisangle.h"
#include "procedure/nodes/calculateaxisangle.h"
#include "procedure/nodes/calculatedistance.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/operateexpression.h"
#include "procedure/nodes/operatenormalise.h"
#include "procedure/nodes/operatenumberdensitynormalise.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/operatesphericalshellnormalise.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/process2d.h"
#include "procedure/nodes/select.h"

// Perform any necessary initialisation for the Module
void CalculateAxisAngleModule::initialise()
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
     *             Calculate  'axisAngle
     *               AxisAngle  'A'  'axis'  'B'  'axis'
     *             EndCalculate
     *             Collect2D  'DAngle(A-BC)'
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
     *   Normalisation
     *     OperateEquationNormalise
     *       Equation  value/sin(y)
     *     EndOperateEquationNormalise
     *     OperateNormalise
     *       Value  1.0
     *     EndOperateNormalise
     *   EndNormalistaion
     *   LabelValue  'g(r)'
     *   LabelX  'r, Angstroms'
     *   LabelY  'theta, Degrees'
     *   LabelValue  'Probability'
     * EndProcess2D
     */

    // Select: Site 'A'
    selectA_ = new SelectProcedureNode(nullptr, true);
    selectA_->setName("A");
    SequenceProcedureNode *forEachA = selectA_->addForEachBranch(ProcedureNode::AnalysisContext);
    analyser_.addRootSequenceNode(selectA_);

    // -- Select: Site 'B'
    selectB_ = new SelectProcedureNode(nullptr, true);
    selectB_->setName("B");
    RefList<SelectProcedureNode> sameMoleculeExclusions(selectA_);
    selectB_->setKeyword<RefList<SelectProcedureNode> &>("ExcludeSameMolecule", sameMoleculeExclusions);
    SequenceProcedureNode *forEachB = selectB_->addForEachBranch(ProcedureNode::AnalysisContext);
    forEachA->addNode(selectB_);

    // -- -- Calculate: 'rAB'
    auto *calcDistance = new CalculateDistanceProcedureNode(selectA_, selectB_);
    forEachB->addNode(calcDistance);

    // -- -- Calculate: 'axisAngle'
    auto *calcAngle = new CalculateAxisAngleProcedureNode(selectA_, OrientedSite::XAxis, selectB_, OrientedSite::XAxis);
    forEachB->addNode(calcAngle);

    // -- -- Collect2D:  'rAB vs axisAngle)'
    collectDAngle_ = new Collect2DProcedureNode(calcDistance, calcAngle, 0.0, 10.0, 0.05, 0.0, 180.0, 1.0);
    SequenceProcedureNode *subCollection = collectDAngle_->addSubCollectBranch(ProcedureNode::AnalysisContext);
    forEachB->addNode(collectDAngle_);

    // -- -- -- Collect1D:  'RDF(AB)'
    collectDistance_ = new Collect1DProcedureNode(calcDistance, 0.0, 10.0, 0.05);
    subCollection->addNode(collectDistance_);

    // -- -- -- Collect1D:  'ANGLE(axis-axis)'
    collectAngle_ = new Collect1DProcedureNode(calcAngle, 0.0, 180.0, 1.0);
    subCollection->addNode(collectAngle_);

    // Process1D: 'RDF(AB)'
    processDistance_ = new Process1DProcedureNode(collectDistance_);
    processDistance_->setName("RDF(AB)");
    processDistance_->setKeyword<std::string>("LabelValue", "g(r)");
    processDistance_->setKeyword<std::string>("LabelX", "r, \\symbol{Angstrom}");

    SequenceProcedureNode *rdfNormalisation = processDistance_->addNormalisationBranch();
    RefList<const SelectProcedureNode> sitePopulationNormalisers;
    sitePopulationNormalisers.append(selectA_);
    // 	sitePopulationNormalisers.append(selectB_);
    rdfNormalisation->addNode(new OperateSitePopulationNormaliseProcedureNode(sitePopulationNormalisers));
    rdfNormalisation->addNode(new OperateNumberDensityNormaliseProcedureNode(selectB_));
    rdfNormalisation->addNode(new OperateSphericalShellNormaliseProcedureNode);
    analyser_.addRootSequenceNode(processDistance_);

    // Process1D: 'ANGLE(axis)'
    processAngle_ = new Process1DProcedureNode(collectAngle_);
    processAngle_->setName("AxisAngle(AB)");
    processAngle_->setKeyword<std::string>("LabelValue", "Normalised Frequency");
    processAngle_->setKeyword<std::string>("LabelX", "\\symbol{theta}, \\symbol{degrees}");
    SequenceProcedureNode *angleNormalisation = processAngle_->addNormalisationBranch();
    angleNormalisation->addNode(new OperateExpressionProcedureNode("value/sin(x)"));
    angleNormalisation->addNode(new OperateNormaliseProcedureNode(1.0));
    analyser_.addRootSequenceNode(processAngle_);

    // Process2D: 'DAngle'
    processDAngle_ = new Process2DProcedureNode(collectDAngle_);
    processDAngle_->setName("DAxisAngle");
    processDAngle_->setKeyword<std::string>("LabelValue", "g(r)");
    processDAngle_->setKeyword<std::string>("LabelX", "r, \\symbol{Angstrom}");
    processDAngle_->setKeyword<std::string>("LabelY", "\\symbol{theta}, \\symbol{degrees}");
    SequenceProcedureNode *dAngleNormalisation = processDAngle_->addNormalisationBranch();
    dAngleNormalisation->addNode(new OperateExpressionProcedureNode("value/sin(y)"));
    dAngleNormalisation->addNode(new OperateNormaliseProcedureNode(1.0));
    analyser_.addRootSequenceNode(processDAngle_);

    /*
     * Keywords (including those exposed from the ProcedureNodes)
     */

    // Calculation
    keywords_.add(
        "Calculation",
        new Vec3DoubleKeyword(Vec3<double>(0.0, 10.0, 0.05), Vec3<double>(0.0, 0.0, 1.0e-5), Vec3Labels::MinMaxBinwidthlabels),
        "DistanceRange", "Range (min, max, binwidth) of distance axis", "<min> <max> <binwidth> (Angstroms)");
    keywords_.add(
        "Calculation",
        new Vec3DoubleKeyword(Vec3<double>(0.0, 180.0, 1.0), Vec3<double>(0.0, 0.0, 1.0e-5), Vec3Labels::MinMaxBinwidthlabels),
        "AngleRange", "Range (min, max, binwidth) of angle axis", "<min> <max> <binwidth> (degrees)");

    // Sites
    keywords_.link("Sites", selectA_->keywords().find("Site"), "SiteA",
                   "Add site(s) which represent 'A' in the interaction A-B...C", "<Species> <Site> [<Species> <Site> ... ]");
    keywords_.link("Sites", calcAngle->keywords().find("AxisI"), "AxisA", "Axis to use from site A");
    keywords_.link("Sites", selectB_->keywords().find("Site"), "SiteB",
                   "Add site(s) which represent 'B' in the interaction A-B...C", "<Species> <Site> [<Species> <Site> ... ]");
    keywords_.link("Sites", calcAngle->keywords().find("AxisJ"), "AxisB", "Axis to use from site B");
    keywords_.add("Sites", new BoolKeyword(false), "ExcludeSameMolecule",
                  "Whether to exclude correlations between B and C sites on the same molecule", "<True|False>");

    // Export
    keywords_.link("Export", processDistance_->keywords().find("Save"), "SaveRDF", "Whether to save calculated B-C RDF to disk",
                   "<True|False>");
    keywords_.link("Export", processAngle_->keywords().find("Save"), "SaveAngle",
                   "Whether to save calculated A-B...C angle histrogram to disk", "<True|False>");
    keywords_.link("Export", processDAngle_->keywords().find("Save"), "SaveDAngle",
                   "Whether to save calculated A-B...C angle map to disk", "<True|False>");
}
