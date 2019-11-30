/*
	*** Renderable - SpeciesSite
	*** src/gui/viewer/render/renderablespeciessite.h
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

#ifndef DISSOLVE_RENDER_RENDERABLESPECIESSITE_H
#define DISSOLVE_RENDER_RENDERABLESPECIESSITE_H

#include "gui/viewer/render/renderable.h"
#include "classes/species.h"
#include "classes/speciessite.h"

// Forward Declarations
class Axes;

// Renderable for SpeciesSite
class RenderableSpeciesSite : public Renderable
{
	public:
	// Constructor / Destructor
	RenderableSpeciesSite(const Species* sp, const SpeciesSite* site);
	~RenderableSpeciesSite();


	/*
	 * Data
	 */
	private:
	// Source Species
	const Species* speciesSource_;
	// Source site in Species
	const SpeciesSite* siteSource_;

	private:
	// Return whether a valid data source is available (attempting to set it if not)
	bool validateDataSource();
	// Return version of data
	int dataVersion() const;


	/*
	 * Transform / Limits
	 */
	protected:
	// Transform data according to current settings
	void transformData();

	public:
	// Calculate min/max y value over specified x range (if possible in the underlying data)
	bool yRangeOverX(double xMin, double xMax, double& yMin, double& yMax);


	/*
	 * Rendering Primitives
	 */
	private:
	// Basic primitives
	Primitive* originPrimitive_, *axesPrimitive_;
	// Main assemblies
	PrimitiveAssembly siteAssembly_;

	protected:
	// Recreate necessary primitives / primitive assemblies for the data
	void recreatePrimitives(const View& view, const ColourDefinition& colourDefinition);
	// Send primitives for rendering
	const void sendToGL(const double pixelScaling);


	/*
	 * Style
	 */
	public:
	// Display Styles enum
	enum DisplayStyle { LinesStyle, SolidStyle, nDisplayStyles };

	public:
	// Return keyword for display style index
	const char* displayStyle(int id);
	// Return display style index from string
	int displayStyle(const char* s);
};

#endif
