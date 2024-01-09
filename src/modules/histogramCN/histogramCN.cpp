// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/histogramCN/histogramCN.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/range.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"
#include "procedure/nodes/calculateExpression.h"
#include "procedure/nodes/integerCollect1D.h"
#include "procedure/nodes/operateNormalise.h"
#include "procedure/nodes/process1D.h"
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

    auto collectCN = forEachA.create<IntegerCollect1DProcedureNode>("Bins", calcExpression_, ProcedureNode::AnalysisContext);

    auto process1D = analyser_.createRootNode<Process1DProcedureNode>("Histogram", collectCN);
    auto &normalisation = process1D->branch()->get();
    auto norm = normalisation.create<OperateNormaliseProcedureNode>({});

    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites", "Specify sites defining the interaction A...B.");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Set the site(s) 'A' which are to represent the reference origin",
                                            selectA_->speciesSites(), selectA_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>(
        "SiteB", "Set the site(s) 'B' for which the coordination number around the origin sites should be calculated",
        selectB_->speciesSites(), selectB_->axesRequired());

    keywords_.setOrganisation("Options", "Ranges", "Range over which to bin quantities from the calculation.");
    keywords_.add<RangeKeyword>("DistanceRange",
                                "Distance range (min, max) over which to calculate coordination number from central site",
                                distanceRange_, 0.0, std::nullopt, Vec3Labels::MinMaxDeltaLabels);
    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("Export", "File format and file name under which to save calculated HitogramCN data",
                                        process1D->exportFileAndFormat(), "EndExport");
}
