/*
	*** Export Module - PairPotential Functions
	*** src/modules/export/functions_pairpotential.cpp
	Copyright T. Youngs 2012-2019

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

#include "modules/export/export.h"
#include "classes/pairpotential.h"
#include "base/lineparser.h"

// Write PairPotential in specified format
bool ExportModule::writePairPotential(PairPotentialExportFileFormat::PairPotentialExportFormat format, LineParser& parser, PairPotential* pp)
{
	// Check supplied format
	if (format == PairPotentialExportFileFormat::BlockPairPotential) return writeBlockPairPotential(parser, pp);
	else if (format == PairPotentialExportFileFormat::DLPOLYTABLEPairPotential) return writeDLPOLYTABLEPairPotential(parser, pp);

	Messenger::error("Unrecognised pair potential format - '%s'.\nKnown formats are: %s.\n", format, PairPotentialExportFileFormat().formats());
	return false;
}

// Write PairPotential in specified format to file
bool ExportModule::writePairPotential(PairPotentialExportFileFormat::PairPotentialExportFormat format, const char* filename, PairPotential* pp)
{
	// Open the file
	LineParser parser;
	if (!parser.openOutput(filename))
	{
		parser.closeFiles();
		return false;
	}

	bool result = writePairPotential(format, parser, pp);

	parser.closeFiles();

	return result;
}

// Write PairPotential as simple block data
bool ExportModule::writeBlockPairPotential(LineParser& parser, PairPotential* pp)
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
bool ExportModule::writeDLPOLYTABLEPairPotential(LineParser& parser, PairPotential* pp)
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
