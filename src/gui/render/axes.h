// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/version.h"
#include "gui/render/lineStyle.h"
#include "gui/render/numberFormat.h"
#include "gui/render/primitive.h"
#include "gui/render/textPrimitiveList.h"
#include "templates/vector3.h"
#include "templates/vector4.h"
#include <vector>

// Forward Declarations
class View;

// Axes
class Axes
{
    private:
    // Parent rendering View in which these axes are displayed
    View &parentView_;
    // Associated FontInstance from parent viewer
    FontInstance &fontInstance_;

    public:
    Axes(View &parent, FontInstance &fontInstance);
    ~Axes();

    /*
     * Definition
     */
    public:
    // Autoscaling for axes
    enum AutoScaleMethod
    {
        NoAutoScale,
        ExpandingAutoScale,
        FullAutoScale,
        nAutoScaleMethods
    };
    // Return enum options for AutoScaleMethod
    static EnumOptions<Axes::AutoScaleMethod> autoScaleMethods();

    private:
    // Data limits for surface generation
    Vec3<double> min_, max_;
    // Limiting values for axis limits (accounting for data, log scales etc.)
    Vec3<double> limitMin_, limitMax_;
    // Axis extreme coordinates
    Vec3<double> coordMin_[3], coordMax_[3];
    // Central coordinate of current axes
    Vec3<double> coordCentre_;
    // Whether to invert axes
    Vec3<bool> inverted_;
    // Whether axes should be plotted as logarithms
    Vec3<bool> logarithmic_;
    // Axis visibility
    Vec3<bool> visible_;
    // Stretch factors to apply to axes
    Vec3<double> stretch_;
    // Whether axis position is fractional or absolute
    Vec3<bool> positionIsFractional_;
    // Axis position in real or surface-space coordinates
    Vec3<double> positionReal_[3];
    // Axis position in fractional axis coordinates
    Vec3<double> positionFractional_[3];
    // Autoscaling method employed for each axis
    Axes::AutoScaleMethod autoScale_[3];

    private:
    // Recalculate minimum, maximum, and centre coordinates of axes
    void updateCoordinates();
    // Clamp axis position and min/max to current limits
    void clamp(int axis);

    public:
    // Set minimum value for specified axis
    void setMin(int axis, double value);
    // Return minimum value for specified axis
    double min(int axis) const;
    // Set maximum value for specified axis
    void setMax(int axis, double value);
    // Return maximum value for specified axis
    double max(int axis) const;
    // Set range of specified axis
    void setRange(int axis, double minValue, double maxValue);
    // Return real axis range (accounting for log axes)
    double realRange(int axis) const;
    // Ensure a sensible (non-zero) range, modifying the supplied values
    static void ensureSensibleRange(double &minValue, double &maxValue, bool alwaysExpand = false,
                                    double expansionFactor = -1.0);
    // Return central value of axes
    Vec3<double> centre() const;
    // Return real axis minimum (accounting for log axes)
    double realMin(int axis) const;
    // Return real axis maximum (accounting for log axes)
    double realMax(int axis) const;
    // Set axis to extreme limit
    void setToLimit(int axis, bool minimumLimit);
    // Set axis minimum limit for specified axis
    void setLimitMin(int axis, double limit);
    // Return axis minimum limit for specified axis
    double limitMin(int axis) const;
    // Set axis maximum limit for specified axis
    void setLimitMax(int axis, double limit);
    // Return axis maximum limit for specified axis
    double limitMax(int axis) const;
    // Set all axis limits at once
    void expandLimits(bool noShrink);
    // Return coordinate at centre of axes
    Vec3<double> coordCentre() const;
    // Return coordinate at minimum of specified axis
    Vec3<double> coordMin(int axis) const;
    // Return coordinate at maximum of specified axis
    Vec3<double> coordMax(int axis) const;
    // Set whether axis is inverted
    void setInverted(int axis, bool b);
    // Return whether axis is inverted
    bool inverted(int axis) const;
    // Set whether axis is logarithmic
    void setLogarithmic(int axis, bool b);
    // Toggle whether axis is logarithmic
    void toggleLogarithmic(int axis);
    // Return whether axis is logarithmic
    bool logarithmic(int axis) const;
    // Set whether axis is visible
    void setVisible(int axis, bool b);
    // Return whether specified axis is visible
    bool visible(int axis) const;
    // Set stretch factor for axis
    void setStretch(int axis, double value);
    // Return stretch factor for axis
    double stretch(int axis) const;
    // Set fractional position flag for axis
    void setPositionIsFractional(int axis, bool b);
    // Return fractional position flag for axis
    bool positionIsFractional(int axis) const;
    // Set axis position (in real surface-space coordinates)
    void setPositionReal(int axis, int dir, double value);
    // Set axis position to axis limit (in real surface-space coordinates)
    void setPositionRealToLimit(int axis, int dir, bool minLim);
    // Return axis position (in real surface-space coordinates)
    Vec3<double> positionReal(int axis) const;
    // Set axis position (in fractional axis coordinates)
    void setPositionFractional(int axis, int dir, double value);
    // Return axis position (in fractional axis coordinates)
    Vec3<double> positionFractional(int axis) const;
    // Set autoscaling method employed for specified axis
    void setAutoScale(int axis, Axes::AutoScaleMethod method);
    // Return autoscaling method employed for specified axis
    Axes::AutoScaleMethod autoScale(int axis) const;

