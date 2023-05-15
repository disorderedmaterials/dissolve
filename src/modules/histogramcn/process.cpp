// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "io/export/data1d.h"
#include "main/dissolve.h"
#include "modules/histogramcn/histogramcn.h"
#include "procedure/nodes/calculateexpression.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"
#include "procedure/nodes/sum1d.h"

// Run main processing
bool HistogramCNModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
        return Messenger::error("No configuration target set for module '{}'.\n", name());

    // Ensure any parameters in our nodes are set correctly
    if (excludeSameMolecule_)
        selectB_->setSameMoleculeExclusions({selectA_});
    else
        selectB_->setSameMoleculeExclusions({});

    // Execute the analysis
    ProcedureContext context(procPool, targetConfiguration_);
    context.setDataListAndPrefix(dissolve.processingModuleData(), name());
    if (!analyser_.execute(context))
        return Messenger::error("HistogramCN experienced problems with its analysis.\n");

    // Coordination Histogram
    selectA_ = analyser_.createRootNode<SelectProcedureNode>("A");

    auto &forEachA = selectA_->branch()->get();

    selectB_ = forEachA.create<SelectProcedureNode>("B");

    selectB_->keywords().set("ExcludeSameSite", ConstNodeVector<SelectProcedureNode>{selectA_});
    selectB_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{selectA_});

    selectB_->keywords().set("InclusiveRange", distanceRange_);

    auto calcExpression_ = forEachA.create<CalculateExpressionProcedureNode>({});
    calcExpression_->keywords().set("Expression", NodeValue("B.nSelected"));

    auto collectCN_ = forEachA.create<Collect1DProcedureNode>("Bins", calcExpression_);

    auto process1D = analyser_.createRootNode<Process1DProcedureNode>("Histogram", collectCN_);
}
