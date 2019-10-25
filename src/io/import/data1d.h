/*
	*** Import - Data1D
	*** src/io/import/data1d.h
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

#ifndef DISSOLVE_IMPORT_DATA1D_H
#define DISSOLVE_IMPORT_DATA1D_H

#include "io/fileandformat.h"
#include "keywords/types.h"

// Forward Declarations
class Data1D;
class ProcessPool;

// Data1D Import Formats
class Data1DImportFileFormat : public FileAndFormat
{
	public:
	// Data1D Formats
	enum Data1DImportFormat { XYData1D, HistogramData1D, GudrunMintData1D, nData1DImportFormats };
	// Constructor
	Data1DImportFileFormat(Data1DImportFormat format = XYData1D);
	// Destructor
	~Data1DImportFileFormat();


	/*
	 * Format Access
	 */
	public:
	// Return number of available formats
	int nFormats() const;
	// Return formats array
	const char** formats() const;
	// Return nice formats array
	const char** niceFormats() const;
	// Return current format as Data1DImportFormat
	Data1DImportFormat data1DFormat() const;


	/*
	 * Filename / Basename
	 */
	public:
	// Return whether the file must exist
	bool fileMustExist() const
	{
		return true;
	}


	/*
	 * Import Functions
	 */
	private:
	// Import simple XY data using specified parser
	bool importXY(LineParser& parser, Data1D& data);
	// Import simple histogram data using specified parser
	bool importHistogram(LineParser& parser, Data1D& data);
	// Import Gudrun merged interference cross-section (mint) data using specified parser
	bool importGudrunMint(LineParser& parser, Data1D& data);

	public:
	// Import Data1D using current filename and format
	bool importData(Data1D& data, ProcessPool* procPool = NULL);
	// Import Data1D using supplied parser and current format
	bool importData(LineParser& parser, Data1D& data);
};

#endif

