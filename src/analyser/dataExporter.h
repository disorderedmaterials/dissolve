// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

namespace DataExporter
{
// Try to export the specified data, if a valid filename has been provided
template <typename DataND, typename DataNDExportFileFormat>
bool exportData(const DataND &targetData, DataNDExportFileFormat &fileAndFormat)
{
    if (fileAndFormat.hasFilename())
    {
        return fileAndFormat.exportData(targetData);
    }
    return true;
}
}; // namespace DataExporter
