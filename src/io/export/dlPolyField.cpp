// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/export/dlPolyField.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/speciesAtom.h"
#include "data/atomicMasses.h"

DlPolyControlExportFileFormat::DlPolyFieldExportFileFormat(std::string_view filename, DlPolyFieldExportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<DlPolyFieldExportFileFormat::DlPolyFieldExportFormat>(
        "FieldExportFileFormat", {{DlPolyFieldExportFormat::DLPOLY, "dlpoly", "DL_POLY FIELD File"}});
}


// Export DlPolyField as Field
bool DlPolyFieldExportFileFormat::exportDLPOLY(LineParser &parser, Configuration *cfg)
{
    // Export title


    return true;
}

// Export DlPolyField using current filename and format
bool DlPolyFieldExportFileFormat::exportData(Configuration *cfg)
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
        case (DlPolyFieldExportFormat::DLPOLY):
            result = exportDLPOLY(parser, cfg);
            break;
        default:
            throw(std::runtime_error(fmt::format("DlPolyField format '{}' export has not been implemented.\n",
                                                 formats_.keywordByIndex(*formatIndex_))));
    }

    return true;
}
