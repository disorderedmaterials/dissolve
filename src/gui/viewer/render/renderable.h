/*
	*** Renderable
	*** src/gui/viewer/render/renderable.h
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

#ifndef DISSOLVE_RENDER_RENDERABLE_H
#define DISSOLVE_RENDER_RENDERABLE_H

#include "gui/viewer/render/primitiveassembly.h"
#include "gui/viewer/render/primitivelist.h"
#include "gui/viewer/render/colourdefinition.h"
#include "gui/viewer/render/linestyle.h"
#include "gui/stockcolours.h"
#include "math/transformer.h"
#include "base/charstring.h"

// Forward Declarations
class PlottableData;
class RenderableGroup;
class RenderableGroupManager;
class View;

class Renderable : public ListItem<Renderable>
{
	public:
	// Renderable type
	enum RenderableType { ConfigurationRenderable, Data1DRenderable, Data2DRenderable, Data3DRenderable, SpeciesRenderable, SpeciesSiteRenderable, nRenderableTypes };
	// Return enum options for RenderableType
	static EnumOptions<RenderableType> renderableTypes();
	// Constructor / Destructor
	Renderable(RenderableType type, const char* objectTag);
	virtual ~Renderable();


	/*
	 * Identity
	 */
	protected:
	// Name of Renderable
	CharString name_;
	// Type of Renderable
	RenderableType type_;

	public:
	// Set name of Renderable
	void setName(const char* name);
	// Return name of Renderable
	const char* name();
	// Return type of Renderable
	RenderableType type() const;


	/*
	 * Data
	 */
	protected:
	// Identifying tag for source data object
	CharString objectTag_;
	// Equation transformer for values
	Transformer valuesTransform_;
	// Coordinate limits of all data (after value transform if enabled)
	Vec3<double> limitsMin_, limitsMax_;
	// Positive coordinate extrema of all data (after value transform if enabled)
	Vec3<double> positiveLimitsMin_, positiveLimitsMax_;
	// Extrema of values (after transform if enabled)
	double valuesMin_, valuesMax_;
	// Extreme positive lixmits of transformed values
	double positiveValuesMin_, positiveValuesMax_;
	// Data version at which values were last transformed
	int valuesTransformDataVersion_;

	private:
	// Return whether a valid data source is available (attempting to set it if not)
	virtual bool validateDataSource() = 0;

	protected:
	// Transform data values
	virtual void transformValues() = 0;

	public:
	// Return identifying tag for source data object
	const char* objectTag() const;
	// Return version of data
	virtual int dataVersion() const = 0;
	// Return coordinate minima of all data (after value transform if enabled)
	Vec3<double> limitsMin();
	// Return coordinate maxima of all data (after value transform if enabled)
	Vec3<double> limitsMax();
	// Return positive coordinate minima of all data (after value transform if enabled)
	Vec3<double> positiveLimitsMin();
	// Return positive coordinate maxima of all data (after value transform if enabled)
	Vec3<double> positiveLimitsMax();
	// Return minimum of transformed values
	double valuesMin();
	// Return maximum of transformed values
	double valuesMax();
	// Return minimum positive of transformed values
	double positiveValuesMin();
	// Return maximum positive of transformed values
	double positiveValuesMax();
	// Set values transform equation
	void setValuesTransformEquation(const char* transformEquation);
	// Return values transform equation
	const char* valuesTransformEquation() const;
	// Return whether the values transform equation is valid
	bool valuesTransformEquationValid() const;
	// Set whether values transform is enabled
	void setValuesTransformEnabled(bool enabled);
	// Return whether values transform is enabled
	bool valuesTransformEnabled() const;
	// Return data version at which values were last transformed
	int valuesTransformDataVersion() const;
	// Calculate min/max y value over specified x range (if possible in the underlying data)
	virtual bool yRangeOverX(double xMin, double xMax, double& yMin, double& yMax);


	/*
	 * Group
	 */
	protected:
	// Group that this Renderable is associated to (if any)
	RenderableGroup* group_;

	public:
	// Set group that this Renderable is associated to
	void setGroup(RenderableGroup* group);
	// Return group that this Renderable is associated to
	RenderableGroup* group() const;


	/*
	 * Basic Style
	 */
	protected:
	// Whether Renderable is visible
	bool visible_;
	// Colour definition
	ColourDefinition colour_;
	// Line style
	LineStyle lineStyle_;
	// Style version (relative to data version)
	int styleVersion_;

	public:
	// Set whether Renderable is visible
	void setVisible(bool visible);
	// Return whether Renderable is visible
	bool isVisible() const;
	// Set basic colour
	void setColour(int r, int g, int b, int a = 255);
	// Set basic colour
	void setColour(StockColours::StockColour stockColour);
	// Return local colour definition for display
	ColourDefinition& colour();
	// Return local colour definition for display (const)
	const ColourDefinition& constColour() const;
	// Return line style
	LineStyle& lineStyle();
	// Return style version
	int styleVersion() const;


	/*
	 * Style I/O
	 */
	public:
	// Write style information
	virtual bool writeStyleBlock(LineParser& parser, int indentLevel = 0) const = 0;
	// Read style information
	virtual bool readStyleBlock(LineParser& parser) = 0;



	/*
	 * Rendering Primitives
	 */
	private:
	// Primitives instance-managed by the Renderable
	PrimitiveList primitives_;
	// Data version at which bespoke primitives / assembled list were last created
	int lastDataVersion_;
	// ColourDefinition fingerprint at which primitives were last created
	CharString lastColourDefinitionFingerprint_;
	// Axes version at which primitives were last created
	int lastAxesVersion_;
	// Style version at which primitives were last created
	int lastStyleVersion_;

	protected:
	// Create single Primitive, whose instances will be managed by the Renderable
	Primitive* createPrimitive(GLenum type = GL_LINES, bool colourData = false);
	// Reinitialise managed Primitive list to the size specified
	void reinitialisePrimitives(int newSize, GLenum type, bool colourData);
	// Return nth Primitive managed by the Renderable
	Primitive* primitive(int n);
	// Remove specified Primitive
	void removePrimitive(Primitive* primitive);
	// Recreate necessary primitives / primitive assemblies for the data
	virtual void recreatePrimitives(const View& view, const ColourDefinition& colourDefinition) = 0;
	// Send primitives for rendering
	virtual const void sendToGL(const double pixelScaling) = 0;

	public:
	// Update primitives and send to display
	void updateAndSendPrimitives(const View& view, const RenderableGroupManager& groupManager, bool forceUpdate, bool pushAndPop, const QOpenGLContext* context, double pixelScaling);
};

#endif
