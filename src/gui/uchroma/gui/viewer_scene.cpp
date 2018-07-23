/*
	*** Viewer - Main scene rendering
	*** src/gui/uchroma/gui/viewer_scene.cpp
	Copyright T. Youngs 2013-2018

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

#include "gui/uchroma/gui/viewer.hui"
#include "gui/uchroma/uchromabase.h"
#include "gui/uchroma/render/fontinstance.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include <QOpenGLFramebufferObjectFormat>
#include <QPainter>
#include <QProgressDialog>

// Setup basic GL properties (called each time before renderScene())
void UChromaViewer::setupGL()
{
	// Define colours etc.
	GLfloat backgroundColour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat spotlightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat spotlightDiffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat spotlightSpecular[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat spotlightPosition[4] = { 1.0f, 1.0f, 0.0f, 0.0f };
	GLfloat specularColour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Clear (background) colour
	glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]);

	// Perspective hint
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// Smooth shading
	glShadeModel(GL_SMOOTH);

	// Auto-normalise surface normals
	glEnable(GL_NORMALIZE);	

	// Set up the light model
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, spotlightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, spotlightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spotlightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, spotlightPosition);
	glEnable(GL_LIGHT0);

	// Set specular reflection colour
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColour);

	// Configure antialiasing
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	// Configure fog effects
//	glFogi(GL_FOG_MODE, GL_LINEAR);
//	prefs.copyColour(Prefs::BackgroundColour, col);
//	glFogfv(GL_FOG_COLOR, col);
//	glFogf(GL_FOG_DENSITY, 0.35f);
//	glHint(GL_FOG_HINT, GL_NICEST);
//	glFogi(GL_FOG_START, prefs.depthNear());
//	glFogi(GL_FOG_END, prefs.depthFar());
//	glEnable(GL_FOG);
	glDisable(GL_FOG);

	// Configure face culling
// 	glCullFace(GL_BACK);
// 	glEnable(GL_CULL_FACE);
}

// Draw full scene
void UChromaViewer::renderFullScene(int xOffset, int yOffset)
{
	// If the Viewer has not been correctly set up, return here
	if (!valid_) return;

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
	for (ViewPane* pane = uChromaBase_->viewLayout().panes(); pane != NULL; pane = pane->next)
	{
		// If we are auto-following, set the axis limits here
		if (pane->autoFollowType() != ViewPane::NoAutoFollow) pane->autoFollowData();
		
		// Before we do anything else, make sure the view is up to date
		pane->recalculateView();

		// Set viewport
		glViewport(pane->viewportMatrix()[0] + xOffset, pane->viewportMatrix()[1] + yOffset, pane->viewportMatrix()[2], pane->viewportMatrix()[3]);
// 		printf("Viewport for pane '%s' is %i %i %i %i (offset = %i %i)\n" , qPrintable(pane->name()), pane->viewportMatrix()[0], pane->viewportMatrix()[1], pane->viewportMatrix()[2], pane->viewportMatrix()[3], xOffset, yOffset);

		// Setup an orthographic matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, pane->viewportMatrix()[2], 0, pane->viewportMatrix()[3], -10, 10);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_LIGHTING);

		// Draw graduated background for current pane (only if rendering on-screen)
// 		if ((pane == uChromaBase_->currentViewPane()) && (!renderingOffScreen_))
// 		{
// 			glBegin(GL_QUADS);
// 			glColor4fv(colourBlue);
// 			glVertex3i(0, 0, 0);
// 			glVertex3i(pane->viewportMatrix()[2]-1, 0, 0);
// 			glColor4fv(colourWhite);
// 			glVertex3i(pane->viewportMatrix()[2]-1, pane->viewportMatrix()[3]*0.5, 0);
// 			glVertex3i(0, pane->viewportMatrix()[3]*0.5, 0);
// 			glEnd();
// 		}

		// Draw a box around the pane
		if (!renderingOffScreen_)
		{
			glColor4fv(colourGray);
			glBegin(GL_LINE_LOOP);
			glVertex3i(0, 0, 1);
			glVertex3i(0, pane->viewportMatrix()[3]-1, 1);
			glVertex3i(pane->viewportMatrix()[2]-1, pane->viewportMatrix()[3]-1, 1);
			glVertex3i(pane->viewportMatrix()[2]-1, 0, 1);
			glEnd();
		}

		// Set projection matrix
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixd(pane->projectionMatrix().matrix());

		// Set modelview matrix as target for the remainder of the routine
		glMatrixMode(GL_MODELVIEW);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Get the pane's view matrix and rotation matrix inverse
		Matrix4 viewMatrix = pane->viewMatrix();
		Matrix4 viewRotationInverse = pane->viewRotationInverse();

		// Send axis primitives to the display first
		glLoadMatrixd(viewMatrix.matrix());
		glColor4fv(colourBlack);
		int skipAxis = -1;
		if (pane->viewType() == ViewPane::FlatXYView) skipAxis = 2;
		else if (pane->viewType() == ViewPane::FlatXZView) skipAxis = 1;
		else if (pane->viewType() == ViewPane::FlatZYView) skipAxis = 0;

		// -- Render axis text
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		if (fontInstance_.fontOK())
		{
			fontInstance_.font()->FaceSize(1);
			for (axis=0; axis<3; ++axis) if (pane->axes().visible(axis) && (axis != skipAxis))
			{
				pane->axes().labelPrimitive(axis).renderAll(fontInstance_, viewMatrix, viewRotationInverse, pane->textZScale());
				if (updateQueryDepth()) setQueryObject(UChromaViewer::AxisTickLabelObject, DissolveSys::itoa(axis));
				pane->axes().titlePrimitive(axis).renderAll(fontInstance_, viewMatrix, viewRotationInverse, pane->textZScale());
				if (updateQueryDepth()) setQueryObject(UChromaViewer::AxisTitleLabelObject, DissolveSys::itoa(axis));
			}
		}

		// -- Render axis (and grid) lines
		glLoadMatrixd(viewMatrix.matrix());
		glDisable(GL_LIGHTING);
		glEnable(GL_LINE_SMOOTH);
		for (axis=0; axis<3; ++axis) if (pane->axes().visible(axis) && (axis != skipAxis))
		{
			pane->axes().gridLineMinorStyle(axis).apply();
			pane->axes().gridLineMinorPrimitive(axis).sendToGL();
			if (updateQueryDepth()) setQueryObject(UChromaViewer::GridLineMinorObject, DissolveSys::itoa(axis));
		}
		for (axis=0; axis<3; ++axis) if (pane->axes().visible(axis) && (axis != skipAxis))
		{
			pane->axes().gridLineMajorStyle(axis).apply();
			pane->axes().gridLineMajorPrimitive(axis).sendToGL();
			if (updateQueryDepth()) setQueryObject(UChromaViewer::GridLineMajorObject, DissolveSys::itoa(axis));
		}
		LineStyle::revert();
		for (axis=0; axis<3; ++axis) if (pane->axes().visible(axis) && (axis != skipAxis))
		{
			pane->axes().axisPrimitive(axis).sendToGL();
			if (updateQueryDepth()) setQueryObject(UChromaViewer::AxisLineObject, DissolveSys::itoa(axis));
		}
		glEnable(GL_LIGHTING);
		glDisable(GL_LINE_SMOOTH);

		// Render bounding box
		pane->boundingBoxPrimitive().sendToGL();

		// Render selection markers (if needed)
		glLoadMatrixd(viewMatrix.matrix());
		int interactionAxis = uChromaBase_->interactionAxis();
		if ((pane == uChromaBase_->currentViewPane()) && (interactionAxis != -1))
		{
			// Note - we do not need to check for inverted or logarithmic axes here, since the transformation matrix A takes care of that
			Vec3<double> v;

			// Draw starting interaction point (if the interaction has been started)
			if (uChromaBase_->interactionStarted())
			{
				v[interactionAxis] = uChromaBase_->clickedInteractionCoordinate() * pane->axes().stretch(interactionAxis);
				glTranslated(v.x, v.y, v.z);
				glColor4d(0.0, 0.0, 1.0, 0.5);
				pane->interactionPrimitive().sendToGL();
				pane->interactionBoxPrimitive().sendToGL();
			}

			// Draw current selection position
			v[interactionAxis] = 	uChromaBase_->currentInteractionCoordinate() * pane->axes().stretch(interactionAxis) - v[interactionAxis];
			glTranslated(v.x, v.y, v.z);
			glColor4d(1.0, 0.0, 0.0, 0.5);
			pane->interactionPrimitive().sendToGL();
			pane->interactionBoxPrimitive().sendToGL();
		}

		// Setup clip planes to enforce Y-axis limits
		glLoadMatrixd(viewMatrix.matrix());
		glPushMatrix();
		glTranslated(0.0, pane->axes().clipPlaneYMin(), 0.0);
		glClipPlane(GL_CLIP_PLANE0, clipPlaneBottom);
		glEnable(GL_CLIP_PLANE0);
		glPopMatrix();
		glPushMatrix();
		glTranslated(0.0, pane->axes().clipPlaneYMax(), 0.0);
		glClipPlane (GL_CLIP_PLANE1, clipPlaneTop);
		glEnable(GL_CLIP_PLANE1);
		glPopMatrix();

		// Render pane data - loop over collection targets
		for (TargetData* target = pane->collectionTargets(); target != NULL; target = target->next)
		{
			// If this is the collection to highlight, set color to transparent grey and disable material colouring....
			if (target->collection() == highlightCollection_)
			{
				glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
				glDisable(GL_COLOR_MATERIAL);
			}

			// Set shininess for collection
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, target->collection()->displaySurfaceShininess());

			// Loop over display primitives in this target...
			for (TargetPrimitive* primitive = target->displayPrimitives(); primitive != NULL; primitive = primitive->next)
			{
				// Make sure the primitive is up to date and send it to GL
				primitive->updateAndSendPrimitive(pane, renderingOffScreen_, renderingOffScreen_, context());
			}

			// Update query
			if (updateQueryDepth()) setQueryObject(UChromaViewer::CollectionObject, target->collection()->locator());

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
		glOrtho(0, pane->viewportMatrix()[2], 0, pane->viewportMatrix()[3], -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glDisable(GL_LIGHTING);

		const double overlaySpacing = 2.0;
		const double overlayTextSize = 12.0;
		const double legendLineLength = 10.0;

		/*
		 * Draw indicators in top-left corner
		 */

		CharString indicatorText;
		if (pane->autoFollowType() == ViewPane::AllAutoFollow) indicatorText += "|A| ";
		else if (pane->autoFollowType() == ViewPane::XFollow) indicatorText += "A\\sub{X} ";
		if (pane->collectionGroupManager().verticalShift() > 0) indicatorText.strcatf("S\\sub{%i}", pane->collectionGroupManager().verticalShift());
		TextPrimitive indicatorPrimitive;
		indicatorPrimitive.set(fontInstance_, indicatorText.get(), Vec3<double>(overlaySpacing, pane->viewportMatrix()[3] - overlaySpacing,0.0), TextPrimitive::TopLeftAnchor, Vec3<double>(), Matrix4(), overlayTextSize, false);
		glColor3d(0.0, 0.0, 0.0);
		Matrix4 identity;
		indicatorPrimitive.render(fontInstance_, identity, identity, 1.0);

		/*
		 * Draw legend in top-right corner
		 */

		// Create RefList of legend entries
		RefList<Collection,double> legendEntries;

		double maxTextWidth = -1.0;
		// Render pane data - loop over collection targets
		for (TargetData* target = pane->collectionTargets(); target != NULL; target = target->next)
		{
			if (!target->collection()->visible()) continue;
			double textWidth = fontInstance_.boundingBoxWidth(target->collection()->name()) * overlayTextSize;
			legendEntries.add(target->collection(), textWidth);
			if (textWidth > maxTextWidth) maxTextWidth = textWidth;
		}

		// Simple column layout - set the render position to be the left-hand edge of the longest text item
		glColor3d(0.0, 0.0, 0.0);
		glLoadIdentity();
		glTranslated(pane->viewportMatrix()[2] - maxTextWidth - overlaySpacing, pane->viewportMatrix()[3] - overlayTextSize - overlaySpacing, 0);

		// Loop over legend entries
		Vec4<float> colour;
		RefListIterator<Collection,double> legendEntryIterator(legendEntries);
		while (Collection* collection = legendEntryIterator.iterate())
		{
			// Grab copy of the relevant colour definition for this Collection
			ColourDefinition colourDefinition = pane->collectionGroupManager().colourDefinition(collection);

			// Draw line indicator
			glPushMatrix();
			glTranslated(-overlaySpacing, (overlayTextSize/2.0) - (collection->displayLineStyle().width()/2.0), 0.0);
			// -- What are we drawing for the line indicator?
			if (colourDefinition.colourSource() == ColourDefinition::SingleColourSource)
			{
				collection->displayLineStyle().apply();
				GLfloat lineWidth;
				glGetFloatv(GL_LINE_WIDTH, &lineWidth);
				glLineWidth(lineWidth*2.0);
				colourDefinition.colourScale().applyColour(0.0);
				glBegin(GL_LINES);
				glVertex2i(0.0, 0.0);
				glVertex2i(-legendLineLength, 0.0);
				glEnd();
				collection->displayLineStyle().revert();
			}
			glPopMatrix();

			// Draw text
			glPushMatrix();
			glColor3d(0.0, 0.0, 0.0);
			glScaled(overlayTextSize, overlayTextSize, overlayTextSize);
			fontInstance_.font()->Render(collection->name());
			glPopMatrix();

			// Shift to next position
			glTranslated(0.0, -(overlayTextSize + overlaySpacing), 0.0);
		}
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

	if (uChromaBase_->interactionMode() == UChromaBase::ViewInteraction)
	{
		// Draw selection box, if the interaction has started
		if (uChromaBase_->interactionStarted())
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

// Set whether we are currently rendering offscreen
void UChromaViewer::setRenderingOffScreen(bool b)
{
	renderingOffScreen_ = b;
}

// Set line width and text scaling to use
void UChromaViewer::setObjectScaling(double scaling)
{
	lineWidthScaling_ = scaling;

	// Pass this value on to those that depend on it
	LineStyle::setLineWidthScale(scaling);
	TextPrimitive::setTextSizeScale(scaling);
}

// Grab current contents of framebuffer
QPixmap UChromaViewer::frameBuffer()
{
	return QPixmap::fromImage(grabFramebuffer());
}

// Render or grab image
QPixmap UChromaViewer::generateImage(int imageWidth, int imageHeight)
{
	// Flag that we are rendering offscreen, and that we want high quality primitives
	renderingOffScreen_ = true;

	// Scale current line width and text scaling to reflect size of exported image
	setObjectScaling( double(imageHeight) / double(contextHeight()) );

	// Make the offscreen surface the current context
	offscreenContext_.makeCurrent(&offscreenSurface_);

	// Set tile size
	int tileWidth = 512;
	int tileHeight = 512;

	// Initialise framebuffer format and object
	QOpenGLFramebufferObjectFormat fboFormat;
	fboFormat.setMipmap(true);
	fboFormat.setSamples(4);
	fboFormat.setAttachment(QOpenGLFramebufferObject::Depth);
	QOpenGLFramebufferObject frameBufferObject(tileWidth, tileHeight, fboFormat);

	if (!frameBufferObject.bind())
	{
		Messenger::print("Failed to bind framebuffer object when generating image.");
		return QPixmap();
	}

	// Create a QPixmap of the desired full size and a QPainter for it
	QPixmap pixmap = QPixmap(imageWidth, imageHeight);
	QPainter painter(&pixmap);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawRect(0,0,imageWidth, imageHeight);

	// Calculate scale factors for ViewLayout, so that the context width/height is scaled to the desired image size
// 	double xScale = double(imageWidth) / double(tileWidth);
// 	double yScale = double(imageHeight) / double(tileHeight);
	int nX = imageWidth / tileWidth + ((imageWidth %tileWidth) ? 1 : 0);
	int nY = imageHeight / tileHeight + ((imageHeight %tileHeight) ? 1 : 0);

	// Recalculate view layout - no offset or scaling, just to the desired full image size
	uChromaBase_->viewLayout().setOffsetAndScale(0, 0, 1.0, 1.0);
	uChromaBase_->viewLayout().recalculate(imageWidth, imageHeight);

	// Loop over tiles in x and y
	QProgressDialog progress("Generating tiled image", "Cancel", 0, nX*nY, uChromaBase_->parent());
	progress.setWindowTitle("uChroma");
	for (int x=0; x<nX; ++x)
	{
		for (int y=0; y<nY; ++y)
		{
			// Set progress value and check for cancellation
			if (progress.wasCanceled()) break;
			progress.setValue(x*nY+y);

			// Generate this tile
			if (!frameBufferObject.bind()) printf("Failed to bind framebuffer object.\n");
			setupGL();
			renderFullScene(-x*tileWidth, -y*tileHeight);
			QImage fboImage(frameBufferObject.toImage());
			QImage tile(fboImage.constBits(), fboImage.width(), fboImage.height(), QImage::Format_ARGB32);

			// Paste this tile into the main image
			painter.drawImage(x*tileWidth, imageHeight-(y+1)*tileHeight, tile);
// 			tile.save(QString("tile-%1%2.png").arg(x).arg(y), "png");
		}
		if (progress.wasCanceled()) break;
	}

	// Finalise and save
	painter.end();

	// Reset line width and text size
	setObjectScaling(1.0);

	// Make sure the Viewer knows we no longer want offscreen rendering, and revert to normal quality primitives
	renderingOffScreen_ = false;

	// The sizes of panes may now be incorrect, so reset everything
	uChromaBase_->viewLayout().setOffsetAndScale(0, 0, 1.0, 1.0);
	uChromaBase_->viewLayout().recalculate(contextWidth_, contextHeight_);

	// Reset context back to main view
	makeCurrent();

	return pixmap;
}

// Set collection to highlight in this pass
void UChromaViewer::setHighlightCollection(Collection* collection)
{
	highlightCollection_ = collection;
}
