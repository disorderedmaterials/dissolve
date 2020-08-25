/*
    *** Calculate RDF Module - Initialisation
    *** src/modules/calculate_rdf/init.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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

    // Calculation
    keywords_.add(
        "Calculation",
        new Vec3DoubleKeyword(Vec3<double>(0.0, 10.0, 0.05), Vec3<double>(0.0, 0.0, 1.0e-5), Vec3Labels::MinMaxDeltaLabels),
        "DistanceRange", "Range (min, max, delta) of distance axis", "<min> <max> <delta> (Angstroms)");

    // Sites
    keywords_.link("Sites", selectA_->keywords().find("Site"), "SiteA",
                   "Set the site(s) 'A' which are to represent the origin of the RDF", "<Species> <Site>");
    keywords_.link("Sites", selectB_->keywords().find("Site"), "SiteB",
                   "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated",
                   "<Species> <Site>");
    keywords_.add("Sites", new BoolKeyword(false), "ExcludeSameMolecule",
                  "Whether to exclude correlations between sites on the same molecule", "<True|False>");

    // Export
    keywords_.link("Export", processDistance_->keywords().find("Save"), "Save", "Whether to save calculated RDF to disk",
                   "<True|False>");
}
