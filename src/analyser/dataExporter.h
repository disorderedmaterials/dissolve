// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/processPool.h"

template <typename DataND, typename DataNDExportFileFormat> class DataExporter
{
    public:
    static bool exportData(const DataND &targetData, DataNDExportFileFormat &fileAndFormat, const ProcessPool &procPool)
    {
        if (fileAndFormat.hasFilename())
        {
            if (procPool.isMaster())
            {
                if (fileAndFormat.exportData(targetData))
                    procPool.decideTrue();
                else
                {
                    procPool.decideFalse();
                    return false;
                }
            }
            else if (!procPool.decision())
                return false;
        }
        return true;
    }
};