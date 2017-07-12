/*
	*** PlotData Functions
	*** src/gui/qcustomplot/plotdata.cpp
	Copyright T. Youngs 2016-2017

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

#include "gui/qcustomplot/plotdata.h"

// Constructor
PlotData::PlotData(QString title) : title_(title)
{
}

// Copy Constructor
PlotData::PlotData(const PlotData& source)
{
	(*this) = source;
}

// Assignment Operator
PlotData& PlotData::operator=(const PlotData& source)
{
	title_ = source.title_;
	x_ = source.x_;
	y_ = source.y_;
}

// Set titles for data and axes
void PlotData::setTitle(QString title)
{
	title_ = title;
}

// Return X data
QVector<double>& PlotData::x()
{
	return x_;
}

// Return Y data
QVector<double>& PlotData::y()
{
	return y_;
}

// Return title of the data
QString PlotData::title()
{
	return title_;
}
