/*
	*** Display DataSet
	*** src/gui/uchroma/classes/displaydataset.h
	Copyright T. Youngs 2013-2017

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

#ifndef DUQ_UCHROMADISPLAYDATASET_H
#define DUQ_UCHROMADISPLAYDATASET_H

#include "base/xydata.h"
#include "templates/list.h"

// Display DataSet
class DisplayDataSet : public ListItem<DisplayDataSet>
{
	public:
	// Constructor / Destructor
	DisplayDataSet();
	~DisplayDataSet();
	// Data point type enum
	enum DataPointType { RealPoint, InterpolatedPoint, NoPoint };


	/*
	 * Data
	 */
	private:
	// Y values of slice data
	Array<double> y_;
	// Type of y value at each point
	Array<DataPointType> yType_;
	// Z value of slice
	double z_;

	public:
	// Add y value and associated flag
	void add(double y, DisplayDataSet::DataPointType type);
	// Add dummy y value and associated flag
	void addDummy();
	// Return y array
	const Array<double>& y() const;
	// Return type array
	const Array<DisplayDataSet::DataPointType>& yType() const;
	// Set specific y value
	void setY(int index, double yNew, DisplayDataSet::DataPointType type);
	// Set z value of slice
	void setZ(double z);
	// Return z value of slice
	double z() const;
};

#endif
