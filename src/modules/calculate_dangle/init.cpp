// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/calculate_dangle/dangle.h"
#include "procedure/nodes/calculateangle.h"
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
     * Process1D  'RDF(BC)'
     *   Normalisation
     *     OperateSitePopulationNormalise
     *       Site  'A'  'B'
     *     EndOperateSitePopulationNormalise
     *     OperateNumberDensityNormalise
     *       Site  'C'
     *     EndOperateNumberDensityNormalise
     *     OperateSphericalShellNormalise
     *     EndOperateSphericalShellNormalise
     *   EndNormalisation
     *   LabelX  'r, Angstroms'
     *   LabelValue  'g(r)'
     * EndProcess1D
     * Process1D  'Angle(ABC)'
     *   Normalisation
     *     OperateExpression
     *       Expression  "value/sin(x)"
     *     EndOperateExpression
     *     OperateNormalise
     *       Value  1.0
     *     EndOperateNormalise
     *   EndNormalisation
     *   LabelX  'theta, Degrees'
     *   LabelValue  'Normalised Frequency'
     * EndProcess1D
     * Process2D  'DAngle(A-BC)'
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
    selectA_ = std::make_shared<SelectProcedureNode>();
    selectA_->setName("A");
    std::shared_ptr<SequenceProcedureNode> forEachA = selectA_->addForEachBranch(ProcedureNode::AnalysisContext);
    analyser_.addRootSequenceNode(selectA_);

    // -- Select: Site 'B'
    selectB_ = std::make_shared<SelectProcedureNode>();
    selectB_->setName("B");
    selectB_->keywords().set("SameMoleculeAsSite", selectA_);
    std::shared_ptr<SequenceProcedureNode> forEachB = selectB_->addForEachBranch(ProcedureNode::AnalysisContext);
    forEachA->addNode(selectB_);

    // -- -- Select: Site 'C'
    selectC_ = std::make_shared<SelectProcedureNode>();
    selectC_->setName("C");
    selectC_->keywords().set("ExcludeSameMolecule", std::vector<std::shared_ptr<const SelectProcedureNode>>{selectA_});
    std::shared_ptr<SequenceProcedureNode> forEachC = selectC_->addForEachBranch(ProcedureNode::AnalysisContext);
    forEachB->addNode(selectC_);

    // -- -- -- Calculate: 'rBC'
    auto calcDistance = std::make_shared<CalculateDistanceProcedureNode>(selectB_, selectC_);
    forEachC->addNode(calcDistance);

    // -- -- -- Calculate: 'aABC'
    auto calcAngle = std::make_shared<CalculateAngleProcedureNode>(selectA_, selectB_, selectC_);
    forEachC->addNode(calcAngle);

    // -- -- -- Collect2D:  'Distance-Angle(B...C vs A-B...C)'
    collectDAngle_ = std::make_shared<Collect2DProcedureNode>(calcDistance, calcAngle, 0.0, 10.0, 0.05, 0.0, 180.0, 1.0);
    std::shared_ptr<SequenceProcedureNode> subCollection = collectDAngle_->addSubCollectBranch(ProcedureNode::AnalysisContext);
    forEachC->addNode(collectDAngle_);

    // -- -- -- -- Collect1D:  'RDF(BC)'
    collectDistance_ = std::make_shared<Collect1DProcedureNode>(calcDistance, 0.0, 10.0, 0.05);
    subCollection->addNode(collectDistance_);

    // -- -- -- -- Collect1D:  'ANGLE(ABC)'
    collectAngle_ = std::make_shared<Collect1DProcedureNode>(calcAngle, 0.0, 180.0, 1.0);
    subCollection->addNode(collectAngle_);

    // Process1D: 'RDF(BC)'
    processDistance_ = std::make_shared<Process1DProcedureNode>(collectDistance_);
    processDistance_->setName("RDF(BC)");
    processDistance_->keywords().set("LabelValue", std::string("g(r)"));
    processDistance_->keywords().set("LabelX", std::string("r, \\symbol{Angstrom}"));

    std::shared_ptr<SequenceProcedureNode> rdfNormalisation = processDistance_->addNormalisationBranch();
    rdfNormalisation->addNode(std::make_shared<OperateSitePopulationNormaliseProcedureNode, std::vector<std::shared_ptr<const SelectProcedureNode> >>({selectA_, selectB_}));
    rdfNormalisation->addNode(
			      std::make_shared<OperateNumberDensityNormaliseProcedureNode>(std::vector<std::shared_ptr<const SelectProcedureNode> >({selectC_})));
    rdfNormalisation->addNode(std::make_shared<OperateSphericalShellNormaliseProcedureNode>());
    analyser_.addRootSequenceNode(processDistance_);

    // Process1D: 'ANGLE(ABC)'
    processAngle_ = std::make_shared<Process1DProcedureNode>(collectAngle_);
    processAngle_->setName("Angle(ABC)");
    processAngle_->keywords().set("LabelValue", std::string("Normalised Frequency"));
    processAngle_->keywords().set("LabelX", std::string("\\symbol{theta}, \\symbol{degrees}"));
    std::shared_ptr<SequenceProcedureNode> angleNormalisation = processAngle_->addNormalisationBranch();
    angleNormalisation->addNode(std::make_shared<OperateExpressionProcedureNode>("value/sin(x)"));
    angleNormalisation->addNode(std::make_shared<OperateNormaliseProcedureNode>(1.0));
    analyser_.addRootSequenceNode(processAngle_);

    // Process2D: 'DAngle'
    processDAngle_ = std::make_shared<Process2DProcedureNode>(collectDAngle_);
    processDAngle_->setName("DAngle(A-BC)");
    processDAngle_->keywords().set("LabelValue", std::string("g(r)"));
    processDAngle_->keywords().set("LabelX", std::string("r, \\symbol{Angstrom}"));
    processDAngle_->keywords().set("LabelY", std::string("\\symbol{theta}, \\symbol{degrees}"));
    std::shared_ptr<SequenceProcedureNode> dAngleNormalisation = processDAngle_->addNormalisationBranch();
    dAngleNormalisation->addNode(std::make_shared<OperateExpressionProcedureNode>("value/sin(y)"));
    dAngleNormalisation->addNode(std::make_shared<OperateNormaliseProcedureNode>(1.0));
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
    keywords_.add<SpeciesSiteVectorKeyword>("Control", "SiteB", "Add site(s) which represent 'B' in the interaction A-B...C",
                                            selectB_->speciesSites(), selectB_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>("Control", "SiteC", "Add site(s) which represent 'C' in the interaction A-B...C",
                                            selectC_->speciesSites(), selectC_->axesRequired());
    keywords_.add<BoolKeyword>("Control", "ExcludeSameMolecule",
                               "Whether to exclude correlations between B and C sites on the same molecule",
                               excludeSameMolecule_);

    // Export
    keywords_.add<FileAndFormatKeyword>("Export", "ExportRDF",
                                        "File format and file name under which to save calculated B-C RDF",
                                        processDistance_->exportFileAndFormat(), "EndExportRDF");
    keywords_.add<FileAndFormatKeyword>("Export", "ExportAngle",
                                        "File format and file name under which to save calculated A-B...C angle histogram",
                                        processAngle_->exportFileAndFormat(), "EndExportAngle");
    keywords_.add<FileAndFormatKeyword>("Export", "ExportDAngle",
                                        "File format and file name under which to save calculated A-B...C angle map",
                                        processDAngle_->exportFileAndFormat(), "EndExportDAngle");
}
