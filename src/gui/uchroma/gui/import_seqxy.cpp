/*
	*** Data Import Dialog - Sequential XY Import 
	*** src/gui/uchroma/gui/dataimport_seqxy.cpp
	Copyright T. Youngs 2013-2018

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/gui/import.h"
#include "base/lineparser.h"

// Import sequential XY data
bool ImportDialog::importSequentialXY()
{
	// Clear any old imported slices
	importedDataSets_.clear();

	// Grab some values from the UI
	Vec3<int> columns(ui.SeqXYColumnXSpin->value()-1, ui.SeqXYColumnYSpin->value()-1,  ui.SeqXYColumnZSpin->value()-1);
	int maxColumn = columns.max();
	int nStartSkip = ui.SeqXYNSkip->value();
	
	// Open file and check that we're OK to proceed reading from it
	LineParser parser;
	parser.openInput(qPrintable(ui.DataFileEdit->text()));

	if (!parser.isFileGoodForReading())
	{
		Messenger::print("Couldn't open file '%s' for reading.\n", qPrintable(ui.DataFileEdit->text()));
		return false;
	}

	// Set up initial dataSet
	DataSet* dataSet = importedDataSets_.add();

	// Skip lines at start
	if (nStartSkip > 0) parser.skipLines(nStartSkip);

	LineParser::ParseReturnValue result;
	Vec3<int> count(0, 0, 0);
	while (!parser.eofOrBlank())
	{
		if (nStartSkip == -1)
		{
			result = parser.getArgsDelim(LineParser::SkipBlanks);
			nStartSkip = 0;
		}
		else result = parser.getArgsDelim(LineParser::Defaults);
		if (result != LineParser::Success)
		{
			parser.closeFiles();
			Messenger::print("Error reading from file '%s'.\n", qPrintable(ui.DataFileEdit->text()));
			return false;
		}

		// Is there anything on this line?
		if (parser.nArgs() == 0)
		{
			// Must be between slices in the file - check the current x count.
			// If it is non-zero, create a new Slice for the next round...
			if (count.x != 0)
			{
				dataSet = importedDataSets_.add();
				++count.z;
			}
			count.x = 0;
			continue;
		}

		// Check requested columns against available columns
		if (maxColumn >= parser.nArgs()) Messenger::print("Not enough columns in file.\n");

		// Add datapoint
		dataSet->addPoint(columns.x == -1 ? count.x : parser.argd(columns.x), parser.argd(columns.y));

		// Set z value for slice
		dataSet->setZ(columns.z == -1 ? count.z : parser.argd(columns.z));

		// Increase x count
		++count.x;
	}

	return true;
}
