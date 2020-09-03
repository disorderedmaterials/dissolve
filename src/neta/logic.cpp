/*
    *** NETA Logic Node
    *** src/neta/logic.cpp
    Copyright T. Youngs 2019-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
int NETALogicNode::score(const SpeciesAtom *i, RefList<const SpeciesAtom> &matchPath) const
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
