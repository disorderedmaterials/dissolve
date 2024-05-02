// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/render/colourDefinition.h"
#include "gui/render/lineStyle.h"
#include "gui/render/primitiveAssembly.h"
#include "gui/render/primitiveList.h"
#include "gui/stockColours.h"
#include "math/transformer.h"

// Forward Declarations
class GenericList;
class RenderableGroup;
class View;

class Renderable
{
    public:
    // Renderable type
    enum RenderableType
    {
        ConfigurationRenderable,
        Data1DRenderable,
        Data2DRenderable,
        Data3DRenderable,
        SpeciesRenderable,
        SpeciesSiteRenderable,
        nRenderableTypes
    };
    // Return enum options for RenderableType
    static EnumOptions<RenderableType> renderableTypes();
    Renderable(RenderableType type, std::string_view tag);
    virtual ~Renderable();

    /*
     * Instances
     */
    private:
    // List of all current renderables
    static std::vector<Renderable *> instances_;

    /*
     * Identity
     */
    protected:
    // Name of Renderable
    std::string name_;
    // Type of Renderable
    RenderableType type_;

    public:
    // Set name of Renderable
    void setName(std::string_view name);
    // Return name of Renderable
    std::string_view name() const;
    // Return type of Renderable
    RenderableType type() const;

    /*
     * Data
     */
    protected:
    // Whether access to source data is currently enabled
    static bool sourceDataAccessEnabled_;
    // Identifying tag for source data object
    std::string tag_;
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

    protected:
    // Transform data values
    virtual void transformValues();

    public:
    // Attempt to set the data source, searching the supplied list for the object
    virtual void validateDataSource(const GenericList &sourceList);
    // Invalidate the current data source
    virtual void invalidateDataSource();
    // Set whether access to source data is currently enabled
    static void setSourceDataAccessEnabled(bool b);
    // Return whether access to source data is currently enabled
    static bool sourceDataAccessEnabled();
    // Return identifying tag for source data object
    std::string_view tag() const;
    // Validate all renderables
    static void validateAll(const GenericList &source);
    // Invalidate renderable data for specified object tag
    static int invalidate(std::string_view tag);
    // Invalidate all renderables
    static void invalidateAll();
    // Return version of data
    virtual int dataVersion() = 0;
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
    void setValuesTransformEquation(std::string_view transformEquation);
    // Return values transform equation
    std::string_view valuesTransformEquation() const;
    // Return whether the values transform equation is valid
    bool valuesTransformEquationValid() const;
    // Set whether values transform is enabled
    void setValuesTransformEnabled(bool enabled);
    // Return whether values transform is enabled
    bool valuesTransformEnabled() const;
    // Return data version at which values were last transformed
    int valuesTransformDataVersion() const;
    // Calculate min/max y value over specified x range (if possible in the underlying data)
    virtual bool yRangeOverX(double xMin, double xMax, double &yMin, double &yMax);

    /*
     * Group
     */
    protected:
    // Group that this Renderable is associated to (if any)
    OptionalReferenceWrapper<RenderableGroup> group_;

    public:
    // Set group that this Renderable is associated to
    void setGroup(RenderableGroup &group);
    // Remove the renderagle's group association
    void unSetGroup();
    // Return group that this Renderable is associated to
    OptionalReferenceWrapper<RenderableGroup> group() const;

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
    ColourDefinition &colour();
    const ColourDefinition &colour() const;
    // Return line style
    LineStyle &lineStyle();
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
    std::string lastColourDefinitionFingerprint_;
    // Axes version at which primitives were last created
    int lastAxesVersion_;
    // Style version at which primitives were last created
    int lastStyleVersion_;

    protected:
    // Create single Primitive, whose instances will be managed by the Renderable
    Primitive *createPrimitive(GLenum type = GL_LINES, bool colourData = false);
    // Reinitialise managed Primitive list to the size specified
    void reinitialisePrimitives(int newSize, GLenum type, bool colourData);
    // Return number of primitives managed by the Renderable
    int nPrimitives() const;
    // Return nth Primitive managed by the Renderable
    Primitive *primitive(int n);
    // Remove specified Primitive
    void removePrimitive(Primitive *primitive);
    // Recreate necessary primitives / primitive assemblies for the data
    virtual void recreatePrimitives(const View &view, const ColourDefinition &colourDefinition) = 0;
    // Send primitives for rendering
    virtual const void sendToGL(double pixelScaling) = 0;

    public:
    // Update primitives and send to display
    void updateAndSendPrimitives(const View &view, bool forceUpdate, bool pushAndPop, const QOpenGLContext *context,
                                 double pixelScaling);
};
