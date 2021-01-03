// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/calculatedistance.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/operatenumberdensitynormalise.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/operatesphericalshellnormalise.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/select.h"

// Perform any necessary initialisation for the Module
void CalculateRDFModule::initialise()
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
     *         Calculate  'rAB'
     *           Distance  'A'  'B'
     *         EndCalculate
     *         Collect1D  RDF
     *           QuantityX  'rAB'
     *           RangeX  0.0  10.0  0.05
     *         EndCollect1D
     *       EndForEach  'B'
     *     EndSelect  'B'
     *   EndForEach  'A'
     * EndSelect  'A'
     * Process1D  RDF
     *   Normalisation
     *     OperateSitePopulationNormalise
     *       Site  'A'
     *     EndOperateSitePopulationNormalise
     *     OperateNumberDensityNormalise
     *       Site  'B'
     *     EndOperateNumberDensityNormalise
     *     OperateSphericalShellNormalise
     *     EndOperateSphericalShellNormalise
     *   EndNormalisation
     *   LabelValue  'g(r)'
     *   LabelX  'r, Angstroms'
     * EndProcess1D
     */

    // Select: Site 'A'
    selectA_ = new SelectProcedureNode;
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

    // -- -- Calculate: 'rAB'
    auto *calcDistance = new CalculateDistanceProcedureNode(selectA_, selectB_);
    forEachB->addNode(calcDistance);

    // -- -- Collect1D: 'RDF'
    collectDistance_ = new Collect1DProcedureNode(calcDistance, 0.0, 10.0, 0.05);
    forEachB->addNode(collectDistance_);

    // Process1D: @dataName
    processDistance_ = new Process1DProcedureNode(collectDistance_);
    processDistance_->setName("RDF");
    processDistance_->setKeyword<std::string>("LabelValue", "g(r)");
    processDistance_->setKeyword<std::string>("LabelX", "r, \\symbol{Angstrom}");

    SequenceProcedureNode *rdfNormalisation = processDistance_->addNormalisationBranch();
    rdfNormalisation->addNode(new OperateSitePopulationNormaliseProcedureNode(selectA_));
    rdfNormalisation->addNode(new OperateNumberDensityNormaliseProcedureNode(selectB_));
    rdfNormalisation->addNode(new OperateSphericalShellNormaliseProcedureNode);
    analyser_.addRootSequenceNode(processDistance_);

    /*
     * Keywords (including those exposed from the ProcedureNodes)
     */

    // Control
    keywords_.add(
        "Control",
        new Vec3DoubleKeyword(Vec3<double>(0.0, 10.0, 0.05), Vec3<double>(0.0, 0.0, 1.0e-5), Vec3Labels::MinMaxDeltaLabels),
        "DistanceRange", "Range (min, max, delta) of distance axis", "<min> <max> <delta> (Angstroms)");
    keywords_.link("Control", selectA_->keywords().find("Site"), "SiteA",
                   "Set the site(s) 'A' which are to represent the origin of the RDF", "<Species> <Site>");
    keywords_.link("Control", selectB_->keywords().find("Site"), "SiteB",
                   "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated",
                   "<Species> <Site>");
    keywords_.add("Control", new BoolKeyword(false), "ExcludeSameMolecule",
                  "Whether to exclude correlations between sites on the same molecule", "<True|False>");

    // Export
    keywords_.link("Export", processDistance_->keywords().find("Save"), "Save", "Whether to save calculated RDF to disk",
                   "<True|False>");
}
