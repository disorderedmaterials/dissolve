// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/restraintPotential.h"
#include "classes/configuration.h"
#include "kernels/potentials/spherical.h"
#include "keywords/interactionPotential.h"
#include "keywords/node.h"
#include "keywords/speciesVector.h"

RestraintPotentialGeneratorNode::RestraintPotentialGeneratorNode()
    : GeneratorNode(NodeType::RestraintPotential), potential_(Functions1D::Form::Harmonic)
{
    keywords_.setOrganisation("Options", "Definition");
    keywords_.add<InteractionPotentialKeyword<Functions1D>>("Potential", "Potential to apply to individual atoms", potential_);

    keywords_.setOrganisation("Options", "Targets");
    keywords_.add<SpeciesVectorKeyword>("Species", "Target species to apply atomic restraints to", speciesToRestrain_);
    keywords_.add<NodeKeyword<PickGeneratorNodeBase>>(
        "Selection", "Picked selection of molecules to apply atomic restraints to", selectionToRestrain_, this,
        NodeTypeVector{NodeType::Pick, NodeType::PickProximity, NodeType::PickRegion});
}

/*
 * Execute
 */

void RestraintPotentialGeneratorNode::restrainMoleculeAtoms(Configuration *cfg, const std::shared_ptr<Molecule> &mol) const
{
    for (auto &i : mol->atoms())
    {
        auto pot = std::make_unique<SphericalPotential>();
        pot->setPotential(potential_);
        pot->setTargetAtomIndices({i->globalIndex()});
        pot->setOrigin(i->r());
        cfg->addTargetedPotential(std::unique_ptr<ExternalPotential>(std::move(pot)));
    }
}

// Execute node
bool RestraintPotentialGeneratorNode::execute(const GeneratorContext &generatorContext)
{
    auto *cfg = generatorContext.configuration();

    // Apply to targeted species
    if (!speciesToRestrain_.empty())
    {
        for (const auto &mol : cfg->molecules())
            if (std::find(speciesToRestrain_.begin(), speciesToRestrain_.end(), mol->species()) != speciesToRestrain_.end())
                restrainMoleculeAtoms(cfg, mol);
    }

    // Apply to targeted molecules
    if (selectionToRestrain_)
    {
        // If species were also specified, make sure we don't restrain molecules twice (we already restrained by species, so
        // just need to check the vector)
        if (speciesToRestrain_.empty())
            for (const auto &mol : selectionToRestrain_->pickedMolecules())
                restrainMoleculeAtoms(cfg, mol);
        else
            for (const auto &mol : selectionToRestrain_->pickedMolecules())
                if (std::find(speciesToRestrain_.begin(), speciesToRestrain_.end(), mol->species()) == speciesToRestrain_.end())
                    restrainMoleculeAtoms(cfg, mol);
    }

    return true;
}
