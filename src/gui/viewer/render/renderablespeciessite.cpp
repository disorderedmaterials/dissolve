/*
	*** Renderable - SpeciesSite
	*** src/gui/viewer/render/renderablespeciessite.cpp
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

#include "gui/viewer/render/renderablespeciessite.h"
#include "gui/viewer/render/renderablegroupmanager.h"
#include "gui/viewer/render/view.h"
#include "classes/atom.h"
#include "classes/molecule.h"
#include "classes/site.h"
#include "data/elementcolours.h"

// Constructor
RenderableSpeciesSite::RenderableSpeciesSite(const Species* sp, const SpeciesSite* site) : Renderable(Renderable::SpeciesSiteRenderable, "SiteRenderable")
{
	// Set defaults
	displayStyle_ = LinesStyle;

	// Set sources
	speciesSource_ = sp;
	siteSource_ = site;

	// Create basic primitives
	originPrimitive_ = createPrimitive(GL_LINES, false);
	originPrimitive_->wireOrthorhomboid(0.7, 0.7, 0.7);
	axesPrimitive_ = createPrimitive(GL_LINES, false);
	axesPrimitive_->axes(1.2);
}

// Destructor
RenderableSpeciesSite::~RenderableSpeciesSite()
{
}

/*
 * Data
 */

// Return whether a valid data source is available (attempting to set it if not)
bool RenderableSpeciesSite::validateDataSource()
{
	return ((speciesSource_ != NULL) && (siteSource_ != NULL));
}

// Return version of data
int RenderableSpeciesSite::dataVersion() const
{
	return (siteSource_ ? siteSource_->version() : -99);
}

/*
 * Transform / Limits
 */

// Transform data according to current settings
void RenderableSpeciesSite::transformData()
{
	return;
}

// Calculate min/max y value over specified x range (if possible in the underlying data)
bool RenderableSpeciesSite::yRangeOverX(double xMin, double xMax, double& yMin, double& yMax)
{
	yMin = 0.0;
	yMax = 1.0;

	return true;
}

/*
 * Rendering Primitives
 */

// Recreate necessary primitives / primitive assemblies for the data
void RenderableSpeciesSite::recreatePrimitives(const View& view, const ColourDefinition& colourDefinition)
{
	Matrix4 A;
	const GLfloat* colour;
	const GLfloat colourBlack[4] = { 0.0, 0.0, 0.0, 1.0 };

	// Clear existing data
	siteAssembly_.clear();

	// Check source pointers
	if (!validateDataSource()) return;

	// Generate a temporary Site from the parent Species
	Site* site = siteSource_->createFromParent();
	if (!site) return;

	// Render according to the current displayStyle
	if (displayStyle_ == LinesStyle)
	{
		// Plot origin
		A.setTranslation(site->origin());
// 		Matrix4 B;
// 		B.setRotation(site->axes());
// 		A *= B;
		if (site->hasAxes()) A.applyRotation(site->axes());
		siteAssembly_.add(originPrimitive_, A, 0.0, 0.0, 0.0, 1.0);

		// Plot axes?
		if (site->hasAxes())
		{
			A.setRotation(site->axes());
			siteAssembly_.add(axesPrimitive_, A);
		}
	}
	else if (displayStyle_ == SolidStyle)
	{
		// Set basic styling for assemblies
		siteAssembly_.add(true, GL_FILL);

		// Plot origin
		A.setTranslation(site->origin());
		siteAssembly_.add(originPrimitive_, A, 0.0, 0.0, 0.0, 0.5);
	}
}

// Send primitives for rendering
const void RenderableSpeciesSite::sendToGL(const double pixelScaling)
{
	// Set appropriate lighting for the speciessite and interaction assemblies
	if (displayStyle_ == LinesStyle) glDisable(GL_LIGHTING);
	else glEnable(GL_LIGHTING);
	siteAssembly_.sendToGL(pixelScaling);
}

/*
 * Style
 */

// Display Style Keywords
const char* SpeciesSiteDisplayStyleKeywords[] = { "Lines", "Solid" };

// Convert display style index to text string
const char* RenderableSpeciesSite::displayStyle(int id)
{
	if ((id < 0) || (id >= RenderableSpeciesSite::nDisplayStyles)) return "INVALID_STYLE";

	return SpeciesSiteDisplayStyleKeywords[id];
}

// Convert text string to display style index
int RenderableSpeciesSite::displayStyle(const char* s)
{
	for (int n=0; n<nDisplayStyles; ++n) if (DissolveSys::sameString(s, SpeciesSiteDisplayStyleKeywords[n])) return (RenderableSpeciesSite::DisplayStyle) n;

	return -1;
}
