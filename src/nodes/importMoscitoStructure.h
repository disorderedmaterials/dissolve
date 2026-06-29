// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/applicative.h"
#include "classes/structure.h"
#include "math/vector3.h"
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
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;
    // parse the header of a moscito file
    static Parsers::Parser<std::tuple<Vector3, int>> header();
    // parse an atom from a moscito file
    static Parsers::Parser<std::tuple<std::string, int, Vector3, Vector3, Vector3>> atom();
    // parse an molecule from a moscito file

    static Parsers::Parser<
        std::tuple<std::string, int, int, int, std::vector<std::tuple<std::string, int, Vector3, Vector3, Vector3>>>>
    molecule();

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
};
