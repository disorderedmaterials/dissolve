// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/structure.h"
#include "nodes/node.h"

class ImportMoscitoStructureNode : public Node
{
    public:
    ImportMoscitoStructureNode(Graph *parentGraph);
    ~ImportMoscitoStructureNode() override = default;

    /*
     * Definition
     */
    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // File path
    std::string filePath_;
    // Structure
    Structure structure_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};