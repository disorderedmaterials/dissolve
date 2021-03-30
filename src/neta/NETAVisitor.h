// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "NETAParserBaseVisitor.h"
#include "neta/neta.h"
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
    std::shared_ptr<NETANode> currentNETAContext() const;

    public:
    // Construct description within supplied object, from given tree
    void create(NETADefinition &neta, NETAParser::NetaContext *tree, const Forcefield *associatedForcefield);

    /*
     * Visitor Overrides
     */
    private:
    // Nodes
    antlrcpp::Any visitOrNode(NETAParser::OrNodeContext *context) override;
    antlrcpp::Any visitConnectionNode(NETAParser::ConnectionNodeContext *context) override;
    antlrcpp::Any visitPresenceNode(NETAParser::PresenceNodeContext *context) override;
    antlrcpp::Any visitRingNode(NETAParser::RingNodeContext *context) override;
    // Target List
    antlrcpp::Any visitElementOrType(NETAParser::ElementOrTypeContext *context) override;
    antlrcpp::Any visitTargetList(NETAParser::TargetListContext *context) override;
    // Context Modifiers
    antlrcpp::Any visitModifier(NETAParser::ModifierContext *context) override;
    antlrcpp::Any visitOption(NETAParser::OptionContext *context) override;
    antlrcpp::Any visitFlag(NETAParser::FlagContext *context) override;
};
