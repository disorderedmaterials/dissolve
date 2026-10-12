// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "nodes/node.h"

class ImportSpeciesNode : public Node
{
    public:
    ImportSpeciesNode(Graph *parentGraph);
    ~ImportSpeciesNode() override = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // File path
    std::filesystem::path filePath_;
    //
    Species species_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};