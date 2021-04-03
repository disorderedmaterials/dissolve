// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include "procedure/nodes/node.h"
#include "templates/array.h"
#include "templates/refdatalist.h"
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
    DynamicSiteProcedureNode(SelectProcedureNode *parent);
    ~DynamicSiteProcedureNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;
    // Return whether a name for the node must be provided
    bool mustBeNamed() const;

    /*
     * Site Criteria
     */
    private:
    // Parent Select node for context
    SelectProcedureNode *parent_;
    // Target Elements for selection as sites
    std::vector<Elements::Element> elements_;
    // Target AtomTypes for selection as sites
    std::vector<std::shared_ptr<AtomType>> atomTypes_;

    public:
    // Return whether axes are specified for the dynamic site
    bool hasAxes() const { return false; }

    /*
     * Site Generation
     */
    private:
    // List of generated sites
    Array<Site> generatedSites_;

    private:
    // Generate dynamic sites from the specified Molecule
    void generateSites(std::shared_ptr<const Molecule> molecule);

    public:
    // Return Array of generated sites
    const Array<Site> &generatedSites() const;

    /*
     * Execute
     */
    public:
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
