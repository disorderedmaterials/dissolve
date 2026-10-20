// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "nodes/node.h"

class SpeciesBaseNode : public Node
{
    public:
    SpeciesBaseNode(Graph *parentGraph);
    ~SpeciesBaseNode() override = default;

    /*
     * Data
     */
    protected:
    // Species object
    Species species_;

    /*
     * Accessors
     */
    public:
    // Return the species object
    Species &species();
    const Species &species() const;
};
