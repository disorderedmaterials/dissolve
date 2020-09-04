/*
    *** Renderable Data
    *** src/gui/view/render/renderable.cpp
    Copyright T. Youngs 2013-2020

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

#include "gui/render/renderable.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "gui/render/renderablegroupmanager.h"
#include "gui/render/view.h"
#include "math/interpolator.h"
#include <limits>

// Static Singletons
bool Renderable::sourceDataAccessEnabled_ = true;
RefList<Renderable> Renderable::instances_;

// Return enum options for RenderableType
EnumOptions<Renderable::RenderableType> Renderable::renderableTypes()
{
    static EnumOptionsList RenderableTypeOptions = EnumOptionsList()
                                                   << EnumOption(Renderable::ConfigurationRenderable, "Configuration")
                                                   << EnumOption(Renderable::Data1DRenderable, "Data1D")
                                                   << EnumOption(Renderable::Data2DRenderable, "Data2D")
                                                   << EnumOption(Renderable::Data3DRenderable, "Data3D")
                                                   << EnumOption(Renderable::SpeciesRenderable, "Species")
                                                   << EnumOption(Renderable::SpeciesSiteRenderable, "SpeciesSite");

    static EnumOptions<Renderable::RenderableType> options("ErrorType", RenderableTypeOptions);

    return options;
}

Renderable::Renderable(Renderable::RenderableType type, std::string_view objectTag)
{
    // Instance
    instances_.append(this);

    // Identity
    type_ = type;
    name_ = "New Renderable";

    // Data tag
    objectTag_ = objectTag;

    // Group
    group_ = nullptr;

    // Transform
    valuesTransformDataVersion_ = -1;
    limitsMin_.zero();
    limitsMax_.set(10.0, 10.0, 10.0);
    positiveLimitsMin_.set(0.1, 0.1, 0.1);
    positiveLimitsMax_.set(10.0, 10.0, 10.0);
    valuesMin_ = 0.0;
    valuesMax_ = 10.0;
    positiveValuesMin_ = 0.0;
    positiveValuesMax_ = 10.0;
    valuesTransform_.setEnabled(false);
    valuesTransform_.setEquation("value");

    // Rendering Versions
    lastDataVersion_ = -1;
    lastAxesVersion_ = -1;
    lastStyleVersion_ = -1;

    // Display
    visible_ = true;
    styleVersion_ = 0;
}

Renderable::~Renderable() { instances_.remove(this); }

/*
 * Identity
 */

// Set name of Renderable
void Renderable::setName(std::string_view name) { name_ = name; }

// Return name of Renderable
std::string_view Renderable::name() { return name_; }

// Return type of Renderable
Renderable::RenderableType Renderable::type() const { return type_; }

/*
 * Data
 */

// Set whether access to source data is currently enabled
void Renderable::setSourceDataAccessEnabled(bool b) { sourceDataAccessEnabled_ = b; }

// Return whether access to source data is currently enabled
bool Renderable::sourceDataAccessEnabled() { return sourceDataAccessEnabled_; }

// Return identifying tag for source data object
std::string_view Renderable::objectTag() const { return objectTag_; }

// Invalidate renderable data for specified object tag
int Renderable::invalidate(std::string_view objectTag)
{
    auto count = 0;
    for (Renderable *rend : instances_)
    {
        if (objectTag != rend->objectTag_)
            continue;

        rend->invalidateDataSource();

        ++count;
    }
    return count;
}

// Invalidate all renderables
void Renderable::invalidateAll()
{
    for (Renderable *rend : instances_)
        rend->invalidateDataSource();
}

// Return coordinate minima of all data (after value transform if enabled)
Vec3<double> Renderable::limitsMin()
{
    // Make sure transformed values are up to date
    transformValues();

    return limitsMin_;
}

// Return coordinate maxima of all data (after value transform if enabled)
Vec3<double> Renderable::limitsMax()
{
    // Make sure transformed values are up to date
    transformValues();

    return limitsMax_;
}

// Return positive coordinate minima of all data (after value transform if enabled)
Vec3<double> Renderable::positiveLimitsMin()
{
    // Make sure transformed values are up to date
    transformValues();

    return positiveLimitsMin_;
}

// Return positive coordinate maxima of all data (after value transform if enabled)
Vec3<double> Renderable::positiveLimitsMax()
{
    // Make sure transformed values are up to date
    transformValues();

    return positiveLimitsMax_;
}

// Return minimum of transformed values
double Renderable::valuesMin()
{
    // Make sure transformed values are up to date
    transformValues();

    return valuesMin_;
}

// Return maximum of transformed values
double Renderable::valuesMax()
{
    // Make sure transformed values are up to date
    transformValues();

    return valuesMax_;
}

// Return minimum positive of transformed values
double Renderable::positiveValuesMin()
{
    // Make sure transformed values are up to date
    transformValues();

    return positiveValuesMin_;
}

// Return maximum positive of transformed values
double Renderable::positiveValuesMax()
{
    // Make sure transformed values are up to date
    transformValues();

    return positiveValuesMax_;
}

