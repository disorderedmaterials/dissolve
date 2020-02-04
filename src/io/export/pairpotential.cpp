/*
	*** Export - PairPotential
	*** src/io/export/pairpotential.cpp
	Copyright T. Youngs 2012-2020

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "io/export/pairpotential.h"
#include "classes/pairpotential.h"
#include "math/data1d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// PairPotential Export Keywords
const char* PairPotentialExportFormatKeywords[] = { "block", "TABLE" };
const char* NicePairPotentialExportFormatKeywords[] = { "Block Data", "DL_POLY TABLE file" };

// Return number of available formats
int PairPotentialExportFileFormat::nFormats() const
{
	return PairPotentialExportFileFormat::nPairPotentialExportFormats;
}

// Return formats array
const char** PairPotentialExportFileFormat::formats() const
{
	return PairPotentialExportFormatKeywords;
}

// Return nice formats array
const char** PairPotentialExportFileFormat::niceFormats() const
{
	return NicePairPotentialExportFormatKeywords;
}

// Return current format as PairPotentialExportFormat
PairPotentialExportFileFormat::PairPotentialExportFormat PairPotentialExportFileFormat::pairPotentialFormat() const
{
	return (PairPotentialExportFileFormat::PairPotentialExportFormat) format_;
}

// Constructor
PairPotentialExportFileFormat::PairPotentialExportFileFormat(const char* filename, PairPotentialExportFormat format) : FileAndFormat(filename, format)
{
}

/*
 * Export Functions
 */

// Write PairPotential as simple block data
bool PairPotentialExportFileFormat::exportBlock(LineParser& parser, PairPotential* pp)
{
	// Get array references for convenience
	const Data1D& uOriginal = pp->uOriginal();
	const Data1D& uAdditional = pp->uAdditional();
	const Data1D& uFull = pp->uFull();
	const Data1D& dUFull = pp->dUFull();
	const int nPoints = pp->nPoints();

	// Write header comment
	if (!parser.writeLineF("#%9s  %12s  %12s  %12s  %12s  %12s  %12s\n", "", "Full", "Derivative", "Original", "Additional", "Exact(Orig)", "Exact(Deriv)")) return false;
	if (!parser.writeLineF("#%9s  %12s  %12s  %12s  %12s  %12s  %12s\n", "r(Angs)", "U(kJ/mol)", "dU(kJ/mol/Ang)", "U(kJ/mol)", "U(kJ/mol)", "U(kJ/mol)", "dU(kJ/mol/Ang)")) return false;

	for (int n = 0; n<nPoints; ++n) if (!parser.writeLineF("%10.6e  %12.6e  %12.6e  %12.6e  %12.6e  %12.6e  %12.6e\n", uOriginal.constXAxis(n), uFull.constValue(n), dUFull.constValue(n), uOriginal.constValue(n), uAdditional.constValue(n), pp->analyticEnergy(uOriginal.constXAxis(n)), pp->analyticForce(uOriginal.constXAxis(n)))) return false;

	return true;
}

// Write PairPotential as a DL_POLY TABLE file
bool PairPotentialExportFileFormat::exportDLPOLY(LineParser& parser, PairPotential* pp)
{
	// Get array references for convenience
	const Data1D& uFull = pp->uFull();
	const Data1D& dUFull = pp->dUFull();
	const int nPoints = pp->nPoints();

	// Write header (record 1)
	if (!parser.writeLineF("%-72s\n", "TABLE file written by Dissolve")) return false;

	// Write mesh information (record 2)
	if (!parser.writeLineF("%20.10e%20.10e%10i\n", pp->delta(), pp->range(), nPoints)) return false;

	// Write pair potential header record
	if (!parser.writeLineF("%-8s%-8s%20.10e%20.10e\n", pp->atomTypeNameI(), pp->atomTypeNameJ(),0.0,0.0)) return false;

	// Write energy data
	for (int n=0; n<nPoints; ++n)
	{
		if (!parser.writeLineF("%17.12e ", uFull.constValue(n))) return false;
		if ( ((n+1)%4 == 0) || (n == (nPoints-1)) )
		{
			if (!parser.writeLineF("\n")) return false;
		}
	}

	// Write force data
	for (int n=0; n<nPoints; ++n)
	{
		if (!parser.writeLineF("%17.12e ", dUFull.constValue(n))) return false;
		if ( ((n+1)%4 == 0) || (n == (nPoints-1)) )
		{
			if (!parser.writeLineF("\n")) return false;
		}
	}

	return true;
}

// Write PairPotential using current filename and format
bool PairPotentialExportFileFormat::exportData(PairPotential* pp)
{
	// Open the file
	LineParser parser;
	if (!parser.openOutput(filename_))
	{
		parser.closeFiles();
		return false;
	}

	// Write data
	bool result = false;
	if (pairPotentialFormat() == PairPotentialExportFileFormat::BlockPairPotential) result = exportBlock(parser, pp);
	else if (pairPotentialFormat() == PairPotentialExportFileFormat::DLPOLYTABLEPairPotential) result = exportDLPOLY(parser, pp);
	else Messenger::error("Unrecognised pair potential format.\nKnown formats are: %s.\n", PairPotentialExportFileFormat().formats());

	return result;
}
