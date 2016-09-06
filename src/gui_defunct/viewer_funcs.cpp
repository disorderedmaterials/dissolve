/*
	*** Viewer Functions
	*** src/gui/viewer_funcs.cpp
	Copyright T. Youngs 2012-2013

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/viewer.uih"
#include "base/messenger.h"

// Static instance counter
int nViewerInstances = 0;

// Constructor
Viewer::Viewer(QWidget* parent) : QGLWidget(parent)
{
	// Character / Setup
	contextWidth_ = 0;
	contextHeight_ = 0;
	valid_ = false;
	drawing_ = false;
	renderingOffscreen_ = false;

	// Preferences (set static members only on first instance creation)
	setDefaultPreferences(nViewerInstances == 0);
	
	// Engine Setup
	linePrimitives_.setColourData(true);
	linePrimitives_.setType(GL_LINES);
	linePrimitives_.setNoInstances();
	pointPrimitives_.setColourData(true);
	pointPrimitives_.setType(GL_POINTS);
	pointPrimitives_.setNoInstances();
	triangleChopper_.initialise(0.0, 1000, 0.2);
	resetView();

	// Prevent QPainter from autofilling widget background
	setAutoFillBackground(false);

	++nViewerInstances;
}

// Destructor
Viewer::~Viewer()
{
}

/*
// Protected Qt Virtuals
*/

// Initialise context widget (when created by Qt)
void Viewer::initializeGL()
{
	// Initialize GL
	valid_ = true;
	
	// Create an instance for each defined user primitive - we do this in every call to initialiseGL so
	// that, when saving a bitmap using QGLWidget::renderPixmap(), we automatically create new display list
	// objects, rather than having to worry about context sharing etc. Slow, but safer and more compatible.
	msg.print("In Viewer::initializeGL, pushing instances for %i primitives...\n", primitiveList_.nItems());
	for (RefListItem<Primitive,int>* ri = primitiveList_.first(); ri != NULL; ri = ri->next) ri->item->pushInstance(context()); 
}

void Viewer::paintGL()
{
	if (renderingOffscreen_) paintEvent(NULL);
}

// General repaint callback
void Viewer::paintEvent(QPaintEvent* event)
{
	makeCurrent();
	
	QColor color;
	QRect currentBox;

	// Do nothing if the canvas is not valid, or we are still drawing from last time.
	if ((!valid_) || drawing_)
	{
		doneCurrent();
		return;
	}

	// Set the drawing flag so we don't have any rendering clashes
	drawing_ = true;

	// Setup basic GL stuff
	setupGL();

	// Note: An internet source suggests that the QPainter documentation is incomplete, and that
	// all OpenGL calls should be made after the QPainter is constructed, and before the QPainter
	// is destroyed. However, this results in mangled graphics on the Linux (and other?) versions,
	// so here it is done in the 'wrong' order.
	
	// Clear view
	msg.printVerbose(" --> Clearing context, background, and setting pen colour\n");
	glViewport(0,0,contextWidth_,contextHeight_);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Check to see if data has changed since the last time drawScene() was called
	// TODO
	if (1)
	{
		solidPrimitives_.clear();
		transparentPrimitives_.clear();
		linePrimitives_.clear();
		pointPrimitives_.clear();
		textPrimitives_.forgetAll();
		isotopeTextPrimitives_.forgetAll();

		// Set colour mode
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);

		// Prepare matrices
	// 	glViewport(vp[0], vp[1], vp[2], vp[3]);
		glMatrixMode(GL_PROJECTION);
		setProjectionMatrix();
		glLoadMatrixd(projectionMatrix_.matrix());
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
		// What to draw depends on the target* pointers...
		drawScene();
	}

	// Send primitives to the display
	sortAndSendGL();
	
	// Start of QPainter code
	static QFont font;
	font.setPointSize(Viewer::fontSize());
	QBrush nobrush(Qt::NoBrush);
	QPen pen;
	QPainter painter(this);
	
	// Render text elements for all models (with QPainter)
	painter.setFont(font);
	painter.setRenderHint(QPainter::Antialiasing);
	textPrimitives_.renderAll(painter, this);
	isotopeTextPrimitives_.renderAll(painter, this, true, false);
	font.setPointSize(Viewer::fontSize()*Viewer::superScriptFraction());
	painter.setFont(font);
	isotopeTextPrimitives_.renderAll(painter, this, true, true);

	// Render selection box (if any)
