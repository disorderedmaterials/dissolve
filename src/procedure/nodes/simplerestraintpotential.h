// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "kernels/potentials/simplerestraint.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/pickbase.h"

// Simple Restraint Potential Procedure Node
class SimpleRestraintPotentialProcedureNode : public ProcedureNode
{
    public:
    SimpleRestraintPotentialProcedureNode();
    ~SimpleRestraintPotentialProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Selection of molecules to remove
    std::shared_ptr<const PickProcedureNodeBase> selectionToRestrain_;
    // Species to remove
    std::vector<const Species *> speciesToRestrain_;
    // Potential form
    InteractionPotential<SimpleRestraintPotentialFunctions> potential_;

    /*
     * Execute
     */
    private:
    // Apply atomic restraints to the individual atoms of the specified molecule
    void restrainMoleculeAtoms(Configuration *cfg, const std::shared_ptr<Molecule> &mol) const;

    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
