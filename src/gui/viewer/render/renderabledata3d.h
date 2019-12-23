/*
	*** Renderable - Data3D
	*** src/gui/viewer/render/renderabledata3d.h
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

#ifndef DISSOLVE_RENDERABLEDATA3D_H
#define DISSOLVE_RENDERABLEDATA3D_H

#include "gui/viewer/render/renderable.h"
#include "math/data3d.h"

// Forward Declarations
class Axes;

// Renderable for Data3D
class RenderableData3D : public Renderable
{
	public:
	// Constructor / Destructor
	RenderableData3D(const Data3D* source, const char* objectTag);
	~RenderableData3D();


	/*
	 * Data
	 */
	private:
	// Source data
	const Data3D* source_;

	private:
	// Return whether a valid data source is available (attempting to set it if not)
	bool validateDataSource();
	// Return version of data
	int dataVersion() const;
	
	
	/*
	 * Transform / Limits
	 */
	private:
	// Transformed data
	Data3D transformedData_;

	protected:
	// Transform data according to current settings
	void transformData();
	// Return reference to transformed data
	const Data3D& transformedData();

	public:
	// Calculate min/max y value over specified x range (if possible in the underlying data)
	bool yRangeOverX(double xMin, double xMax, double& yMin, double& yMax);

	/*
	 * Rendering Primitives
	 */
	private:
	// Primitive to represent data
	Primitive* dataPrimitive_;
	
	private:
	// Create line strip primitive
	void constructLine( const Array< double >& displayXAbscissa, const Array< double >& displayYAbscissa, const Array< double >& displayAbscissa, const Array3D< double >& displayValues, const Axes& axes, const ColourDefinition& colourDefinition);
	void marchingCubesOriginal(const Array<double>& displayXAbscissa, const Array<double>& displayYAbscissa, const Array<double>& displayZAbscissa, const Array3D<double>& displayValues, double lowerCutoff, double upperCutoff, const ColourDefinition& colourDefinition, const Axes& axes, Primitive* primitive);

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
	enum Data3DDisplayStyle
	{
		SolidStyle, nData3DDisplayStyles
	};
	// Return EnumOptions for Data3DDisplayStyle
	static EnumOptions<Data3DDisplayStyle> data3DDisplayStyles();

	private:
	// Display style for the renderable
	Data3DDisplayStyle displayStyle_;
	// Surface shininess for SolidSurface style
	double surfaceShininess_;

	public:
	// Set display style for renderable
	void setDisplayStyle(Data3DDisplayStyle displayStyle);
	// Return display style for the renderable
	Data3DDisplayStyle displayStyle() const;


	/*
	 * Style I/O
	 */
	public:
	// Data3DStyle Keywords Enum
	enum Data3DStyleKeyword
	{
		DisplayKeyword,			/* 'Display' - General display style for renderable */
		EndStyleKeyword,		/* 'EndStyle' - End of Style block */
		nData3DStyleKeywords
	};
	// Return enum option info for RenderableKeyword
	static EnumOptions<RenderableData3D::Data3DStyleKeyword> data3DStyleKeywords();
	// Write style information
	bool writeStyleBlock(LineParser& parser, int indentLevel = 0) const;
	// Read style information
	bool readStyleBlock(LineParser& parser);
};

#endif
