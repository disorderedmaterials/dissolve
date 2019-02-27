/*
	*** Renderable - Data1D
	*** src/gui/viewer/render/renderabledata1d.h
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

#ifndef DISSOLVE_RENDERABLEDATA1D_H
#define DISSOLVE_RENDERABLEDATA1D_H

#include "gui/viewer/render/renderable.h"
#include "math/data1d.h"

// Forward Declarations
class Axes;

// Renderable for Data1D
class RenderableData1D : public Renderable
{
	public:
	// Constructor / Destructor
	RenderableData1D(const Data1D* source, const char* objectTag);
	~RenderableData1D();


	/*
	 * Data
	 */
	private:
	// Source data
	const Data1D* source_;

	private:
	// Return whether a valid data source is available (attempting to set it if not)
	bool validateDataSource();
	// Return version of data
	int version() const;


	/*
	 * Transform / Limits
	 */
	private:
	// Transformed data
	Data1D transformedData_;

	protected:
	// Transform data according to current settings
	void transformData();
	// Return reference to transformed data
	const Data1D& transformedData();

	public:
	// Calculate min/max y value over specified x range (if possible in the underlying data)
	bool yRangeOverX(double xMin, double xMax, double& yMin, double& yMax);


	/*
	 * Rendering Primitives
	 */
	private:
	// Create line strip primitive
	void constructLineXY(const Array<double>& displayAbscissa, const Array<double>& displayValues, Primitive* primitive, const Axes& axes, const ColourDefinition& colourDefinition, double zCoordinate = 0.0);

	public:
	// Update primitives and send for display
	void updateAndSendPrimitives(View& view, RenderableGroupManager& groupManager, bool forceUpdate, bool pushAndPop, const QOpenGLContext* context);
};

#endif
