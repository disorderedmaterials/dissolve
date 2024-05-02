// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "io/import/data3D.h"
#include "math/data3D.h"

// Read pdens data from specified file
bool Data3DImportFileFormat::importPDens(LineParser &parser, Data3D &data)
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

    // Get array dimensioos
    if (parser.getArgsDelim() != LineParser::Success)
        return false;
    auto N = parser.argi(0);

    // Get voxel sizes, assuming cubic grid
    if (parser.getArgsDelim() != LineParser::Success)
        return false;
    auto delta = Vec3<double>(parser.argd(0), parser.argd(4), parser.argd(8));

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
