// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/calculate_angle/angle.h"
#include "procedure/nodes/calculateangle.h"
#include "procedure/nodes/calculatedistance.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/collect3d.h"
#include "procedure/nodes/operateexpression.h"
#include "procedure/nodes/operatenormalise.h"
#include "procedure/nodes/operatenumberdensitynormalise.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/operatesphericalshellnormalise.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/process2d.h"
#include "procedure/nodes/process3d.h"
#include "procedure/nodes/select.h"

// Perform any necessary initialisation for the Module
void CalculateAngleModule::initialise()
{
    /*
     * Assemble the following Procedure:
     *
     * Select  'B'
     *   Site  ...
     *   ForEach
     *     Select  'A'
     *       Site  ...
     *       ForEach
     *         Select  'C'
     *           Site  ...
     *           ForEach
     *             Calculate  'rAB'
     *               Distance  'A'  'B'
     *             EndCalculate
     *             Calculate  'rBC'
     *               Distance  'B'  'C'
     *             EndCalculate
     *             Calculate  'aABC'
     *               Angle  'A'  'B'  'C'
     *             EndCalculate
     *             Collect3D  'DDA'
     *               QuantityX  'rAB'
     *               QuantityY  'rBC'
     *               QuantityZ  'aABC'
     *               RangeX  0.0  10.0  0.05
     *               RangeY  0.0  10.0  0.05
     *               RangeZ  0.0  180.0  1.0
     *               SubCollect
     *                 Collect1D  'RDF(AB)'
     *                   QuantityX  'rAB'
     *                   RangeX  0.0  10.0  0.05
     *                 EndCollect
     *                 Collect1D  'RDF(BC)'
     *                   QuantityX  'rBC'
     *                   RangeX  0.0  10.0  0.05
     *                 EndCollect
     *                 Collect1D  'ANGLE(ABC)'
     *                   QuantityX  'aABC'
     *                   RangeX  0.0  180.0  1.0
     *                 EndCollect
     *                 Collect2D  'DAngle((A-B)-C)'
     *                   QuantityX  'rAB'
     *                   QuantityY  'aABC'
     *                   RangeX  0.0  10.0  0.05
     *                   RangeY  0.0  180.0  1.0
     *                 EndCollect2D
     *                 Collect2D  'DAngle(A-(B-C))'
     *                   QuantityX  'rBC'
     *                   QuantityY  'aABC'
     *                   RangeX  0.0  10.0  0.05
     *                   RangeY  0.0  180.0  1.0
     *                 EndCollect2D
     *               EndSubCollect
     *             EndCollect3D
     *           EndForEach  'C'
     *         EndSelect  'C'
     *       EndForEach  'B'
     *     EndSelect  'B'
     *   EndForEach  'A'
     * EndSelect  'A'
     * Process1D  'RDF(AB)'
     *   Normalisation
     *     OperateSitePopulationNormalise
     *       Site  'B'
     *     EndOperateSitePopulationNormalise
     *     OperateNumberDensityNormalise
     *       Site  'A'
     *     EndOperateNumberDensityNormalise
     *     OperateSphericalShellNormalise
     *     EndOperateSphericalShellNormalise
     *   EndNormalisation
     *   LabelX  'r, Angstroms'
     *   LabelValue  'g\\sub{AB}(r)'
     * EndProcess1D
     * Process1D  'RDF(BC)'
     *   Normalisation
     *     OperateSitePopulationNormalise
     *       Site  'B'
     *     EndOperateSitePopulationNormalise
     *     OperateNumberDensityNormalise
     *       Site  'C'
     *     EndOperateNumberDensityNormalise
     *     OperateSphericalShellNormalise
     *     EndOperateSphericalShellNormalise
     *   EndNormalisation
     *   LabelX  'r, Angstroms'
     *   LabelValue  'g\\sub{BC}(r)'
     * EndProcess1D
     * Process1D  'Angle(ABC)'
     *   Normalisation
     *     OperateExpression
     *       Expression("value/sin(x)")
     *     EndOperateExpression
     *     OperateNormalise
     *       Value  1.0
     *     EndOperateNormalise
     *   EndNormalisation
     *   LabelValue  'g(r)'
     *   LabelX  'theta, Degrees'
     *   LabelValue  'Normalised Frequency'
     * EndProcess1D
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

    // Select: Site 'B'
    selectB_ = std::make_shared<SelectProcedureNode>();
    selectB_->setName("B");
    std::shared_ptr<SequenceProcedureNode> forEachB = selectB_->addForEachBranch(ProcedureNode::AnalysisContext);
    analyser_.addRootSequenceNode(selectB_);

    // -- Select: Site 'A'
    selectA_ = std::make_shared<SelectProcedureNode>();
    selectA_->setName("A");
    std::shared_ptr<SequenceProcedureNode> forEachA = selectA_->addForEachBranch(ProcedureNode::AnalysisContext);
    forEachB->addNode(selectA_);

    // -- -- Select: Site 'C'
    selectC_ = std::make_shared<SelectProcedureNode>();
    selectC_->setName("C");
    std::shared_ptr<SequenceProcedureNode> forEachC = selectC_->addForEachBranch(ProcedureNode::AnalysisContext);
    forEachA->addNode(selectC_);

    // -- -- -- Calculate: 'rAB'
    auto calcAB = std::make_shared<CalculateDistanceProcedureNode>(selectA_, selectB_);
    forEachC->addNode(calcAB);

    // -- -- -- Calculate: 'rBC'
    auto calcBC = std::make_shared<CalculateDistanceProcedureNode>(selectB_, selectC_);
    forEachC->addNode(calcBC);

    // -- -- -- Calculate: 'aABC'
    auto calcABC = std::make_shared<CalculateAngleProcedureNode>(selectA_, selectB_, selectC_);
    forEachC->addNode(calcABC);

    // -- -- -- Collect3D:  'rAB vs rBC vs aABC'
    collectDDA_ = std::make_shared<Collect3DProcedureNode>(calcAB, calcBC, calcABC, 0.0, 10.0, 0.05, 0.0, 10.0, 0.05, 0.0, 180.0, 1.0);
    std::shared_ptr<SequenceProcedureNode> subCollection = collectDDA_->addSubCollectBranch(ProcedureNode::AnalysisContext);
    forEachC->addNode(collectDDA_);

    // -- -- -- -- Collect1D:  'RDF(AB)'
    collectAB_ = std::make_shared<Collect1DProcedureNode>(calcAB, 0.0, 10.0, 0.05);
    subCollection->addNode(collectAB_);

    // -- -- -- -- Collect1D:  'RDF(BC)'
    collectBC_ = std::make_shared<Collect1DProcedureNode>(calcBC, 0.0, 10.0, 0.05);
    subCollection->addNode(collectBC_);

    // -- -- -- -- Collect1D:  'ANGLE(ABC)'
    collectABC_ = std::make_shared<Collect1DProcedureNode>(calcABC, 0.0, 180.0, 1.0);
    subCollection->addNode(collectABC_);

    // -- -- -- -- Collect2D:  'DAngle (A-B)-C'
    collectDAngleAB_ = std::make_shared<Collect2DProcedureNode>(calcAB, calcABC, 0.0, 10.0, 0.05, 0.0, 180.0, 1.0);
    subCollection->addNode(collectDAngleAB_);

    // -- -- -- -- Collect3D:  'DAngle A-(B-C)'
    collectDAngleBC_ = std::make_shared<Collect2DProcedureNode>(calcBC, calcABC, 0.0, 10.0, 0.05, 0.0, 180.0, 1.0);
    subCollection->addNode(collectDAngleBC_);

    // Process1D: 'RDF(AB)'
    processAB_ = std::make_shared<Process1DProcedureNode>(collectAB_);
    processAB_->setName("RDF(AB)");
    processAB_->setKeyword<std::string>("LabelValue", "g\\sub{AB}(r)");
    processAB_->setKeyword<std::string>("LabelX", "r, \\symbol{Angstrom}");
    std::shared_ptr<SequenceProcedureNode> rdfABNormalisation = processAB_->addNormalisationBranch();
    rdfABNormalisation->addNode(std::make_shared<OperateSitePopulationNormaliseProcedureNode, std::vector<const ProcedureNode*>>({selectB_.get(), selectA_.get(), selectC_.get()}));
    rdfABNormalisation->addNode(std::make_shared<OperateNumberDensityNormaliseProcedureNode, std::vector<const ProcedureNode*>>({selectA_.get()}));
    rdfABNormalisation->addNode(std::make_shared<OperateSphericalShellNormaliseProcedureNode>());
    analyser_.addRootSequenceNode(processAB_);

    // Process1D: 'RDF(BC)'
    processBC_ = std::make_shared<Process1DProcedureNode>(collectBC_);
    processBC_->setName("RDF(BC)");
    processBC_->setKeyword<std::string>("LabelValue", "g\\sub{BC}(r)");
    processBC_->setKeyword<std::string>("LabelX", "r, \\symbol{Angstrom}");
    std::shared_ptr<SequenceProcedureNode> rdfBCNormalisation = processBC_->addNormalisationBranch();
    rdfBCNormalisation->addNode(std::make_shared<OperateSitePopulationNormaliseProcedureNode, std::vector<const ProcedureNode*>>({selectB_.get(), selectA_.get(), selectC_.get()}));
    rdfBCNormalisation->addNode(std::make_shared<OperateNumberDensityNormaliseProcedureNode, std::vector<const ProcedureNode*>>({selectC_.get()}));
    rdfBCNormalisation->addNode(std::make_shared<OperateSphericalShellNormaliseProcedureNode>());
    analyser_.addRootSequenceNode(processBC_);

    // Process1D: 'ANGLE(ABC)'
    processAngle_ = std::make_shared<Process1DProcedureNode>(collectABC_);
    processAngle_->setName("Angle(ABC)");
    processAngle_->setKeyword<std::string>("LabelValue", "Normalised Frequency");
    processAngle_->setKeyword<std::string>("LabelX", "\\symbol{theta}, \\symbol{degrees}");
    std::shared_ptr<SequenceProcedureNode> angleNormalisation = processAngle_->addNormalisationBranch();
    angleNormalisation->addNode(std::make_shared<OperateExpressionProcedureNode>("value/sin(x)"));
    angleNormalisation->addNode(std::make_shared<OperateNormaliseProcedureNode>(1.0));
    analyser_.addRootSequenceNode(processAngle_);

    // Process2D: 'DAngle (A-B)-C'
    processDAngleAB_ = std::make_shared<Process2DProcedureNode>(collectDAngleAB_);
    processDAngleAB_->setName("DAngle((A-B)-C)");
    processDAngleAB_->setKeyword<std::string>("LabelValue", "g\\sub{AB}(r)");
    processDAngleAB_->setKeyword<std::string>("LabelX", "r, \\symbol{Angstrom}");
    processDAngleAB_->setKeyword<std::string>("LabelY", "\\symbol{theta}, \\symbol{degrees}");
    std::shared_ptr<SequenceProcedureNode> dAngleABNormalisation = processDAngleAB_->addNormalisationBranch();
    dAngleABNormalisation->addNode(std::make_shared<OperateExpressionProcedureNode>("value/sin(y)"));
    dAngleABNormalisation->addNode(std::make_shared<OperateNormaliseProcedureNode>(1.0));
    analyser_.addRootSequenceNode(processDAngleAB_);

    // Process2D: 'DAngle A-(B-C)'
    processDAngleBC_ = std::make_shared<Process2DProcedureNode>(collectDAngleBC_);
    processDAngleBC_->setName("DAngle(A-(B-C))");
    processDAngleBC_->setKeyword<std::string>("LabelValue", "g\\sub{BC}(r)");
    processDAngleBC_->setKeyword<std::string>("LabelX", "r, \\symbol{Angstrom}");
    processDAngleBC_->setKeyword<std::string>("LabelY", "\\symbol{theta}, \\symbol{degrees}");
    std::shared_ptr<SequenceProcedureNode> dAngleBCNormalisation = processDAngleBC_->addNormalisationBranch();
    dAngleBCNormalisation->addNode(std::make_shared<OperateExpressionProcedureNode>("value/sin(y)"));
    dAngleBCNormalisation->addNode(std::make_shared<OperateNormaliseProcedureNode>(1.0));
    analyser_.addRootSequenceNode(processDAngleBC_);

    /*
     * Keywords (including those exposed from the ProcedureNodes)
     */

    // Control
    keywords_.add(
        "Control",
        new Vec3DoubleKeyword(Vec3<double>(0.0, 10.0, 0.05), Vec3<double>(0.0, 0.0, 1.0e-5), Vec3Labels::MinMaxBinwidthlabels),
        "RangeAB", "Range (min, max, binwidth) of A-B distance axis", "<min> <max> <binwidth> (Angstroms)");
    keywords_.add(
        "Control",
        new Vec3DoubleKeyword(Vec3<double>(0.0, 10.0, 0.05), Vec3<double>(0.0, 0.0, 1.0e-5), Vec3Labels::MinMaxBinwidthlabels),
        "RangeBC", "Range (min, max, binwidth) of B-C distance axis", "<min> <max> <binwidth> (Angstroms)");
    keywords_.add(
        "Control",
        new Vec3DoubleKeyword(Vec3<double>(0.0, 180.0, 1.0), Vec3<double>(0.0, 0.0, 1.0e-5), Vec3Labels::MinMaxBinwidthlabels),
        "AngleRange", "Range (min, max, binwidth) of angle axis", "<min> <max> <binwidth> (degrees)");
    keywords_.link("Control", selectA_->keywords().find("Site"), "SiteA",
                   "Add site(s) which represent 'A' in the interaction A-B-C", "<Species> <Site> [<Species> <Site> ... ]");
    keywords_.link("Control", selectB_->keywords().find("Site"), "SiteB",
                   "Add site(s) which represent 'B' in the interaction A-B-C", "<Species> <Site> [<Species> <Site> ... ]");
    keywords_.link("Control", selectC_->keywords().find("Site"), "SiteC",
                   "Add site(s) which represent 'C' in the interaction A-B-C", "<Species> <Site> [<Species> <Site> ... ]");
    keywords_.add("Control", new BoolKeyword(false), "ExcludeSameMoleculeAB",
                  "Whether to exclude correlations between A and B sites on the same molecule", "<True|False>");
    keywords_.add("Control", new BoolKeyword(false), "ExcludeSameMoleculeBC",
                  "Whether to exclude correlations between B and C sites on the same molecule", "<True|False>");
    keywords_.add("Control", new BoolKeyword(false), "ExcludeSameSiteAC",
                  "Whether to exclude correlations between A and C sites on the same molecule", "<True|False>");

    // Export
    keywords_.link("Export", processAB_->keywords().find("Export"), "ExportAB",
                   "File format and file name under which to save calculated A-B RDF data");
    keywords_.link("Export", processBC_->keywords().find("Export"), "ExportBC",
                   "File format and file name under which to save calculated B-C RDF data");
    keywords_.link("Export", processAngle_->keywords().find("Export"), "ExportAngle",
                   "File format and file name under which to save calculated A-B-C angle histogram");
    keywords_.link("Export", processDAngleAB_->keywords().find("Export"), "ExportDAngleAB",
                   "File format and file name under which to save calculated (A-B)-C distance-angle map");
    keywords_.link("Export", processDAngleBC_->keywords().find("Export"), "ExportDAngleBC",
                   "File format and file name under which to save calculated A-(B-C) distance-angle map");
}
