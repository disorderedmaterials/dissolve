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

    // -- Select: Site 'NF' - Network Former
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
    auto collectNFIndex = forEachNF.create<IntegerCollect1DProcedureNode>("NFBins", siteIndex, ProcedureNode::AnalysisContext);

    // Get number of BO within NF
    auto qIterator = analyser_.createRootNode<IterateData1DProcedureNode>("QIterator", collectNFIndex);
    auto &forEachQIterator = qIterator->branch()->get();
    auto value = forEachQIterator.create<CalculateExpressionProcedureNode>({});
    value->setExpression("QIterator.value");

    // Collect and process BO
    auto collectQ = forEachNF.create<IntegerCollect1DProcedureNode>("Bins", value, ProcedureNode::AnalysisContext);
    auto processNF = analyser_.createRootNode<Process1DProcedureNode>("Histogram", collectQ);
    auto &normalisation = processNF->branch()->get();
    normalisation.create<OperateNormaliseProcedureNode>({});

    // Percentage of BO
    auto percentBO = forEachBO.create<CalculateExpressionProcedureNode>({});
    percentBO->setExpression("NF.nSelected");
    auto collectBO = forEachBO.create<IntegerCollect1DProcedureNode>("NumberBO", percentBO, ProcedureNode::AnalysisContext);
    auto processPercent = analyser_.createRootNode<Process1DProcedureNode>("PercentBO", collectQ);
    auto &normalisationPercent = processNF->branch()->get();
    normalisationPercent.create<OperateNormaliseProcedureNode>({});
    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Set the site(s) 'A' which are to represent the origin of the RDF",
                                            selectBO_->speciesSites(), selectBO_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>(
        "SiteB", "Set the site(s) 'NF' for which the distribution around the origin sites 'A' should be calculated",
        selectNF_->speciesSites(), selectNF_->axesRequired());

    keywords_.setOrganisation("Options", "Ranges");
    keywords_.add<RangeKeyword>("DistanceRange",
                                "Distance range (min, max) over which to calculate coordination number from central site",
                                distanceRange_, 0.0, std::nullopt, Vec3Labels::MinMaxDeltaLabels);
    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("Export", "File format and file name under which to save calculated QSpecies data",
                                        processNF->exportFileAndFormat(), "EndExport");
}
