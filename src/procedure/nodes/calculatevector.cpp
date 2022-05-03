// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/calculatevector.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/bool.h"
#include "procedure/nodes/select.h"

CalculateVectorProcedureNode::CalculateVectorProcedureNode(std::shared_ptr<SelectProcedureNode> site0,
                                                           std::shared_ptr<SelectProcedureNode> site1, bool rotateIntoFrame)
    : CalculateProcedureNodeBase(ProcedureNode::NodeType::CalculateVector, site0, site1), rotateIntoFrame_(rotateIntoFrame)
{
    keywords_.add<NodeKeyword<SelectProcedureNode>>("Control", "I", "Site that represents 'i' in the vector i->j", sites_[0],
                                                    this, ProcedureNode::NodeType::Select, true);
    keywords_.add<NodeKeyword<SelectProcedureNode>>("Control", "J", "Site that represents 'j' in the vector i->j", sites_[1],
                                                    this, ProcedureNode::NodeType::Select, true);
    keywords_.add<BoolKeyword>("Control", "RotateIntoFrame",
                               "Whether to rotate the calculated vector into the local frame defined on 'I'", rotateIntoFrame_);
}

/*
 * Observable Target
 */

// Return number of sites required to calculate observable
int CalculateVectorProcedureNode::nSitesRequired() const { return 2; }

// Return dimensionality of calculated observable
int CalculateVectorProcedureNode::dimensionality() const { return 3; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool CalculateVectorProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    // Call the base class function
    if (!CalculateProcedureNodeBase::prepare(procedureContext))
        return false;

    return true;
}

// Execute node
bool CalculateVectorProcedureNode::execute(const ProcedureContext &procedureContext)
{
    assert(sites_[0] && sites_[0]->currentSite());
    assert(sites_[1] && sites_[1]->currentSite());

    // Determine the value of the observable
    value_ = procedureContext.configuration()->box()->minimumVector(sites_[0]->currentSite()->origin(),
                                                                    sites_[1]->currentSite()->origin());

    // Rotate the vector into the local frame defined on the first site?
    if (rotateIntoFrame_)
        value_ = sites_[0]->currentSite()->axes() * value_;

    return true;
}