    /*
     * Data Transforms
     */
    public:
    // Return supplied data x value in local axes coordinates
    double transformX(double x) const;
    // Transform entire array of values into local axes coordinates
    void transformX(std::vector<double> &xArray) const;
    // Return supplied data y value in local axes coordinates
    double transformY(double y) const;
    // Transform entire array of values into local axes coordinates
    void transformY(std::vector<double> &yArray) const;
    // Return supplied data z value in local axes coordinates
    double transformZ(double z) const;
    // Transform entire array of values into local axes coordinates
    void transformZ(std::vector<double> &zArray) const;
    // Transform a 2D array of values into local axes coordinates
    void transformX(Array2D<double> &xArray) const;
    // Transform a 2D array of values into local axes coordinates
    void transformY(Array2D<double> &yArray) const;
    // Transform a 2D array of values into local axes coordinates
    void transformZ(Array2D<double> &zArray) const;

    /*
     * Ticks
     */
    private:
    // Axis tick direction
    Vec3<double> tickDirection_[3];
    // Axis tick size (relative to font size)
    Vec3<double> tickSize_;
    // Position of first tick delta on axes
    Vec3<double> tickFirst_;
    // Tick delta for axes
    Vec3<double> tickDelta_;
    // Whether to calculate ticks automatically
    Vec3<bool> autoTicks_;
    // Number of minor ticks in major tick intervals
    Vec3<int> minorTicks_;

    private:
    // Recalculate tick deltas for specified axis
    void calculateTickDeltas(int axis);

    public:
    // Set axis tick direction
    void setTickDirection(int axis, int dir, double value);
    // Return axis tick direction for specified axis
    Vec3<double> tickDirection(int axis) const;
    // Set axis tick size (relative to font size)
    void setTickSize(int axis, double size);
    // Return axis tick size (relative to font size)
    double tickSize(int axis) const;
    // Set position of first tick delta on specified axis
    void setFirstTick(int axis, double value);
    // Return position of first tick delta on specified axis
    double tickFirst(int axis) const;
    // Set tick delta for specified axis
    void setTickDelta(int axis, double value);
    // Return tick delta for specified axis
    double tickDelta(int axis) const;
    // Set whether to calculate ticks automatically for specified axis
    void setAutoTicks(int axis, bool b);
    // Return whether to calculate ticks automatically for specified axis
    bool autoTicks(int axis) const;
    // Set number of minor ticks in major tick intervals for specified axis
    void setMinorTicks(int axis, int value);
    // Return number of minor ticks in major tick intervals for specified axis
    int minorTicks(int axis) const;

    /*
     * Labels
     */
    private:
    // Number formats for labels
    NumberFormat numberFormat_[3];
    // Whether to determine number formats automatically
    Vec3<bool> autoNumberFormat_;
    // Orientation of axis labels (axial rot, in-plane rot, distance)
    Vec3<double> labelOrientation_[3];
    // Axis label text anchor positions
    TextPrimitive::TextAnchor labelAnchor_[3];
    // Axis titles
    std::string title_[3];
    // Orientation of axis titles (rotations around x, y, and z axes)
    Vec3<double> titleOrientation_[3];
    // Title distances from axes
    Vec3<double> titleDistances_;
    // Title horizontal offsets
    Vec3<double> titleHorizontalOffsets_;
    // Axis title text anchor positions
    TextPrimitive::TextAnchor titleAnchor_[3];

    private:
    // Determine suitable label format for the supplied axis
    void determineLabelFormat(int axis);

