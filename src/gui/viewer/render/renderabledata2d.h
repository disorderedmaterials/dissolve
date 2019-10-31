/*
	*** Renderable - Data2D
	*** src/gui/viewer/render/renderabledata2d.h
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

#ifndef DISSOLVE_RENDERABLEDATA2D_H
#define DISSOLVE_RENDERABLEDATA2D_H

#include "gui/viewer/render/renderable.h"
#include "math/data2d.h"

// Forward Declarations
class Axes;

// Renderable for Data2D
class RenderableData2D : public Renderable
{
	public:
	// Constructor / Destructor
	RenderableData2D(const Data2D* source, const char* objectTag);
	~RenderableData2D();


	/*
	 * Data
	 */
	private:
	// Source data
	const Data2D* source_;

	private:
	// Return whether a valid data source is available (attempting to set it if not)
	bool validateDataSource();
	// Return version of data
	int dataVersion() const;
	Vec3<double> min(Array<double> A);
	Vec3<double> max(Array<double> A);
	Vec3<double> minValue();
	Vec3<double> maxValue();
	
	
	/*
	 * Transform / Limits
	 */
	private:
	// Transformed data
	Data2D transformedData_;

	protected:
	// Transform data according to current settings
	void transformData();
	// Return reference to transformed data
	const Data2D& transformedData();

	public:
	// Calculate min/max y value over specified x range (if possible in the underlying data)
	bool yRangeOverX(double xMin, double xMax, double& yMin, double& yMax);


	/*
	 * Rendering Primitives
	 */
	private:
	// Primitive to represent data
	PrimitiveList* dataPrimitive_;
	
	private:
	// Create line strip primitive
	void constructLineXY(const Array<double>& displayAbscissa, const Array<double>& displayValues, PrimitiveList* primitive, const Axes& axes, const ColourDefinition& colourDefinition, double zCoordinate = 0.0);

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
	enum DisplayStyle { LinesStyle, nDisplayStyles };
	// Return keyword for display style index
	const char* displayStyle(int id);
	// Return display style index from string
	int displayStyle(const char* s);

};

#endif
