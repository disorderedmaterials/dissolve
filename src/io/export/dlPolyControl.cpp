// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/export/dlPolyControl.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/speciesAtom.h"
#include "data/atomicMasses.h"

DlPolyControlExportFileFormat::DlPolyControlExportFileFormat(std::string_view filename, DlPolyControlExportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<DlPolyControlExportFileFormat::DlPolyControlExportFormat>(
        "CoordinateExportFileFormat", {{DlPolyControlExportFormat::DLPOLY, "dlpoly", "DL_POLY CONFIG File"}});
}


// Export DlPolyControl as CONTROL
bool DlPolyControlExportFileFormat::exportDLPOLY(LineParser &parser, Configuration *cfg)
{
    // Export title
    if (!parser.writeLineF("{} @ {}\n", cfg->name(), cfg->contentsVersion()))
        return false;
    

    return true;
}

// Export DlPolyControl using current filename and format
bool DlPolyControlExportFileFormat::exportData(Configuration *cfg)
{
    // Open the file
    LineParser parser;
    if (!parser.openOutput(filename_))
    {
        parser.closeFiles();
        return false;
    }

    // Write data
    auto result = false;
    switch (formats_.enumerationByIndex(*formatIndex_))
    {
        case (DlPolyControlExportFormat::DLPOLY):
            result = exportDLPOLY(parser, cfg);
            break;
        default:
            throw(std::runtime_error(fmt::format("DlPolyControl format '{}' export has not been implemented.\n",
                                                 formats_.keywordByIndex(*formatIndex_))));
    }

    return true;
}
