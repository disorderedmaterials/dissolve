// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "classes/site.h"
#include "data/elements.h"
#include "procedure/nodes/node.h"
#include "templates/reflist.h"
#include <memory>
#include <vector>

// Forward Declarations
class SelectProcedureNode;
class AtomType;
class LineParser;
class Molecule;
class NodeScopeStack;

// Procedure Node - Dynamic Site
class DynamicSiteProcedureNode : public ProcedureNode
{
    public:
    DynamicSiteProcedureNode(std::shared_ptr<SelectProcedureNode> parent);

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Site Criteria
     */
    private:
    // Parent Select node for context
    std::shared_ptr<SelectProcedureNode> parent_;
    // Target atom types for selection as sites
    std::vector<std::shared_ptr<AtomType>> atomTypes_;
    // Target Elements for selection as sites
    std::vector<Elements::Element> elements_;

    public:
    // Return whether axes are specified for the dynamic site
    bool hasAxes() const { return false; }

    /*
     * Site Generation
     */
    private:
    // Generated sites
    std::vector<Site> generatedSites_;

    private:
    // Generate dynamic sites from the specified Molecule
    void generateSites(const std::shared_ptr<const Molecule> &molecule);

    public:
    // Return Array of generated sites
    const std::vector<Site> &generatedSites() const;

    /*
     * Execute
     */
    public:
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
