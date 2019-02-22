/*
	*** Data Viewer - Input
	*** src/gui/viewer/dataviewer_input.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/viewer/dataviewer.hui"
#include <QMouseEvent>
#include <QMenu>

// Mouse Moved
void DataViewer::mouseMoved(int dx, int dy, Qt::KeyboardModifiers modifiers)
{
	bool refresh = false;

	if (buttonState_&Qt::LeftButton)
	{
		// What we do here depends on the current mode, and whether interaction has started
		switch (interactionMode())
		{
			case (DataViewer::ViewInteraction):
				if (interactionStarted()) refresh = true;
				break;
			default:
				break;
		}
	}
	else if (buttonState_&Qt::RightButton)
	{
		// View manipulation
		if (modifiers&Qt::ShiftModifier)
		{
		}
		else if (modifiers&Qt::ControlModifier)
		{
		}
		else 
		{
			view_.rotateView(dy/2.0, dx/2.0);
			refresh = true;
		}
	}
	else if (buttonState_&Qt::MidButton)
	{
		// If this is a flat view, shift the axis limits rather than the view
		if (view_.isFlatView()) view_.shiftFlatAxisLimits(dx, -dy);
		else view_.translateView(dx/15.0, -dy/15.0, 0.0);
		refresh = true;
	}
	
	// Update interaction position
	if (updateInteractionPosition(rMouseLast_.x, contextHeight_-rMouseLast_.y)) refresh = true;

	// Notify any interested widgets that our current coordinate has changed
	emit(currentCoordinateChanged());

	if (refresh) postRedisplay();
}

// Mouse 'wheeled'
void DataViewer::mouseWheeled(int delta)
{
	bool scrollup = delta > 0;

	// Perform camera zoom
	double zrange = view_.axes().stretch(2) * view_.axes().realRange(2);
	if (zrange < 1.0) zrange = 1.0;
	view_.translateView(0.0, 0.0, 0.5*zrange*(scrollup ? -1.0 : 1.0));

	postRedisplay();
}

// Mouse double clicked
void DataViewer::mouseDoubleClicked()
{
	printf("Mouse double-click event has occurred!\n");
	doubleClickInteraction(rMouseLast_.x, height()-rMouseLast_.y);
}

// Key pressed
bool DataViewer::keyPressed(int key, Qt::KeyboardModifiers modifiers)
{
	bool refresh = true;
	bool accept = true;
	switch (key)
	{
		case (Qt::Key_Left):
			if (view_.isFlatView()) view_.shiftFlatAxisLimitsFractional(-0.1, 0.0);
			else view_.rotateView(0.0, modifiers.testFlag(Qt::ShiftModifier) ? -1.0 : -10.0);
			break;
		case (Qt::Key_Right):
			if (view_.isFlatView()) view_.shiftFlatAxisLimitsFractional(0.1, 0.0);
			else view_.rotateView(0.0, modifiers.testFlag(Qt::ShiftModifier) ? 1.0 : 10.0);
			break;
		case (Qt::Key_Up):
			if (view_.isFlatView()) view_.shiftFlatAxisLimitsFractional(0.0, -0.1);
			else view_.rotateView(modifiers.testFlag(Qt::ShiftModifier) ? -1.0 : -10.0, 0.0);
			break;
		case (Qt::Key_Down):
			if (view_.isFlatView()) view_.shiftFlatAxisLimitsFractional(0.0, 0.1);
			else view_.rotateView(modifiers.testFlag(Qt::ShiftModifier) ? 1.0 : 10.0, 0.0);
			break;
		case (Qt::Key_A):
			if (modifiers.testFlag(Qt::ShiftModifier))
			{
				// Show only top 20% of vertical axis
				if (view_.viewType() == View::FlatXYView) view_.showAllData(1.0, 0.2);
				else if (view_.viewType() == View::FlatXZView) view_.showAllData(1.0, 1.0, 0.2);
				else if (view_.viewType() == View::FlatZYView) view_.showAllData(1.0, 0.2);
			}
			else view_.showAllData();
			break;
		case (Qt::Key_F):
			view_.cycleAutoFollowType();
			break;
		case (Qt::Key_L):
			if (modifiers.testFlag(Qt::ShiftModifier)) view_.axes().toggleLogarithmic(view_.viewType() == View::FlatXZView ? 2 : 1);
			else view_.axes().toggleLogarithmic(view_.viewType() == View::FlatZYView ? 2 : 0);
			break;
		case (Qt::Key_S):
			groupManager_.cycleVerticalShifts();
			break;
		default:
			refresh = false;
			accept = false;
			break;
	}
	
	// Update display if necessary
	if (refresh) postRedisplay();

	return accept;
}

// Key released
bool DataViewer::keyReleased(int key, Qt::KeyboardModifiers modifiers)
{
	bool refresh = false, accept = true;
	
	switch (key)
	{
		// Cycle render styles
		case (Qt::Key_F8):
			break;
		// Cycle colouring styles
		case (Qt::Key_F9):
			break;
		default:
			accept = false;
			break;
	}
	
	// Update display if necessary
	if (refresh) postRedisplay();

	return accept;
}
