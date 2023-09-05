// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/orientedSDF/orientedSDF.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/range.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"
#include "procedure/nodes/calculateAxisAngle.h"
#include "procedure/nodes/calculateVector.h"
#include "procedure/nodes/collect3D.h"
#include "procedure/nodes/ifValueInRange.h"
#include "procedure/nodes/operateGridNormalise.h"
#include "procedure/nodes/operateSitePopulationNormalise.h"
#include "procedure/nodes/process3D.h"
#include "procedure/nodes/select.h"

OrientedSDFModule::OrientedSDFModule() : Module(ModuleTypes::OrientedSDF), analyser_(ProcedureNode::AnalysisContext)
{
    // Select: Site 'A'
    selectA_ = analyser_.createRootNode<SelectProcedureNode>("A", std::vector<const SpeciesSite *>{},
                                                             ProcedureNode::AnalysisContext, true);
    auto &forEachA = selectA_->branch()->get();

    // -- Select: Site 'B'
    selectB_ =
        forEachA.create<SelectProcedureNode>("B", std::vector<const SpeciesSite *>{}, ProcedureNode::AnalysisContext, true);
    selectB_->keywords().set("ExcludeSameSite", ConstNodeVector<SelectProcedureNode>{selectA_});
    selectB_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{selectA_});
    auto &forEachB = selectB_->branch()->get();

    // -- -- Calculate: A-B axis angle
    calculateAxisAngle_ = forEachB.create<CalculateAxisAngleProcedureNode>("ABangle", selectA_, OrientedSite::XAxis, selectB_,
                                                                           OrientedSite::XAxis);
    calculateAxisAngle_->keywords().set("Symmetric", symmetric_);

    // -- -- IfValueInRange: Check axis angle
    checkAxisValue_ = forEachB.create<IfValueInRangeProcedureNode>({});
    checkAxisValue_->keywords().set("Value", NodeValueProxy("ABangle.theta"));
    auto &ifThen = checkAxisValue_->branch()->get();

    // -- -- -- Calculate: 'v(B->A)'
    auto calcVector = ifThen.create<CalculateVectorProcedureNode>({}, selectA_, selectB_, true);

    // -- -- -- Collect3D: 'OSDF'
    collectVector_ =
        ifThen.create<Collect3DProcedureNode>({}, calcVector, ProcedureNode::AnalysisContext, rangeX_, rangeY_, rangeZ_);

    // Process3D
    processPosition_ = analyser_.createRootNode<Process3DProcedureNode>("SDF", collectVector_);
    processPosition_->keywords().set("LabelValue", std::string("\\symbol{rho}(x,y,z)"));
    processPosition_->keywords().set("LabelX", std::string("x, \\symbol{Angstrom}"));
    processPosition_->keywords().set("LabelY", std::string("y, \\symbol{Angstrom}"));
    processPosition_->keywords().set("LabelZ", std::string("z, \\symbol{Angstrom}"));
    auto &sdfNormalisation = processPosition_->branch()->get();
    sdfNormalisation.create<OperateSitePopulationNormaliseProcedureNode>({}, ConstNodeVector<SelectProcedureNode>({selectA_}));
    sdfNormalisation.create<OperateGridNormaliseProcedureNode>({});

    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Set the site(s) 'A' which are to represent the origin of the SDF",
                                            selectA_->speciesSites(), selectA_->axesRequired());
    keywords_.add<EnumOptionsKeyword<OrientedSite::SiteAxis>>("AxisA", "Axis to use from site A", calculateAxisAngle_->axis(0),
                                                              OrientedSite::siteAxis());
    keywords_.add<SpeciesSiteVectorKeyword>(
        "SiteB", "Set the site(s) 'B' for which the distribution around the origin site(s) 'A' should be calculated",
        selectB_->speciesSites(), selectB_->axesRequired());
    keywords_.add<EnumOptionsKeyword<OrientedSite::SiteAxis>>("AxisB", "Axis to use from site B", calculateAxisAngle_->axis(1),
                                                              OrientedSite::siteAxis());

    keywords_.setOrganisation("Options", "Ranges");
    keywords_.add<RangeKeyword>("AngleRange", "Axis angle range required to permit a site to be binned in the SDF",
                                axisAngleRange_, Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<Vec3DoubleKeyword>("RangeX", "Range along X axis", rangeX_, Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                     Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<Vec3DoubleKeyword>("RangeY", "Range along Y axis", rangeY_, Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                     Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<Vec3DoubleKeyword>("RangeZ", "Range along Z axis", rangeZ_, Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                     Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels);

    keywords_.setOrganisation("Options", "Control");
    keywords_.add<BoolKeyword>("ExcludeSameMolecule", "Whether to exclude correlations between sites on the same molecule",
                               excludeSameMolecule_);
    keywords_.add<BoolKeyword>(
        "Symmetric", "Whether the calculated angle should be mapped to 0 - 90 (i.e. is symmetric about 90)", symmetric_);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("ExportSDF", "Save the SDF to the specified file / format", sdfFileAndFormat_,
                                        "EndExportSDF");
}
