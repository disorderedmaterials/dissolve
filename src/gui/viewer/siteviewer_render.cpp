/*
	*** Species Viewer - Render
	*** src/gui/viewer/siteviewer_render.cpp
	Copyright T. Youngs 2019

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

#include "gui/viewer/siteviewer.hui"

// Perform post-initialisation operations
void SiteViewer::postInitialiseGL()
{
}

// Perform post-resize operations
void SiteViewer::postResizeGL()
{
}

// Render 2D overlay content
void SiteViewer::render2DOverlay()
{
	/*
	 * Draw interaction mode embellishments
	 */
	glLoadIdentity();

	static LineStyle selectionBoxStyle(1.0, LineStipple::HalfDashStipple);

	if (interacting()) switch (interactionMode())
	{
		case (SiteViewer::SelectAreaInteraction):
			// Draw dashed box indicating selection area, form clicked to current mouse coordinates
			selectionBoxStyle.sendToGL();
			glBegin(GL_LINE_LOOP);
			glVertex2d(rMouseDown_.x, rMouseDown_.y);
			glVertex2d(rMouseLast_.x, rMouseDown_.y);
			glVertex2d(rMouseLast_.x, rMouseLast_.y);
			glVertex2d(rMouseDown_.x, rMouseLast_.y);
			glEnd();
			break;
		default:
			break;
	}
}
