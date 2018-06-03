/*
	*** Module Chart HotSpot
	*** src/gui/modulecharthotspot.h
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

#ifndef DISSOLVE_MODULECHARTHOTSPOT_H
#define DISSOLVE_MODULECHARTHOTSPOT_H

#include "templates/listitem.h"
#include <QRect>

// Forward Declarations
class FlowBlock;

// Chart HotSpot - Definition of an area onto which a dragged object can be dropped
class ModuleChartHotSpot : public ListItem<ModuleChartHotSpot>
{
	public:
	// Constructor / Destructor
	ModuleChartHotSpot();
	~ModuleChartHotSpot();
	// HotSpot type
	enum HotSpotType { ModuleInsertionHotSpot, ModuleAppendHotSpot, ModuleDeleteHotSpot, nHotSpotTypes };


	/*
	 * Area Information
	 */
	private:
	// Row in which the hotspot exists
	int row_;
	// Area type
	HotSpotType type_;
	// Geometry of area
	QRect geometry_;
	// FlowBlock following area which, if an object is dropped here, it will appear before (if any)
	FlowBlock* flowBlockAfterArea_;

	public:
	// Set area information
	void set(int row, HotSpotType type, QRect geometry, FlowBlock* blockAfter);
	// Return row in which the hotspot exists
	int row() const;
	// Return the area type
	HotSpotType type() const;
	// Set the height of the defined area
	void setAreaHeight(int height);
	// Return the geometry of the area
	QRect geometry() const;
	// Return if the supplied point exists within the defined area
	bool contains(QPoint p) const;
	// FlowBlock following area which, if an object is dropped here, it will appear before (if any)
	FlowBlock* flowBlockAfterArea() const;
};

#endif
