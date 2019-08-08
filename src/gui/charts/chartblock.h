/*
	*** Chart Block
	*** src/gui/charts/chartblock.h
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

#ifndef DISSOLVE_CHARTS_BLOCK_H
#define DISSOLVE_CHARTS_BLOCK_H

#include "templates/listitem.h"
#include <QPoint>
#include <QRect>

// Forward Declarations
class QWidget;

// Chart Block - Base class for any widget displayed in a chart
class ChartBlock : public ListItem<ChartBlock>
{
	public:
	// Constructor / Destructor
	ChartBlock();
	virtual ~ChartBlock();


	/*
	 * Type
	 */
	public:
	// Return type of this block
	virtual const char* blockType() = 0;


	/*
	 * Widget
	 */
	private:
	// Proposed new geometry of the widget
	QRect newGeometry_;
	
	public:
	// Return underlying widget
	virtual QWidget* widget() = 0;
	// Return width of underlying widget
	virtual int widgetWidth() const = 0;
	// Return height of underlying widget
	virtual int widgetHeight() const = 0;
	// Set underlying widget position
	void setNewPosition(int left, int top);
	// Set underlying widget geometry
	void setNewSize(int width, int height);
	// Commit new geometry for widget
	void setNewGeometry(bool animate);
	// Return whether the supplied point (in global coordinates) allows a drag operation to begin
	virtual bool isDragPoint(QPoint globalPos) const = 0;


	/*
	 * Update
	 */
	public:
	// Update controls within widget
	virtual void updateControls() = 0;
	// Disable sensitive controls
	virtual void disableSensitiveControls() = 0;
	// Enable sensitive controls
	virtual void enableSensitiveControls() = 0;
};

#endif
