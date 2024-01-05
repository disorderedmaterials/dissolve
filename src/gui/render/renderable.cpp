// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/renderable.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "gui/render/renderableGroupManager.h"
#include "gui/render/view.h"

// Static Singletons
bool Renderable::sourceDataAccessEnabled_ = true;
std::vector<Renderable *> Renderable::instances_;

// Return enum options for RenderableType
EnumOptions<Renderable::RenderableType> Renderable::renderableTypes()
{
    return EnumOptions<Renderable::RenderableType>("ErrorType", {{Renderable::ConfigurationRenderable, "Configuration"},
                                                                 {Renderable::Data1DRenderable, "Data1D"},
                                                                 {Renderable::Data2DRenderable, "Data2D"},
                                                                 {Renderable::Data3DRenderable, "Data3D"},
                                                                 {Renderable::SpeciesRenderable, "Species"},
                                                                 {Renderable::SpeciesSiteRenderable, "SpeciesSite"}});
}

Renderable::Renderable(Renderable::RenderableType type, std::string_view tag)
{
    // Instance
    instances_.push_back(this);

    // Identity
    type_ = type;
    name_ = "New Renderable";

    // Data tag
    tag_ = tag;

    // Group
    group_ = std::nullopt;

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

Renderable::~Renderable()
{
    auto it = std::find(instances_.begin(), instances_.end(), this);
    instances_.erase(it);
}

/*
 * Identity
 */

// Set name of Renderable
void Renderable::setName(std::string_view name) { name_ = name; }

// Return name of Renderable
std::string_view Renderable::name() const { return name_; }

// Return type of Renderable
Renderable::RenderableType Renderable::type() const { return type_; }

/*
 * Data
 */

// Transform data values
void Renderable::transformValues() {}

// Attempt to set the data source, searching the supplied list for the object
void Renderable::validateDataSource(const GenericList &sourceList) {}

// Invalidate the current data source
void Renderable::invalidateDataSource() {}

// Set whether access to source data is currently enabled
void Renderable::setSourceDataAccessEnabled(bool b) { sourceDataAccessEnabled_ = b; }

// Return whether access to source data is currently enabled
bool Renderable::sourceDataAccessEnabled() { return sourceDataAccessEnabled_; }

// Return identifying tag for source data object
std::string_view Renderable::tag() const { return tag_; }

// Validate all renderables
void Renderable::validateAll(const GenericList &source)
{
    for (Renderable *rend : instances_)
        rend->validateDataSource(source);
}

// Invalidate renderable data for specified object tag
int Renderable::invalidate(std::string_view tag)
{
    auto count = 0;
    for (Renderable *rend : instances_)
    {
        if (tag != rend->tag_)
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

    if (valuesTransform_.enabled())
        valuesTransformDataVersion_ = -1;
}

// Return values transform equation
std::string_view Renderable::valuesTransformEquation() const { return valuesTransform_.text(); }

// Return whether values transform equation is valid
bool Renderable::valuesTransformEquationValid() const { return valuesTransform_.valid(); }

// Set whether values transform is enabled
void Renderable::setValuesTransformEnabled(bool enabled)
{
    valuesTransform_.setEnabled(enabled);

    valuesTransformDataVersion_ = -1;
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
void Renderable::setGroup(RenderableGroup &group) { group_ = group; }

// Remove the renderagle's group association
void Renderable::unSetGroup() { group_ = std::nullopt; }

// Return group that this Renderable is associated to
OptionalReferenceWrapper<RenderableGroup> Renderable::group() const { return group_; }

/*
 * Style
 */

// Set whether Renderable is visible
void Renderable::setVisible(bool visible) { visible_ = visible; }

// Return whether Renderable is visible
bool Renderable::isVisible() const
{
    // Group visibility overrides our own (*if* we are currently visible)...
    return visible_ && (group_ ? group_->get().isVisible() : visible_);
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

const ColourDefinition &Renderable::colour() const { return colour_; }

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
    const Axes &axes = view.axes();

    // Grab copy of the relevant colour definition for this Renderable
    const ColourDefinition &colourDefinition = colour();

    // If the primitive is out of date, recreate it
    if (forceUpdate || lastAxesVersion_ != axes.version() || lastDataVersion_ != dataVersion() ||
        valuesTransformDataVersion_ != dataVersion() || lastStyleVersion_ != styleVersion() ||
        !DissolveSys::sameString(lastColourDefinitionFingerprint_,
                                 fmt::format("{}@{}", group_ ? group_->get().name() : "NoGroup", colourDefinition.version()),
                                 true))
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
    lastColourDefinitionFingerprint_ =
        fmt::format("{}@{}", group_ ? group_->get().name() : "NoGroup", colourDefinition.version());
    lastDataVersion_ = dataVersion();
    lastStyleVersion_ = styleVersion();
}
