// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/interactionPotential.h"
#include "generator/node.h"
#include "generator/pickBase.h"
#include "math/function1D.h"

// Restraint Potential
class RestraintPotentialGeneratorNode : public GeneratorNode
{
    public:
    RestraintPotentialGeneratorNode();
    ~RestraintPotentialGeneratorNode() override = default;

    /*
     * Data
     */
    private:
    // Selection of molecules to remove
    std::shared_ptr<const PickGeneratorNodeBase> selectionToRestrain_;
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
    bool execute(const GeneratorContext &generatorContext) override;
};
