// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/calculatevector.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/bool.h"
#include "procedure/nodes/select.h"

CalculateVectorProcedureNode::CalculateVectorProcedureNode(SelectProcedureNode *site0, SelectProcedureNode *site1,
                                                           bool rotateIntoFrame)
    : CalculateProcedureNodeBase(ProcedureNode::NodeType::CalculateVector, site0, site1)
{
    // Create keywords - store the pointers to the superclasses for later use
    siteKeywords_[0] = new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::NodeType::Select, true, site0);
    keywords_.add("Control", siteKeywords_[0], "I", "Site that represents 'i' in the vector i->j");
    siteKeywords_[1] = new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::NodeType::Select, true, site1);
    keywords_.add("Control", siteKeywords_[1], "J", "Site that represents 'j' in the vector i->j");
    keywords_.add("Control", new BoolKeyword(rotateIntoFrame), "RotateIntoFrame",
                  "Whether to rotate the calculated vector into the local frame defined on 'I'");
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
bool CalculateVectorProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Call the base class function
    if (!CalculateProcedureNodeBase::prepare(cfg, prefix, targetList))
        return false;

    // Get orientation flag
    rotateIntoFrame_ = keywords_.asBool("RotateIntoFrame");

    return true;
}

// Execute node, targetting the supplied Configuration
bool CalculateVectorProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                           GenericList &targetList)
{
    assert(sites_[0] && sites_[0]->currentSite());
    assert(sites_[1] && sites_[1]->currentSite());

    // Determine the value of the observable
    value_ = cfg->box()->minimumVector(sites_[0]->currentSite()->origin(), sites_[1]->currentSite()->origin());

    // Rotate the vector into the local frame defined on the first site?
    if (rotateIntoFrame_)
        value_ = sites_[0]->currentSite()->axes() * value_;

    return true;
}
