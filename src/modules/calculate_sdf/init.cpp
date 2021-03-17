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
    selectA_ = new SelectProcedureNode(nullptr, true);
    selectA_->setName("A");
    SequenceProcedureNode *forEachA = selectA_->addForEachBranch(ProcedureNode::AnalysisContext);
    analyser_.addRootSequenceNode(selectA_);

    // -- Select: Site 'B'
    selectB_ = new SelectProcedureNode();
    selectB_->setName("B");
    RefList<SelectProcedureNode> exclusions(selectA_);
    selectB_->setKeyword<RefList<SelectProcedureNode> &>("ExcludeSameSite", exclusions);
    selectB_->setKeyword<RefList<SelectProcedureNode> &>("ExcludeSameMolecule", exclusions);
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
    processPosition_->setKeyword<std::string>("LabelValue", "\\symbol{rho}(x,y,z)");
    processPosition_->setKeyword<std::string>("LabelX", "x, \\symbol{Angstrom}");
    processPosition_->setKeyword<std::string>("LabelY", "y, \\symbol{Angstrom}");
    processPosition_->setKeyword<std::string>("LabelZ", "z, \\symbol{Angstrom}");
    SequenceProcedureNode *sdfNormalisation = processPosition_->addNormalisationBranch();
    sdfNormalisation->addNode(new OperateSitePopulationNormaliseProcedureNode(selectA_));
    sdfNormalisation->addNode(new OperateGridNormaliseProcedureNode());
    analyser_.addRootSequenceNode(processPosition_);

    /*
     * Keywords (including those exposed from the ProcedureNodes)
     */

    // Control
    keywords_.add("Control",
                  new Vec3DoubleKeyword(Vec3<double>(-10.0, 10.0, 0.5), Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                        Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels),
                  "RangeX", "Range along X axis", "<min> <max> <delta> (Angstroms)");
    keywords_.add("Control",
                  new Vec3DoubleKeyword(Vec3<double>(-10.0, 10.0, 0.5), Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                        Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels),
                  "RangeY", "Range along Y axis", "<min> <max> <delta> (Angstroms)");
    keywords_.add("Control",
                  new Vec3DoubleKeyword(Vec3<double>(-10.0, 10.0, 0.5), Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                        Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels),
                  "RangeZ", "Range along Z axis", "<min> <max> <delta> (Angstroms)");
    keywords_.link("Control", selectA_->keywords().find("Site"), "SiteA",
                   "Set the site(s) 'A' which are to represent the origin of the SDF", "<Species> <Site>");
    keywords_.link("Control", selectB_->keywords().find("Site"), "SiteB",
                   "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated",
                   "<Species> <Site>");
    keywords_.add("Control", new BoolKeyword(true), "ExcludeSameMolecule",
                  "Whether to exclude correlations between sites on the same molecule", "<True|False>");

    // Export
    keywords_.add("Export", new FileAndFormatKeyword(sdfFileAndFormat_, "EndExportSDF"), "ExportSDF",
                  "Save the SDF to the specified file / format");
}
