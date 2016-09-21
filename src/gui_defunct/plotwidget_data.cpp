/*
	*** PlotData
	*** src/gui/plotdata.cpp
	Copyright T. Youngs 2012-2013

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

#include "gui/plotwidget.uih"
#include "base/messenger.h"

// Constructor
PlotData::PlotData() : ListItem<PlotData>()
{
	externalSourceData_ = NULL;
	lineColour_ = Qt::black;
	lineStyle_ = Qt::SolidLine;
	xDataModifier_ = PlotData::NoModifier;
	yDataModifier_ = PlotData::NoModifier;
}

// Destructor
PlotData::~PlotData()
{
}

/*
 * Plot Data
 */

/*
 * \brief Return modified value
 */
double PlotData::modifiedValue(double value, PlotData::DataModifier modifier)
{
	switch (modifier)
	{
		case (PlotData::NoModifier):
			return value;
		case (PlotData::SqrtModifier):
			return sqrt(value);
		case (PlotData::nDataModifiers):
			msg.print("INTERNAL_ERROR - No DataModifier set in PlotData.\n");
			return value;
		default:
			msg.print("INTERNAL_ERROR - DataModifier %i not accounted for in PlotData::modifiedValue().\n", modifier);
			return value;
	}
}

/*
 * \brief Set internal source data
 */
void PlotData::setSourceData(Data2D& source, PlotData::DataModifier xModifier, PlotData::DataModifier yModifier)
{
	localSourceData_ = source;
	externalSourceData_ = NULL;
	xDataModifier_ = xModifier;
	yDataModifier_ = yModifier;
}

/*
 * \brief Set source data (external CheckPoint<Data2D>)
 */
void PlotData::setSourceData(CheckPoint< Data2D >* source, PlotData::DataModifier xModifier, PlotData::DataModifier yModifier)
{
	externalSourceData_ = source;
	xDataModifier_ = xModifier;
	yDataModifier_ = yModifier;
}

/*
 * \brief Return external source data 
 */
CheckPoint<Data2D>* PlotData::externalSourceData()
{
	return externalSourceData_;
}

/*
 * \brief (Re)Generate display list
 */
void PlotData::generate()
{
	// Check out the required Data2D
	Data2D& data = externalSourceData_ ? externalSourceData_->checkOut() : localSourceData_;

	// Copy name of dataset while we're here...
	name_ = data.name();

	// Generate QPainterPath, determining minimum / maximum values along the way
	painterPath_ = QPainterPath();
	const double* xarray = data.arrayX().array();
	const double* yarray = data.arrayY().array();
	double x, y;
	if ((xarray != NULL) || (yarray != NULL))
	{
		// Get first point and use to set min/max values and painter path origin
		x = modifiedValue(xarray[0], xDataModifier_);
		y = modifiedValue(yarray[0], yDataModifier_);
		xMin_ = x;
		xMax_ = x;
		yMin_ = y;
		yMax_ = y;
		painterPath_.moveTo(x, y);
		for (int n=1; n<data.arrayX().nItems(); ++n)
		{
			// Grab modified array values
			x = modifiedValue(xarray[n], xDataModifier_);
			y = modifiedValue(yarray[n], yDataModifier_);

			// Append to path, and recalculate min/max values
			painterPath_.lineTo(x, y);
			if (x > xMax_) xMax_ = x;
			if (x < xMin_) xMin_ = x;
			if (y > yMax_) yMax_ = y;
			if (y < yMin_) yMin_ = y;
		}
	}

	// Done with Data2D reference, so check it back in (if external source was used)
	if (externalSourceData_) externalSourceData_->checkIn();
}

/*
 * \brief Set group index
 */
void PlotData::setGroupIndex(int index)
{
	groupIndex_ = index;
}

/*
 * \brief Return group index
 */
int PlotData::groupIndex()
{
	return groupIndex_;
}

/*
 * \brief Return QPainterPath
 */
QPainterPath& PlotData::painterPath()
{
	return painterPath_;
}

/*
 * \brief Set whether this data is visible
 */
void PlotData::setVisible(bool v)
{
	visible_ = v;
}

/*
 * \brief Return whether this data is visible
 */
bool PlotData::visible()
{
	return visible_;
}

/*
 * \brief Set vertical offset to apply to data
 */
void PlotData::setVerticalOffset(int offset)
{
	verticalOffset_ = offset;
}

/*
 * \brief Return vertical offset to apply to data
 */
int PlotData::verticalOffset()
{
	return verticalOffset_;
}

/*
 * \brief Return minimum x value for data
 */
double PlotData::xMin()
{
	return xMin_;
}

/*
 * \brief Return maximum x value for data
 */
double PlotData::xMax()
{
	return xMax_;
}

/*
 * \brief Return minimum y value for data
 */
double PlotData::yMin()
{
	return yMin_;
}

/*
 * \brief Return maximum y value for data
 */
double PlotData::yMax()
{
	return yMax_;
}

/*
 * \brief Return name
 */
QString PlotData::name()
{
	return name_;
}

/*
 * Style
 */

/*
 * \brief Set line colour
 */
void PlotData::setLineColour(QColor color)
{
	lineColour_ = color;
}

/*
 * \brief Return line colour
 */
QColor PlotData::lineColour()
{
	return lineColour_;
}

/*
 * \brief Set line style
 */
void PlotData::setLineStyle(Qt::PenStyle style)
{
	lineStyle_ = style;
}

/*
 * \brief Return line style
 */
Qt::PenStyle PlotData::lineStyle()
{
	return lineStyle_;
}
