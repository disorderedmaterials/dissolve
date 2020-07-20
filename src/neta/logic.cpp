// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "neta/logic.h"

NETALogicNode::NETALogicNode(NETADefinition *parent, NETALogicNode::LogicType logic, NETANode *arg1, NETANode *arg2)
    : NETANode(parent, NETANode::LogicNode)
{
    // Private variables
    logic_ = logic;
    argument1_ = arg1;
    argument2_ = arg2;
};

NETALogicNode::~NETALogicNode() {}

/*
 * Scoring
 */

// Evaluate the node and return its score
int NETALogicNode::score(const SpeciesAtom *i, std::vector<const SpeciesAtom*> &matchPath) const
{
    int score1 = NETANode::NoMatch, score2 = NETANode::NoMatch, totalscore = NETANode::NoMatch;
    switch (logic_)
    {
        case (NETALogicNode::AndLogic):
            // Evaluate first argument
            score1 = argument1_->score(i, matchPath);
            if (score1 == NETANode::NoMatch)
                break;

            // Evaluate second argument
            score2 = argument2_->score(i, matchPath);
            if (score2 == NETANode::NoMatch)
                break;

            // Generate final score
            totalscore = score1 + score2;
            break;
        case (NETALogicNode::OrLogic):
            score1 = argument1_->score(i, matchPath);
            if (score1 != NETANode::NoMatch)
                totalscore = score1;
            else
            {
                score2 = argument2_->score(i, matchPath);
                if (score2 != NETANode::NoMatch)
                    totalscore = score2;
            }
            break;
        case (NETALogicNode::AndNotLogic):
            score1 = argument1_->score(i, matchPath);
            if (score1 != NETANode::NoMatch)
            {
                score2 = argument2_->score(i, matchPath);
                if (score2 == NETANode::NoMatch)
                    totalscore = score1;
            }
            break;
        default:
            Messenger::error("Unrecognised logic in NETA::score.\n");
            break;
    }

    // Check for reverse logic
    if (reverseLogic_)
        totalscore = (totalscore == NETANode::NoMatch ? 1 : NETANode::NoMatch);

    return totalscore;
}
