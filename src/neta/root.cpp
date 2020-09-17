/*
    *** NETA Root Node
    *** src/neta/root.cpp
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

#include "neta/root.h"
#include "classes/speciesatom.h"
#include "data/ffatomtype.h"
#include "templates/dynamicarray.h"
#include "templates/refdatalist.h"

NETARootNode::NETARootNode(NETADefinition *parent) : NETANode(parent, NETANode::RootNode)
{
    nBondsValue_ = -1;
    nBondsValueOperator_ = NETANode::EqualTo;
    nHydrogensValue_ = -1;
    nHydrogensValueOperator_ = NETANode::EqualTo;
}

NETARootNode::~NETARootNode() {}

/*
 * Modifiers
 */

// Return enum options for NETARootModifiers
EnumOptions<NETARootNode::NETARootModifier> NETARootNode::modifiers()
{
    static EnumOptionsList ModifierOptions = EnumOptionsList()
                                             << EnumOption(NBondsModifier, "nbonds") << EnumOption(NHydrogensModifier, "nh");

    static EnumOptions<NETARootNode::NETARootModifier> options("RootModifier", ModifierOptions);

    return options;
}

// Return whether the specified modifier is valid for this node
bool NETARootNode::isValidModifier(std::string_view s) const { return (modifiers().isValid(s)); }

// Set value and comparator for specified modifier
bool NETARootNode::setModifier(std::string_view modifier, ComparisonOperator op, int value)
{
    // Check that the supplied index is valid
    if (!modifiers().isValid(modifier))
        return Messenger::error("Invalid modifier '{}' passed to NETARootNode.\n", modifier);

    switch (modifiers().enumeration(modifier))
    {
        case (NETARootNode::NBondsModifier):
            nBondsValue_ = value;
            nBondsValueOperator_ = op;
            break;
        case (NETARootNode::NHydrogensModifier):
            nHydrogensValue_ = value;
            nHydrogensValueOperator_ = op;
            break;
        default:
            return Messenger::error("Don't know how to handle modifier '{}' in root node.\n", modifier);
    }

    return true;
}

/*
 * Scoring
 */

// Evaluate the node and return its score
int NETARootNode::score(const SpeciesAtom *i, RefList<const SpeciesAtom> &matchPath) const
{
    auto totalScore = 0;

    // Check any specified modifier values
    if (nBondsValue_ >= 0 && (!compareValues(i->nBonds(), nBondsValueOperator_, nBondsValue_)))
        return NETANode::NoMatch;
    else
        ++totalScore;
    if (nHydrogensValue_ >= 0)
    {
        // Count number of hydrogens attached to this atom
        auto nH = std::count_if(i->bonds().begin(), i->bonds().end(),
                                [i](const SpeciesBond &bond) { return bond.partner(i)->element()->Z() == ELEMENT_H; });
        if (!compareValues(nH, nHydrogensValueOperator_, nHydrogensValue_))
            return NETANode::NoMatch;

        ++totalScore;
    }

    // Process branch definition via the base class
    auto branchScore = NETANode::score(i, matchPath);
    if (branchScore == NETANode::NoMatch)
        return NETANode::NoMatch;

    totalScore += branchScore;

    return totalScore;
}
