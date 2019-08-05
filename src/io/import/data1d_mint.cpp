/*
	*** Import - Gudrun MINT Data1D
	*** src/io/import/data1d_mint.cpp
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

#include "io/import/data1d.h"
#include "math/data1d.h"
#include "math/filters.h"
#include "base/lineparser.h"

// Read Gudrun merged interference cross-section (mint) data using specified parser
bool Data1DImportFileFormat::importGudrunMint(LineParser& parser, Data1D& data)
{
	// Read in the data first (assuming simple XY format)
	if (!importXY(parser, data)) return false;

	// Gudrun mint01 files are in histogram format, so convert bin boundaries to centre-bin values
	Filters::convertBinBoundaries(data);

	return true;
}