// Set values transform equation specified
void Renderable::setValuesTransformEquation(std::string_view transformEquation)
{
    valuesTransform_.setEquation(transformEquation);

    // Make sure transformed data is up to date
    if (valuesTransform_.enabled())
    {
        valuesTransformDataVersion_ = -1;
        transformValues();
    }
}

// Return values transform equation
std::string_view Renderable::valuesTransformEquation() const { return valuesTransform_.text(); }

// Return whether values transform equation is valid
bool Renderable::valuesTransformEquationValid() const { return valuesTransform_.valid(); }

// Set whether values transform is enabled
void Renderable::setValuesTransformEnabled(bool enabled)
{
    valuesTransform_.setEnabled(enabled);

    // Make sure transformed data is up to date
    valuesTransformDataVersion_ = -1;
    transformValues();
}

// Return whether values transform is enabled
bool Renderable::valuesTransformEnabled() const { return valuesTransform_.enabled(); }

// Return data version at which values were last transformed
int Renderable::valuesTransformDataVersion() const { return valuesTransformDataVersion_; }

// Calculate min/max y value over specified x range (if possible in the underlying data)
bool Renderable::yRangeOverX(double xMin, double xMax, double &yMin, double &yMax) { return false; }

/*
 * Group
 */

// Set group that this Renderable is associated to
void Renderable::setGroup(RenderableGroup *group) { group_ = group; }

// Return group that this Renderable is associated to
RenderableGroup *Renderable::group() const { return group_; }

/*
 * Style
 */

// Set whether Renderable is visible
void Renderable::setVisible(bool visible) { visible_ = visible; }

// Return whether Renderable is visible
bool Renderable::isVisible() const
{
    // Group visibility overrides our own (*if* we are currently visible)...
    return (visible_ ? (group_ ? group_->isVisible() : visible_) : false);
}

// Set basic colour
void Renderable::setColour(int r, int g, int b, int a) { colour_.setSingleColour(QColor(r, g, b, a)); }

// Set basic colour
void Renderable::setColour(StockColours::StockColour stockColour)
{
    colour_.setSingleColour(StockColours::stockColour(stockColour));
}

// Return local colour definition for display
ColourDefinition &Renderable::colour() { return colour_; }

// Return local colour definition for display (const)
const ColourDefinition &Renderable::constColour() const { return colour_; }

// Return line style
LineStyle &Renderable::lineStyle() { return lineStyle_; }

// Return style version
int Renderable::styleVersion() const { return styleVersion_; }

/*
 * Rendering Primitives
 */

// Create single Primitive, whose instances will be managed by the Renderable
Primitive *Renderable::createPrimitive(GLenum type, bool colourData) { return primitives_.add(type, colourData); }

// Reinitialise managed Primitive list to the size specified
void Renderable::reinitialisePrimitives(int newSize, GLenum type, bool colourData)
{
    primitives_.reinitialise(newSize, type, colourData);
}

// Return number of primitives managed by the Renderable
int Renderable::nPrimitives() const { return primitives_.nPrimitives(); }

// Return nth Primitive managed by the Renderable
Primitive *Renderable::primitive(int n) { return primitives_[n]; }

// Remove specified Primitive
void Renderable::removePrimitive(Primitive *primitive) { primitives_.remove(primitive); }

// Update primitives and send to display
void Renderable::updateAndSendPrimitives(const View &view, bool forceUpdate, bool pushAndPop, const QOpenGLContext *context,
                                         double pixelScaling)
{
    // If this Renderable is not visible, return now
    if (!visible_)
        return;

    // Grab axes for the View
    const Axes &axes = view.constAxes();

    // Grab copy of the relevant colour definition for this Renderable
    const ColourDefinition &colourDefinition = colour();

    // Check whether the primitive for this Renderable needs updating
    auto upToDate = true;
    if (forceUpdate)
        upToDate = false;
    else if (lastAxesVersion_ != axes.version())
        upToDate = false;
    else if (!DissolveSys::sameString(lastColourDefinitionFingerprint_,
                                      fmt::format("{}@{}", fmt::ptr(group_), colourDefinition.version()), true))
        upToDate = false;
    else if (lastDataVersion_ != dataVersion())
        upToDate = false;
    else if (lastStyleVersion_ != styleVersion())
        upToDate = false;

    // If the primitive is out of date, recreate it's data.
    if (!upToDate)
    {
        // Recreate Primitives for the underlying data
        recreatePrimitives(view, colourDefinition);

        // Pop old Primitive instance (if they exist)
        if (primitives_.nInstances() != 0)
            primitives_.popInstance(context);
    }

    // If there are no current instances, or we are forcing a push/pop of an instance, push an instance here
    if ((primitives_.nInstances() == 0) || pushAndPop)
        primitives_.pushInstance(context);

    // Send to GL
    sendToGL(pixelScaling);

    // Pop current instances if required
    if (pushAndPop)
        primitives_.popInstance(context);

    // Store version points for the up-to-date primitive
    lastAxesVersion_ = axes.version();
    lastColourDefinitionFingerprint_ = fmt::format("{}@{}", fmt::ptr(group_), colourDefinition.version());
    lastDataVersion_ = dataVersion();
    lastStyleVersion_ = styleVersion();
}
