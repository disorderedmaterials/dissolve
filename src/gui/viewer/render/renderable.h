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

#include "gui/viewer/render/primitiveinfo.h"
#include "gui/viewer/render/primitivelist.h"
#include "gui/viewer/render/colourdefinition.h"
#include "gui/viewer/render/linestyle.h"
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
	enum RenderableType { Data1DRenderable, SpeciesRenderable, nRenderableTypes };
	// Convert text string to RenderableType
	static RenderableType renderableType(const char* s);
	// Convert RenderableType to text string
	static const char* renderableType(RenderableType rt);

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
	virtual const int version() const = 0;

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
	 * Display
	 */
	public:
	// Display Styles enum
	enum DisplayStyle { LineXYStyle, LineZYStyle, GridStyle, SurfaceStyle, UnlitSurfaceStyle, nDisplayStyles };
	// Convert text string to DisplayStyle
	static DisplayStyle displayStyle(const char* s);
	// Convert DisplayStyle to text string
	static const char* displayStyle(DisplayStyle kwd);

	protected:
	// Colour definition for data
	ColourDefinition colour_;
	// Whether data is visible
	bool visible_;
	// Display style of data
	DisplayStyle displayStyle_;
	// Line style
	LineStyle lineStyle_;
	// Surface shininess
	double displaySurfaceShininess_;
	// Style version (relative to data version)
	int displayStyleVersion_;
	// Title to display in legend, if any
	CharString title_;

	public:
	// Set display style of data
	void setDisplayStyle(DisplayStyle style);
	// Return display style of data
	DisplayStyle displayStyle() const;
	// Set basic colour
	void setColour(int r, int g, int b, int a = 255);
	// Set basic colour
	void setColour(ColourDefinition::StockColour stockColour);
	// Return local colour definition for display
	ColourDefinition& colour();
	// Return local colour definition for display (const)
	const ColourDefinition& constColour() const;
	// Set whether data is visible
	void setVisible(bool visible);
	// Return whether data is visible
	bool isVisible() const;
	// Return line style
	LineStyle& lineStyle();
	// Set surface shininess
	void setDisplaySurfaceShininess(double shininess);
	// Return surface shininess
	double displaySurfaceShininess() const;
	// Return style version
	int displayStyleVersion() const;
	// Set title to display in legend
	void setTitle(const char* title);
	// Return title to display in legend, if any
	const char* title() const;
	// Return whether a title to display in legend has been set
	bool hasTitle() const;


	/*
	 * Rendering Primitives
	 */
	protected:
	// Basic Primitives managed by the Renderable, used to assemble representations of data
	PrimitiveList basicPrimitives_;
	// Representation of data using local Primitives
	Array<PrimitiveInfo> primitiveAssembly_;
	// Bespoke primitives create to represent the data
	PrimitiveList bespokePrimitives_;
	// Data version at which bespoke primitives / assembled list were last created
	int lastDataVersion_;
	// ColourDefinition fingerprint at which primitives were last created
	CharString lastColourDefinitionFingerprint_;
	// Axes version at which primitives were last created
	int lastAxesVersion_;
	// Style version at which primitives were last created
	int lastStyleVersion_;

	private:
	// Recreate necessary primitives / primitive assemblies for the data
	virtual void recreatePrimitives(const View& view, const ColourDefinition& colourDefinition) = 0;

	public:
	// Update primitives and send for display
	void updateAndSendPrimitives(const View& view, const RenderableGroupManager& groupManager, bool forceUpdate, bool pushAndPop, const QOpenGLContext* context);
	// Send primitives to GL
	void sendToGL();
};

#endif
