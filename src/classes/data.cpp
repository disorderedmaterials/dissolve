/*
	*** Data
	*** src/classes/data.cpp
	Copyright T. Youngs 2012-2018

	This file is part of Data.

	Data is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Data is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Data.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/data.h"
#include "classes/weights.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Constructor
Data::Data() : ListItem<Data>()
{
	// Data
	subtractAverageLevel_ = -1.0;
	type_ = Data::NeutronData;

	// Associated Module
	associatedModule_ = NULL;

	// Setup
	setUp_ = false;
}

// Destructor
Data::~Data()
{
}

/*
 * Name
 */

// Set name of the data
void Data::setName(const char* name)
{
	name_ = name;
	niceName_ = DUQSys::niceName(name);

	data_.setObjectName(CharString("Data//%s", niceName_.get()));
}

// Return name of the Data
const char* Data::name()
{
	return name_.get();
}

// Return nice name of the Data
const char* Data::niceName()
{
	return niceName_.get();
}

/*
 * Data
 */

// DataType keywords
const char* DataTypeKeywords[] = { "Neutron" };

// Convert text string to DataType
Data::DataType Data::dataType(const char* s)
{
	for (int n=0; n<Data::nDataTypes; ++n) if (DUQSys::sameString(s,DataTypeKeywords[n])) return (Data::DataType) n;
	return Data::nDataTypes;
}

// Convert DataType to text string
const char* Data::dataType(Data::DataType dt)
{
	return DataTypeKeywords[dt];
}

// Type of supplied data
void Data::setType(Data::DataType dt)
{
	type_ = dt;
}

// Return type of supplied data
Data::DataType Data::type()
{
	return type_;
}

// Load XY reference data from specified file
bool Data::loadData(ProcessPool& procPool, const char* filename, int xcol, int ycol)
{
	LineParser fileParser(&procPool);
	if (!fileParser.openInput(filename)) return false;
	return data_.load(fileParser, xcol, ycol);
}

// Return XY reference data
XYData& Data::data()
{
	return data_;
}

// Set X value from which to calculate and subtract average Y level, if any
void Data::setSubtractAverageLevel(double xMin)
{
	subtractAverageLevel_ = xMin;
}

// Return X value from which to calculate and subtract average Y level, if any
double Data::subtractAverageLevel()
{
	return subtractAverageLevel_;
}

// Return weights matrix for scattering data
Weights& Data::scatteringWeights()
{
	return scatteringWeights_;
}

/*
 * Set up
 */

// Perform necessary tasks (normalisation etc.) on supplied data
bool Data::setUp(GenericList& processingModuleData, bool strict)
{
	// If we're already set up return now 
	if (setUp_) return true;

	// Subtract average level from data?
	if (subtractAverageLevel_ >= 0.0)
	{
		double sum = 0.0;
		int nPoints = 0;
		for (int n=0; n<data_.nPoints(); ++n)
		{
			if (data_.x(n) >= subtractAverageLevel_)
			{
				sum += data_.y(n);
				++nPoints;
			}
		}
		data_.arrayY() -= sum / nPoints;
		Messenger::print("Removed average level from Data '%s': from x >= %f, value = %f.\n", name_.get(), subtractAverageLevel_, sum / nPoints);
	}

	setUp_ = true;

	return true;
}

// Return whether data is set up
bool Data::isSetUp() const
{
	return setUp_;
}

/*
 * Associated Module
 */

// Set associated  name
void Data::setAssociatedModuleName(const char* name)
{
	associatedModuleName_ = name;
}

// Return whether an associated Module name has been set
bool Data::hasAssociatedModuleName() const
{
	return (!associatedModuleName_.isEmpty());
}

// Return associated Module name
const char* Data::associatedModuleName() const
{
	return associatedModuleName_.get();
}

// Set associated Module
void Data::setAssociatedModule(Module* module)
{
	associatedModule_ = module;
}

// Return associated Module
Module* Data::associatedModule()
{
	return associatedModule_;
}

// Return whether the associated Module is of the type specified
bool Data::isAssociatedModule(const char* moduleName)
{
	if (!associatedModule_) return false;

	return DUQSys::sameString(associatedModule_->name(), moduleName);
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool Data::broadcast(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
#endif
	return true;
}
