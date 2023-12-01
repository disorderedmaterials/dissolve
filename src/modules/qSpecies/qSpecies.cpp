// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/qSpecies/qSpecies.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/range.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"
#include "procedure/nodes/calculateExpression.h"
#include "procedure/nodes/ifValueInRange.h"
#include "procedure/nodes/integerCollect1D.h"
#include "procedure/nodes/iterateData1D.h"
#include "procedure/nodes/operateNormalise.h"
#include "procedure/nodes/process1D.h"
#include "procedure/nodes/select.h"

QSpeciesModule::QSpeciesModule() : Module(ModuleTypes::QSpecies), analyser_(ProcedureNode::AnalysisContext)
{
    // Select: Site 'BO' - Bonding Oxygen
    selectBO_ = analyser_.createRootNode<SelectProcedureNode>("BO");
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

    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites",
                              "Specify sites representing the bonding oxygen (BO) and network forming (NF) sites.");
    keywords_.add<SpeciesSiteVectorKeyword>("BondingOxygen", "Set the site(s) 'BO' which are to represent the bonding oxygen",
                                            selectBO_->speciesSites(), selectBO_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>(
        "NetworkFormer", "Set the site(s) 'NF' for which the distribution around the origin sites 'A' should be calculated",
        selectNF_->speciesSites(), selectNF_->axesRequired());

    keywords_.setOrganisation("Options", "Ranges", "Ranges over which to bin quantities from the calculation.");
    keywords_.add<RangeKeyword>("DistanceRange",
                                "Distance range (min, max) over which to calculate Q-Species from central site", distanceRange_,
                                0.0, std::nullopt, Vec3Labels::MinMaxDeltaLabels);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("Export", "File format and file name under which to save calculated QSpecies data",
                                        processQ->exportFileAndFormat(), "EndExport");
}
