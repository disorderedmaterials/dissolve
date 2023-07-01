// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/histogramcn/histogramcn.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/range.h"
#include "keywords/speciessitevector.h"
#include "keywords/vec3double.h"
#include "procedure/nodes/calculateexpression.h"
#include "procedure/nodes/integercollect1d.h"
#include "procedure/nodes/operatenormalise.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/select.h"

HistogramCNModule::HistogramCNModule() : Module(ModuleTypes::HistogramCN), analyser_(ProcedureNode::AnalysisContext)
{
    // Select: Site 'A'
    selectA_ = analyser_.createRootNode<SelectProcedureNode>("A");
    auto &forEachA = selectA_->branch()->get();

    // -- Select: Site 'B'
    selectB_ = forEachA.create<SelectProcedureNode>("B");
    selectB_->keywords().set("ExcludeSameSite", ConstNodeVector<SelectProcedureNode>{selectA_});
    selectB_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{selectA_});
    selectB_->keywords().set("ReferenceSite", selectA_);
    selectB_->keywords().set("InclusiveRange", distanceRange_);

    // Coordination Histogram
    auto calcExpression_ = forEachA.create<CalculateExpressionProcedureNode>({});
    calcExpression_->setExpression("B.nSelected");

    auto collectCN_ =
        forEachA.create<IntegerCollect1DProcedureNode>("Bins", calcExpression_, ProcedureNode::AnalysisContext, 0, 10);

    auto process1D = analyser_.createRootNode<Process1DProcedureNode>("Histogram", collectCN_);
    auto &normalisation = process1D->branch()->get();
    auto norm = normalisation.create<OperateNormaliseProcedureNode>({});

    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Set the site(s) 'A' which are to represent the origin of the RDF",
                                            selectA_->speciesSites(), selectA_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>(
        "SiteB", "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated",
        selectB_->speciesSites(), selectB_->axesRequired());

    keywords_.setOrganisation("Options", "Ranges");
    keywords_.add<RangeKeyword>("DistanceRange",
                                "Distance range (min, max) over which to calculate coordination number from central site",
                                distanceRange_, 0.0, std::nullopt, Vec3Labels::MinMaxDeltaLabels);
}