// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/import/trajectory.h"
#include "base/lineParser.h"
#include "classes/configuration.h"
#include "io/import/coordinates.h"
#include "templates/algorithms.h"

TrajectoryImportFileFormat::TrajectoryImportFileFormat(std::string_view filename,
                                                       TrajectoryImportFileFormat::TrajectoryImportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<TrajectoryImportFileFormat::TrajectoryImportFormat>(
        "TrajectoryImportFileFormat",
        {{TrajectoryImportFormat::DLPOLYFormatted, "hisf", "Formatted DL_POLY Trajectory (no header)"},
         {TrajectoryImportFormat::XYZ, "xyz", "XYZ Trajectory"}});
}

/*
 * Import Functions
 */

// Import trajectory using supplied parser and current format
bool TrajectoryImportFileFormat::importData(LineParser &parser, Configuration *cfg, std::optional<Matrix3> &unitCell)
{
    // Check the format
    if (!formatIndex_)
        return Messenger::error("No format set for TrajectoryImportFileFormat so can't import.\n");

    // Import the data
    std::vector<Vec3<double>> r;
    auto result = false;
    switch (formats_.enumerationByIndex(*formatIndex_))
    {
        case (TrajectoryImportFormat::DLPOLYFormatted):
            result = importDLPOLY(parser, r, unitCell);
            if (result)
            {
                // All good, so copy atom coordinates over into our array
                for (auto &&[i, ri] : zip(cfg->atoms(), r))
                {
                    i.setCoordinates(ri);
                }
            }
            break;
        case (TrajectoryImportFormat::XYZ):
            return CoordinateImportFileFormat("", CoordinateImportFileFormat::CoordinateImportFormat::XYZ)
                .importData(parser, cfg);
        default:
            throw(std::runtime_error(fmt::format("Trajectory format '{}' import has not been implemented.\n",
                                                 formats_.keywordByIndex(*formatIndex_))));
    }

    return result;
}
