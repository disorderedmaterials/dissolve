// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include <memory>
#include <set>

// Forward Declarations
class DynamicSiteProcedureNode;
class Element;
class Molecule;
class SiteStack;
class Species;
class SpeciesSite;

// Copy Node
class CopyProcedureNode : public ProcedureNode
{
    public:
    explicit CopyProcedureNode(Configuration *cfg = nullptr);

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Copy Target
     */
    private:
    // Source configuration
    Configuration *source_{nullptr};
    // Vector of Species to exclude from copy
    std::vector<const Species *> excludedSpecies_;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
