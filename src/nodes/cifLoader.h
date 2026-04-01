// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "io/import/cif.h"
#include "nodes/node.h"

// CIFLoader Node
class CIFLoaderNode : public Node
{
    public:
    using CIFContext = CIFHandler;

    public:
    CIFLoaderNode(Graph *parentGraph);
    ~CIFLoaderNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // CIF handler context
    CIFContext context_;
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
