// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/intrardf/intrardf.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/sum1d.h"

// Return Collect1DNode for A-B RDF
std::shared_ptr<Collect1DProcedureNode> IntraRDFModule::collectDistanceNode() const { return collectDistance_; }

// Return SelectNode for site A
std::shared_ptr<SelectProcedureNode> IntraRDFModule::selectANode() const { return selectA_; }

// Return whether specified coordination number range is enabled
bool IntraRDFModule::isRangeEnabled(int id) const { return sumCN_ && sumCN_->rangeEnabled(id); }

// Return Process1DNode result (i.e. RDF)
std::shared_ptr<Process1DProcedureNode> IntraRDFModule::rdfResult() const
{
    if ((!processDistance_) || (!processDistance_->hasProcessedData()))
        return nullptr;

    return processDistance_;
}
