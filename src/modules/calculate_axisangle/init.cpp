// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    selectA_ = new SelectProcedureNode({}, true);
    selectA_->setName("A");
    SequenceProcedureNode *forEachA = selectA_->addForEachBranch(ProcedureNode::AnalysisContext);
    analyser_.addRootSequenceNode(selectA_);

    // -- Select: Site 'B'
    selectB_ = new SelectProcedureNode({}, true);
    selectB_->setName("B");
    selectB_->keywords().set("ExcludeSameMolecule", std::vector<const SelectProcedureNode *>{selectA_});
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
    processDistance_->keywords().set("LabelValue", std::string("g(r)"));
    processDistance_->keywords().set("LabelX", std::string("r, \\symbol{Angstrom}"));

    SequenceProcedureNode *rdfNormalisation = processDistance_->addNormalisationBranch();
    rdfNormalisation->addNode(new OperateSitePopulationNormaliseProcedureNode({selectA_}));
    rdfNormalisation->addNode(new OperateNumberDensityNormaliseProcedureNode({selectB_}));
    rdfNormalisation->addNode(new OperateSphericalShellNormaliseProcedureNode);
    analyser_.addRootSequenceNode(processDistance_);

    // Process1D: 'ANGLE(axis)'
    processAngle_ = new Process1DProcedureNode(collectAngle_);
    processAngle_->setName("AxisAngle(AB)");
    processAngle_->keywords().set("LabelValue", std::string("Normalised Frequency"));
    processAngle_->keywords().set("LabelX", std::string("\\symbol{theta}, \\symbol{degrees}"));
    SequenceProcedureNode *angleNormalisation = processAngle_->addNormalisationBranch();
    angleNormalisation->addNode(new OperateExpressionProcedureNode("value/sin(x)"));
    angleNormalisation->addNode(new OperateNormaliseProcedureNode(1.0));
    analyser_.addRootSequenceNode(processAngle_);

    // Process2D: 'DAngle'
    processDAngle_ = new Process2DProcedureNode(collectDAngle_);
    processDAngle_->setName("DAxisAngle");
    processDAngle_->keywords().set("LabelValue", std::string("g(r)"));
    processDAngle_->keywords().set("LabelX", std::string("r, \\symbol{Angstrom}"));
    processDAngle_->keywords().set("LabelY", std::string("\\symbol{theta}, \\symbol{degrees}"));
    SequenceProcedureNode *dAngleNormalisation = processDAngle_->addNormalisationBranch();
    dAngleNormalisation->addNode(new OperateExpressionProcedureNode("value/sin(y)"));
    dAngleNormalisation->addNode(new OperateNormaliseProcedureNode(1.0));
    analyser_.addRootSequenceNode(processDAngle_);

    /*
     * Keywords (including those exposed from the ProcedureNodes)
     */

    // Control
    keywords_.add<Vec3DoubleKeyword>("Control", "DistanceRange", "Range (min, max, binwidth) of distance axis", distanceRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("Control", "AngleRange", "Range (min, max, binwidth) of angle axis", angleRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<SpeciesSiteVectorKeyword>("Control", "SiteA", "Add site(s) which represent 'A' in the interaction A-B...C",
                                            selectA_->speciesSites(), selectA_->axesRequired());
    keywords_.add<EnumOptionsKeyword<OrientedSite::SiteAxis>>("Control", "AxisA", "Axis to use from site A", calcAngle->axis(0),
                                                              OrientedSite::siteAxis());
    keywords_.add<SpeciesSiteVectorKeyword>("Control", "SiteB", "Add site(s) which represent 'B' in the interaction A-B...C",
                                            selectB_->speciesSites(), selectB_->axesRequired());
    keywords_.add<EnumOptionsKeyword<OrientedSite::SiteAxis>>("Control", "AxisB", "Axis to use from site B", calcAngle->axis(1),
                                                              OrientedSite::siteAxis());
    keywords_.add<BoolKeyword>("Control", "ExcludeSameMolecule",
                               "Whether to exclude correlations between B and C sites on the same molecule",
                               excludeSameMolecule_);

    // Export
    keywords_.add<FileAndFormatKeyword>("Export", "ExportRDF",
                                        "File format and file name under which to save calculated B-C RDF",
                                        processDistance_->exportFileAndFormat(), "EndExportRDF");
    keywords_.add<FileAndFormatKeyword>(
        "Export", "ExportAngle", "File format and file name under which to save calculated A-B...C angle histogram to disk",
        processAngle_->exportFileAndFormat(), "EndExportAngle");
    keywords_.add<FileAndFormatKeyword>("Export", "ExportDAngle",
                                        "File format and file name under which to save calculated A-B...C angle map to disk",
                                        processDAngle_->exportFileAndFormat(), "EndExportDAngle");
}
