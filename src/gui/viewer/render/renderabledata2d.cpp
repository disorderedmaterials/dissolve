/*
	*** Renderable - Data2D
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

#include "gui/viewer/render/renderabledata2d.h"
#include "gui/viewer/render/renderablegroupmanager.h"
#include "gui/viewer/render/view.h"
#include "math/data2d.h"
#include "templates/array2d.h"

// Constructor
RenderableData2D::RenderableData2D(const Data2D* source, const char* objectTag) : Renderable(Renderable::Data2DRenderable, objectTag), source_(source)
{
	// Set defaults
	displayStyle_ = LinesStyle;
}

// Destructor
RenderableData2D::~RenderableData2D()
{
}

/*
 * Data
 */

// Return whether a valid data source is available (attempting to set it if not)
bool RenderableData2D::validateDataSource()
{
	// If there is no valid source set, attempt to set it now...
	if (!source_) source_ = Data2D::findObject(objectTag_);

	return source_;
}

// Return version of data
int RenderableData2D::dataVersion() const
{
	return (source_ ? source_->version() : -99);
}



/*
 * Transform / Limits
 */

// Transform data according to current settings
void RenderableData2D::transformData()
{
	// If the transformed data are already up-to-date, no need to do anything
	if (transformDataVersion_ == dataVersion()) return;

	// Copy original data and transform now. We do this even if the transformers are disabled, since they may have previously been active
	if (!validateDataSource()) transformedData_.clear();
	else transformedData_ = *source_;
	Transformer::transform2D(transformedData_, transforms_[0], transforms_[1], transforms_[2] );

	transformMin_ = transformedData_.min(transformedData_.constXAxis());
	transformMax_ = transformedData_.max(transformedData_.constXAxis());
	transformMinPositive_ = 0.1;
	transformMaxPositive_ = 0.0;
	
	// Set initial limits if we can
	if (transformedData_.nValues() > 0)
	{
		
		transformMin_.set(transformedData_.constXAxis().firstValue(), transformedData_.constYAxis().firstValue(), transformedData_.minValue());
		transformMax_.set(transformedData_.constXAxis().lastValue(), transformedData_.constXAxis().lastValue(), transformedData_.maxValue());
		
	}

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
		
		if (transformedData_.constYAxis(n) > 0.0)
		{
			if (transformedData_.constYAxis(n) < transformMinPositive_.y) transformMinPositive_.y = transformedData_.constYAxis(n);
			if (transformedData_.constYAxis(n) > transformMaxPositive_.y) transformMaxPositive_.y = transformedData_.constYAxis(n);
		}
		
		if (transformedData_.constValue(n) > 0.0)
		{
			if (transformedData_.constValue(n) < transformMinPositive_.z) transformMinPositive_.z = transformedData_.constValue(n);
			if (transformedData_.constValue(n) > transformMaxPositive_.z) transformMaxPositive_.z = transformedData_.constValue(n);
		}
	}
	
	// Update the transformed data 'version'
	transformDataVersion_ = dataVersion();
}


// Return reference to transformed data
const Data2D& RenderableData2D::transformedData()
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
bool RenderableData2D::yRangeOverX(double xMin, double xMax, double& yMin, double& yMax)
{

	return true;
}

/*
 * Rendering Primitives
 */

// Recreate necessary primitives / primitive assemblies for the data
void RenderableData2D::recreatePrimitives(const View& view, const ColourDefinition& colourDefinition)
{	
	dataPrimitives_.reinitialise(transformedData().nValues(), GL_LINE_STRIP, true);

	constructLineXZ(transformedData().constXAxis(), transformedData().constYAxis(), transformedData().constValues2D(), dataPrimitives_, view.constAxes(), colourDefinition);
}

// Send primitives for rendering
const void RenderableData2D::sendToGL(const double pixelScaling)
{
	// Apply the LineStyle of the Renderable
	lineStyle_.sendToGL(pixelScaling);

	// Disable lighting
	glDisable(GL_LIGHTING);

	dataPrimitives_.sendToGL();

	// Reset LineStyle back to defaults
	LineStyle().sendToGL();
}

// Create line strip primitive
void RenderableData2D::constructLineXZ(const Array<double>& displayXAbscissa, const Array<double>& displayYAbscissa, const Array2D<double>& displayValues, PrimitiveList primitive, const Axes& axes, const ColourDefinition& colourDefinition, double yCoordinate)
{
	// Copy and transform abscissa values (still in data space) into axes coordinates
	Array<double> x = displayXAbscissa;
	axes.transformX(x);
	int nX = x.nItems();
	if (nX < 2) return;
	
	// Copy and transform abscissa values (still in data space) into axes coordinates
	Array<double> y = displayYAbscissa;
	axes.transformY(y);
	int nY = y.nItems();
	if (nY < 2) return;

	// Get some values from axes so we can calculate colours properly
	bool yLogarithmic = axes.logarithmic(1);
	double yStretch = axes.stretch(1);

	// Temporary variables
	GLfloat colour[4];
	Vec3<double> nrm(0.0, 1.0, 0.0);

	// Create lines for slices
	int vertexA, vertexB;
	Array2D<double> v = displayValues;
	axes.transformZ(v);
	double z = axes.transformY(yCoordinate);

	// Set vertexA to -1 so we don't draw a line at n=0
	vertexA = -1;
	Primitive* p;

	// Check for a single colour style in the colourDefinition - use optimised case in that eventuality
	if (colourDefinition.style() == ColourDefinition::SingleColourStyle)
	{
		// Get the single colour
		colourDefinition.colour(0.0, colour);
		
		// Loop over  values
		for (int n=0; n < nY ; ++n)
		{
			for (int m = 0; m < nX; ++m)
			{
				p = primitive[n];
				vertexB = p->defineVertex(x.constAt(m), y.constAt(n), nrm, colour);
			}
			// If both vertices are valid, plot a line
			if (vertexA != -1) p->defineIndices(vertexA, vertexB);

			vertexA = vertexB;
		}
	}
	else
	{
		// Loop over y values
		for (int n=0; n<primitive.nDefinedVertices(); ++n)
		{
			colourDefinition.colour(yLogarithmic ? pow(10.0, y.constAt(n) / yStretch) : y.constAt(n) / yStretch, colour);
			vertexB = p->defineVertex(x.constAt(n), y.constAt(n), z, nrm, colour);

			// If both vertices are valid, plot a line
			if (vertexA != -1) p->defineIndices(vertexA, vertexB);

			vertexA = vertexB;
		}
	}
}
/*
 * Style
 */

// Display Style Keywords
const char* Data2DDisplayStyleKeywords[] = { "Lines" };

// Return keyword for display style index
const char* RenderableData2D::displayStyle(int id)
{
	if ((id < 0) || (id >= RenderableData2D::nDisplayStyles)) return "INVALID_STYLE";

	return Data2DDisplayStyleKeywords[id];
}

// Return display style index from string
int RenderableData2D::displayStyle(const char* s)
{
	for (int n=0; n<nDisplayStyles; ++n) if (DissolveSys::sameString(s, Data2DDisplayStyleKeywords[n])) return (RenderableData2D::DisplayStyle) n;

	return -1;
}