// 	if (selectionBox_.width() != 0) painter.drawRect(selectionBox_);

	// Done.
	painter.end();

	// Set the rendering flag to false
	drawing_ = false;

	// If we were rendering offscreen, we may delete the topmost primitive instance here
	if (renderingOffscreen_)
	{
		msg.print("In Viewer::PaintGL, popping instances for %i primitives...\n", primitiveList_.nItems());
		for (RefListItem<Primitive,int>* ri = primitiveList_.first(); ri != NULL; ri = ri->next) ri->item->popInstance(context());
	}

	doneCurrent();
}

// Resize function
void Viewer::resizeGL(int newwidth, int newheight)
{
	// Store the new width and height of the widget
	contextWidth_ = (GLsizei) newwidth;
	contextHeight_ = (GLsizei) newheight;
	viewportMatrix_[0] = 0;
	viewportMatrix_[1] = 0;
	viewportMatrix_[2] = newwidth;
	viewportMatrix_[3] = newheight;
	setProjectionMatrix();
}

/*
// Character / Setup
*/

// Return the current height of the drawing area
GLsizei Viewer::contextHeight() const
{
	return contextHeight_;
}

// Return the current width of the drawing area
GLsizei Viewer::contextWidth() const
{
	return contextWidth_;
}

// Probe features
void Viewer::probeFeatures()
{
	QGLFormat fmt = context()->format();
	// Probe this format!
	printf(" QGLFormat: Alpha buffer is %s.\n", fmt.alpha() ? "enabled" : "disabled");
	printf(" QGLFormat: Accumulation buffer is %s.\n", fmt.accum() ? "enabled" : "disabled");
	printf(" QGLFormat: Depth buffer is %s.\n", fmt.depth() ? "enabled" : "disabled");
	printf(" QGLFormat: Double-buffering is %s.\n", fmt.doubleBuffer() ? "enabled" : "disabled");
	printf(" QGLFormat: Direct rendering is %s.\n", fmt.directRendering() ? "enabled" : "disabled");
	printf(" QGLFormat: RGBA colour mode is %s.\n", fmt.rgba() ? "enabled" : "disabled");
	printf(" QGLFormat: Multisample buffer is %s.\n", fmt.sampleBuffers() ? "enabled" : "disabled");
	printf(" QGLFormat: Stencil buffer is %s.\n", fmt.stencil() ? "enabled" : "disabled");
	printf(" QGLWidget: Autoswap buffers is %s.\n", autoBufferSwap() ? "enabled" : "disabled");
}

// Check for GL error
void Viewer::checkGlError()
{
	// Do GL error check
	GLenum glerr = GL_NO_ERROR;
	do
	{
		switch (glGetError())
		{
			case (GL_INVALID_ENUM): msg.printVerbose("GLenum argument out of range\n"); break;
			case (GL_INVALID_VALUE): msg.printVerbose("N(umeric argument out of range\n"); break;
			case (GL_INVALID_OPERATION): msg.printVerbose("Operation illegal in current state\n"); break;
			case (GL_STACK_OVERFLOW): msg.printVerbose("Command would cause a stack overflow\n"); break;
			case (GL_STACK_UNDERFLOW): msg.printVerbose("Command would cause a stack underflow\n"); break;
			case (GL_OUT_OF_MEMORY): msg.printVerbose("Not enough memory left to execute command\n"); break;
			case (GL_NO_ERROR): msg.printVerbose("No GL error\n"); break;
			default:
				msg.printVerbose("Unknown GL error?\n");
				break;
		}
	} while (glerr != GL_NO_ERROR);
}

// Refresh widget / scene
void Viewer::postRedisplay()
{
	if ((!valid_) || drawing_) return;
	update();
}

