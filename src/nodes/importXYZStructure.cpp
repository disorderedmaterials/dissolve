// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importXYZStructure.h"
#include "base/parserLibrary.h"
#include <fstream>
#include <iostream>

ImportXYZStructureNode::ImportXYZStructureNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption("FilePath", "File path", filePath_);

    // Outputs
    addOutput("Structure", "Imported structure", structure_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view ImportXYZStructureNode::type() const { return "ImportXYZStructure"; }

// Return short summary of the node's purpose
std::string_view ImportXYZStructureNode::summary() const { return "Import an XYZ file"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ImportXYZStructureNode::process()
{
    structure_.clear();

    std::ifstream infile{filePath_};
    if (!infile)
        return error("Couldn't open file '{}' for loading XYZ data.\n", filePath_);
    return read(infile, structure_);

    // // Open file and check that we're OK to proceed importing from it
    // LineParser parser;
    // if ((!parser.openInput(filePath_)) || (!parser.isFileGoodForReading()))
    //     return error("Couldn't open file '{}' for loading XYZ data.\n", filePath_);

    // return read(parser, structure_);
}

// Read structure from the specified file parser
NodeConstants::ProcessResult ImportXYZStructureNode::read(std::istream &input, Structure &structure)
{
    using namespace parsers;
    auto xyz = (maybe(spaces()) >> natural() << spaces() & inlines() >> newlines() >> some(structureAtom())).parse(input);

    if (!xyz)
        return NodeConstants::ProcessResult::Failed;
    auto &rest = std::get<1>(*xyz);
    auto &[nAtoms, atoms] = std::get<0>(*xyz);

    for (auto &[elem, v, q] : atoms)
        structure.addAtom(Elements::element(elem), v, q.value_or(0.0));
    assert(nAtoms == atoms.size());

    return NodeConstants::ProcessResult::Success;
}
