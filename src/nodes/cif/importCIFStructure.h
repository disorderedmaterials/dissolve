// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/structure.h"
#include "nodes/cif/io/cifContext.h"
#include "nodes/node.h"

// ImportCIFStructure Node
class ImportCIFStructureNode : public Node
{
    public:
    ImportCIFStructureNode(Graph *parentGraph);
    ~ImportCIFStructureNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // CIF handler context
    CIFContext context_;
    // CIF strucutre
    Structure *structure_;
    // Space group ID
    SpaceGroups::SpaceGroupId spaceGroup_{SpaceGroups::SpaceGroupId::NoSpaceGroup};
    // CIF filepath
    std::string filePath_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
