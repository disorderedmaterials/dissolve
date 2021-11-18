// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/calculate_sdf/sdf.h"
#include "procedure/nodes/calculatevector.h"
#include "procedure/nodes/collect3d.h"
#include "procedure/nodes/operategridnormalise.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/process3d.h"
#include "procedure/nodes/select.h"

// Perform any necessary initialisation for the Module
void CalculateSDFModule::initialise()
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
    selectA_ = new SelectProcedureNode({}, true);
    selectA_->setName("A");
    SequenceProcedureNode *forEachA = selectA_->addForEachBranch(ProcedureNode::AnalysisContext);
    analyser_.addRootSequenceNode(selectA_);

    // -- Select: Site 'B'
    selectB_ = new SelectProcedureNode();
    selectB_->setName("B");
    selectB_->keywords().set("ExcludeSameSite", std::vector<const SelectProcedureNode *>{selectA_});
    selectB_->keywords().set("ExcludeSameMolecule", std::vector<const SelectProcedureNode *>{selectA_});
    SequenceProcedureNode *forEachB = selectB_->addForEachBranch(ProcedureNode::AnalysisContext);
    forEachA->addNode(selectB_);

    // -- -- Calculate: 'v(B->A)'
    auto *calcVector = new CalculateVectorProcedureNode(selectA_, selectB_, true);
    forEachB->addNode(calcVector);

    // -- -- Collect3D: 'SDF'
    collectVector_ = new Collect3DProcedureNode(calcVector, -10.0, 10.0, 0.5, -10.0, 10.0, 0.5, -10.0, 10.0, 0.5);
    forEachB->addNode(collectVector_);

    // Process3D: @dataName
    processPosition_ = new Process3DProcedureNode(collectVector_);
    processPosition_->setName("SDF");
    processPosition_->keywords().set("LabelValue", std::string("\\symbol{rho}(x,y,z)"));
    processPosition_->keywords().set("LabelX", std::string("x, \\symbol{Angstrom}"));
    processPosition_->keywords().set("LabelY", std::string("y, \\symbol{Angstrom}"));
    processPosition_->keywords().set("LabelZ", std::string("z, \\symbol{Angstrom}"));
    SequenceProcedureNode *sdfNormalisation = processPosition_->addNormalisationBranch();
    sdfNormalisation->addNode(new OperateSitePopulationNormaliseProcedureNode({selectA_}));
    sdfNormalisation->addNode(new OperateGridNormaliseProcedureNode());
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
