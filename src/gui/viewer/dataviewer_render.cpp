/*
	*** Data Viewer - Rendering
	*** src/gui/viewer/dataviewer_render.cpp
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

// Perform post-initialisation operations
void DataViewer::postInitialiseGL()
{
}

// Perform post-resize operations
void DataViewer::postResizeGL()
{
}

// Render 2D overlay content
void DataViewer::render2DOverlay()
{
	const double overlaySpacing = 2.0;
	const double overlayTextSize = 12.0;
	const double legendLineLength = 10.0;

	/*
	 * Draw indicators in top-left corner
	 */

	CharString indicatorText;
	if (view_.autoFollowType() == View::AllAutoFollow) indicatorText += "|A| ";
	else if (view_.autoFollowType() == View::XFollow) indicatorText += "A\\sub{X} ";
	if (groupManager_.verticalShift() > 0) indicatorText.strcatf("S\\sub{%i}", groupManager_.verticalShift());
	TextPrimitive indicatorPrimitive;
	indicatorPrimitive.set(fontInstance_, indicatorText.get(), Vec3<double>(overlaySpacing, view_.viewportMatrix()[3] - overlaySpacing,0.0), TextPrimitive::TopLeftAnchor, Vec3<double>(), Matrix4(), overlayTextSize, false);
	glColor3d(0.0, 0.0, 0.0);
	Matrix4 identity;
	if (fontInstance_.fontOK()) indicatorPrimitive.render(fontInstance_, identity, identity, 1.0);

	/*
	 * Draw legend in top-right corner
	 */

	// Create RefList of legend entries
	RefList<Renderable,double> legendEntries;

	double maxTextWidth = -1.0;
	for (Renderable* rend = renderables_.first(); rend != NULL; rend = rend->next)
	{
		if (!rend->isVisible()) continue;

		double textWidth = fontInstance_.boundingBoxWidth(rend->name()) * overlayTextSize;
		legendEntries.add(rend, textWidth);
		if (textWidth > maxTextWidth) maxTextWidth = textWidth;
	}

	// Simple column layout - set the render position to be the left-hand edge of the longest text item
	glColor3d(0.0, 0.0, 0.0);
	glLoadIdentity();
	glTranslated(view_.viewportMatrix()[2] - maxTextWidth - overlaySpacing, view_.viewportMatrix()[3] - overlayTextSize - overlaySpacing, 0);

	// Loop over legend entries
	Vec4<float> colour;
	RefListIterator<Renderable,double> legendEntryIterator(legendEntries);
	while (Renderable* rend = legendEntryIterator.iterate())
	{
		// Grab copy of the relevant colour definition for this Renderable
		ColourDefinition colourDefinition = groupManager_.colourDefinition(rend);

		// Draw line indicator
		glPushMatrix();
		glTranslated(-overlaySpacing, (overlayTextSize/2.0) - (rend->lineStyle().width()/2.0), 0.0);
		// -- What are we drawing for the line indicator?
		if (colourDefinition.style() == ColourDefinition::SingleColourStyle)
		{
			rend->lineStyle().apply();
			GLfloat lineWidth;
			glGetFloatv(GL_LINE_WIDTH, &lineWidth);
			glLineWidth(lineWidth*2.0);
			colourDefinition.colour(0.0, colour);
			glColor4f(colour.x, colour.y, colour.z, colour.w);
			glBegin(GL_LINES);
			glVertex2i(0.0, 0.0);
			glVertex2i(-legendLineLength, 0.0);
			glEnd();
			rend->lineStyle().revert();
		}
		glPopMatrix();

		// Draw text
		glPushMatrix();
		glColor3d(0.0, 0.0, 0.0);
		glScaled(overlayTextSize, overlayTextSize, overlayTextSize);
		fontInstance_.font()->Render(rend->name());
		glPopMatrix();

		// Shift to next position
		glTranslated(0.0, -(overlayTextSize + overlaySpacing), 0.0);
	}

	// Draw interaction mode embellishments
	glLoadIdentity();

	if (interactionMode() == ViewInteraction)
	{
		// Draw selection box, if the interaction has started
		if (interactionStarted())
		{
			glColor3d(0.0, 0.0, 0.0);
			glLineWidth(1.0);
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(1, 0xf0f0);
			glBegin(GL_LINE_LOOP);
			glVertex2d(rMouseDown_.x, contextHeight_-rMouseDown_.y);
			glVertex2d(rMouseLast_.x, contextHeight_-rMouseDown_.y);
			glVertex2d(rMouseLast_.x, contextHeight_-rMouseLast_.y);
			glVertex2d(rMouseDown_.x, contextHeight_-rMouseLast_.y);
			glEnd();
			glDisable(GL_LINE_STIPPLE);
		}
	}
}
