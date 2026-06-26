// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importDLPOLYStructure.h"
#include "base/applicative.h"
#include "base/parserLibrary.h"
#include "data/elements.h"

ImportDLPOLYStructureNode::ImportDLPOLYStructureNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption("FilePath", "File path", filePath_);

    // Outputs
    addOutput("Structure", "Imported structure", structure_);
    addOutput("Forces", "Atomic forces (if present)", forces_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view ImportDLPOLYStructureNode::type() const { return "ImportDLPOLYStructure"; }

// Return short summary of the node's purpose
std::string_view ImportDLPOLYStructureNode::summary() const { return "Import a DL_POLY CONFIG or REVCON file"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ImportDLPOLYStructureNode::process()
{
    std::ifstream infile{filePath_};
    if (!infile)
        return error("Couldn't open file '{}' for loading coordinates data.\n", filePath_);
    auto head = header().parse(infile);
    if (!head)
        return error("Failed to parse file header");
    auto &[title, keytrj, imcon, natoms] = std::get<0>(*head);
    if (!natoms)
        message(" --> Expecting coordinates for an unknown number of atoms (DLPOLY keytrj={}, imcon={}) - will read "
                "until end of file.\n",
                keytrj, imcon);
    else
        message(" --> Expecting coordinates for {} atoms (DLPOLY keytrj={}, imcon={}).\n", *natoms, keytrj, imcon);
    return read(infile, keytrj, imcon, natoms.value_or(0), structure_, forces_);
}

// Read structure from the specified file parser
NodeConstants::ProcessResult ImportDLPOLYStructureNode::read(std::istream &input, int keytrj, int imcon, int nAtoms,
                                                             Structure &structure,
                                                             OptionalReferenceWrapper<std::vector<Vector3>> optForces)
{
    // /*
    //  * Import DL_POLY coordinates information through the specified line parser.
    //  * We assume HISf, CONFIG or REVCON format (only the first two lines differ)
    //  *
    //  * Line 1:    Title
    //  * Line 2:    keytrj   imcon    natoms    []
    //  * Line 3-5:  cell matrix (if imcon > 0)
    //  * Line 6:    atomtype        id
    //  * Line 7:    rx   ry   rz
    //  * Line 8:    vx   vy   vz      if (keytrj > 0)
    //  * Line 9:    fx   fy   fz	if (keytrj > 1)
    //  *   ...
    //  */

    using namespace Parsers;

    // Clear storage objects
    structure.clear();
    std::vector<Vector3> forces;

    // Read cell information if given
    if (imcon > 0)
    {
        auto mat = matrix3().parse(input);
        if (!mat)
            return NodeConstants::ProcessResult::Failed;
        structure.createBox(std::get<0>(*mat));
    }

    auto atomType = graphs() << inlineSpaces() & natural() << inlineSpaces() &
                    maybe(real() << maybe(inlineSpaces() << real()) << newlines());

    if (keytrj == 0)
    {
        auto terms = atomType >> spaces() >> vector3() << spaces();
        auto result = some(terms).parse(input);
        if (!result)
            return NodeConstants::ProcessResult::Failed;
        for (auto position : std::get<0>(*result))
            structure.addAtom(Elements::Unknown, position);
    }
    else if (keytrj == 1)
    {
        auto terms = atomType >> vector3() << spaces() & vector3() << spaces();
        auto result = some(terms).parse(input);
        if (!result)
            return NodeConstants::ProcessResult::Failed;
        for (auto &[position, velocity] : std::get<0>(*result))
            structure.addAtom(Elements::Unknown, position);
    }
    else
    {
        auto result = some(atom()).parse(input);
        if (!result)
            return NodeConstants::ProcessResult::Failed;
        for (auto &[position, velocity, force] : std::get<0>(*result))
        {
            structure.addAtom(Elements::Unknown, position);
            forces.push_back(*force);
        }
    }

    // // Copy forces out?
    if (optForces)
        optForces.value().get() = forces;

    return NodeConstants::ProcessResult::Success;
}

Parsers::Parser<std::tuple<std::string, int, int, std::optional<int>>> ImportDLPOLYStructureNode::header()
{
    using namespace Parsers;
    return inlines() << newlines() & maybe(inlineSpaces()) >> natural() << inlineSpaces() & natural() &
           maybe(inlineSpaces() >> natural() << maybe(inlineSpaces() << real())) << spaces();
}

Parsers::Parser<std::tuple<Vector3, std::optional<Vector3>, std::optional<Vector3>>> ImportDLPOLYStructureNode::atom()
{
    using namespace Parsers;
    auto atomType =
        graphs() << inlineSpaces() & natural() & maybe(inlineSpaces() >> real()) & maybe(inlineSpaces() >> real()) << spaces();
    return atomType >> vector3() << spaces() & maybe(vector3() << spaces()) & maybe(vector3() << spaces());
}