// Setup projection matrix
void Viewer::setProjectionMatrix(double perspectiveFov)
{
	// Create projection matrix for model
	GLdouble top, bottom, right, left, aspect = (GLdouble) contextWidth_ / (GLdouble) contextHeight_;
	GLdouble nearClip = 0.5, farClip = 2000.0;
	if (hasPerspective_)
	{
		// Use reversed top and bottom values so we get y-axis (0,1,0) pointing up
		top = tan(perspectiveFov / DEGRAD) * 0.5;
		bottom = -top;
		left = -aspect*top;
		right = aspect*top;

		projectionMatrix_.setColumn(0, (nearClip*2.0) / (right-left), 0.0, 0.0, 0.0);
		projectionMatrix_.setColumn(1, 0.0, (nearClip*2.0) / (top-bottom), 0.0, 0.0);
		projectionMatrix_.setColumn(2, (right+left)/(right-left), (top+bottom)/(top-bottom), -(farClip+nearClip)/(farClip-nearClip), -1.0);
		projectionMatrix_.setColumn(3, 0.0, 0.0, -(2.0*nearClip*farClip) / (farClip-nearClip), 0.0);
		// Equivalent to the following code:
		// glMatrixMode(GL_PROJECTION);
		// glLoadIdentity();
		// top = tan(prefs.perspectiveFov() / DEGRAD) * prefs.clipNear();
		// bottom = -top;
		// glFrustum(aspect*bottom, aspect*top, bottom, top, prefs.clipNear(), prefs.clipFar());
		// glGetDoublev(GL_PROJECTION_MATRIX, modelProjectionMatrix_.matrix());
	}
	else
	{
		top = -tan(perspectiveFov / DEGRAD) * viewMatrix_[14];
		bottom = -top;
		left = -aspect*top;
		right = aspect*top;

		projectionMatrix_.setColumn(0, 2.0 / (right-left), 0.0, 0.0, (right+left)/(right-left));
		projectionMatrix_.setColumn(1, 0.0, 2.0 / (top-bottom), 0.0, (top+bottom)/(top-bottom));
		projectionMatrix_.setColumn(2, 0.0, 0.0, -1.0/farClip, 0.0);
		projectionMatrix_.setColumn(3, 0.0, 0.0, 0.0, 1.0);
		// Equivalent to the following code:
		// glMatrixMode(GL_PROJECTION);
		// glLoadIdentity();
		// top = tan(prefs.perspectiveFov() / DEGRAD) * prefs.clipNear();
		// bottom = -top;
		// glOrtho(aspect*top, aspect*bottom, top, bottom, -prefs.clipFar(), prefs.clipFar());
		// glGetDoublev(GL_PROJECTION_MATRIX, modelProjectionMatrix_.matrix());
	}
}
                
// Update transformation (view) matrix
void Viewer::setViewMatrix(Matrix4& mat)
{
	viewMatrix_ = mat;
}

// Render or grab image
QPixmap Viewer::generateImage(int w, int h)
{
	renderingOffscreen_ = true;
	if (useFrameBuffer_ == false)
	{

		// Generate offscreen bitmap (a temporary context will be created)
		QPixmap pixmap = renderPixmap(w, h, false);
		
		// Ensure correct widget context size is stored
		contextWidth_ = (GLsizei) width();
		contextHeight_ = (GLsizei) height();

		renderingOffscreen_ = false;
		return pixmap;
	}
	else
	{
		postRedisplay();
		QImage image = grabFrameBuffer();

		renderingOffscreen_ = false;
		return QPixmap::fromImage(image);
	}
}

// Reset current view
void Viewer::resetView()
{
	// Setup right-handed transformation matrix
	viewMatrix_.setColumn(0, 1.0, 0.0, 0.0, 0.0);
	viewMatrix_.setColumn(1, 0.0, 1.0, 0.0, 0.0);
	viewMatrix_.setColumn(2, 0.0, 0.0, -1.0, 0.0);
	viewMatrix_[14] = -5.0;
}

