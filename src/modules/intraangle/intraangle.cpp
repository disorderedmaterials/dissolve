// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/intraangle/intraangle.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileandformat.h"
#include "keywords/speciessitevector.h"
#include "keywords/vec3double.h"
#include "procedure/nodes/calculateangle.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect3d.h"
#include "procedure/nodes/operateexpression.h"
#include "procedure/nodes/operatenormalise.h"
#include "procedure/nodes/operatenumberdensitynormalise.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/operatesphericalshellnormalise.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/process3d.h"
#include "procedure/nodes/select.h"

IntraAngleModule::IntraAngleModule() : Module("CalculateIntraAngle"), analyser_(ProcedureNode::AnalysisContext)
{
    try
    {
        // Select: Site 'A'
        selectA_ = analyser_.createRootNode<SelectProcedureNode>("A");
        auto &forEachA = selectA_->branch()->get();

        // -- Select: Site 'B'
        selectB_ = forEachA.create<SelectProcedureNode>("B");
        auto &forEachB = selectB_->branch()->get();

        // -- -- Select: Site 'C'
        selectC_ = forEachB.create<SelectProcedureNode>("C");
        auto &forEachC = selectC_->branch()->get();

        // -- -- -- Calculate: 'aABC'
        calculateAngle_ = forEachC.create<CalculateAngleProcedureNode>({}, selectA_, selectB_, selectC_);
        calculateAngle_->keywords().set("Symmetric", symmetric_);

        // -- -- -- Collect1D:  'ANGLE(ABC)'
        collectABC_ = forEachC.create<Collect1DProcedureNode>({}, calculateAngle_, ProcedureNode::AnalysisContext,
                                                              angleRange_.x, angleRange_.y, angleRange_.z);

        // Process1D: 'ANGLE(ABC)'
        processAngle_ = analyser_.createRootNode<Process1DProcedureNode>("Angle(ABC)", collectABC_);
        processAngle_->keywords().set("LabelValue", std::string("Normalised Frequency"));
        processAngle_->keywords().set("LabelX", std::string("\\symbol{theta}, \\symbol{degrees}"));
        auto &angleNormalisation = processAngle_->branch()->get();
        angleNormalisation.create<OperateExpressionProcedureNode>({}, "value/sin(x)");
        angleNormalisation.create<OperateNormaliseProcedureNode>({}, 1.0);
    }
    catch (...)
    {
        Messenger::error("Failed to create analysis procedure for module '{}'\n", name_);
    }

    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Specify site(s) which represent 'A' in the interaction A-B-C",
                                            selectA_->speciesSites(), selectA_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>("SiteB", "Specify site(s) which represent 'B' in the interaction A-B-C",
                                            selectB_->speciesSites(), selectB_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>("SiteC", "Specify site(s) which represent 'C' in the interaction A-B-C",
                                            selectC_->speciesSites(), selectC_->axesRequired());

    keywords_.setOrganisation("Options", "Ranges");
    keywords_.add<Vec3DoubleKeyword>("RangeAB", "Range (min, max, binwidth) of A-B distance binning", rangeAB_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("RangeBC", "Range (min, max, binwidth) of B-C distance binning", rangeBC_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("AngleRange", "Range (min, max, binwidth) of angle binning", angleRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);

    keywords_.setOrganisation("Options", "Control");
    keywords_.add<BoolKeyword>("ExcludeSameMoleculeAB",
                               "Whether to exclude correlations between A and B sites on the same molecule",
                               excludeSameMoleculeAB_);
    keywords_.add<BoolKeyword>("ExcludeSameMoleculeBC",
                               "Whether to exclude correlations between B and C sites on the same molecule",
                               excludeSameMoleculeBC_);
    keywords_.add<BoolKeyword>(
        "ExcludeSameSiteAC", "Whether to exclude correlations between A and C sites on the same molecule", excludeSameSiteAC_);
    keywords_.add<BoolKeyword>(
        "Symmetric", "Whether the calculated angle should be mapped to 0 - 90 (i.e. is symmetric about 90)", symmetric_);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("ExportAngle",
                                        "File format and file name under which to save calculated A-B-C angle histogram",
                                        processAngle_->exportFileAndFormat(), "EndExportAngle");
}