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
}

// Read structure from the specified input stream
NodeConstants::ProcessResult ImportXYZStructureNode::read(std::istream &input, Structure &structure)
{
    using namespace Parsers;
    auto xyz = (maybe(inlineSpaces()) >> natural() << newlines() &
                inlines() >> newlines() >> some(structureAtom() << maybe(inlineSpaces()) << maybe(newlines())))
                   .parse(input);

    if (!xyz)
        return NodeConstants::ProcessResult::Failed;
    auto &rest = std::get<1>(*xyz);
    auto &[nAtoms, atoms] = std::get<0>(*xyz);

    structure.clear();
    for (auto &[elem, v, q] : atoms)
        structure.addAtom(Elements::element(elem), v, q.value_or(0.0));
    assert(nAtoms == atoms.size());

    return NodeConstants::ProcessResult::Success;
}
