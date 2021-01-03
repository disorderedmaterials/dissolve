// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/process1d.h"

// Return Collect1DNode for A-B RDF
const Collect1DProcedureNode *CalculateRDFModule::collectDistanceNode() const { return collectDistance_; }

// Return SelectNode for site A
const SelectProcedureNode *CalculateRDFModule::selectANode() const { return selectA_; }

// Return Process1DNode result (i.e. RDF)
const Process1DProcedureNode *CalculateRDFModule::rdfResult() const
{
    if ((!processDistance_) || (!processDistance_->hasProcessedData()))
        return nullptr;

    return processDistance_;
}