    public:
    // Return number format for specified axis
    const NumberFormat &numberFormat(int axis) const;
    // Set number format for specified axis
    void setNumberFormat(int axis, const NumberFormat &numberFormat);
    // Return whether to determine number format automatically for the specified axis
    bool autoNumberFormat(int axis) const;
    // Set whether to determine number format automatically for the specified axis
    void setAutoNumberFormat(int axis, bool b);
    // Set orientation of labels for specified axis
    void setLabelOrientation(int axis, int component, double value);
    // Return orientation of labels for specified axis
    Vec3<double> labelOrientation(int axis) const;
    // Set axis label text anchor position for specified axis
    void setLabelAnchor(int axis, TextPrimitive::TextAnchor anchor);
    // Return axis label text anchor position for specified axis
    TextPrimitive::TextAnchor labelAnchor(int axis) const;
    // Set title for specified axis
    void setTitle(int axis, std::string_view title);
    // Return title for specified axis
    std::string_view title(int axis) const;
    // Set orientation of titles for specified axis
    void setTitleOrientationNEW(int axis, int component, double value);
    // Return orientation of titles for specified axis
    Vec3<double> titleOrientation(int axis) const;
    // Set title distance from axis
    void setTitleDistance(int axis, double distance);
    // Return title distance from axis
    double titleDistance(int axis) const;
    // Set title horizontal offset
    void setTitleHorizontalOffset(int axis, double offset);
    // Return title horizontal offset
    double titleHorizontalOffset(int axis) const;
    // Set axis title text anchor position for specified axis
    void setTitleAnchor(int axis, TextPrimitive::TextAnchor anchor);
    // Return axis title text anchor position for specified axis
    TextPrimitive::TextAnchor titleAnchor(int axis) const;

    /*
     * Style Helpers
     */
    private:
    // Whether to use best tick/label orientation for flat views, or the user's definitions
    bool useBestFlatView_;
    // Whether to automatically place titles at a sensible position after label text
    bool autoPositionTitles_;

    public:
    // Set whether to use best tick/label orientation for view
    void setUseBestFlatView(bool b);
    // Return whether to use best tick/label orientation for view
    bool useBestFlatView() const;
    // Set whether to automatically place titles at a sensible position after label text
    void setAutoPositionTitles(bool b);
    // Return whether to automatically place titles at a sensible position after label text
    bool autoPositionTitles() const;

    /*
     * GridLines
     */
    private:
    // Whether gridlines cover entire volume or just at axis lines
    Vec3<bool> gridLinesFull_;
    // Whether gridLines at major tick intervals are active
    Vec3<bool> gridLinesMajor_;
    // Whether gridLines at minor tick intervals are active
    Vec3<bool> gridLinesMinor_;

    public:
    // Set whether gridlines cover entire volume or just at axis lines
    void setGridLinesFull(int axis, bool b);
    // Return whether gridlines cover entire volume or just at axis lines
    bool gridLinesFull(int axis) const;
    // Set whether gridLines at major tick intervals are active for specified axis
    void setGridLinesMajor(int axis, bool on);
    // Return whether gridLines at major tick intervals are active for specified axis
    bool gridLinesMajor(int axis) const;
    // Set whether gridLines at minor tick intervals are active for specified axis
    void setGridLinesMinor(int axis, bool on);
    // Return whether gridLines at minor tick intervals are active for specified axis
    bool gridLinesMinor(int axis) const;

    /*
     * Versions
     */
    private:
    // Version of axes definitions
    VersionCounter version_;

    public:
    // Return version of axes definitions
    int version() const;

    /*
     * GL
     */
    private:
    // Clip plane coordinates
    Vec3<double> clipMin_, clipMax_;
    // Display primitives
    Primitive axisPrimitives_[3];
    // Axis text primitives
    TextPrimitiveList labelPrimitives_[3];
    // Axis title primitives
    TextPrimitiveList titlePrimitives_[3];
    // GridLine primitives
    Primitive gridLineMajorPrimitives_[3], gridLineMinorPrimitives_[3];
    // GridLine styles
    LineStyle gridLineMajorStyle_[3], gridLineMinorStyle_[3];
    // Versions at which primitives were last generated
    int primitiveVersion_;

    private:
    // Update primitives for axis
    void updateAxisPrimitives();

    public:
    // Return clip plane lower Y value
    Vec3<double> clipMin() const;
    // Return clip plane upper Y value
    Vec3<double> clipMax() const;
    // Flag primitives as invalid
    void setPrimitivesInvalid();
    // Return axis primitive for axis specified
    Primitive &axisPrimitive(int axis);
    // Return axis label primitive list for axis specified
    TextPrimitiveList &labelPrimitive(int axis);
    // Return axis title primitive list for axis specified
    TextPrimitiveList &titlePrimitive(int axis);
    // Return minor gridline primitive for axis specified
    Primitive &gridLineMajorPrimitive(int axis);
    // Return major gridline primitive for axis specified
    Primitive &gridLineMinorPrimitive(int axis);
    // Set major gridline style
    void setGridLineMajorStyle(int axis, const LineStyle &style);
    // Return major gridline style
    const LineStyle &gridLineMajorStyle(int axis) const;
    // Set minor gridline style
    void setGridLineMinorStyle(int axis, const LineStyle &style);
    // Return minor gridline style
    const LineStyle &gridLineMinorStyle(int axis) const;
};
