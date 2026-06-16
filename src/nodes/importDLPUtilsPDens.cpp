// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importDLPUtilsPDens.h"
#include "base/lineParser.h"

ImportDLPUtilsPDensNode::ImportDLPUtilsPDensNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption<std::string>("FilePath", "File path", filePath_);

    // Outputs
    addOutput<std::optional<Data3D>>("Data", "Imported data", data_);
}

std::string_view ImportDLPUtilsPDensNode::type() const { return "ImportDLPUtilsPDens"; }

std::string_view ImportDLPUtilsPDensNode::summary() const { return "Import DLPUtils 3D pdens data"; }

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
    data.clear();

    // Open file and check that we're OK to proceed importing from it
    LineParser parser;
    if ((!parser.openInput(filePath)) || (!parser.isFileGoodForReading()))
        return false;

    // Get array dimensioos
    if (parser.getArgsDelim() != LineParser::Success)
        return false;
    auto N = parser.argi(0);

    // Get voxel sizes, assuming cubic grid
    if (parser.getArgsDelim() != LineParser::Success)
        return false;
    auto delta = Vector3(parser.argd(0), parser.argd(4), parser.argd(8));

    // Get grid origin coordinates
    if (parser.getArgsDelim() != LineParser::Success)
        return false;
    auto axisOrigin = parser.arg3d(0);

    // Get loop order - we handle `zyx` and nothing else for now
    if (parser.getArgsDelim() != LineParser::Success)
        return false;
    if (parser.args(0) != "zyx")
        return Messenger::error("Only 'zyx' loop order is allowed.\n");

    // Set up our data
    data.initialise(N, axisOrigin.x, delta.x, N, axisOrigin.y, delta.y, N, axisOrigin.z, delta.z);

    // Loop over data values ('zyx' loop order, meaning fastest varying is z)
    for (auto x = 0; x < N; ++x)
        for (auto y = 0; y < N; ++y)
            for (auto z = 0; z < N; ++z)
            {
                // Read line
                if (parser.getArgsDelim() != LineParser::Success)
                    return false;

                // Set the value
                data.value(x, y, z) = parser.argd(0);
            }

    return true;
}