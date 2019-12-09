/*
	*** Export - Data3D
	*** src/io/export/data3d.cpp
	Copyright T. Youngs 3013-3019

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

#include "io/export/data3d.h"
#include "math/data3d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Data3D Export Keywords
const char* Data3DExportFormatKeywords[] = { "block", "cartesian", "pdens" };
const char* NiceData3DExportFormatKeywords[] = { "Block Data", "Cartesian (x,y,z,value) Data", "PDens Data" };

// Return number of available formats
int Data3DExportFileFormat::nFormats() const
{
	return Data3DExportFileFormat::nData3DExportFormats;
}

// Return formats array
const char** Data3DExportFileFormat::formats() const
{
	return Data3DExportFormatKeywords;
}

// Return nice formats array
const char** Data3DExportFileFormat::niceFormats() const
{
	return NiceData3DExportFormatKeywords;
}

// Return current format as CoordinateExportFormat
Data3DExportFileFormat::Data3DExportFormat Data3DExportFileFormat::data3DFormat() const
{
	return (Data3DExportFileFormat::Data3DExportFormat) format_;
}

// Constructor
Data3DExportFileFormat::Data3DExportFileFormat(const char* filename, Data3DExportFormat format) : FileAndFormat(filename, format)
{
}

/*
 * Export Functions
 */

// Export Data3D as simple block data
bool Data3DExportFileFormat::exportBlock(LineParser& parser, const Data3D& data)
{
	// Export header comment
	if (!parser.writeLineF("# %i blocks (nX*nY) of %i points (nZ).\n", data.constXAxis().nItems()*data.constYAxis().nItems(), data.constZAxis().nItems())) return false;

	// Export datapoints, separating each block of a specific x value with a single blank line
	const Array3D<double>& values = data.constValues3D();
	for (int x=0; x<values.nX(); ++x)
	{
		for (int y=0; y<values.nY(); ++y)
		{
			for (int z=0; z<values.nZ(); ++z)
			{
				if (!parser.writeLineF("%15.9e\n", values.constAt(x,y,z))) return false;
			}
			if (!parser.writeLineF("\n")) return false;
		}
	}

	return true;
}

// Export Data3D as cartesian data
bool Data3DExportFileFormat::exportCartesian(LineParser& parser, const Data3D& data)
{
	// Four-column format (x  y  z  value) in blocks of similar x and y value, separated by blank lines
	const Array3D<double>& values = data.constValues3D();
	const Array<double>& xAxis = data.constXAxis();
	const Array<double>& yAxis = data.constYAxis();
	const Array<double>& zAxis = data.constZAxis();
	for (int x=0; x<values.nX(); ++x)
	{
		double xVal = xAxis.constAt(x);
		for (int y=0; y<values.nY(); ++y)
		{
			double yVal = yAxis.constAt(y);
			for (int z=0; z<values.nZ(); ++z)
			{
				if (!parser.writeLineF("%15.9e %15.9e %15.9e %15.9e\n", xVal, yVal, zAxis.constAt(z), values.constAt(x,y,z))) return false;
			}
		}
		if (!parser.writeLineF("\n")) return false;
	}

	return true;
}

// Export Data3D as pdens data
bool Data3DExportFileFormat::exportPDens(LineParser& parser, const Data3D& data)
{
	// Line 1 (Integer Extents): nx, ny, nz, xmin, ymin, zmin, xmax, ymax, zmax
	const Array3D<double>& values = data.constValues3D();
	if (!parser.writeLineF("%5i%5i%5i%5i%5i%5i%5i%5i%5i\n", values.nX(), values.nY(), values.nZ(), 0, 0, 0, values.nX(), values.nY(), values.nZ())) return false;

	// Line 2 (Axis Definitions) - assume square
	if (!parser.writeLineF("%9.3e %9.3e %9.3e %9.3e %9.3e %9.3e %9.3e %9.3e %9.3e\n", data.constXAxis().constAt(1) - data.constXAxis().constAt(0), 0.0, 0.0, 0.0, data.constYAxis().constAt(1) - data.constYAxis().constAt(0), 0.0, 0.0, 0.0, data.constZAxis().constAt(1) - data.constZAxis().constAt(0))) return false;

	// Line 3 (Origin)
	if (!parser.writeLineF("%10.4f%10.4f%10.4f\n", data.constXAxis().constAt(0), data.constYAxis().constAt(0), data.constZAxis().constAt(0))) return false;

	// Line 4 (Loop order)
	if (!parser.writeLineF("zyx\n")) return false;

	// Lines 5+ (Data)
	for (int x=0; x<values.nX(); ++x)
	{
		for (int y=0; y<values.nY(); ++y)
		{
			for (int z=0; z<values.nZ(); ++z)
			{
				if (!parser.writeLineF("%15.9e\n", values.constAt(x,y,z))) return false;
			}
		}
	}

	return true;
}

// Export Data3D using current filename and format
bool Data3DExportFileFormat::exportData(const Data3D& data)
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
	if (data3DFormat() == Data3DExportFileFormat::BlockData) result = exportBlock(parser, data);
	else if (data3DFormat() == Data3DExportFileFormat::CartesianData) result = exportCartesian(parser, data);
	else if (data3DFormat() == Data3DExportFileFormat::PDensData) result = exportPDens(parser, data);
	else Messenger::error("Unrecognised data format.\nKnown formats are: %s.\n", Data3DExportFileFormat().formats());

	return result;
}
