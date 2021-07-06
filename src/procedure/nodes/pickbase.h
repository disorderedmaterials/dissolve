// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include "templates/optionalref.h"
#include <memory>

// Forward Declarations
class Molecule;

// Pick Node Base
class PickProcedureNodeBase : public ProcedureNode
{
    public:
    explicit PickProcedureNodeBase(ProcedureNode::NodeType nodeType);
    ~PickProcedureNodeBase() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;

    /*
     * Picked Molecules
     */
    protected:
    // Vector of picked Molecules
    std::vector<std::shared_ptr<Molecule>> pickedMolecules_;

    protected:
    // Return source molecule pool
    const std::vector<std::shared_ptr<Molecule>> &moleculePool(const Configuration *cfg) const;

    public:
    // Return vector of picked Molecules
    std::vector<std::shared_ptr<Molecule>> &pickedMolecules();
    const std::vector<std::shared_ptr<Molecule>> &pickedMolecules() const;

    /*
     * Execution
     */
    protected:
    // Finalise any necessary data after execution
    bool finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
