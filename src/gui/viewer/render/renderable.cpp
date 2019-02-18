/*
	*** Renderable Data
	*** src/gui/view/render/renderable.cpp
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

#include "gui/viewer/render/renderable.h"
#include "gui/viewer/render/view.h"
#include "math/interpolator.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include <limits>

// Renderable Type
const char* RenderableTypeKeywords[] = { "Data1D" };

// Convert text string to RenderableType
Renderable::RenderableType Renderable::renderableType(const char* s)
{
	for (int n=0; n<nRenderableTypes; ++n) if (DissolveSys::sameString(s, RenderableTypeKeywords[n])) return (Renderable::RenderableType) n;
	return nRenderableTypes;
}

// Convert RenderableType to text string
const char* Renderable::renderableType(RenderableType rt)
{
	return RenderableTypeKeywords[rt];
}

// Constructor
Renderable::Renderable(Renderable::RenderableType type, const char* objectTag)
{
	// Identity
	type_ = type;
	name_ = "New Renderable";

	// Data tag
	objectTag_ = objectTag;

	// Group
	group_ = NULL;

	// Transform
	transformDataVersion_ = -1;
	transformMin_.zero();
	transformMax_.set(10.0, 10.0, 10.0);
	transformMinPositive_.set(0.1, 0.1, 0.1);
	transformMaxPositive_.set(10.0, 10.0, 10.0);
	transforms_[0].setEnabled(false);
	transforms_[1].setEnabled(false);
	transforms_[2].setEnabled(false);
	transforms_[0].setEquation("x");
	transforms_[1].setEquation("y");
	transforms_[2].setEquation("z");

	// GL
	primitivesDataVersion_ = -1;
	primitivesAxesVersion_ = -1;
	primitivesStyleVersion_ = -1;

	// Display
	visible_ = true;
	displayStyle_ = Renderable::LineXYStyle;
	displaySurfaceShininess_ = 128.0;
	displayStyleVersion_ = 0;
}

// Destructor
Renderable::~Renderable()
{
}

/*
 * Identity
 */

// Set name of Renderable
void Renderable::setName(const char* name)
{
	name_ = name;
}

// Return name of Renderable
const char* Renderable::name()
{
	return name_.get();
}

// Return type of Renderable
Renderable::RenderableType Renderable::type() const
{
	return type_;
}

/*
 * Data
 */

// Return identifying tag for source data object
const char* Renderable::objectTag() const
{
	return objectTag_.get();
}

/*
 * Transforms
 */

// Return transformed data minima, calculating if necessary
Vec3<double> Renderable::transformMin()
{
	// Make sure transformed data is up to date
	transformData();

	return transformMin_;
}

// Return transformed data maxima, calculating if necessary
Vec3<double> Renderable::transformMax()
{
	// Make sure transformed data is up to date
	transformData();

	return transformMax_;
}

// Return transformed positive data minima, calculating if necessary
Vec3<double> Renderable::transformMinPositive()
{
	// Make sure transformed data is up to date
	transformData();

	return transformMinPositive_;
}

// Return transformed positive data maxima, calculating if necessary
Vec3<double> Renderable::transformMaxPositive()
{
	// Make sure transformed data is up to date
	transformData();

	return transformMaxPositive_;
}

// Set transform equation for data
void Renderable::setTransformEquation(int axis, const char* transformEquation)
{
	transforms_[axis].setEquation(transformEquation);

	// Make sure transformed data is up to date
	if (transforms_[axis].enabled())
	{
		transformDataVersion_ = -1;
		transformData();
	}
}

// Return transform equation for data
const char* Renderable::transformEquation(int axis) const
{
	return transforms_[axis].text();
}

// Return whether specified transform equation is valid
bool Renderable::transformEquationValid(int axis) const
{
	return transforms_[axis].valid();
}

// Set whether specified transform is enabled
void Renderable::setTransformEnabled(int axis, bool enabled)
{
	transforms_[axis].setEnabled(enabled);

	// Make sure transformed data is up to date
	transformDataVersion_ = -1;
	transformData();
}

// Return whether specified transform is enabled
bool Renderable::transformEnabled(int axis) const
{
	return transforms_[axis].enabled();
}

/*
 * Group
 */

// Set group that this Renderable is associated to
void Renderable::setGroup(RenderableGroup* group)
{
	group_ = group;
}

// Return group that this Renderable is associated to
RenderableGroup* Renderable::group() const
{
	return group_;
}

/*
 * Display
 */

// Display Style Keywords
const char* DisplayStyleKeywords[] = { "LineXY", "LineZY", "Grid", "Surface", "UnlitSurface" };

// Convert text string to DisplayStyle
Renderable::DisplayStyle Renderable::displayStyle(const char* s)
{
	for (int n=0; n<nDisplayStyles; ++n) if (DissolveSys::sameString(s, DisplayStyleKeywords[n])) return (Renderable::DisplayStyle) n;
	return nDisplayStyles;
}

// Convert DisplayStyle to text string
const char* Renderable::displayStyle(Renderable::DisplayStyle kwd)
{
	return DisplayStyleKeywords[kwd];
}

// Set display style of data
void Renderable::setDisplayStyle(DisplayStyle style)
{
	displayStyle_ = style;

	++displayStyleVersion_;
}

// Return display style of data
Renderable::DisplayStyle Renderable::displayStyle() const
{
	return displayStyle_;
}

// Set basic colour
void Renderable::setColour(int r, int g, int b, int a)
{
	colour_.setSingleColour(QColor(r, g, b, a));
}

// Set basic colour
void Renderable::setColour(ColourDefinition::StockColour stockColour)
{
	colour_.setSingleColour(ColourDefinition::stockColour(stockColour));
}

// Return local colour definition for display
ColourDefinition& Renderable::colour()
{
	return colour_;
}

// Return local colour definition for display (const)
const ColourDefinition& Renderable::constColour() const
{
	return colour_;
}

// Set whether data is visible
void Renderable::setVisible(bool visible)
{
	visible_ = visible;
}

// Return whether data is visible
bool Renderable::isVisible() const
{
	return visible_;
}

// Return line style
LineStyle& Renderable::lineStyle()
{
	return lineStyle_;
}

// Set surface shininess
void Renderable::setDisplaySurfaceShininess(double shininess)
{
	displaySurfaceShininess_ = shininess;
}

// Return surface shininess
double Renderable::displaySurfaceShininess() const
{
	return displaySurfaceShininess_;
}

// Return style version
int Renderable::displayStyleVersion() const
{
	return displayStyleVersion_;
}

// Set title to display in legend
void Renderable::setTitle(const char* title)
{
	title_ = title;
}

// Return title to display in legend, if any
const char* Renderable::title() const
{
	return title_.get();
}

// Return whether a title to display in legend has been set
bool Renderable::hasTitle() const
{
	return (!title_.isEmpty());
}

/*
 * Rendering Primitives
 */

// Send primitives to GL
void Renderable::sendToGL()
{
        // If this collection is not visible return now
        if (!visible_) return;

        if (displayStyle() == Renderable::SurfaceStyle)
        {
                glEnable(GL_LIGHTING);
                glDisable(GL_LINE_SMOOTH);
        }
        else
        {
                glEnable(GL_LINE_SMOOTH);
                lineStyle().apply();
                glDisable(GL_LIGHTING);
        }

        // Send Primitives to display
        primitives_.sendToGL();
}
