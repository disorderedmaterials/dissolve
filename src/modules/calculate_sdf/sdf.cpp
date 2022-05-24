// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/calculate_sdf/sdf.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileandformat.h"
#include "keywords/speciessitevector.h"
#include "keywords/vec3double.h"
#include "procedure/nodes/calculatevector.h"
#include "procedure/nodes/collect3d.h"
#include "procedure/nodes/operategridnormalise.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/process3d.h"
#include "procedure/nodes/select.h"

CalculateSDFModule::CalculateSDFModule() : Module("CalculateSDF"), analyser_(ProcedureNode::AnalysisContext)
{
    try
    {
        // Select: Site 'A'
        selectA_ = analyser_.createRootNode<SelectProcedureNode>("A", std::vector<const SpeciesSite *>{}, true);
        auto forEachA = selectA_->addForEachBranch(ProcedureNode::AnalysisContext);

        // -- Select: Site 'B'
        selectB_ = forEachA->create<SelectProcedureNode>("B");
        selectB_->keywords().set("ExcludeSameSite", ConstNodeVector<SelectProcedureNode>{selectA_});
        selectB_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{selectA_});
        auto forEachB = selectB_->addForEachBranch(ProcedureNode::AnalysisContext);

        // -- -- Calculate: 'v(B->A)'
        auto calcVector = forEachB->create<CalculateVectorProcedureNode>({}, selectA_, selectB_, true);

        // -- -- Collect3D: 'SDF'
        collectVector_ = forEachB->create<Collect3DProcedureNode>({}, calcVector, rangeX_.x, rangeX_.y, rangeX_.z, rangeY_.x,
                                                                  rangeY_.y, rangeY_.z, rangeZ_.x, rangeZ_.y, rangeZ_.z);

        // Process3D: @dataName
        processPosition_ = analyser_.createRootNode<Process3DProcedureNode>("SDF", collectVector_);
        processPosition_->keywords().set("LabelValue", std::string("\\symbol{rho}(x,y,z)"));
        processPosition_->keywords().set("LabelX", std::string("x, \\symbol{Angstrom}"));
        processPosition_->keywords().set("LabelY", std::string("y, \\symbol{Angstrom}"));
        processPosition_->keywords().set("LabelZ", std::string("z, \\symbol{Angstrom}"));
        auto sdfNormalisation = processPosition_->addNormalisationBranch();
        sdfNormalisation->create<OperateSitePopulationNormaliseProcedureNode>({},
                                                                              ConstNodeVector<SelectProcedureNode>({selectA_}));
        sdfNormalisation->create<OperateGridNormaliseProcedureNode>({});
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
    keywords_.add<Vec3DoubleKeyword>("Control", "RangeX", "Range along X axis", rangeX_, Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                     Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<Vec3DoubleKeyword>("Control", "RangeY", "Range along Y axis", rangeY_, Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                     Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<Vec3DoubleKeyword>("Control", "RangeZ", "Range along Z axis", rangeZ_, Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                     Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<SpeciesSiteVectorKeyword>("Control", "SiteA",
                                            "Set the site(s) 'A' which are to represent the origin of the SDF",
                                            selectA_->speciesSites(), selectA_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>(
        "Control", "SiteB", "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated",
        selectB_->speciesSites(), selectB_->axesRequired());
    keywords_.add<BoolKeyword>("Control", "ExcludeSameMolecule",
                               "Whether to exclude correlations between sites on the same molecule", excludeSameMolecule_);

    // Export
    keywords_.add<FileAndFormatKeyword>("Export", "ExportSDF", "Save the SDF to the specified file / format", sdfFileAndFormat_,
                                        "EndExportSDF");
}
