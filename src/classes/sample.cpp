/*
	*** Sample Definition
	*** src/classes/sample.cpp
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/sample.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include <string.h>

// Constructor
Sample::Sample() : ListItem<Sample>()
{
	// Reference F(Q)
	hasReferenceData_ = false;
}

// Copy Constructor
Sample::Sample(const Sample& source)
{
	(*this) = source;
}

// Assignment Operator
void Sample::operator=(const Sample& source)
{
	// Basic Information
	name_ = source.name_;
	niceName_ = source.niceName_;

	// Reference Data
	hasReferenceData_ = source.hasReferenceData_;
	referenceDataFileName_ = source.referenceDataFileName_;
	referenceData_ = source.referenceData_;
}

/*
 * Basic Information
 */

// Set name of Sample
void Sample::setName(const char* name)
{
	 name_ = name;

	// Generate a nice name (i.e. no spaces, slashes etc.)
	niceName_ = name_;
	niceName_.replace(" /\\#*$", '_');
}

// Return name of Sample
const char* Sample::name() const
{
	return name_.get();
}

// Return nice name of the Sample
const char* Sample::niceName()
{
	return niceName_.get();
}

/*
 * Reference Data
 */

// Return whether reference data exists
bool Sample::hasReferenceData()
{
	return hasReferenceData_;
}

// Load reference F(Q) data
bool Sample::loadReferenceData(const char* filename)
{
	// Check that the specified file actually exists...
	if (!DUQSys::fileExists(filename))
	{
		Messenger::error("Sample reference data '%s' not found.\n", filename);
		return false;
	}

	referenceDataFileName_ = filename;
	referenceData_.clear();

	// Open file first...
	LineParser parser;
	parser.openInput(filename);
	if (!parser.isFileGoodForReading())
	{
		Messenger::error("Couldn't open datafile '%s'.\n", filename);
		return false;
	}

	Messenger::print("--> Loading reference data from '%s'...\n", referenceDataFileName_.get());
	bool result = true;
	while (!parser.eofOrBlank())
	{
		if (parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments) == 1)
		{
			Messenger::error("File error while reading '%s'.\n", referenceDataFileName_.get());
			result = false;
			break;
		}
		referenceData_.addPoint(parser.argd(0), parser.argd(1));
	}

	// Tidy up
	parser.closeFiles();
	hasReferenceData_ = result;

	return result;
}

// Return reference data filename (if any)
CharString& Sample::referenceDataFileName()
{
	return referenceDataFileName_;
}

// Return reference data
Data2D& Sample::referenceData()
{
	return referenceData_;
}

// Return calculated data
Data2D& Sample::calculatedData()
{
	return calculatedData_;
}

/*
 * Module List
 */

// Add Module (or an instance of it) to the Sample
Module* Sample::addModule(Module* masterInstance, bool autoAddDependents)
{
	return modules_.addModule(masterInstance, moduleData_, autoAddDependents);
}

// Return number of Modules associated to this Sample
int Sample::nModules() const
{
	return modules_.nModules();
}

// Return list of Modules associated to this Sample
RefList<Module,bool>& Sample::modules()
{
	return modules_.modules();
}

// Return list of data variables set by Modules
GenericList& Sample::moduleData()
{
	return moduleData_;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool Sample::broadcast(ProcessPool& procPool)
{
#ifdef PARALLEL
	int index, topeCount;
	double relPop;

	// Send name
	if (!procPool.broadcast(name_)) return false;

	// Reference data
	if (!procPool.broadcast(hasReferenceData_)) return false;
	if (!referenceData_.broadcast(procPool)) return false;
	if (!procPool.broadcast(referenceDataFileName_)) return false;
#endif
	return true;
}
