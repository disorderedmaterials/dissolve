// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/export/pairpotential.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/pairpotential.h"
#include "math/data1d.h"

PairPotentialExportFileFormat::PairPotentialExportFileFormat(std::string_view filename, PairPotentialExportFormat format)
    : FileAndFormat(filename, format)
{
}

/*
 * Format Access
 */

// Return enum options for PairPotentialExportFormat
EnumOptions<PairPotentialExportFileFormat::PairPotentialExportFormat>
PairPotentialExportFileFormat::pairPotentialExportFormats()
{
    return EnumOptions<PairPotentialExportFileFormat::PairPotentialExportFormat>(
        "PairPotentialExportFileFormat",
        {{PairPotentialExportFileFormat::BlockPairPotential, "block", "Block Data"},
         {PairPotentialExportFileFormat::DLPOLYTABLEPairPotential, "table", "DL_POLY TABLE File"}});
}

// Return number of available formats
int PairPotentialExportFileFormat::nFormats() const { return PairPotentialExportFileFormat::nPairPotentialExportFormats; }

// Return format keyword for supplied index
std::string PairPotentialExportFileFormat::formatKeyword(int id) const
{
    return pairPotentialExportFormats().keywordByIndex(id);
}

// Return description string for supplied index
std::string PairPotentialExportFileFormat::formatDescription(int id) const
{
    return pairPotentialExportFormats().descriptionByIndex(id);
}

// Return current format as PairPotentialExportFormat
PairPotentialExportFileFormat::PairPotentialExportFormat PairPotentialExportFileFormat::pairPotentialFormat() const
{
    return (PairPotentialExportFileFormat::PairPotentialExportFormat)format_;
}

/*
 * Export Functions
 */

// Write PairPotential as simple block data
bool PairPotentialExportFileFormat::exportBlock(LineParser &parser, PairPotential *pp)
{
    // Get array references for convenience
    const auto &uOriginal = pp->uOriginal();
    const auto &uAdditional = pp->uAdditional();
    const auto &uFull = pp->uFull();
    const auto &dUFull = pp->dUFull();
    const auto nPoints = pp->nPoints();

    // Write header comment
    if (!parser.writeLineF("#{:9}  {:12}  {:12}  {:12}  {:12}  {:12}  {:12}\n", "", "Full", "Derivative", "Original",
                           "Additional", "Exact(Orig)", "Exact(Deriv)"))
        return false;
    if (!parser.writeLineF("#{:9}  {:12}  {:12}  {:12}  {:12}  {:12}  {:12}\n", "r(Angs)", "U(kJ/mol)", "dU(kJ/mol/Ang)",
                           "U(kJ/mol)", "U(kJ/mol)", "U(kJ/mol)", "dU(kJ/mol/Ang)"))
        return false;

    for (auto n = 0; n < nPoints; ++n)
        if (!parser.writeLineF("{:10.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}\n", uOriginal.xAxis(n),
                               uFull.value(n), dUFull.value(n), uOriginal.value(n), uAdditional.value(n),
                               pp->analyticEnergy(uOriginal.xAxis(n)), pp->analyticForce(uOriginal.xAxis(n))))
            return false;

    return true;
}

// Write PairPotential as a DL_POLY TABLE file
bool PairPotentialExportFileFormat::exportDLPOLY(LineParser &parser, PairPotential *pp)
{
    // Get array references for convenience
    const auto &uFull = pp->uFull();
    const auto &dUFull = pp->dUFull();
    const auto nPoints = pp->nPoints();

    // Write header (record 1)
    if (!parser.writeLineF("{:<72}\n", "TABLE file written by Dissolve"))
        return false;

    // Write mesh information (record 2)
    if (!parser.writeLineF("{:20.10e}{:20.10e}{:10d}\n", pp->delta(), pp->range(), nPoints))
        return false;

    // Write pair potential header record
    if (!parser.writeLineF("{:<8}{:<8}{:20.10e}{:20.10e}\n", pp->atomTypeNameI(), pp->atomTypeNameJ(), 0.0, 0.0))
        return false;

    // Write energy data
    for (auto n = 0; n < nPoints; ++n)
    {
        if (!parser.writeLineF("{:17.12e} ", uFull.value(n)))
            return false;
        if (((n + 1) % 4 == 0) || (n == (nPoints - 1)))
        {
            if (!parser.writeLineF("\n"))
                return false;
        }
    }

    // Write force data
    for (auto n = 0; n < nPoints; ++n)
    {
        if (!parser.writeLineF("{:17.12e} ", dUFull.value(n)))
            return false;
        if (((n + 1) % 4 == 0) || (n == (nPoints - 1)))
        {
            if (!parser.writeLineF("\n"))
                return false;
        }
    }

    return true;
}

// Write PairPotential using current filename and format
bool PairPotentialExportFileFormat::exportData(PairPotential *pp)
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
    if (pairPotentialFormat() == PairPotentialExportFileFormat::BlockPairPotential)
        result = exportBlock(parser, pp);
    else if (pairPotentialFormat() == PairPotentialExportFileFormat::DLPOLYTABLEPairPotential)
        result = exportDLPOLY(parser, pp);
    else
    {
        Messenger::error("Unrecognised pair potential format.\nKnown formats are:\n");
        printAvailableFormats();
    }
    return result;
}
