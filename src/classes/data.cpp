/*
	*** Data
	*** src/classes/data.cpp
	Copyright T. Youngs 2012-2017

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
#include "templates/genericlist.h"

// Constructor
Data::Data() : ListItem<Data>()
{
	// Data
	subtractAverageLevel_ = -1.0;
	neutronNormalisation_ = PartialsModule::NoNormalisation;
	type_ = Data::NeutronData;

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

// Set normalisation type for neutron data
void Data::setNeutronNormalisation(PartialsModule::NormalisationType normalisation)
{
	neutronNormalisation_ = normalisation;
}

// Return normalisation type for neutron data
PartialsModule::NormalisationType Data::neutronNormalisation()
{
	return neutronNormalisation_;
}

/*
 * Set up
 */

// Perform necessary tasks (normalisation etc.) on supplied data
bool Data::setUp(GenericList& processingModuleData)
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

	// Perform type-specific normalisation of contained data
	switch (type_)
	{
		case (Data::NeutronData):
			if (neutronNormalisation_ != PartialsModule::NoNormalisation)
			{
				// Probe the associated Module for Weights data
				if (!associatedModule_)
				{
					Messenger::error("No associated Module set for Data '%s', so can't perform normalisation of neutron data.\n", name_.get());
					return false;
				}
				bool found;
				Weights& weights = GenericListHelper<Weights>::retrieve(processingModuleData, "FullWeights", associatedModule_->uniqueName(), Weights(), &found);
				if (!found)
				{
					Messenger::error("Couldn't find FullWeights for Data '%s', and so can't perform requested normalisation.\n", name_.get());
					return false;
				}

				// Undo normalisation of data
				if (neutronNormalisation_ == PartialsModule::AverageOfSquaresNormalisation)
				{
					data_.arrayY() *= weights.boundCoherentAverageOfSquares();
					Messenger::print("Removed <b>**2 normalisation from Data '%s', factor = %f.\n", name_.get(), weights.boundCoherentAverageOfSquares());
				}
				else if (neutronNormalisation_ == PartialsModule::SquareOfAverageNormalisation)
				{
					data_.arrayY() *= weights.boundCoherentSquareOfAverage();
					Messenger::print("Removed <b**2> normalisation from Data '%s', factor = %f.\n", name_.get(), weights.boundCoherentSquareOfAverage());
				}
			}
			break;
		default:
			Messenger::error("No handler defined for set up of this type of data (%s)\n", name_.get());
			return false;
	}

	setUp_ = true;

	return true;
}

/*
 * Associated Module
 */

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
