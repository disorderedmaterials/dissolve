// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "NETAParserBaseVisitor.h"
#include "neta/neta.h"
#include "neta/node.h"
#include <antlr4-runtime.h>

// NETA Visitor for ANTLR
class NETAVisitor : private NETAParserBaseVisitor
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
    std::vector<std::shared_ptr<NETANode>> contextStack_;
    // Parse error counter
    int errorCounter_;

    private:
    // Return the topmost context in the stack
    NETANode *currentContext();

    public:
    // Construct description within supplied object, from given tree
    void create(NETADefinition &neta, NETAParser::NetaContext *tree, const Forcefield *associatedForcefield);

    /*
     * Visitor Overrides
     */
    private:
    // Nodes
    antlrcpp::Any visitNeta(NETAParser::NetaContext *context) override;
    antlrcpp::Any visitOrSequence(NETAParser::OrSequenceContext *context) override;
    antlrcpp::Any visitSequence(NETAParser::SequenceContext *context) override;
    antlrcpp::Any visitRingSequence(NETAParser::RingSequenceContext *context) override;
    antlrcpp::Any visitBondCountNode(NETAParser::BondCountNodeContext *context) override;
    antlrcpp::Any visitCharacterNode(NETAParser::CharacterNodeContext *context) override;
    antlrcpp::Any visitConnectionNode(NETAParser::ConnectionNodeContext *context) override;
    antlrcpp::Any visitGeometryNode(NETAParser::GeometryNodeContext *context) override;
    antlrcpp::Any visitHydrogenCountNode(NETAParser::HydrogenCountNodeContext *context) override;
    antlrcpp::Any visitRingAtomNode(NETAParser::RingAtomNodeContext *context) override;
    antlrcpp::Any visitRingNode(NETAParser::RingNodeContext *context) override;
    antlrcpp::Any visitSubSequence(NETAParser::SubSequenceContext *context) override;
    // Target List
    antlrcpp::Any visitElementOrType(NETAParser::ElementOrTypeContext *context) override;
    antlrcpp::Any visitTargetList(NETAParser::TargetListContext *context, NETANode *node);
    // Context Modifiers
    void visitModifier(NETAParser::ModifierContext *context, NETANode *contextNode);
    void visitOption(NETAParser::OptionContext *context, NETANode *contextNode);
    void visitFlag(NETAParser::FlagContext *context, NETANode *contextNode);
    void visitIdentifier(NETAParser::IdentifierContext *context, NETANode *contextNode);
};
