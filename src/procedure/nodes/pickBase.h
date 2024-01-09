// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include "templates/optionalRef.h"
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
     * Picked Molecules
     */
    protected:
    // Existing selection from which to pick
    std::shared_ptr<const PickProcedureNodeBase> selection_;
    // Vector of picked Molecules
    std::vector<std::shared_ptr<Molecule>> pickedMolecules_;

    protected:
    // Return source molecule pool
    const std::vector<std::shared_ptr<Molecule>> &moleculePool(const Configuration *cfg) const;
    // Return source molecule pool name
    std::string moleculePoolName() const;

    public:
    // Return vector of picked Molecules
    std::vector<std::shared_ptr<Molecule>> &pickedMolecules();
    const std::vector<std::shared_ptr<Molecule>> &pickedMolecules() const;

    /*
     * Execution
     */
    protected:
    // Finalise any necessary data after execution
    bool finalise(const ProcedureContext &procedureContext) override;
};
