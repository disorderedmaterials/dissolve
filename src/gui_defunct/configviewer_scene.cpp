/*
	*** Config Viewer Rendering
	*** src/gui/configviewer_scene.cpp
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

#include "gui/configviewer.uih"
#include "classes/configuration.h"
#include "classes/atom.h"
#include "classes/isotopologue.h"
#include "classes/box.h"
#include "base/sysfunc.h"

// Create necessary primitives (must be called before any rendering is done)
void ConfigViewer::createPrimitives()
{
	// Create basic primitives
	spherePrimitive03_.plotSphere(0.3, 8, 10);
	spherePrimitive04_.plotSphere(0.4, 8, 10);
	cubePrimitive_.plotCube(1.0, 4, 0, 0, 0);	
	cylinderPrimitive_.plotCylinder(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.15, 0.15, 8, 8);
	wireCubePrimitive_.createWireCube(1.0);

	// Every created primitive must be added to the primitiveList_
	primitiveList_.add(&spherePrimitive03_);
	primitiveList_.add(&spherePrimitive04_);
	primitiveList_.add(&cubePrimitive_);
	primitiveList_.add(&cylinderPrimitive_);
	primitiveList_.add(&wireCubePrimitive_);
}

// Setup basic GL properties (called each time before renderScene())
void ConfigViewer::setupGL()
{
	// Define colours etc.
	GLfloat backgroundColour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat spotlightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat spotlightDiffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat spotlightSpecular[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat spotlightPosition[4] = { 100.0f, 100.0f, 100.0f, 0.0f };
	GLfloat specularColour[4] = { 0.9f, 0.9f, 0.9f, 1.0f };

	// Clear (background) colour
	glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]);
	//glClearDepth(1.0);

	// Perspective hint
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_FASTEST);

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// Smooth shading
	glShadeModel(GL_SMOOTH);

	// Auto-calculate surface normals
	glEnable(GL_NORMALIZE);

	// Set alpha-blending function
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);

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
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 127);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_MULTISAMPLE);

	// Configure antialiasing
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glEnable(GL_LINE_SMOOTH);

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

/*
 * \brief Draw scene
 */
