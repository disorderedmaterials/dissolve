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
#include "base/messenger.h"
#include "base/sysfunc.h"
#include <QOpenGLFramebufferObjectFormat>
#include <QPainter>
#include <QProgressDialog>

// Perform post-initialisation operations
void DataViewer::postInitialiseGL()
{
	// Recalculate viewport
	view_.recalculateViewport(contextWidth_, contextHeight_);
	view_.recalculateView();
}

// Perform post-resize operations
void DataViewer::postResizeGL()
{
	// Recalculate viewport
	view_.recalculateViewport(contextWidth_, contextHeight_);
	view_.recalculateView();
}

// Render content
void DataViewer::render(int xOffset, int yOffset)
{
	int axis;

	// Set colour mode
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	// Clear view
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Set some colours
	GLfloat colourBlack[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat colourGray[4] = { 0.8, 0.8, 0.8, 1.0 };
// 	GLfloat colourBlue[4] = { 0.88, 0.95, 1.0, 1.0 };
// 	GLfloat colourWhite[4] = { 1.0, 1.0, 1.0, 1.0 };

	// Loop over defined viewpanes
	GLdouble clipPlaneBottom[4] = { 0.0, 1.0, 0.0, 0.0 }, clipPlaneTop[4] = { 0.0, -1.0, 0.0, 0.0 };

	// If we are auto-following, set the axis limits here
	if (view_.autoFollowType() != View::NoAutoFollow) view_.autoFollowData();
	
	// Before we do anything else, make sure the view is up to date
	view_.recalculateView();

	// Set viewport
	glViewport(view_.viewportMatrix()[0] + xOffset, view_.viewportMatrix()[1] + yOffset, view_.viewportMatrix()[2], view_.viewportMatrix()[3]);
// 		printf("Viewport for pane '%s' is %i %i %i %i (offset = %i %i)\n" , qPrintable(view_.name()), view_.viewportMatrix()[0], view_.viewportMatrix()[1], view_.viewportMatrix()[2], view_.viewportMatrix()[3], xOffset, yOffset);

	// Setup an orthographic matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, view_.viewportMatrix()[2], 0, view_.viewportMatrix()[3], -10, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);

	// Draw a box around the pane
	if (!renderingOffScreen_)
	{
		glColor4fv(colourGray);
		glBegin(GL_LINE_LOOP);
		glVertex3i(0, 0, 1);
		glVertex3i(0, view_.viewportMatrix()[3]-1, 1);
		glVertex3i(view_.viewportMatrix()[2]-1, view_.viewportMatrix()[3]-1, 1);
		glVertex3i(view_.viewportMatrix()[2]-1, 0, 1);
		glEnd();
	}

	// Set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(view_.projectionMatrix().matrix());

	// Set modelview matrix as target for the remainder of the routine
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Get the pane's view matrix and rotation matrix inverse
	Matrix4 viewMatrix = view_.viewMatrix();
	Matrix4 viewRotationInverse = view_.viewRotationInverse();

	// Send axis primitives to the display first
	glLoadMatrixd(viewMatrix.matrix());
	glColor4fv(colourBlack);
	int skipAxis = -1;
	if (view_.viewType() == View::FlatXYView) skipAxis = 2;
	else if (view_.viewType() == View::FlatXZView) skipAxis = 1;
	else if (view_.viewType() == View::FlatZYView) skipAxis = 0;

	// -- Render axis text
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	if (fontInstance_.fontOK())
	{
		fontInstance_.font()->FaceSize(1);
		for (axis=0; axis<3; ++axis) if (view_.axes().visible(axis) && (axis != skipAxis))
		{
			view_.axes().labelPrimitive(axis).renderAll(fontInstance_, viewMatrix, viewRotationInverse, view_.textZScale());
			if (updateQueryDepth()) setQueryObject(DataViewer::AxisTickLabelObject, DissolveSys::itoa(axis));
			view_.axes().titlePrimitive(axis).renderAll(fontInstance_, viewMatrix, viewRotationInverse, view_.textZScale());
			if (updateQueryDepth()) setQueryObject(DataViewer::AxisTitleLabelObject, DissolveSys::itoa(axis));
		}
	}

	// -- Render axis (and grid) lines
	glLoadMatrixd(viewMatrix.matrix());
	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	for (axis=0; axis<3; ++axis) if (view_.axes().visible(axis) && (axis != skipAxis))
	{
		view_.axes().gridLineMinorStyle(axis).apply();
		view_.axes().gridLineMinorPrimitive(axis).sendToGL();
		if (updateQueryDepth()) setQueryObject(DataViewer::GridLineMinorObject, DissolveSys::itoa(axis));
	}
	for (axis=0; axis<3; ++axis) if (view_.axes().visible(axis) && (axis != skipAxis))
	{
		view_.axes().gridLineMajorStyle(axis).apply();
		view_.axes().gridLineMajorPrimitive(axis).sendToGL();
		if (updateQueryDepth()) setQueryObject(DataViewer::GridLineMajorObject, DissolveSys::itoa(axis));
	}
	LineStyle::revert();
	for (axis=0; axis<3; ++axis) if (view_.axes().visible(axis) && (axis != skipAxis))
	{
		view_.axes().axisPrimitive(axis).sendToGL();
		if (updateQueryDepth()) setQueryObject(DataViewer::AxisLineObject, DissolveSys::itoa(axis));
	}
	glEnable(GL_LIGHTING);
	glDisable(GL_LINE_SMOOTH);

	// Render bounding box
	view_.boundingBoxPrimitive().sendToGL();

	// Render selection markers (if needed)
	glLoadMatrixd(viewMatrix.matrix());
	if (interactionAxis() != -1)
	{
		// Note - we do not need to check for inverted or logarithmic axes here, since the transformation matrix A takes care of that
		Vec3<double> v;

		// Draw starting interaction point (if the interaction has been started)
		if (interactionStarted())
		{
			v[interactionAxis()] = clickedInteractionCoordinate() * view_.axes().stretch(interactionAxis());
			glTranslated(v.x, v.y, v.z);
			glColor4d(0.0, 0.0, 1.0, 0.5);
			view_.interactionPrimitive().sendToGL();
			view_.interactionBoxPrimitive().sendToGL();
		}

		// Draw current selection position
		v[interactionAxis()] = currentInteractionCoordinate() * view_.axes().stretch(interactionAxis()) - v[interactionAxis()];
		glTranslated(v.x, v.y, v.z);
		glColor4d(1.0, 0.0, 0.0, 0.5);
		view_.interactionPrimitive().sendToGL();
		view_.interactionBoxPrimitive().sendToGL();
	}

	// Setup clip planes to enforce Y-axis limits
	glLoadMatrixd(viewMatrix.matrix());
	glPushMatrix();
	glTranslated(0.0, view_.axes().clipPlaneYMin(), 0.0);
	glClipPlane(GL_CLIP_PLANE0, clipPlaneBottom);
	glEnable(GL_CLIP_PLANE0);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0.0, view_.axes().clipPlaneYMax(), 0.0);
	glClipPlane (GL_CLIP_PLANE1, clipPlaneTop);
	glEnable(GL_CLIP_PLANE1);
	glPopMatrix();

	// Draw all renderables
	for (Renderable* rend = renderables_.first(); rend != NULL; rend = rend->next)
	{
		if (!rend->isVisible()) continue;

		// If this is the collection to highlight, set color to transparent grey and disable material colouring....
		if (rend == highlightedRenderable_)
		{
			glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
			glDisable(GL_COLOR_MATERIAL);
		}

		// Set shininess for collection
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, rend->displaySurfaceShininess());

		rend->updateAndSendPrimitives(view_, groupManager_, renderingOffScreen_, renderingOffScreen_, context());

		// Update query
		if (updateQueryDepth()) setQueryObject(DataViewer::CollectionObject, rend->objectTag());

		glEnable(GL_COLOR_MATERIAL);
	}

	// Disable current clip planes
	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_PLANE1);

	/*
	 * Set orthographic, one-to-one pixel view
	 */

	// Setup an orthographic matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, view_.viewportMatrix()[2], 0, view_.viewportMatrix()[3], -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);

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
	// Render pane data - loop over collection targets
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
		// Grab copy of the relevant colour definition for this Collection
		ColourDefinition colourDefinition = groupManager_.colourDefinition(rend);

		// Draw line indicator
		glPushMatrix();
		glTranslated(-overlaySpacing, (overlayTextSize/2.0) - (rend->lineStyle().width()/2.0), 0.0);
		// -- What are we drawing for the line indicator?
		if (colourDefinition.colourSource() == ColourDefinition::SingleColourSource)
		{
			rend->lineStyle().apply();
			GLfloat lineWidth;
			glGetFloatv(GL_LINE_WIDTH, &lineWidth);
			glLineWidth(lineWidth*2.0);
			colourDefinition.colourScale().colour(0.0, colour);
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

	// Draw 2D interaction primitives
	glViewport(xOffset, yOffset, contextWidth_, contextHeight_);

	// Setup an orthographic matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, contextWidth_, 0, contextHeight_, -10, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);

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
