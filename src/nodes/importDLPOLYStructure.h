// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/structure.h"
#include "nodes/node.h"
#include "templates/optionalRef.h"

class ImportDLPOLYStructureNode : public Node
{
    public:
    ImportDLPOLYStructureNode(Graph *parentGraph);
    ~ImportDLPOLYStructureNode() override = default;

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
    std::string filePath_;
    // Structure
    Structure structure_;
    // Forces
    std::vector<Vector3> forces_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;

    public:
    // Read structure from the specified file parser
    static NodeConstants::ProcessResult read(LineParser &parser, int keytrj, int imcon, int nAtoms, Structure &structure,
                                             OptionalReferenceWrapper<std::vector<Vector3>> forces = {});
};