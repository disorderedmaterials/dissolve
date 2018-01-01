/*
	*** Display DataSet
	*** src/gui/uchroma/classes/displaydataset.cpp
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

#include "gui/uchroma/classes/displaydataset.h"
#include <QMessageBox>

// Constructor
DisplayDataSet::DisplayDataSet() : ListItem<DisplayDataSet>()
{
}

// Destructor
DisplayDataSet::~DisplayDataSet()
{
}

// Add y value and associated flag
void DisplayDataSet::add(double y, DisplayDataSet::DataPointType type)
{
	y_.add(y);
	yType_.add(type);
}

// Add dummy y value and associated flag
void DisplayDataSet::addDummy()
{
	y_.add(0.0);
	yType_.add(DisplayDataSet::NoPoint);
}

// Return y array
const Array<double>& DisplayDataSet::y() const
{
	return y_;
}

// Return flags array
const Array<DisplayDataSet::DataPointType>& DisplayDataSet::yType() const
{
	return yType_;
}

// Set specific y value
void DisplayDataSet::setY(int index, double yNew, DisplayDataSet::DataPointType type)
{
	y_[index] = yNew;
	yType_[index] = type;
}

// Set z value of slice
void DisplayDataSet::setZ(double z)
{
	z_ = z;
}

// Return z value of slice
double DisplayDataSet::z() const
{
	return z_;
}
