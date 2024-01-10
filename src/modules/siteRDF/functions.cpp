// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/siteRDF/siteRDF.h"
#include "procedure/nodes/process1D.h"
#include "procedure/nodes/sum1D.h"

// Return Collect1DNode for A-B RDF
std::shared_ptr<Collect1DProcedureNode> SiteRDFModule::collectDistanceNode() const { return collectDistance_; }

// Return SelectNode for site A
std::shared_ptr<SelectProcedureNode> SiteRDFModule::selectANode() const { return selectA_; }

// Return whether specified coordination number range is enabled
bool SiteRDFModule::isRangeEnabled(int id) const { return sumCN_ && sumCN_->rangeEnabled(id); }

// Return Process1DNode result (i.e. RDF)
std::shared_ptr<Process1DProcedureNode> SiteRDFModule::rdfResult() const
{
    if ((!processDistance_) || (!processDistance_->hasProcessedData()))
        return nullptr;

    return processDistance_;
}
