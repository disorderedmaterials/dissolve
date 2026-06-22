// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/structure.h"
#include "nodes/node.h"

class ImportXYZStructureNode : public Node
{
    public:
    ImportXYZStructureNode(Graph *parentGraph);
    ~ImportXYZStructureNode() override = default;

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
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;

    public:
    // Read structure from the specified file parser
    static NodeConstants::ProcessResult read(LineParser &parser, Structure &structure);
};