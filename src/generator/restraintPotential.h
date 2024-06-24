// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/interactionPotential.h"
#include "generator/node.h"
#include "generator/pickBase.h"
#include "math/function1D.h"

// Restraint Potential Procedure Node
class RestraintPotentialProcedureNode : public ProcedureNode
{
    public:
    RestraintPotentialProcedureNode();
    ~RestraintPotentialProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Selection of molecules to remove
    std::shared_ptr<const PickProcedureNodeBase> selectionToRestrain_;
    // Species to remove
    std::vector<const Species *> speciesToRestrain_;
    // Potential form
    InteractionPotential<Functions1D> potential_;

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
