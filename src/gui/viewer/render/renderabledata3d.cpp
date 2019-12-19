/*
	*** Renderable - Data3D
	*** src/gui/viewer/render/renderabledata3d.cpp
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

#include "gui/viewer/render/renderabledata3d.h"
#include "gui/viewer/render/renderablegroupmanager.h"
#include "gui/viewer/render/view.h"
#include "math/data3d.h"
#include "templates/array3d.h"
#include "templates/array2d.h"
#include "math/limits.h"
#include "axes.h"

// Constructor
RenderableData3D::RenderableData3D(const Data3D* source, const char* objectTag) : Renderable(Renderable::Data3DRenderable, objectTag), source_(source)
{
	// Set defaults
	displayStyle_ = LinesStyle;	
	//colour().setStyle(ColourDefinition::HSVGradientStyle);
	dataPrimitive_ = createPrimitive();
	
}

// Destructor
RenderableData3D::~RenderableData3D()
{
}

/*
 * Data
 */

// Return whether a valid data source is available (attempting to set it if not)
bool RenderableData3D::validateDataSource()
{
	// If there is no valid source set, attempt to set it now...
	if (!source_) source_ = Data3D::findObject(objectTag_);

	return source_;
}

// Return version of data
int RenderableData3D::dataVersion() const
{
	return (source_ ? source_->version() : -99);
}


/*
 * Transform / Limits
 */

// Transform data according to current settings
void RenderableData3D::transformData()
{
	// If the transformed data are already up-to-date, no need to do anything
	if (transformDataVersion_ == dataVersion()) return;

	// Copy original data and transform now. We do this even if the transformers are disabled, since they may have previously been active
	if (!validateDataSource()) transformedData_.clear();
	else transformedData_ = *source_;

	Transformer::transform3D(transformedData_, transforms_[0], transforms_[1], transforms_[2]);
	
	transformMin_ = Limits::min(transformedData_.constXAxis());
	transformMax_ = Limits::max(transformedData_.constXAxis());
	axisTransformMinPositive_ = 0.1;
	axisTransformMaxPositive_ = 0.0;
	valuesTransformMinPositive_ = 0.01;
	valuesTransformMaxPositive_ = 0.0;
	
	// Set initial limits if we can
	if (transformedData_.nValues() > 0)
	{
		transformMin_.set(transformedData_.constXAxis().firstValue(), transformedData_.constYAxis().firstValue(), transformedData_.constZAxis().firstValue());
		transformMax_.set(transformedData_.constXAxis().lastValue(), transformedData_.constYAxis().lastValue(), transformedData_.constZAxis().lastValue());
	}

	// Now determine minimum positive limits - loop over points in data, searching for first positive, non-zero value	
	// X
	for (int n=0; n<transformedData_.constXAxis().nItems(); ++n)
	{
		if (transformedData_.constXAxis(n) > 0.0)
		{
			if (transformedData_.constXAxis(n) < axisTransformMinPositive_.x) axisTransformMinPositive_.x = transformedData_.constXAxis(n);
			if (transformedData_.constXAxis(n) > axisTransformMaxPositive_.x) axisTransformMaxPositive_.x = transformedData_.constXAxis(n);
		}
	}
	
	// Y
	for (int n=0; n<transformedData_.constYAxis().nItems(); ++n)
	{	
		if (transformedData_.constYAxis(n) > 0.0)
		{
			if (transformedData_.constYAxis(n) < axisTransformMinPositive_.y) axisTransformMinPositive_.y = transformedData_.constYAxis(n);
			if (transformedData_.constYAxis(n) > axisTransformMaxPositive_.y) axisTransformMaxPositive_.y = transformedData_.constYAxis(n);
		}
	}
	
	// Z
	for (int n=0; n<transformedData_.constZAxis().nItems(); ++n)
	{	
		if (transformedData_.constZAxis(n) > 0.0)
		{
			if (transformedData_.constZAxis(n) < axisTransformMinPositive_.z) axisTransformMinPositive_.z = transformedData_.constZAxis(n);
			if (transformedData_.constZAxis(n) > axisTransformMaxPositive_.z) axisTransformMaxPositive_.z = transformedData_.constZAxis(n);
		}
	}
	
	// Values
	for (int n=0; n<transformedData_.nValues(); ++n)
	{
		double val = transformedData_.values().linearValue(n);
		if ( val > 0.0)
		{
			if (val < valuesTransformMinPositive_)
				valuesTransformMinPositive_  = val;
			if (val > valuesTransformMaxPositive_)
				valuesTransformMaxPositive_  = val;
		}
	}
			
	// Update the transformed data 'version'
	transformDataVersion_ = dataVersion();
}


