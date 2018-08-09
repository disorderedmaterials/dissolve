/*
	*** Module Chart Hot Spot
	*** src/gui/modulecharthotspot.cpp
	Copyright T. Youngs 2012-2018

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

#include "gui/modulecharthotspot.h"
#include <stdio.h>

// Constructor
ModuleChartHotSpot::ModuleChartHotSpot() : ListItem<ModuleChartHotSpot>()
{
	row_ = -1;
	type_ = ModuleChartHotSpot::nHotSpotTypes;
	moduleBlockAfter_ = NULL;
}

ModuleChartHotSpot::~ModuleChartHotSpot()
{
}

/*
 * Area Information
 */

// Set area information
void ModuleChartHotSpot::set(int row, HotSpotType type, QRect geometry, ModuleChartModuleBlock* moduleBlockAfter)
{
	row_ = row;
	type_ = type;
	geometry_ = geometry;
	moduleBlockAfter_ = moduleBlockAfter;
}

// Return row in which the hotspot exists
int ModuleChartHotSpot::row() const
{
	return row_;
}

// Return the area type
ModuleChartHotSpot::HotSpotType ModuleChartHotSpot::type() const
{
	return type_;
}

// Set the height of the defined area
void ModuleChartHotSpot::setAreaHeight(int height)
{
	geometry_.setHeight(height);
}

// Return the geometry of the area
QRect ModuleChartHotSpot::geometry() const
{
	return geometry_;
}

// Return if the supplied point exists within the defined area
bool ModuleChartHotSpot::contains(QPoint p) const
{
	return geometry_.contains(p);
}

// Return pointer to ModuleChartModuleBlock immediately after the hot spot
ModuleChartModuleBlock* ModuleChartHotSpot::moduleBlockAfter() const
{
	return moduleBlockAfter_;
}
