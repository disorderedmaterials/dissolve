// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/node.h"
#include <memory>
#include <set>

// Forward Declarations
class Element;
class Molecule;
class SiteStack;
class Species;
class SpeciesSite;

// Copy Node
class CopyGeneratorNode : public GeneratorNode
{
    public:
    explicit CopyGeneratorNode(Configuration *cfg = nullptr);

    /*
     * Identity
     */
    public:
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
    // Whether to copy global potentials
    bool copyGlobalPotentials_{true};

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const GeneratorContext &generatorContext) override;
    // Execute node
    bool execute(const GeneratorContext &generatorContext) override;
};