// Return reference to transformed data
const Data3D& RenderableData3D::transformedData()
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
bool RenderableData3D::yRangeOverX(double xMin, double xMax, double& yMin, double& yMax)
{

	return true;
}

/*
 * Rendering Primitives
 */

// Recreate necessary primitives / primitive assemblies for the data
void RenderableData3D::recreatePrimitives(const View& view, const ColourDefinition& colourDefinition)
{	
	dataPrimitive_->initialise(GL_TRIANGLES, true);
	//marchingCubesOriginal(transformedData_.constXAxis(), transformedData_.constYAxis(), transformedData_.constZAxis(), transformedData_.constValues3D(), (valuesTransformMinPositive_+valuesTransformMaxPositive_)/2, valuesTransformMaxPositive_, colourDefinition, view.constAxes(), dataPrimitive_);
	printf("SIZE OF X = %i\n", transformedData_.constXAxis().nItems());
	printf("SIZE OF Y = %i\n", transformedData_.constYAxis().nItems());
	printf("SIZE OF Z = %i\n", transformedData_.constZAxis().nItems());
	marchingCubesOriginal(transformedData_.constXAxis(), transformedData_.constYAxis(), transformedData_.constZAxis(), transformedData_.constValues3D(), 0.0, 0.0007, colourDefinition, view.constAxes(), dataPrimitive_);
}

// Send primitives for rendering
const void RenderableData3D::sendToGL(const double pixelScaling)
{
	// Apply the LineStyle of the Renderable
	lineStyle_.sendToGL(pixelScaling);

	// Disable lighting
	glDisable(GL_LIGHTING);
	
	dataPrimitive_->sendToGL();
	// Reset LineStyle back to defaults
	LineStyle().sendToGL();
}

// Marching Cube Edge Vertex Lookup Table
int edgevertices[12][2] = { { 0,1 }, { 1,2 }, { 2,3 }, { 0,3 },
	{ 4,5 }, { 5,6 }, { 6,7 }, { 4,7 },
	{ 0,4 }, { 1,5 }, { 3,7 }, { 2,6 } };

Vec3<double> vertexPos[8] = { Vec3<double>(0,0,0), Vec3<double>(1,0,0), Vec3<double>(1,1,0), Vec3<double>(0,1,0), Vec3<double>(0,0,1), Vec3<double>(1,0,1), Vec3<double>(1,1,1), Vec3<double>(0,1,1) };
	
