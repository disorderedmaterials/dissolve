/*
    *** NETA Visitor
    *** src/neta/NETAVisitor.h
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

#ifndef DISSOLVE_NETA_VISITOR_H
#define DISSOLVE_NETA_VISITOR_H

#include "NETAParserVisitor.h"
#include "neta/neta.h"
#include <antlr4-runtime.h>

// NETA Visitor for ANTLR
class NETAVisitor : private NETAParserVisitor
{
    /*
     * Creation Entry-Point
     */
    private:
    // Target NETADefinition
    NETADefinition *neta_;
    // Encompassing forcefield for the definition (if any)
    const Forcefield *associatedForcefield_;
    // Context stack
    std::vector<NETANode *> contextStack_;
    // Parse error counter
    int errorCounter_;

    private:
    // Return the topmost context in the stack
    NETANode *currentNETAContext() const;

    public:
    // Construct description within supplied object, from given tree
    bool create(NETADefinition &neta, NETAParser::NetaContext *tree, const Forcefield *associatedForcefield = nullptr);

    /*
     * Visitor Overrides
     */
    private:
    antlrcpp::Any visitNeta(NETAParser::NetaContext *context) override;
    // Nodes
    antlrcpp::Any visitNode(NETAParser::NodeContext *context) override;
    antlrcpp::Any visitNodeSequence(NETAParser::NodeSequenceContext *context) override;
    antlrcpp::Any visitRingOnlyNode(NETAParser::RingOnlyNodeContext *context) override;
    antlrcpp::Any visitRingNodeSequence(NETAParser::RingNodeSequenceContext *context) override;
    antlrcpp::Any visitConnectionNode(NETAParser::ConnectionNodeContext *context) override;
    antlrcpp::Any visitPresenceNode(NETAParser::PresenceNodeContext *context) override;
    antlrcpp::Any visitRingNode(NETAParser::RingNodeContext *context) override;
    // Target List
    antlrcpp::Any visitElementOrType(NETAParser::ElementOrTypeContext *context) override;
    antlrcpp::Any visitTargetList(NETAParser::TargetListContext *context) override;
    // Context Modifiers
    antlrcpp::Any visitModifier(NETAParser::ModifierContext *context) override;
    antlrcpp::Any visitFlag(NETAParser::FlagContext *context) override;
};

#endif
