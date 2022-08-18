// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/sum1d.h"

// Return Collect1DNode for A-B RDF
std::shared_ptr<Collect1DProcedureNode> CalculateRDFModule::collectDistanceNode() const { return collectDistance_; }

// Return SelectNode for site A
std::shared_ptr<SelectProcedureNode> CalculateRDFModule::selectANode() const { return selectA_; }

// Return whether specified coordination number range is enabled
bool CalculateRDFModule::isRangeEnabled(int id) const { return sumCN_ && sumCN_->rangeEnabled(id); }

// Return Process1DNode result (i.e. RDF)
std::shared_ptr<Process1DProcedureNode> CalculateRDFModule::rdfResult() const
{
    if ((!processDistance_) || (!processDistance_->hasProcessedData()))
        return nullptr;

    return processDistance_;
}
