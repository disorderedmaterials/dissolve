// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/intraAngle/intraAngle.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"
#include "procedure/nodes/calculateAngle.h"
#include "procedure/nodes/collect1D.h"
#include "procedure/nodes/collect3D.h"
#include "procedure/nodes/operateExpression.h"
#include "procedure/nodes/operateNormalise.h"
#include "procedure/nodes/operateNumberDensityNormalise.h"
#include "procedure/nodes/operateSitePopulationNormalise.h"
#include "procedure/nodes/operateSphericalShellNormalise.h"
#include "procedure/nodes/process1D.h"
#include "procedure/nodes/process3D.h"
#include "procedure/nodes/select.h"

IntraAngleModule::IntraAngleModule() : Module(ModuleTypes::IntraAngle), analyser_(ProcedureNode::AnalysisContext)
{
    // Select: Site 'A'
    selectA_ = analyser_.createRootNode<SelectProcedureNode>("A");
    auto &forEachA = selectA_->branch()->get();

    // -- Select: Site 'B'
    selectB_ = forEachA.create<SelectProcedureNode>("B");
    selectB_->keywords().set("SameMoleculeAsSite", selectA_);
    selectB_->keywords().set("ExcludeSameSite", ConstNodeVector<SelectProcedureNode>{selectA_});
    auto &forEachB = selectB_->branch()->get();

    // -- -- Select: Site 'C'
    selectC_ = forEachB.create<SelectProcedureNode>("C");
    selectC_->keywords().set("SameMoleculeAsSite", selectA_);
    selectC_->keywords().set("ExcludeSameSite", ConstNodeVector<SelectProcedureNode>{selectA_, selectB_});
    auto &forEachC = selectC_->branch()->get();

    // -- -- -- Calculate: 'aABC'
    calculateAngle_ = forEachC.create<CalculateAngleProcedureNode>({}, selectA_, selectB_, selectC_);
    calculateAngle_->keywords().set("Symmetric", symmetric_);

    // -- -- -- Collect1D:  'ANGLE(ABC)'
    collectABC_ = forEachC.create<Collect1DProcedureNode>({}, calculateAngle_, ProcedureNode::AnalysisContext, angleRange_.x,
                                                          angleRange_.y, angleRange_.z);

    // Process1D: 'ANGLE(ABC)'
    processAngle_ = analyser_.createRootNode<Process1DProcedureNode>("NormalisedHistogram", collectABC_);
    processAngle_->keywords().set("LabelValue", std::string("Normalised Frequency"));
    processAngle_->keywords().set("LabelX", std::string("\\symbol{theta}, \\symbol{degrees}"));
    auto &angleNormalisation = processAngle_->branch()->get();
    angleNormalisation.create<OperateExpressionProcedureNode>({}, "value/sin(toRad(x))");
    angleNormalisation.create<OperateNormaliseProcedureNode>({}, 1.0);

    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites", "Specify sites defining the angle interaction A-B-C.");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Specify site(s) which represent 'A' in the interaction A-B-C",
                                            selectA_->speciesSites(), selectA_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>("SiteB", "Specify site(s) which represent 'B' in the interaction A-B-C",
                                            selectB_->speciesSites(), selectB_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>("SiteC", "Specify site(s) which represent 'C' in the interaction A-B-C",
                                            selectC_->speciesSites(), selectC_->axesRequired());

    keywords_.setOrganisation("Options", "Ranges", "Ranges over which to bin quantities from the calculation.");
    keywords_.add<Vec3DoubleKeyword>("RangeAB", "Range (min, max, binwidth) of A-B distance binning", rangeAB_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("RangeBC", "Range (min, max, binwidth) of B-C distance binning", rangeBC_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("AngleRange", "Range (min, max, binwidth) of angle binning", angleRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);

    keywords_.setOrganisation("Options", "Restrictions", "Options to restrict the angle range considered.");
    keywords_.add<BoolKeyword>(
        "Symmetric", "Whether the calculated angle should be mapped to 0 - 90 (i.e. is symmetric about 90)", symmetric_);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("ExportAngle",
                                        "File format and file name under which to save calculated A-B-C angle histogram",
                                        processAngle_->exportFileAndFormat(), "EndExportAngle");
}
