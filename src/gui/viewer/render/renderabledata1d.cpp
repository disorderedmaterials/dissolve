/*
	*** Renderable - Data1D
	*** src/gui/viewer/render/renderabledata1d.cpp
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

#include "gui/viewer/render/renderabledata1d.h"
#include "gui/viewer/render/renderablegroupmanager.h"
#include "gui/viewer/render/view.h"

// Constructor
RenderableData1D::RenderableData1D(const Data1D* source, const char* objectTag) : Renderable(Renderable::Data1DRenderable, objectTag), source_(source)
{
	dataPrimitive_ = createPrimitive();
}

// Destructor
RenderableData1D::~RenderableData1D()
{
}

/*
 * Data
 */

// Return whether a valid data source is available (attempting to set it if not)
bool RenderableData1D::validateDataSource()
{
	// If there is no valid source set, attempt to set it now...
	if (!source_) source_ = Data1D::findObject(objectTag_);

	return source_;
}

// Return version of data
int RenderableData1D::dataVersion() const
{
	return (source_ ? source_->version() : -99);
}

/*
 * Transform / Limits
 */

// Transform data according to current settings
void RenderableData1D::transformData()
{
	// If the transformed data are already up-to-date, no need to do anything
	if (transformDataVersion_ == dataVersion()) return;

	// Copy original data and transform now. We do this even if the transformers are disabled, since they may have previously been active
	if (!validateDataSource()) transformedData_.clear();
	else transformedData_ = *source_;
	Transformer::transform(transformedData_, transforms_[0], transforms_[1]);

	transformMin_ = 0.0;
	transformMax_ = 0.0;
	transformMinPositive_ = 0.1;
	transformMaxPositive_ = -1.0;
	
	transformMin_.set(transformedData_.constXAxis().firstValue(), transformedData_.minValue(), 0.0);
	transformMax_.set(transformedData_.constXAxis().lastValue(), transformedData_.maxValue(), 0.0);

	// Now determine minimum positive limits - loop over points in data, searching for first positive, non-zero value
	for (int n=0; n<transformedData_.nValues(); ++n)
	{
		// X
		if (transformedData_.constXAxis(n) > 0.0)
		{
			if (transformedData_.constXAxis(n) < transformMinPositive_.x) transformMinPositive_.x = transformedData_.constXAxis(n);
			if (transformedData_.constXAxis(n) > transformMaxPositive_.x) transformMaxPositive_.x = transformedData_.constXAxis(n);
		}
		// Y
		if (transformedData_.constValue(n) > 0.0)
		{
			if (transformedData_.constValue(n) < transformMinPositive_.y) transformMinPositive_.y = transformedData_.constValue(n);
			if (transformedData_.constValue(n) > transformMaxPositive_.y) transformMaxPositive_.y = transformedData_.constValue(n);
		}
	}
	
	transformMinPositive_.z = 1.0;
	transformMaxPositive_.z = 1.0;

	// Check maximum positive values (since all datapoints might have been negative
	if (transformMaxPositive_.x < 0.0) transformMaxPositive_.x = 1.0;
	if (transformMaxPositive_.y < 0.0) transformMaxPositive_.y = 1.0;
	if (transformMaxPositive_.z < 0.0) transformMaxPositive_.z = 1.0;

	// Update the transformed data 'version'
	transformDataVersion_ = dataVersion();
}

// Return reference to transformed data
const Data1D& RenderableData1D::transformedData()
{
	// Check that we have a valid source
	if (!validateDataSource()) return transformedData_;

	// If no transforms are enabled, just return the original data
	if ((!transforms_[0].enabled()) && (!transforms_[1].enabled())) return *source_;

	// Make sure the transformed data is up-to-date
	transformData();

	return transformedData_;
}

// Calculate min/max y value over specified x range (if possible in the underlying data)
bool RenderableData1D::yRangeOverX(double xMin, double xMax, double& yMin, double& yMax)
{
	// Ensure transformed data is up-to-date
	transformData();

	// Grab reference to transformed data
	const Data1D& data = transformedData();

	for (int n=0; n<data.nValues(); ++n)
	{
		if (data.constXAxis(n) < xMin) continue;
		else if (data.constXAxis(n) > xMax) break;

		if (n == 0)
		{
			yMin = data.constValue(n);
			yMax = yMin;
		}
		else
		{
			if (data.constValue(n) < yMin) yMin = data.constValue(n);
			else if (data.constValue(n) > yMax) yMax = data.constValue(n);
		}
	}

	return true;
}

/*
 * Rendering Primitives
 */

// Recreate necessary primitives / primitive assemblies for the data
void RenderableData1D::recreatePrimitives(const View& view, const ColourDefinition& colourDefinition)
{
	dataPrimitive_->initialise(GL_LINE_STRIP, true);
	constructLineXY(transformedData().constXAxis(), transformedData().constValues(), dataPrimitive_, view.constAxes(), colourDefinition);
}

// Create line strip primitive
void RenderableData1D::constructLineXY(const Array<double>& displayAbscissa, const Array<double>& displayValues, Primitive* primitive, const Axes& axes, const ColourDefinition& colourDefinition, double zCoordinate)
{
	// Copy and transform abscissa values (still in data space) into axes coordinates
	Array<double> x = displayAbscissa;
	axes.transformX(x);
	int nX = x.nItems();
	if (nX < 2) return;

	// Get some values from axes so we can calculate colours properly
	bool yLogarithmic = axes.logarithmic(1);
	double yStretch = axes.stretch(1);

	// Temporary variables
	Vec4<GLfloat> colour(0,0,0,1);
	Vec3<double> nrm(0.0, 1.0, 0.0);

	// Create lines for slices
	int vertexA, vertexB;
	// Grab y and z values
	Array<double> y = displayValues;
	axes.transformY(y);
	double z = axes.transformZ(zCoordinate);

	// Set vertexA to -1 so we don't draw a line at n=0
	vertexA = -1;

	// Check for a single colour style in the colourDefinition - use optimised case in that eventuality
	if (colourDefinition.style() == ColourDefinition::SingleColourStyle)
	{
		// Get the single colour
		colourDefinition.colour(0.0, colour);

		// Loop over x values
		for (int n=0; n<nX; ++n)
		{
			vertexB = primitive->defineVertex(x.constAt(n), y.constAt(n), z, nrm, colour);

			// If both vertices are valid, plot a line
			if (vertexA != -1) primitive->defineIndices(vertexA, vertexB);

			vertexA = vertexB;
		}
	}
	else
	{
		// Loop over x values
		for (int n=0; n<nX; ++n)
		{
			colourDefinition.colour(yLogarithmic ? pow(10.0, y.constAt(n) / yStretch) : y.constAt(n) / yStretch, colour);
			vertexB = primitive->defineVertex(x.constAt(n), y.constAt(n), z, nrm, colour);

			// If both vertices are valid, plot a line
			if (vertexA != -1) primitive->defineIndices(vertexA, vertexB);

			vertexA = vertexB;
		}
	}
}
