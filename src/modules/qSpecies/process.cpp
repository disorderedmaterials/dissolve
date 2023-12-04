// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "main/dissolve.h"
#include "module/context.h"
#include "modules/qSpecies/qSpecies.h"
#include "analyser/siteFilter.h"
#include "analyser/siteSelector.h"

// Run main processing
Module::ExecutionResult QSpeciesModule::process(ModuleContext &moduleContext)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Ensure any parameters in our nodes are set correctly
//    if (excludeSameMolecule_)
//        selectNF_->setSameMoleculeExclusions({selectBO_});
//    else
//        selectNF_->setSameMoleculeExclusions({});
//    selectNF_->keywords().set("InclusiveRange", distanceRange_);

    // Execute the analysis
//    if (!analyser_.execute({moduleContext.dissolve(), targetConfiguration_, name()}))
//    {
//        Messenger::error("Q Species experienced problems with its analysis.\n");
//        return ExecutionResult::Failed;
//    }

    // Select all potential bridging oxygen sites - we will determine which actually are
    // involved in NF-BO-NF interactions once we have the available NF sites
    SiteSelector allOxygenSites(targetConfiguration_, bridgingOxygenSites_);

    // Select all NF centres
    SiteSelector NF(targetConfiguration_, networkFormerSites_);

    // Filter the oxygen sites into those surrounded by exactly two NF sites
    SiteFilter filter(targetConfiguration_, allOxygenSites.sites());
    auto BO = filter.filterBySiteProximity(NF.sites(), distanceRange_, 2, 2);


    return ExecutionResult::Success;
}


/* OLD SETUP CODE HERE
 * // Select: Site 'BO' - Bonding Oxygen
selectBO_ = std::make_shared<SelectProcedureNode>("BO")
auto &forEachBO = selectBO_->branch()->get();

// Select: Site 'NF' - Network Former
selectNF_ = forEachBO.create<SelectProcedureNode>("NF");
selectNF_->keywords().set("ExcludeSameSite", ConstNodeVector<SelectProcedureNode>{selectBO_});
selectNF_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{selectBO_});
selectNF_->keywords().set("ReferenceSite", selectBO_);
selectNF_->keywords().set("InclusiveRange", distanceRange_);

// Get all BO in range
auto &forEachNF = selectNF_->branch()->get();
auto valuesInRange = forEachNF.create<IfValueInRangeProcedureNode>({});
valuesInRange->keywords().set("Value", NodeValueProxy("NF.nSelected"));
valuesInRange->keywords().set("ValidRange", Range(1.9, 2.1));
auto &ifThen = valuesInRange->branch()->get();

// Get site index of NF
auto siteIndex = ifThen.create<CalculateExpressionProcedureNode>({});
siteIndex->setExpression("NF.siteIndex");
auto collectNFIndex = ifThen.create<IntegerCollect1DProcedureNode>("NFBins", siteIndex, ProcedureNode::AnalysisContext);

// Collect / convert BO counts about each NF into Q species information
auto iterateBO = analyser_.createRootNode<IterateData1DProcedureNode>("BOIterator", collectNFIndex);
auto &forEachQIterator = iterateBO->branch()->get();
auto nBO = forEachQIterator.create<CalculateExpressionProcedureNode>({});
nBO->setExpression("BOIterator.value");
auto qConvert = forEachQIterator.create<IntegerCollect1DProcedureNode>("QBins", nBO, ProcedureNode::AnalysisContext);

// Count NFs around each potential BO site
auto nNF = forEachBO.create<CalculateExpressionProcedureNode>({});
nNF->setExpression("NF.nSelected");

// Process Q
auto processQ = analyser_.createRootNode<Process1DProcedureNode>("QSpecies", qConvert);
auto &normaliseQ = processQ->branch()->get();
normaliseQ.create<OperateNormaliseProcedureNode>({});
 */
