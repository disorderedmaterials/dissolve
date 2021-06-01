// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/import/trajectory.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "io/import/coordinates.h"

TrajectoryImportFileFormat::TrajectoryImportFileFormat(TrajectoryImportFileFormat::TrajectoryImportFormat format)
    : FileAndFormat(formats_)
{
    formats_ = EnumOptions<TrajectoryImportFileFormat::TrajectoryImportFormat>(
        "TrajectoryImportFileFormat", {{TrajectoryImportFileFormat::XYZTrajectory, "xyz", "XYZ Trajectory"}}, format);
}
TrajectoryImportFileFormat::TrajectoryImportFileFormat(std::string_view filename,
                                                       TrajectoryImportFileFormat::TrajectoryImportFormat format)
    : FileAndFormat(formats_, filename)
{
    formats_ = EnumOptions<TrajectoryImportFileFormat::TrajectoryImportFormat>(
        "TrajectoryImportFileFormat", {{TrajectoryImportFileFormat::XYZTrajectory, "xyz", "XYZ Trajectory"}}, format);
}

TrajectoryImportFileFormat::~TrajectoryImportFileFormat() = default;

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
        case (TrajectoryImportFileFormat::XYZTrajectory):
            result = CoordinateImportFileFormat(CoordinateImportFileFormat::XYZCoordinates).importData(parser, cfg);
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Trajectory format '{}' import has not been implemented.\n", formats_.keyword())));
    }

    return result;
}
