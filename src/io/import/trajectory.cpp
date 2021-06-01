// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/import/trajectory.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "io/import/coordinates.h"

TrajectoryImportFileFormat::TrajectoryImportFileFormat(std::string_view filename,
                                                       TrajectoryImportFileFormat::TrajectoryImportFormat format)
    : FileAndFormat(formats_, filename)
{
    formats_ = EnumOptions<TrajectoryImportFileFormat::TrajectoryImportFormat>(
        "TrajectoryImportFileFormat", {{TrajectoryImportFormat::XYZ, "xyz", "XYZ Trajectory"}}, format);
}

/*
 * Import Functions
 */

// Import trajectory using supplied parser and current format
bool TrajectoryImportFileFormat::importData(LineParser &parser, Configuration *cfg)
{
    // Import the data
    auto result = false;
    switch (formats_.enumeration())
    {
        case (TrajectoryImportFormat::XYZ):
            result =
                CoordinateImportFileFormat("", CoordinateImportFileFormat::CoordinateImportFormat::XYZ).importData(parser, cfg);
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Trajectory format '{}' import has not been implemented.\n", formats_.keyword())));
    }

    return result;
}