// Project given model coordinates into world coordinates
Vec3<double> Viewer::modelToWorld(const Vec3<double> &modelr)
{
	Vec3<double> worldr;
	Matrix4 vmat;
	Vec4<double> pos, temp;
	// Projection formula is : worldr = P x M x modelr
	pos.set(modelr, 1.0);
	// Get the world coordinates of the atom - Multiply by modelview matrix 'view'
	vmat = viewMatrix_;
//	vmat.applyTranslation(-cell_.centre().x, -cell_.centre().y, -cell_.centre().z);
	temp = vmat * pos;
	worldr.set(temp.x, temp.y, temp.z);
	return worldr;
}

// Project given model coordinates into screen coordinates
Vec4<double> Viewer::modelToScreen(const Vec3<double> &modelr, double screenradius)
{
	Vec4<double> screenr, tempscreen;
	Vec4<double> worldr;
	Matrix4 vmat;
	Vec4<double> pos;
	// Projection formula is : worldr = P x M x modelr
	pos.set(modelr, 1.0);
	// Get the world coordinates of the atom - Multiply by modelview matrix 'view'
	vmat = viewMatrix_;
//	vmat.applyTranslation(-cell_.centre().x, -cell_.centre().y, -cell_.centre().z);
	worldr = vmat * pos;

	screenr = projectionMatrix_ * worldr;
	screenr.x /= screenr.w;
	screenr.y /= screenr.w;
	screenr.x = viewportMatrix_[0] + viewportMatrix_[2]*(screenr.x+1)*0.5;
	screenr.y = viewportMatrix_[1] + viewportMatrix_[3]*(screenr.y+1)*0.5;
	screenr.z = screenr.z / screenr.w;
	// Calculate 2D 'radius' around the point - Multiply world[x+delta] coordinates by P
	if (screenradius > 0.0)
	{
		worldr.x += screenradius;
		tempscreen = projectionMatrix_ * worldr;
		tempscreen.x /= tempscreen.w;
		screenr.w = fabs( (viewportMatrix_[0] + viewportMatrix_[2]*(tempscreen.x+1)*0.5) - screenr.x);
	}
	return screenr;
}

// Convert screen coordinates into model space coordinates
Vec3<double> Viewer::screenToModel(int x, int y, double z)
{
	static Vec3<double> modelr;
	Vec4<double> temp, worldr;
	int newx, newy;
	double dx, dy;

	// Grab transformation matrix, apply translation correction, and invert
	Matrix4 itransform = viewMatrix_;
//	itransform.applyTranslation(-cell_.centre().x, -cell_.centre().y, -cell_.centre().z);
	itransform.invert();

	// Mirror y-coordinate
	y = viewportMatrix_[3] - y;

	// Project points at guide z-position and two other points along literal x and y to get scaling factors for screen coordinates
	worldr.set(0.0,0.0,z, 1.0);
	temp = projectionMatrix_ * worldr;
	newx = viewportMatrix_[0] + viewportMatrix_[2]*(temp.x / temp.w + 1.0)*0.5;
	newy = viewportMatrix_[1] + viewportMatrix_[3]*(temp.y / temp.w + 1.0)*0.5;

	for (int n=0; n<10; ++n)
	{
		// Determine new (better) coordinate from a yardstick centred at current world coordinates
		temp = projectionMatrix_ * Vec4<double>(worldr.x+1.0, worldr.y+1.0, worldr.z, worldr.w);
		dx = viewportMatrix_[0] + viewportMatrix_[2]*(temp.x / temp.w + 1.0)*0.5 - newx;
		dy = viewportMatrix_[1] + viewportMatrix_[3]*(temp.y / temp.w + 1.0)*0.5 - newy;

		worldr.add((x-newx)/dx, (y-newy)/dy, 0.0, 0.0);
// 		printf ("N=%i", n); worldr.print();
		temp = projectionMatrix_ * worldr;
		newx = viewportMatrix_[0] + viewportMatrix_[2]*(temp.x / temp.w + 1.0)*0.5;
		newy = viewportMatrix_[1] + viewportMatrix_[3]*(temp.y / temp.w + 1.0)*0.5;
// 		printf("NEW dx = %f, dy = %f, wantedxy = %f, %f\n", newx, newy, x, y);
		if ((x == newx) && (y == newy)) break;
	}

	// Finally, invert to model coordinates
	modelr = itransform * Vec3<double>(worldr.x, worldr.y, worldr.z);

	return modelr;
}
