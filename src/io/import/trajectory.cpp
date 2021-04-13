// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/import/trajectory.h"
#include "base/sysfunc.h"

TrajectoryImportFileFormat::TrajectoryImportFileFormat(TrajectoryImportFileFormat::TrajectoryImportFormat format)
    : FileAndFormat(format)
{
    setUpKeywords();
}
TrajectoryImportFileFormat::TrajectoryImportFileFormat(std::string_view filename,
                                                       TrajectoryImportFileFormat::TrajectoryImportFormat format)
    : FileAndFormat(filename, format)
{
    setUpKeywords();
}

TrajectoryImportFileFormat::~TrajectoryImportFileFormat() {}

/*
 * Keyword Options
 */

// Set up keywords for the format
void TrajectoryImportFileFormat::setUpKeywords() {}

/*
 * Format Access
 */

// Return enum options for TrajectoryImportFormat
EnumOptions<TrajectoryImportFileFormat::TrajectoryImportFormat> TrajectoryImportFileFormat::trajectoryImportFormats()
{
    return EnumOptions<TrajectoryImportFileFormat::TrajectoryImportFormat>(
        "TrajectoryImportFileFormat", {{TrajectoryImportFileFormat::XYZTrajectory, "xyz", "XYZ Trajectory"}});
}

// Return number of available formats
int TrajectoryImportFileFormat::nFormats() const { return TrajectoryImportFileFormat::nTrajectoryImportFormats; }

// Return format keyword for supplied index
std::string TrajectoryImportFileFormat::formatKeyword(int id) const { return trajectoryImportFormats().keywordByIndex(id); }

// Return description string for supplied index
std::string TrajectoryImportFileFormat::formatDescription(int id) const
{
    return trajectoryImportFormats().descriptionByIndex(id);
}

// Return current format as TrajectoryImportFormat
TrajectoryImportFileFormat::TrajectoryImportFormat TrajectoryImportFileFormat::trajectoryFormat() const
{
    return (TrajectoryImportFileFormat::TrajectoryImportFormat)format_;
}
