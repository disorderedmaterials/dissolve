// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/calculate_angle/angle.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileandformat.h"
#include "keywords/speciessitevector.h"
#include "keywords/vec3double.h"
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

CalculateAngleModule::CalculateAngleModule() : Module("CalculateAngle"), analyser_(ProcedureNode::AnalysisContext)
{
    try
    {
        // Select: Site 'A'
        selectA_ = analyser_.createRootNode<SelectProcedureNode>("A");
        auto forEachA = selectA_->addForEachBranch(ProcedureNode::AnalysisContext);

        // -- Select: Site 'B'
        selectB_ = forEachA->create<SelectProcedureNode>("B");
        auto forEachB = selectB_->addForEachBranch(ProcedureNode::AnalysisContext);

        // -- -- Calculate: 'rAB'
        auto calcAB = forEachB->create<CalculateDistanceProcedureNode>("rAB", selectA_, selectB_);

        // -- -- Collect1D:  'RDF(AB)'
        collectAB_ = forEachB->create<Collect1DProcedureNode>({}, calcAB, rangeAB_.x, rangeAB_.y, rangeAB_.z);

        // -- -- Select: Site 'C'
        selectC_ = forEachB->create<SelectProcedureNode>("C");
        auto forEachC = selectC_->addForEachBranch(ProcedureNode::AnalysisContext);

        // -- -- -- Calculate: 'rBC'
        auto calcBC = forEachC->create<CalculateDistanceProcedureNode>({}, selectB_, selectC_);

        // -- -- -- Calculate: 'aABC'
        calculateAngle_ = forEachC->create<CalculateAngleProcedureNode>({}, selectA_, selectB_, selectC_);
        calculateAngle_->keywords().set("Symmetric", symmetric_);

        // -- -- -- Collect1D:  'RDF(BC)'
        collectBC_ = forEachC->create<Collect1DProcedureNode>({}, calcBC, rangeBC_.x, rangeBC_.y, rangeBC_.z);

        // -- -- -- Collect1D:  'ANGLE(ABC)'
        collectABC_ =
            forEachC->create<Collect1DProcedureNode>({}, calculateAngle_, angleRange_.x, angleRange_.y, angleRange_.z);

        // -- -- -- Collect2D:  'DAngle (A-B)-C'
        collectDAngleAB_ = forEachC->create<Collect2DProcedureNode>({}, calcAB, calculateAngle_, rangeAB_.x, rangeAB_.y,
                                                                    rangeAB_.z, angleRange_.x, angleRange_.y, angleRange_.z);

        // -- -- -- Collect2D:  'DAngle A-(B-C)'
        collectDAngleBC_ = forEachC->create<Collect2DProcedureNode>({}, calcBC, calculateAngle_, rangeBC_.x, rangeBC_.y,
                                                                    rangeBC_.z, angleRange_.x, angleRange_.y, angleRange_.z);

        // -- -- -- Collect3D:  'rAB vs rBC vs aABC'
        collectDDA_ = forEachC->create<Collect3DProcedureNode>({}, calcAB, calcBC, calculateAngle_, rangeAB_.x, rangeAB_.y,
                                                               rangeAB_.z, rangeBC_.x, rangeBC_.y, rangeBC_.z, angleRange_.x,
                                                               angleRange_.y, angleRange_.z);

        // Process1D: 'RDF(AB)'
        processAB_ = analyser_.createRootNode<Process1DProcedureNode>("RDF(AB)", collectAB_);
        processAB_->keywords().set("LabelValue", std::string("g\\sub{AB}(r)"));
        processAB_->keywords().set("LabelX", std::string("r, \\symbol{Angstrom}"));
        auto rdfABNormalisation = processAB_->addNormalisationBranch();
        rdfABNormalisation->create<OperateSitePopulationNormaliseProcedureNode>(
            {}, ConstNodeVector<SelectProcedureNode>({selectA_}));
        rdfABNormalisation->create<OperateNumberDensityNormaliseProcedureNode>(
            {}, ConstNodeVector<SelectProcedureNode>({selectB_}));
        rdfABNormalisation->create<OperateSphericalShellNormaliseProcedureNode>({});

        // Process1D: 'RDF(BC)'
        processBC_ = analyser_.createRootNode<Process1DProcedureNode>("RDF(BC)", collectBC_);
        processBC_->keywords().set("LabelValue", std::string("g\\sub{BC}(r)"));
        processBC_->keywords().set("LabelX", std::string("r, \\symbol{Angstrom}"));
        auto rdfBCNormalisation = processBC_->addNormalisationBranch();
        rdfBCNormalisation->create<OperateSitePopulationNormaliseProcedureNode>(
            {}, ConstNodeVector<SelectProcedureNode>({selectB_, selectA_}));
        rdfBCNormalisation->create<OperateNumberDensityNormaliseProcedureNode>(
            {}, ConstNodeVector<SelectProcedureNode>({selectC_}));
        rdfBCNormalisation->create<OperateSphericalShellNormaliseProcedureNode>({});

        // Process1D: 'ANGLE(ABC)'
        processAngle_ = analyser_.createRootNode<Process1DProcedureNode>("Angle(ABC)", collectABC_);
        processAngle_->keywords().set("LabelValue", std::string("Normalised Frequency"));
        processAngle_->keywords().set("LabelX", std::string("\\symbol{theta}, \\symbol{degrees}"));
        auto angleNormalisation = processAngle_->addNormalisationBranch();
        angleNormalisation->create<OperateExpressionProcedureNode>({}, "value/sin(x)");
        angleNormalisation->create<OperateNormaliseProcedureNode>({}, 1.0);

        // Process2D: 'DAngle (A-B)-C'
        processDAngleAB_ = analyser_.createRootNode<Process2DProcedureNode>("DAngle((A-B)-C)", collectDAngleAB_);
        processDAngleAB_->keywords().set("LabelValue", std::string("g\\sub{AB}(r)"));
        processDAngleAB_->keywords().set("LabelX", std::string("r, \\symbol{Angstrom}"));
        processDAngleAB_->keywords().set("LabelY", std::string("\\symbol{theta}, \\symbol{degrees}"));
        auto dAngleABNormalisation = processDAngleAB_->addNormalisationBranch();
        dAngleABNormalisationExpression_ = dAngleABNormalisation->create<OperateExpressionProcedureNode>(
            {}, fmt::format("{} * value/sin(y)/sin(yDelta)", symmetric_ ? 1.0 : 2.0));
        dAngleABNormalisation->create<OperateSitePopulationNormaliseProcedureNode>(
            {}, ConstNodeVector<SelectProcedureNode>({selectA_, selectC_}));
        dAngleABNormalisation->create<OperateNumberDensityNormaliseProcedureNode>(
            {}, ConstNodeVector<SelectProcedureNode>({selectB_}));
        dAngleABNormalisation->create<OperateSphericalShellNormaliseProcedureNode>({});

        // Process2D: 'DAngle A-(B-C)'
        processDAngleBC_ = analyser_.createRootNode<Process2DProcedureNode>("DAngle(A-(B-C))", collectDAngleBC_);
        processDAngleBC_->keywords().set("LabelValue", std::string("g\\sub{BC}(r)"));
        processDAngleBC_->keywords().set("LabelX", std::string("r, \\symbol{Angstrom}"));
        processDAngleBC_->keywords().set("LabelY", std::string("\\symbol{theta}, \\symbol{degrees}"));
        auto dAngleBCNormalisation = processDAngleBC_->addNormalisationBranch();
        dAngleBCNormalisationExpression_ = dAngleBCNormalisation->create<OperateExpressionProcedureNode>(
            {}, fmt::format("{} * value/sin(y)/sin(yDelta)", symmetric_ ? 1.0 : 2.0));
        dAngleBCNormalisation->create<OperateSitePopulationNormaliseProcedureNode>(
            {}, ConstNodeVector<SelectProcedureNode>({selectB_, selectA_}));
        dAngleBCNormalisation->create<OperateNumberDensityNormaliseProcedureNode>(
            {}, ConstNodeVector<SelectProcedureNode>({selectC_}));
        dAngleBCNormalisation->create<OperateSphericalShellNormaliseProcedureNode>({});
    }
    catch (...)
    {
        Messenger::error("Failed to create analysis procedure for module '{}'\n", name_);
    }

    /*
     * Keywords
     */

    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    // Control
    keywords_.add<Vec3DoubleKeyword>("Control", "RangeAB", "Range (min, max, binwidth) of A-B distance binning", rangeAB_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("Control", "RangeBC", "Range (min, max, binwidth) of B-C distance binning", rangeBC_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("Control", "AngleRange", "Range (min, max, binwidth) of angle binning", angleRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<SpeciesSiteVectorKeyword>("Control", "SiteA", "Specify site(s) which represent 'A' in the interaction A-B-C",
                                            selectA_->speciesSites(), selectA_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>("Control", "SiteB", "Specify site(s) which represent 'B' in the interaction A-B-C",
                                            selectB_->speciesSites(), selectB_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>("Control", "SiteC", "Specify site(s) which represent 'C' in the interaction A-B-C",
                                            selectC_->speciesSites(), selectC_->axesRequired());
    keywords_.add<BoolKeyword>("Control", "ExcludeSameMoleculeAB",
                               "Whether to exclude correlations between A and B sites on the same molecule",
                               excludeSameMoleculeAB_);
    keywords_.add<BoolKeyword>("Control", "ExcludeSameMoleculeBC",
                               "Whether to exclude correlations between B and C sites on the same molecule",
                               excludeSameMoleculeBC_);
    keywords_.add<BoolKeyword>("Control", "ExcludeSameSiteAC",
                               "Whether to exclude correlations between A and C sites on the same molecule",
                               excludeSameSiteAC_);
    keywords_.add<BoolKeyword>("Control", "Symmetric",
                               "Whether the calculated angle should be mapped to 0 - 90 (i.e. is symmetric about 90)",
                               symmetric_);

    // Export
    keywords_.add<FileAndFormatKeyword>("Export", "ExportAB",
                                        "File format and file name under which to save calculated A-B RDF data",
                                        processAB_->exportFileAndFormat(), "EndExportAB");
    keywords_.add<FileAndFormatKeyword>("Export", "ExportBC",
                                        "File format and file name under which to save calculated B-C RDF data",
                                        processBC_->exportFileAndFormat(), "EndExportBC");
    keywords_.add<FileAndFormatKeyword>("Export", "ExportAngle",
                                        "File format and file name under which to save calculated A-B-C angle histogram",
                                        processAngle_->exportFileAndFormat(), "EndExportAngle");
    keywords_.add<FileAndFormatKeyword>("Export", "ExportDAngleAB",
                                        "File format and file name under which to save calculated (A-B)-C distance-angle map",
                                        processDAngleAB_->exportFileAndFormat(), "EndExportDAngleAB");
    keywords_.add<FileAndFormatKeyword>("Export", "ExportDAngleBC",
                                        "File format and file name under which to save calculated A-(B-C) distance-angle map",
                                        processDAngleBC_->exportFileAndFormat(), "EndExportDAngleBC");
}