// Marching Cube Face Triplet Lookup Table
int facetriples[256][15] = {
	{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0, 8, 3, 1, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 9, 2, 11, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 2, 8, 3, 2, 11, 8, 11, 9, 8, -1, -1, -1, -1, -1, -1},
	{ 3, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0, 10, 2, 8, 10, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1, 9, 0, 2, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1, 10, 2, 1, 9, 10, 9, 8, 10, -1, -1, -1, -1, -1, -1},
	{ 3, 11, 1, 10, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0, 11, 1, 0, 8, 11, 8, 10, 11, -1, -1, -1, -1, -1, -1},
	{ 3, 9, 0, 3, 10, 9, 10, 11, 9, -1, -1, -1, -1, -1, -1},
	{ 9, 8, 11, 11, 8, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1}, 
	{ 1, 2, 11, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 3, 4, 7, 3, 0, 4, 1, 2, 11, -1, -1, -1, -1, -1, -1}, 
	{ 9, 2, 11, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1}, 
	{ 2, 11, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1}, 
	{ 8, 4, 7, 3, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 10, 4, 7, 10, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1}, 
	{ 9, 0, 1, 8, 4, 7, 2, 3, 10, -1, -1, -1, -1, -1, -1}, 
	{ 4, 7, 10, 9, 4, 10, 9, 10, 2, 9, 2, 1, -1, -1, -1}, 
	{ 3, 11, 1, 3, 10, 11, 7, 8, 4, -1, -1, -1, -1, -1, -1}, 
	{ 1, 10, 11, 1, 4, 10, 1, 0, 4, 7, 10, 4, -1, -1, -1}, 
	{ 4, 7, 8, 9, 0, 10, 9, 10, 11, 10, 0, 3, -1, -1, -1}, 
	{ 4, 7, 10, 4, 10, 9, 9, 10, 11, -1, -1, -1, -1, -1, -1}, 
	{ 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1}, 
	{ 1, 2, 11, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 3, 0, 8, 1, 2, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1}, 
	{ 5, 2, 11, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1}, 
	{ 2, 11, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1}, 
	{ 9, 5, 4, 2, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0, 10, 2, 0, 8, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1}, 
	{ 0, 5, 4, 0, 1, 5, 2, 3, 10, -1, -1, -1, -1, -1, -1}, 
	{ 2, 1, 5, 2, 5, 8, 2, 8, 10, 4, 8, 5, -1, -1, -1}, 
	{ 11, 3, 10, 11, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1}, 
	{ 4, 9, 5, 0, 8, 1, 8, 11, 1, 8, 10, 11, -1, -1, -1}, 
	{ 5, 4, 0, 5, 0, 10, 5, 10, 11, 10, 0, 3, -1, -1, -1}, 
	{ 5, 4, 8, 5, 8, 11, 11, 8, 10, -1, -1, -1, -1, -1, -1}, 
	{ 9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1}, 
	{ 0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1}, 
	{ 1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 9, 7, 8, 9, 5, 7, 11, 1, 2, -1, -1, -1, -1, -1, -1}, 
	{ 11, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1}, 
	{ 8, 0, 2, 8, 2, 5, 8, 5, 7, 11, 5, 2, -1, -1, -1}, 
	{ 2, 11, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1}, 
	{ 7, 9, 5, 7, 8, 9, 3, 10, 2, -1, -1, -1, -1, -1, -1}, 
	{ 9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 10, -1, -1, -1}, 
	{ 2, 3, 10, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1}, 
	{ 10, 2, 1, 10, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1}, 
	{ 9, 5, 8, 8, 5, 7, 11, 1, 3, 11, 3, 10, -1, -1, -1}, 
	{ 5, 7, 10, 5, 10, 11, 1, 0, 9, -1, -1, -1, -1, -1, -1}, 
	{ 10, 11, 5, 10, 5, 7, 8, 0, 3, -1, -1, -1, -1, -1, -1}, 
	{ 10, 11, 5, 7, 10, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 11, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 0, 8, 3, 5, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 9, 0, 1, 5, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 1, 8, 3, 1, 9, 8, 5, 11, 6, -1, -1, -1, -1, -1, -1}, 
	{ 1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1}, 
	{ 9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1}, 
	{ 5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1}, 
	{ 2, 3, 10, 11, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 10, 0, 8, 10, 2, 0, 11, 6, 5, -1, -1, -1, -1, -1, -1}, 
	{ 0, 1, 9, 2, 3, 10, 5, 11, 6, -1, -1, -1, -1, -1, -1}, 
	{ 5, 11, 6, 1, 9, 2, 9, 10, 2, 9, 8, 10, -1, -1, -1}, 
	{ 6, 3, 10, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1}, 
	{ 0, 8, 10, 0, 10, 5, 0, 5, 1, 5, 10, 6, -1, -1, -1}, 
	{ 3, 10, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1}, 
	{ 6, 5, 9, 6, 9, 10, 10, 9, 8, -1, -1, -1, -1, -1, -1}, 
	{ 5, 11, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 4, 3, 0, 4, 7, 3, 6, 5, 11, -1, -1, -1, -1, -1, -1}, 
	{ 1, 9, 0, 5, 11, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1}, 
	{ 11, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1}, 
	{ 6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1}, 
	{ 1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1}, 
	{ 8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1}, 
	{ 7, 3, 2, 7, 2, 6, 5, 9, 4, -1, -1, -1, -1, -1, -1}, 
	{ 3, 10, 2, 7, 8, 4, 11, 6, 5, -1, -1, -1, -1, -1, -1}, 
	{ 5, 11, 6, 4, 7, 2, 4, 2, 0, 2, 7, 10, -1, -1, -1}, 
	{ 0, 1, 9, 4, 7, 8, 2, 3, 10, 5, 11, 6, -1, -1, -1}, 
	{ 9, 4, 5, 11, 2, 1, 7, 10, 6, -1, -1, -1, -1, -1, -1}, 
	{ 8, 4, 7, 3, 10, 5, 3, 5, 1, 5, 10, 6, -1, -1, -1}, 
	{ 5, 1, 0, 5, 0, 4, 7, 10, 6, -1, -1, -1, -1, -1, -1}, 
	{ 0, 3, 8, 4, 5, 9, 10, 6, 7, -1, -1, -1, -1, -1, -1}, 
	{ 4, 5, 9, 7, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 11, 4, 9, 6, 4, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 4, 11, 6, 4, 9, 11, 0, 8, 3, -1, -1, -1, -1, -1, -1}, 
	{ 11, 0, 1, 11, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1}, 
	{ 8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 11, -1, -1, -1}, 
	{ 1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1}, 
	{ 3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1}, 
	{ 0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1}, 
	{ 11, 4, 9, 11, 6, 4, 10, 2, 3, -1, -1, -1, -1, -1, -1}, 
	{ 0, 8, 2, 2, 8, 10, 4, 9, 11, 4, 11, 6, -1, -1, -1}, 
	{ 3, 10, 2, 0, 1, 6, 0, 6, 4, 6, 1, 11, -1, -1, -1}, 
	{ 6, 4, 8, 6, 8, 10, 2, 1, 11, -1, -1, -1, -1, -1, -1}, 
	{ 9, 6, 4, 9, 3, 6, 9, 1, 3, 10, 6, 3, -1, -1, -1}, 
	{ 8, 10, 6, 8, 6, 4, 9, 1, 0, -1, -1, -1, -1, -1, -1}, 
	{ 3, 10, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1}, 
	{ 6, 4, 8, 10, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 7, 11, 6, 7, 8, 11, 8, 9, 11, -1, -1, -1, -1, -1, -1}, 
	{ 0, 7, 3, 0, 11, 7, 0, 9, 11, 6, 7, 11, -1, -1, -1}, 
	{ 11, 6, 7, 1, 11, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1}, 
	{ 11, 6, 7, 11, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1}, 
	{ 1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1}, 
	{ 2, 6, 7, 2, 7, 3, 0, 9, 1, -1, -1, -1, -1, -1, -1}, 
	{ 7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1}, 
	{ 7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 2, 3, 10, 11, 6, 8, 11, 8, 9, 8, 6, 7, -1, -1, -1}, 
	{ 2, 0, 9, 2, 9, 11, 6, 7, 10, -1, -1, -1, -1, -1, -1}, 
	{ 1, 11, 2, 3, 8, 0, 6, 7, 10, -1, -1, -1, -1, -1, -1}, 
	{ 11, 2, 1, 6, 7, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 8, 9, 1, 8, 1, 3, 10, 6, 7, -1, -1, -1, -1, -1, -1}, 
	{ 0, 9, 1, 10, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 3, 8, 0, 10, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 7, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 7, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 3, 0, 8, 10, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 0, 1, 9, 10, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 8, 1, 9, 8, 3, 1, 10, 7, 6, -1, -1, -1, -1, -1, -1}, 
	{ 11, 1, 2, 6, 10, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 1, 2, 11, 3, 0, 8, 6, 10, 7, -1, -1, -1, -1, -1, -1}, 
	{ 2, 9, 0, 2, 11, 9, 6, 10, 7, -1, -1, -1, -1, -1, -1}, 
	{ 2, 10, 3, 11, 8, 6, 11, 9, 8, 8, 7, 6, -1, -1, -1}, 
	{ 7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1}, 
	{ 2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1}, 
	{ 1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1}, 
	{ 11, 7, 6, 11, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1}, 
	{ 11, 7, 6, 1, 7, 11, 1, 8, 7, 1, 0, 8, -1, -1, -1}, 
	{ 0, 3, 7, 0, 7, 11, 0, 11, 9, 6, 11, 7, -1, -1, -1}, 
	{ 7, 6, 11, 7, 11, 8, 8, 11, 9, -1, -1, -1, -1, -1, -1}, 
	{ 6, 8, 4, 10, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 3, 6, 10, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1}, 
	{ 8, 6, 10, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1}, 
	{ 9, 4, 6, 9, 6, 3, 9, 3, 1, 10, 3, 6, -1, -1, -1}, 
	{ 6, 8, 4, 6, 10, 8, 2, 11, 1, -1, -1, -1, -1, -1, -1}, 
	{ 3, 2, 10, 0, 6, 1, 0, 4, 6, 6, 11, 1, -1, -1, -1}, 
	{ 0, 2, 8, 2, 10, 8, 4, 11, 9, 4, 6, 11, -1, -1, -1}, 
	{ 11, 9, 4, 11, 4, 6, 10, 3, 2, -1, -1, -1, -1, -1, -1}, 
	{ 8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1}, 
	{ 0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 3, 8, 0, 1, 9, 2, 2, 9, 4, 2, 4, 6, -1, -1, -1}, 
	{ 1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1}, 
	{ 8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 11, 1, -1, -1, -1}, 
	{ 11, 1, 0, 11, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1}, 
	{ 4, 6, 11, 4, 11, 9, 0, 3, 8, -1, -1, -1, -1, -1, -1}, 
	{ 11, 9, 4, 6, 11, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 4, 9, 5, 7, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 0, 8, 3, 4, 9, 5, 10, 7, 6, -1, -1, -1, -1, -1, -1}, 
	{ 5, 0, 1, 5, 4, 0, 7, 6, 10, -1, -1, -1, -1, -1, -1}, 
	{ 8, 7, 4, 3, 5, 10, 3, 1, 5, 5, 6, 10, -1, -1, -1}, 
	{ 9, 5, 4, 11, 1, 2, 7, 6, 10, -1, -1, -1, -1, -1, -1}, 
	{ 0, 9, 1, 4, 8, 7, 2, 10, 3, 5, 6, 11, -1, -1, -1}, 
	{ 5, 6, 11, 4, 2, 7, 4, 0, 2, 2, 10, 7, -1, -1, -1}, 
	{ 3, 2, 10, 7, 4, 8, 11, 5, 6, -1, -1, -1, -1, -1, -1}, 
	{ 7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1}, 
	{ 8, 7, 4, 9, 5, 0, 0, 5, 6, 0, 6, 2, -1, -1, -1}, 
	{ 1, 5, 2, 5, 6, 2, 3, 4, 0, 3, 7, 4, -1, -1, -1}, 
	{ 6, 2, 1, 6, 1, 5, 4, 8, 7, -1, -1, -1, -1, -1, -1}, 
	{ 11, 5, 6, 1, 7, 9, 1, 3, 7, 7, 4, 9, -1, -1, -1}, 
	{ 1, 0, 9, 5, 6, 11, 8, 7, 4, -1, -1, -1, -1, -1, -1}, 
	{ 4, 0, 3, 4, 3, 7, 6, 11, 5, -1, -1, -1, -1, -1, -1}, 
	{ 5, 6, 11, 4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 6, 9, 5, 6, 10, 9, 10, 8, 9, -1, -1, -1, -1, -1, -1}, 
	{ 3, 6, 10, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1}, 
	{ 0, 10, 8, 0, 5, 10, 0, 1, 5, 5, 6, 10, -1, -1, -1}, 
	{ 6, 10, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1}, 
	{ 5, 6, 11, 1, 2, 9, 9, 2, 10, 9, 10, 8, -1, -1, -1}, 
	{ 0, 9, 1, 2, 10, 3, 5, 6, 11, -1, -1, -1, -1, -1, -1}, 
	{ 10, 8, 0, 10, 0, 2, 11, 5, 6, -1, -1, -1, -1, -1, -1}, 
	{ 2, 10, 3, 11, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1}, 
	{ 9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1}, 
	{ 1, 5, 6, 1, 6, 2, 3, 8, 0, -1, -1, -1, -1, -1, -1}, 
	{ 1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 1, 3, 8, 1, 8, 9, 5, 6, 11, -1, -1, -1, -1, -1, -1}, 
	{ 9, 1, 0, 5, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 0, 3, 8, 5, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 11, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 10, 5, 11, 7, 5, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 10, 5, 11, 10, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1}, 
	{ 5, 10, 7, 5, 11, 10, 1, 9, 0, -1, -1, -1, -1, -1, -1}, 
	{ 9, 8, 5, 8, 7, 5, 11, 3, 1, 11, 10, 3, -1, -1, -1}, 
	{ 10, 1, 2, 10, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1}, 
	{ 2, 10, 3, 0, 8, 1, 1, 8, 7, 1, 7, 5, -1, -1, -1}, 
	{ 9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 10, 7, -1, -1, -1}, 
	{ 7, 5, 9, 7, 9, 8, 3, 2, 10, -1, -1, -1, -1, -1, -1}, 
	{ 2, 5, 11, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1}, 
	{ 8, 2, 0, 8, 5, 2, 8, 7, 5, 11, 2, 5, -1, -1, -1}, 
	{ 11, 2, 1, 9, 0, 5, 5, 0, 3, 5, 3, 7, -1, -1, -1}, 
	{ 9, 8, 7, 9, 7, 5, 11, 2, 1, -1, -1, -1, -1, -1, -1}, 
	{ 1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1}, 
	{ 9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1}, 
	{ 9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 5, 8, 4, 5, 11, 8, 11, 10, 8, -1, -1, -1, -1, -1, -1}, 
	{ 5, 0, 4, 5, 10, 0, 5, 11, 10, 10, 3, 0, -1, -1, -1}, 
	{ 4, 5, 9, 0, 1, 8, 8, 1, 11, 8, 11, 10, -1, -1, -1}, 
	{ 11, 10, 3, 11, 3, 1, 9, 4, 5, -1, -1, -1, -1, -1, -1}, 
	{ 2, 5, 1, 2, 8, 5, 2, 10, 8, 4, 5, 8, -1, -1, -1}, 
	{ 0, 4, 5, 0, 5, 1, 2, 10, 3, -1, -1, -1, -1, -1, -1}, 
	{ 0, 2, 10, 0, 10, 8, 4, 5, 9, -1, -1, -1, -1, -1, -1}, 
	{ 9, 4, 5, 2, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 2, 5, 11, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1}, 
	{ 5, 11, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1}, 
	{ 3, 8, 0, 1, 11, 2, 4, 5, 9, -1, -1, -1, -1, -1, -1}, 
	{ 1, 11, 2, 9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1}, 
	{ 0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 9, 4, 5, 0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 4, 10, 7, 4, 9, 10, 9, 11, 10, -1, -1, -1, -1, -1, -1}, 
	{ 4, 8, 7, 9, 10, 0, 9, 11, 10, 10, 3, 0, -1, -1, -1}, 
	{ 1, 11, 10, 1, 10, 4, 1, 4, 0, 7, 4, 10, -1, -1, -1}, 
	{ 3, 1, 11, 3, 11, 10, 7, 4, 8, -1, -1, -1, -1, -1, -1}, 
	{ 4, 10, 7, 9, 10, 4, 9, 2, 10, 9, 1, 2, -1, -1, -1}, 
	{ 9, 1, 0, 8, 7, 4, 2, 10, 3, -1, -1, -1, -1, -1, -1}, 
	{ 10, 7, 4, 10, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1}, 
	{ 8, 7, 4, 3, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 2, 9, 11, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1}, 
	{ 9, 11, 2, 9, 2, 0, 8, 7, 4, -1, -1, -1, -1, -1, -1}, 
	{ 3, 7, 4, 3, 4, 0, 1, 11, 2, -1, -1, -1, -1, -1, -1}, 
	{ 1, 11, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1}, 
	{ 0, 9, 1, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 9, 11, 8, 11, 10, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 3, 0, 9, 3, 9, 10, 10, 9, 11, -1, -1, -1, -1, -1, -1}, 
	{ 0, 1, 11, 0, 11, 8, 8, 11, 10, -1, -1, -1, -1, -1, -1}, 
	{ 3, 1, 11, 10, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 1, 2, 10, 1, 10, 9, 9, 10, 8, -1, -1, -1, -1, -1, -1}, 
	{ 1, 0, 9, 2, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 0, 2, 10, 8, 0, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 3, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 2, 3, 8, 2, 8, 11, 11, 8, 9, -1, -1, -1, -1, -1, -1}, 
	{ 9, 11, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 0, 3, 8, 1, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 1, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
	{ 0, 3, 8, 11, 1, 2, 6, 10, 7, 4, 9, 5, -1, -1, -1 }
// 	{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

// marching cubes 
// Render volumetric isosurface with Marching Cubes ORIGINAL
void RenderableData3D::marchingCubesOriginal ( const Array< double >& displayXAbscissa, const Array< double >& displayYAbscissa, const Array< double >& displayZAbscissa, const Array3D< double >& displayValues, double lowerCutoff, double upperCutoff, const ColourDefinition& colourDefinition, const Axes& axes, Primitive* primitive )
{
	int i, j, k, n, cubeType, *faces;
	Vec3<GLfloat> normal, gradient[8];
	Vec3<double> r, v1, v2;
	double vertex[8], ipol, a, b, mult = 1.0;
	GLfloat colour[4];
	Array<double> x = displayXAbscissa, y = displayYAbscissa, z = displayZAbscissa;

// 	printf("SIZE OF X = %i\n", x.nItems());
// 	printf("SIZE OF Y = %i\n", y.nItems());
// 	printf("SIZE OF Z = %i\n", z.nItems());
	// Get distances between grid points
	double dx, dy, dz;
	dx = x.constAt(1) - x.constAt(0);
	dy = y.constAt(1) - y.constAt(0);
	dz = z.constAt(1) - z.constAt(0);
	
	// Transform abscissa values (still in data space) into axes coordinates
	axes.transformX(x);
	int nX = x.nItems();
	if (nX < 2) return;
	
	// Transform abscissa values (still in data space) into axes coordinates
	axes.transformY(y);
	int nY = y.nItems();
	if (nY < 2) return;

	// Transform abscissa values (still in data space) into axes coordinates
	axes.transformZ(z);
	int nZ = z.nItems();
	if (nZ < 2) return;

	
	// Generate isosurface
	for (i=0; i< x.nItems()-1; ++i)
	{
		//if ((i < 2) || (i > (x.nItems()-3))) continue;
		for (j=0; j< y.nItems()-3; ++j)
		{
			
			//if ((j < 2) || (j > (y.nItems()-3))) continue;
			for (k=0; k< z.nItems()-1; ++k)
			{
				//if ((k < 2) || (k > (z.nItems()-3))) continue;

				// Grab values that form vertices of cube.
				vertex[0] = displayValues.constAt(i, j, k);
				vertex[1] = displayValues.constAt(i+1, j, k);
				vertex[2] = displayValues.constAt(i+1, j+1, k);
				vertex[3] = displayValues.constAt(i, j+1, k);
				vertex[4] = displayValues.constAt(i, j, k+1);
				vertex[5] = displayValues.constAt(i+1, j, k+1);
				vertex[6] = displayValues.constAt(i+1, j+1, k+1);
				vertex[7] = displayValues.constAt(i, j +1, k+1);
/*
				// Calculate gradients at the cube vertices
				gradient[0].x = (vertex[1] - displayValues.constAt(i-1, j, k)) / dx;
				gradient[0].y = (vertex[3] - displayValues.constAt(i, j-1, k)) / dy;
				gradient[0].z = (vertex[4] - displayValues.constAt(i, j, k-1)) / dz;
				gradient[1].x = (displayValues.constAt(i+2, j, k) - vertex[0]) / dx;
				gradient[1].y = (vertex[2] - displayValues.constAt(i+1, j-1, k)) / dy;
				gradient[1].z = (vertex[5] - displayValues.constAt(i+1, j, k-1)) / dz;
				gradient[2].x = (displayValues.constAt(i+2, j+1, k) - vertex[3]) / dx;
				gradient[2].y = (displayValues.constAt(i+1, j+2, k) - vertex[1]) / dy;
				gradient[2].z = (vertex[6] - displayValues.constAt(i+1, j+1, k-1)) / dz;
				gradient[3].x = (vertex[2] - displayValues.constAt(i-1, j+1, k)) / dx;
				gradient[3].y = (displayValues.constAt(i , j+2, k) - vertex[0]) / dy;
				gradient[3].z = (vertex[7] - displayValues.constAt(i, j+1, k-1)) / dz;
				gradient[4].x = (vertex[5] - displayValues.constAt(i-1, j, k+1)) / dx;
				gradient[4].y = (vertex[7] - displayValues.constAt(i, j-1, k+1)) / dy;
				gradient[4].z = (displayValues.constAt(i, j, k+2) - vertex[0]) / dz;
				gradient[5].x = (displayValues.constAt(i+2, j, k+1) - vertex[4]) / dx;
				gradient[5].y = (vertex[6] - displayValues.constAt(i+1, j-1, k+1)) / dy;
				gradient[5].z = (displayValues.constAt(i+1, j, k+2) - vertex[1]) / dz;
				gradient[6].x = (displayValues.constAt(i+2, j+1, k+1) - vertex[7]) / dx;
				gradient[6].y = (displayValues.constAt(i+1, j+2, k+1) - vertex[5]) / dy;
				gradient[6].z = (displayValues.constAt(i+1, j+1, k+2) - vertex[2]) / dz;
				gradient[7].x = (vertex[6] - displayValues.constAt(i-1, j+1, k+1)) / dx;
				gradient[7].y = (displayValues.constAt(i, j+2, k+1) - vertex[4]) / dy;
				gradient[7].z = (displayValues.constAt(i, j+1, k+2) - vertex[3]) / dz;*/

				// Determine cube type
				cubeType = 0;
				if ((vertex[0] >= lowerCutoff) && (vertex[0] <= upperCutoff)) cubeType += 1;
				if ((vertex[1] >= lowerCutoff) && (vertex[1] <= upperCutoff)) cubeType += 2;
				if ((vertex[2] >= lowerCutoff) && (vertex[2] <= upperCutoff)) cubeType += 4;
				if ((vertex[3] >= lowerCutoff) && (vertex[3] <= upperCutoff)) cubeType += 8;
				if ((vertex[4] >= lowerCutoff) && (vertex[4] <= upperCutoff)) cubeType += 16;
				if ((vertex[5] >= lowerCutoff) && (vertex[5] <= upperCutoff)) cubeType += 32;
				if ((vertex[6] >= lowerCutoff) && (vertex[6] <= upperCutoff)) cubeType += 64;
				if ((vertex[7] >= lowerCutoff) && (vertex[7] <= upperCutoff)) cubeType += 128;
				
				//if (cubeType != 0) printf("CubeType %i %i %i = %i\n", i, j, k, cubeType); 
				if (cubeType != 0)
				{
					// Get edges from list and draw triangles or points
					faces = facetriples[cubeType];
					for (n = 0; n<15; n++)
					{
						if (faces[n] == -1) break;

						// Get edge vectors, interpolate, and set tri-points
						a = vertex[edgevertices[faces[n]][0]];
						b = vertex[edgevertices[faces[n]][1]];
						ipol = ((mult*lowerCutoff) - a) / (b-a);
						if (ipol> 1.0) ipol = 1.0;
						if (ipol < 0.0) ipol = 0.0;
						v1 = vertexPos[edgevertices[faces[n]][0]];
						v2 = vertexPos[edgevertices[faces[n]][1]];
						r.set(v2[0]-v1[0], v2[1]-v1[1], v2[2]-v1[2]);

						r *= ipol;
						//printf("Gradient A = "); gradient[edgevertices[faces[n]][0]].print();
						//printf("Gradient B = "); gradient[edgevertices[faces[n]][1]].print();
						normal = (gradient[edgevertices[faces[n]][0]] + (gradient[edgevertices[faces[n]][1]] - gradient[edgevertices[faces[n]][0]]) * ipol) * -mult;
						normal.normalise();
						r.add(i+v1[0], j+v1[1], k+v1[2]);

						// Set triangle coordinates and add cube position
						//if (colourScale != -1)
							if (colourDefinition.style() == ColourDefinition::SingleColourStyle)
							{
								// Get the single colour
								colourDefinition.colour(0.0, colour);
								//colourScale[colourScale].colour((a+b)/2.0, colour);
								primitive -> defineVertex(r.x, r.y, r.z, normal.x, normal.y, normal.z, colour);
							}
							else 
								primitive -> defineVertex(x.constAt(i), y.constAt(j), z.constAt(k), normal.x, normal.y, normal.z);
					}
				}
			}
		}
	}
	
}

/*
 * Style
 */

// Display Style Keywords
const char* Data3DDisplayStyleKeywords[] = { "Lines" };

// Return keyword for display style index
const char* RenderableData3D::displayStyle(int id)
{
	if ((id < 0) || (id >= RenderableData3D::nDisplayStyles)) return "INVALID_STYLE";

	return Data3DDisplayStyleKeywords[id];
}

// Return display style index from string
int RenderableData3D::displayStyle(const char* s)
{
	for (int n=0; n<nDisplayStyles; ++n) if (DissolveSys::sameString(s, Data3DDisplayStyleKeywords[n])) return (RenderableData3D::DisplayStyle) n;

	return -1;
}
