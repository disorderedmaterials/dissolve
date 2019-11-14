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

#ifndef DISSOLVE_RENDERABLE_H
#define DISSOLVE_RENDERABLE_H

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
	enum RenderableType { ConfigurationRenderable, Data1DRenderable, Data2DRenderable, Data3DRenderable, SpeciesRenderable, nRenderableTypes };
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

	private:
	// Return whether a valid data source is available (attempting to set it if not)
	virtual bool validateDataSource() = 0;
	// Return version of data
	virtual int dataVersion() const = 0;

	public:
	// Return identifying tag for source data object
	const char* objectTag() const;


	/*
	 * Transform
	 */
	protected:
	// Equation transformers for each axis
	Transformer transforms_[3];
	// Extreme values of transformed data 
	Vec3<double> transformMin_, transformMax_;
	Vec3<double> transformMinX_, transformMaxX_, transformMinY_, transformMaxY_;
	// Extreme positive values of transformed data
	Vec3<double> transformMinPositive_, transformMaxPositive_;
	// Data version at which transformed data was last calculated
	int transformDataVersion_;

	protected:
	// Transform data according to current settings
	virtual void transformData() = 0;

	public:
	// Return transformed data minima, calculating if necessary
	Vec3<double> transformMin();
	// Return transformed data maxima, calculating if necessary
	Vec3<double> transformMax();
	// Return transformed positive data minima
	Vec3<double> transformMinPositive();
	// Return transformed positive data maxima
	Vec3<double> transformMaxPositive();
	// Set transform equation for data
	void setTransformEquation(int axis, const char* transformEquation);
	// Return transform equation for data
	const char* transformEquation(int axis) const;
	// Return whether specified transform equation is valid
	bool transformEquationValid(int axis) const;
	// Set whether specified transform is enabled
	void setTransformEnabled(int axis, bool enabled);
	// Return whether specified transform is enabled
	bool transformEnabled(int axis) const;
	// Calculate min/max y value over specified x range (if possible in the underlying data)
	virtual bool yRangeOverX(double xMin, double xMax, double& yMin, double& yMax) = 0;


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
	 * Style
	 */
	protected:
	// Whether data is visible
	bool visible_;
	// Display style for Renderable (set from derived class enum)
	int displayStyle_;
	// Colour definition for data
	ColourDefinition colour_;
	// Line style
	LineStyle lineStyle_;
	// Surface shininess
	double displaySurfaceShininess_;
	// Style version (relative to data version)
	int styleVersion_;

	public:
	// Set whether data is visible
	void setVisible(bool visible);
	// Return whether data is visible
	bool isVisible() const;
	// Return keyword for display style index
	virtual const char* displayStyle(int id) = 0;
	// Return display style index from string
	virtual int displayStyle(const char* s) = 0;
	// Set display style index
	void setDisplayStyle(int id);
	// Return display style index
	int displayStyleIndex() const;
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