void ConfigViewer::drawScene()
{
	Matrix4 A;
	Vec3<double> v;
	GLfloat colourBlack[4] = { 0.0, 0.0, 0.0, 1.0 };
	const float *col;

	// Check pointer is valid
	if (sourceConfiguration_ == NULL)
	{
		msg.error("Called ConfigViewer::drawConfiguration() with a NULL Configuration pointer.\n");
		return;
	}

	if (sourceConfiguration_->box() == NULL) return;

	// Draw Box
	Vec3<double> boxCentre;
	if (sourceConfiguration_->box()->type() != Box::NonPeriodicBox)
	{
		boxCentre = sourceConfiguration_->box()->axes() * Vec3<double>(-0.5,-0.5,-0.5);
		A.setColumn(0, sourceConfiguration_->box()->axes().columnAsVec3(0), 0.0);
		A.setColumn(1, sourceConfiguration_->box()->axes().columnAsVec3(1), 0.0);
		A.setColumn(2, sourceConfiguration_->box()->axes().columnAsVec3(2), 0.0);
		A.setColumn(3, 0.0, 0.0, 0.0, 1.0);
		renderPrimitive(&wireCubePrimitive_, colourBlack, A, GL_LINE);
		A.setIdentity();
	}

	if ((drawStyle_ == ConfigViewer::LineStyle) || (drawStyle_ == ConfigViewer::PointStyle))
	{
		Vec3<double> ri, halfvij;
		const float* colour;

		// Draw Atoms
		Atom** atoms = sourceConfiguration_->atomReferences(), *i;
		for (int n=0; n<sourceConfiguration_->nAtoms(); ++n)
		{
			i = atoms[n];
			ri = i->r() + boxCentre;
			colour = PeriodicTable::element(i->element()).colour();
			pointPrimitives_.defineVertex(ri.x, ri.y, ri.z, 0.0, 0.0, 1.0, colour[0], colour[1], colour[2], colour[3], false);
		}

		// Draw Bonds  TODO
// 		if (drawStyle_ == ConfigViewer::LineStyle) for (Bond* b = sourceConfiguration_->bonds(); b != NULL; b = b->next)
// 		{
// 			// Get vector between Atoms i->j and move to Bond centre
// 			ri = b->i()->r();
// 			halfvij = b->j()->r() - ri;
// 			halfvij *= 0.5;
// 			ri += boxCentre;
// 
// 			// Render half of Bond in colour of Atom i
// 			colour = PeriodicTable::element(b->i()->element()).colour();
// 			linePrimitives_.defineVertex(ri.x, ri.y, ri.z, 0.0, 0.0, 1.0, colour[0], colour[1], colour[2], colour[3], false);
// 			ri += halfvij;
// 			linePrimitives_.defineVertex(ri.x, ri.y, ri.z, 0.0, 0.0, 1.0, colour[0], colour[1], colour[2], colour[3], false);
// 			
// 			// Render half of Bond in colour of Atom j
// 			colour = PeriodicTable::element(b->j()->element()).colour();
// 			linePrimitives_.defineVertex(ri.x, ri.y, ri.z, 0.0, 0.0, 1.0, colour[0], colour[1], colour[2], colour[3], false);
// 			ri += halfvij;
// 			linePrimitives_.defineVertex(ri.x, ri.y, ri.z, 0.0, 0.0, 1.0, colour[0], colour[1], colour[2], colour[3], false);
// 		}
	}
	else if (drawStyle_ == ConfigViewer::BallAndStickStyle)
	{
		// Draw Atoms
		Isotope* iso;
		Atom** atoms = sourceConfiguration_->atomReferences(), *i;
		for (int n=0; n<sourceConfiguration_->nAtoms(); ++n)
		{
			i = atoms[n];
			A.setTranslation(i->r() + boxCentre);
			renderPrimitive(&spherePrimitive03_, PeriodicTable::element(i->element()).colour(), A);
		}

		// Draw Bonds  TODO
		Vec3<double> vij;
		double mag;
// 		for (Bond* b = sourceConfiguration_->bonds(); b != NULL; b = b->next)
// 		{
// 			// Get vector between Atoms i->j and move to Bond centre
// 			vij = b->j()->r() - b->i()->r();
// 			A.setTranslation(b->i()->r()+vij*0.5 + boxCentre);
// 			mag = vij.magAndNormalise();
// 			
// 			// Create rotation matrix for Bond
// 			A.setColumn(2, vij.x, vij.y, vij.z, 0.0);
// 			A.setColumn(0, vij.orthogonal(), 0.0);
// 			A.setColumn(1, vij * A.columnAsVec3(0), 0.0);
// 			A.columnMultiply(2, 0.5*mag);
// 
// 			// Render half of Bond in colour of Atom j
// 			renderPrimitive(&cylinderPrimitive_, PeriodicTable::element(b->j()->element()).colour(), A);
// 			
// 			// Render half of Bond in colour of Atom i
// 			A.columnMultiply(2,-1.0);
// 			renderPrimitive(&cylinderPrimitive_, PeriodicTable::element(b->i()->element()).colour(), A);
// 		}
	}
}

/*
 * \brief Set source (Configuration)
 */
void ConfigViewer::setSource(Configuration* cfg)
{
	sourceConfiguration_ = cfg;
}

/*
 * \brief Set draw style
 */
void ConfigViewer::setDrawStyle(ConfigViewer::DrawStyle style)
{
	drawStyle_ = style;
	update();
}

/*
 * \brief Change draw style
 */
void ConfigViewer::changeDrawStyle(int style)
{
	drawStyle_ = (ConfigViewer::DrawStyle) style;
	update();
}
