// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileandformat.h"
#include "keywords/speciessitevector.h"
#include "keywords/vec3double.h"
#include "modules/calculate_sdf/sdf.h"
#include "procedure/nodes/calculatevector.h"
#include "procedure/nodes/collect3d.h"
#include "procedure/nodes/operategridnormalise.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/process3d.h"
#include "procedure/nodes/select.h"

CalculateSDFModule::CalculateSDFModule() : Module(), analyser_(ProcedureNode::AnalysisContext)
{
    /*
     * Assemble the following Procedure:
     *
     * Select  'A'
     *   Site  ...
     *   ForEach
     *     Select  'B'
     *       Site  ...
     *       ExcludeSameSite  'A'
     *       ExcludeSameMolecule  'A'
     *       ForEach
     *         CalculateVector  'vAB'
     *           I  'A'
     *           J  'B'
     *           RotateIntoFrame  True
     *         EndCalculate
     *         Collect3D  RDF
     *           QuantityX  'vAB'  1
     *           QuantityY  'vAB'  2
     *           QuantityZ  'vAB'  3
     *           RangeX  -10.0  10.0  0.5
     *           RangeY  -10.0  10.0  0.5
     *           RangeZ  -10.0  10.0  0.5
     *         EndCollect3D
     *       EndForEach  'B'
     *     EndSelect  'B'
     *   EndForEach  'A'
     * EndSelect  'A'
     * Process3D  SDF
     *   Normalisation
     *     OperateSitePopulationNormalise
     *       Site  'A'
     *     EndOperateSitePopulationNormalise
     *     OperateGridNormalise
     *     EndOperateGridNormalise
     *   EndNormalisation
     *   LabelValue  'rho(x,y,z)'
     *   LabelX  'x, Angstroms'
     *   LabelY  'y, Angstroms'
     *   LabelZ  'z, Angstroms'
     * EndProcess3D
     */

    // Select: Site 'A'
    selectA_ = std::make_shared<SelectProcedureNode, std::vector<const SpeciesSite *>, bool>({}, true);
    selectA_->setName("A");
    std::shared_ptr<SequenceProcedureNode> forEachA = selectA_->addForEachBranch(ProcedureNode::AnalysisContext);
    analyser_.addRootSequenceNode(selectA_);

    // -- Select: Site 'B'
    selectB_ = std::make_shared<SelectProcedureNode>();
    selectB_->setName("B");
    selectB_->keywords().set("ExcludeSameSite", std::vector<std::shared_ptr<const SelectProcedureNode>>{selectA_});
    selectB_->keywords().set("ExcludeSameMolecule", std::vector<std::shared_ptr<const SelectProcedureNode>>{selectA_});
    std::shared_ptr<SequenceProcedureNode> forEachB = selectB_->addForEachBranch(ProcedureNode::AnalysisContext);
    forEachA->addNode(selectB_);

    // -- -- Calculate: 'v(B->A)'
    auto calcVector = std::make_shared<CalculateVectorProcedureNode>(selectA_, selectB_, true);
    forEachB->addNode(calcVector);

    // -- -- Collect3D: 'SDF'
    collectVector_ = std::make_shared<Collect3DProcedureNode>(calcVector, -10.0, 10.0, 0.5, -10.0, 10.0, 0.5, -10.0, 10.0, 0.5);
    forEachB->addNode(collectVector_);

    // Process3D: @dataName
    processPosition_ = std::make_shared<Process3DProcedureNode>(collectVector_);
    processPosition_->setName("SDF");
    processPosition_->keywords().set("LabelValue", std::string("\\symbol{rho}(x,y,z)"));
    processPosition_->keywords().set("LabelX", std::string("x, \\symbol{Angstrom}"));
    processPosition_->keywords().set("LabelY", std::string("y, \\symbol{Angstrom}"));
    processPosition_->keywords().set("LabelZ", std::string("z, \\symbol{Angstrom}"));
    std::shared_ptr<SequenceProcedureNode> sdfNormalisation = processPosition_->addNormalisationBranch();
    sdfNormalisation->addNode(std::make_shared<OperateSitePopulationNormaliseProcedureNode>(
        std::vector<std::shared_ptr<const SelectProcedureNode>>({selectA_})));
    sdfNormalisation->addNode(std::make_shared<OperateGridNormaliseProcedureNode>());
    analyser_.addRootSequenceNode(processPosition_);

    /*
     * Keywords (including those exposed from the ProcedureNodes)
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

// Return type of module
std::string_view CalculateSDFModule::type() const { return "CalculateSDF"; }
