/*
	*** Chart HotSpot
	*** src/gui/charts/charthotspot.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_CHARTS_HOTSPOT_H
#define DISSOLVE_CHARTS_HOTSPOT_H

#include "templates/listitem.h"
#include <QRect>

// Forward Declarations
class ChartBlock;

// Chart HotSpot - Definition of an area onto which a dragged object can be dropped
class ChartHotSpot : public ListItem<ChartHotSpot>
{
	public:
	// Constructor / Destructor
	ChartHotSpot();
	~ChartHotSpot();
	// HotSpot type
	enum HotSpotType { InsertionHotSpot, AppendHotSpot, DeleteHotSpot, nHotSpotTypes };


	/*
	 * Definition
	 */
	private:
	// Hotspot type
	HotSpotType type_;
	// Row of the parent layout in which the hotspot exists
	int row_;
	// Column of the parent layout in which the hotspot exists
	int column_;
	// Hotspot geometry
	QRect geometry_;
	// ChartBlock immediately before the hot spot
	ChartBlock* blockBefore_;
	// ChartBlock immediately after the hot spot
	ChartBlock* blockAfter_;

	public:
	// Set type of the hotspot
	void setType(HotSpotType type);
	// Return the hotspot type
	HotSpotType type() const;
	// Set location information
	void setLocation(int row, int column);
	// Return row in which the hotspot exists
	int row() const;
	// Return column in which the hotspot exists
	int column() const;
	// Set area information
	void setGeometry(QRect geometry);
	// Set the height of the hotspot
	void setHeight(int height);
	// Return the geometry of the area
	QRect geometry() const;
	// Return if the supplied point exists within the hotspot
	bool contains(QPoint p) const;
	// Return pointer to ChartBlock immediately before the hotspot
	ChartBlock* blockBefore() const;
	// Return pointer to ChartBlock immediately after the hotspot
	ChartBlock* blockAfter() const;
};

#endif
