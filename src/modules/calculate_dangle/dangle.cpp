// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/calculate_dangle/dangle.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileandformat.h"
#include "keywords/speciessitevector.h"
#include "keywords/vec3double.h"
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

CalculateDAngleModule::CalculateDAngleModule() : Module("CalculateDAngle"), analyser_(ProcedureNode::AnalysisContext)
{
    try
    {
        // Select: Site 'A'
        selectA_ = analyser_.createRootNode<SelectProcedureNode>("A");
        auto forEachA = selectA_->addForEachBranch(ProcedureNode::AnalysisContext);

        // -- Select: Site 'B'
        selectB_ = forEachA->create<SelectProcedureNode>("B");
        selectB_->keywords().set("SameMoleculeAsSite", selectA_);
        auto forEachB = selectB_->addForEachBranch(ProcedureNode::AnalysisContext);

        // -- -- Select: Site 'C'
        selectC_ = forEachB->create<SelectProcedureNode>("C");
        selectC_->keywords().set("ExcludeSameMolecule", std::vector<std::shared_ptr<const SelectProcedureNode>>{selectA_});
        auto forEachC = selectC_->addForEachBranch(ProcedureNode::AnalysisContext);

        // -- -- -- Calculate: 'rBC'
        auto calcDistance = forEachC->create<CalculateDistanceProcedureNode>({}, selectB_, selectC_);

        // -- -- -- Calculate: 'aABC'
        auto calcAngle = forEachC->create<CalculateAngleProcedureNode>({}, selectA_, selectB_, selectC_);

        // -- -- -- Collect2D:  'Distance-Angle(B...C vs A-B...C)'
        collectDAngle_ =
            forEachC->create<Collect2DProcedureNode>({}, calcDistance, calcAngle, 0.0, 10.0, 0.05, 0.0, 180.0, 1.0);
        auto subCollection = collectDAngle_->addSubCollectBranch(ProcedureNode::AnalysisContext);

        // -- -- -- -- Collect1D:  'RDF(BC)'
        collectDistance_ = subCollection->create<Collect1DProcedureNode>({}, calcDistance, 0.0, 10.0, 0.05);

        // -- -- -- -- Collect1D:  'ANGLE(ABC)'
        collectAngle_ = subCollection->create<Collect1DProcedureNode>({}, calcAngle, 0.0, 180.0, 1.0);

        // Process1D: 'RDF(BC)'
        processDistance_ = analyser_.createRootNode<Process1DProcedureNode>("RDF(BC)", collectDistance_);
        processDistance_->keywords().set("LabelValue", std::string("g(r)"));
        processDistance_->keywords().set("LabelX", std::string("r, \\symbol{Angstrom}"));

        auto rdfNormalisation = processDistance_->addNormalisationBranch();
        rdfNormalisation->create<OperateSitePopulationNormaliseProcedureNode>(
            {}, std::vector<std::shared_ptr<const SelectProcedureNode>>{selectA_, selectB_});
        rdfNormalisation->create<OperateNumberDensityNormaliseProcedureNode>(
            {}, std::vector<std::shared_ptr<const SelectProcedureNode>>{selectC_});
        rdfNormalisation->create<OperateSphericalShellNormaliseProcedureNode>({});

        // Process1D: 'ANGLE(ABC)'
        processAngle_ = analyser_.createRootNode<Process1DProcedureNode>("Angle(ABC)", collectAngle_);
        processAngle_->keywords().set("LabelValue", std::string("Normalised Frequency"));
        processAngle_->keywords().set("LabelX", std::string("\\symbol{theta}, \\symbol{degrees}"));
        auto angleNormalisation = processAngle_->addNormalisationBranch();
        angleNormalisation->create<OperateExpressionProcedureNode>({}, "value/sin(x)");
        angleNormalisation->create<OperateNormaliseProcedureNode>({}, 1.0);

        // Process2D: 'DAngle'
        processDAngle_ = analyser_.createRootNode<Process2DProcedureNode>("DAngle(A-BC)", collectDAngle_);
        processDAngle_->keywords().set("LabelValue", std::string("g(r)"));
        processDAngle_->keywords().set("LabelX", std::string("r, \\symbol{Angstrom}"));
        processDAngle_->keywords().set("LabelY", std::string("\\symbol{theta}, \\symbol{degrees}"));
        auto dAngleNormalisation = processDAngle_->addNormalisationBranch();
        dAngleNormalisation->create<OperateExpressionProcedureNode>({}, "value/sin(y)");
        dAngleNormalisation->create<OperateNormaliseProcedureNode>({}, 1.0);
    }
    catch (...)
    {
        Messenger::error("Failed to create analysis procedure for module '{}'\n", uniqueName_);
    }

    /*
     * Keywords
     */

    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

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
