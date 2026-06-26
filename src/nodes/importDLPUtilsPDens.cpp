// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importDLPUtilsPDens.h"
#include "base/applicative.h"
#include "base/parserLibrary.h"

ImportDLPUtilsPDensNode::ImportDLPUtilsPDensNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption("FilePath", "File path", filePath_);

    // Outputs
    addOutput("Data", "Imported data", data_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view ImportDLPUtilsPDensNode::type() const { return "ImportDLPUtilsPDens"; }

// Return short summary of the node's purpose
std::string_view ImportDLPUtilsPDensNode::summary() const { return "Import DLPUtils 3D pdens data"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ImportDLPUtilsPDensNode::process()
{
    // Create the data
    data_.emplace();

    if (!read(*data_, filePath_))
        return error("Failed to read DLPUtils Surface data from file '{}'.\n", filePath_);

    return NodeConstants::ProcessResult::Success;
}

// Read data specified
bool ImportDLPUtilsPDensNode::read(Data3D &data, std::string filePath)
{
    /*
     * PDens format is the following:
     *
     * Line 1:  nx  ny  nz   iminx  iminy  iminz   imaxx  imaxy  imaxz
     * Line 2:  ax ay az bx by bz cx cy cz
     * Line 3 : originx, originy, originz
     * Line 4 : loop order (e.g. 'zyx')
     * Line 5+: data (N = gridx*gridy*gridz)
     */
    using namespace Parsers;
    data.clear();

    auto vectI = inlineSpaces() >> integer() & inlineSpaces() >> integer() & inlineSpaces() >> integer();
    auto firstLine = vectI & vectI & vectI << spaces();
    auto secondLine = vector3() << inlineSpaces() & vector3() << inlineSpaces() & vector3() << spaces();
    auto thirdLine = vector3() << spaces();
    auto fourthLine = literal("zyx") << spaces();
    auto header = firstLine & secondLine & thirdLine << fourthLine;

    // Open file and check that we're OK to proceed importing from it
    std::ifstream infile(filePath);
    if (!infile)
        return false;

    auto head = header.parse(infile);
    if (!head)
        return false;

    auto &[nx, ny, nz, iminx, iminy, iminz, imaxx, imaxy, imaxz, a, b, c, axisOrigin] = std::get<0>(*head);

    // Set up our data
    data.initialise(nx, axisOrigin.x, a.x, ny, axisOrigin.y, b.y, nz, axisOrigin.z, c.z);
    auto points = some(maybe(spaces()) >> real() << spaces()).exact(infile);
    if (!points)
        return false;

    auto idx = 0;
    // Loop over data values ('zyx' loop order, meaning fastest varying is z)
    for (auto x = 0; x < nx; ++x)
        for (auto y = 0; y < ny; ++y)
            for (auto z = 0; z < nz; ++z)
                // Set the value
                data.value(x, y, z) = (*points)[idx++];

    return true;
}
