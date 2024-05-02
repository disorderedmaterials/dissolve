// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/axes.h"
#include "base/sysFunc.h"
#include "gui/render/fontInstance.h"
#include "gui/render/view.h"
#include "math/doubleExp.h"

Axes::Axes(View &parent, FontInstance &fontInstance) : parentView_(parent), fontInstance_(fontInstance)
{
    // Definition
    limitMin_.zero();
    limitMax_.set(10.0, 10.0, 10.0);
    min_.zero();
    max_.set(10.0, 10.0, 10.0);
    inverted_.set(false, false, false);
    logarithmic_.set(false, false, false);
    visible_.set(true, true, true);
    positionIsFractional_.set(true, true, true);
    positionReal_[0].zero();
    positionReal_[1].zero();
    positionReal_[2].zero();
    positionFractional_[0].zero();
    positionFractional_[1].zero();
    positionFractional_[2].zero();
    coordMin_[0].zero();
    coordMin_[1].zero();
    coordMin_[2].zero();
    coordMax_[0].zero();
    coordMax_[1].zero();
    coordMax_[2].zero();
    coordCentre_.set(5.0, 5.0, 5.0);
    stretch_.set(1.0, 1.0, 1.0);
    autoScale_[0] = Axes::NoAutoScale;
    autoScale_[1] = Axes::NoAutoScale;
    autoScale_[2] = Axes::NoAutoScale;

    // Ticks
    tickDirection_[0].set(0.0, -1.0, 0.0);
    tickDirection_[1].set(-1.0, 0.0, 0.0);
    tickDirection_[2].set(-1.0, 0.0, 0.0);
    tickSize_.set(0.2, 0.2, 0.2);
    tickFirst_.zero();
    tickDelta_.set(1.0, 1.0, 1.0);
    autoTicks_.set(true, true, true);
    minorTicks_.set(1, 1, 1);

    // Labels
    autoNumberFormat_.set(true, true, true);
    labelAnchor_[0] = TextPrimitive::TopMiddleAnchor;
    labelAnchor_[1] = TextPrimitive::MiddleRightAnchor;
    labelAnchor_[2] = TextPrimitive::MiddleRightAnchor;
    labelOrientation_[0].set(0.0, 0.0, 0.2);
    labelOrientation_[1].set(0.0, 0.0, 0.2);
    labelOrientation_[2].set(90.0, 0.0, 0.2);
    title_[0] = "X Axis";
    title_[1] = "Y Axis";
    title_[2] = "Z Axis";
    titleOrientation_[0].set(0.0, 0.0, 0.0);
    titleOrientation_[1].set(0.0, 0.0, 270.0);
    titleOrientation_[2].set(90.0, 270.0, 0.0);
    titleDistances_.set(1.2, 1.2, 1.2);
    titleHorizontalOffsets_.set(0.5, 0.5, 0.5);
    titleAnchor_[0] = TextPrimitive::TopMiddleAnchor;
    titleAnchor_[1] = TextPrimitive::BottomMiddleAnchor;
    titleAnchor_[2] = TextPrimitive::BottomMiddleAnchor;

    // GridLines
    gridLinesMajor_.set(false, false, false);
    gridLinesMinor_.set(false, false, false);

    // Style override
    useBestFlatView_ = false;
    autoPositionTitles_ = true;

    // GL
    for (auto n = 0; n < 3; ++n)
    {
        axisPrimitives_[n].initialise(GL_LINES, true);
        axisPrimitives_[n].setNoInstances();
        gridLineMajorPrimitives_[n].initialise(GL_LINES, false);
        gridLineMajorPrimitives_[n].setNoInstances();
        gridLineMinorPrimitives_[n].initialise(GL_LINES, false);
        gridLineMinorPrimitives_[n].setNoInstances();
    }
    clipMin_.set(0.0, 0.0, 0.0);
    clipMax_.set(0.0, 0.0, 0.0);
    gridLineMajorStyle_[0].set(1.0, LineStipple::NoStipple, 0.5, 0.5, 0.5, 1.0);
    gridLineMinorStyle_[0].set(1.0, LineStipple::QuarterDashStipple, 0.75, 0.75, 0.75, 1.0);
    gridLineMajorStyle_[1] = gridLineMajorStyle_[0];
    gridLineMinorStyle_[1] = gridLineMinorStyle_[0];
    gridLineMajorStyle_[2] = gridLineMajorStyle_[0];
    gridLineMinorStyle_[2] = gridLineMinorStyle_[0];
    primitiveVersion_ = -1;
}

Axes::~Axes() {}

/*
 * Definition
 */

// Return enum options for AutoScaleMethod
EnumOptions<Axes::AutoScaleMethod> Axes::autoScaleMethods()
{
    return EnumOptions<Axes::AutoScaleMethod>(
        "AutoScaleMethod",
        {{Axes::NoAutoScale, "None"}, {Axes::ExpandingAutoScale, "Expanding"}, {Axes::FullAutoScale, "Full"}});
}

// Recalculate minimum, maximum, and centre coordinates of axes
void Axes::updateCoordinates()
{
    // Loop over axes
    for (auto axis = 0; axis < 3; ++axis)
    {
        // Determine central coordinate component
        if (logarithmic_[axis])
            coordCentre_[axis] =
                (inverted_[axis] ? log10(max_[axis] / min_[axis]) : log10(max_[axis] * min_[axis])) * 0.5 * stretch_[axis];
        else
            coordCentre_[axis] = (max_[axis] + min_[axis]) * 0.5 * stretch_[axis];

        // Set axis position along other directions
        for (auto n = 0; n < 3; ++n)
        {
            // Get axis position
            double position = (positionIsFractional_[axis] ? positionFractional_[axis][n] * (max_[n] - min_[n]) + min_[n]
                                                           : positionReal_[axis][n]);
            if (logarithmic_[n])
                coordMin_[axis].set(n, (inverted_[n] ? log10(max_[n] / position) : log10(position)) * stretch_[n]);
            else
                coordMin_[axis].set(n, (inverted_[n] ? max_[n] - position + min_[n] : position) * stretch_[n]);
        }
        coordMax_[axis] = coordMin_[axis];

        // Set axis min/max coordinates
        if (logarithmic_[axis])
        {
            coordMin_[axis].set(axis, (inverted_[axis] ? log10(max_[axis] / min_[axis]) : log10(min_[axis])) * stretch_[axis]);
            coordMax_[axis].set(axis, (inverted_[axis] ? log10(max_[axis] / max_[axis]) : log10(max_[axis])) * stretch_[axis]);
        }
        else
        {
            coordMin_[axis].set(axis, (inverted_[axis] ? max_[axis] : min_[axis]) * stretch_[axis]);
            coordMax_[axis].set(axis, (inverted_[axis] ? min_[axis] : max_[axis]) * stretch_[axis]);
        }
    }

    parentView_.updateViewMatrix();
}

// Clamp axis position and min/max to current limits if necessary
void Axes::clamp(int axis)
{
    if (logarithmic_[axis])
    {
        if (min_[axis] <= 0.0)
            setToLimit(axis, true);
    }
}

// Set minimum value for specified axis
void Axes::setMin(int axis, double value)
{
    min_[axis] = value;

    updateCoordinates();

    ++version_;
}

// Return minimum value for specified axis
double Axes::min(int axis) const { return min_.get(axis); }

// Set maximum value for specified axis
void Axes::setMax(int axis, double value)
{
    max_[axis] = value;

    updateCoordinates();

    ++version_;
}

// Return maximum value for specified axis
double Axes::max(int axis) const { return max_.get(axis); }

// Set range of specified axis
void Axes::setRange(int axis, double minValue, double maxValue)
{
    min_[axis] = minValue;
    max_[axis] = maxValue;

    updateCoordinates();

    ++version_;
}

// Return real axis range (accounting for log axes)
double Axes::realRange(int axis) const
{
    if (logarithmic_.get(axis))
        return log10(max_.get(axis)) - log10(min_.get(axis));
    else
        return max_.get(axis) - min_.get(axis);
}

// Ensure a sensible (non-zero) range, modifying the supplied values
void Axes::ensureSensibleRange(double &minValue, double &maxValue, bool alwaysExpand, double expansionFactor)
{
    /*
     * If exponents are different or the difference in the significands is large enough, we consider the range to be
     * acceptable, and only expand the range if 'alwaysExpand' is true. The degree to which we expand the range is governed
     * by the 'expansionFactor'. If positive this represents a fractional amount of the total range that we will increase
     * by. If negative, this represents an absolute value by which we will increase the range.
     */
    DoubleExp minimum(minValue), maximum(maxValue);
    if (alwaysExpand ||
        ((minimum.exponent() == maximum.exponent()) && (fabs(minimum.mantissa() - maximum.mantissa()) < 1.0e-4)))
    {
        if (expansionFactor > 0.0)
        {
            double yDelta = (maxValue - minValue) * expansionFactor;
            maxValue += yDelta;
            minValue -= yDelta;
        }
        else
        {
            maxValue += fabs(expansionFactor);
            minValue -= fabs(expansionFactor);
        }
    }
}

// Return central value of axes
Vec3<double> Axes::centre() const { return min_ + (max_ - min_) * 0.5; }

// Return real axis minimum (accounting for log axes)
double Axes::realMin(int axis) const { return (logarithmic_.get(axis) ? log10(min_.get(axis)) : min_.get(axis)); }

// Return real axis maximum (accounting for log axes)
double Axes::realMax(int axis) const { return (logarithmic_.get(axis) ? log10(max_.get(axis)) : max_.get(axis)); }

// Set axis to extreme limit
void Axes::setToLimit(int axis, bool minLim)
{
    if (minLim)
        min_[axis] = limitMin_[axis];
    else
        max_[axis] = limitMax_[axis];

    updateCoordinates();

    ++version_;
}

// Set axis minimum limit for specified axis
void Axes::setLimitMin(int axis, double limit)
{
    limitMin_[axis] = limit;

    if ((autoScale_[axis] == Axes::ExpandingAutoScale) && (min_[axis] > limitMin_[axis]))
        setToLimit(axis, true);
    else if (autoScale_[axis] == Axes::FullAutoScale)
        setToLimit(axis, true);

    clamp(axis);

    updateCoordinates();

    ++version_;
}

// Return axis minimum limit for specified axis
double Axes::limitMin(int axis) const { return limitMin_.get(axis); }

// Set axis maximum limit for specified axis
void Axes::setLimitMax(int axis, double limit)
{
    limitMax_[axis] = limit;

    if ((autoScale_[axis] == Axes::ExpandingAutoScale) && (max_[axis] < limitMax_[axis]))
        setToLimit(axis, false);
    else if (autoScale_[axis] == Axes::FullAutoScale)
        setToLimit(axis, false);

    clamp(axis);

    updateCoordinates();

    ++version_;
}

// Return axis maximum limit for specified axis
double Axes::limitMax(int axis) const { return limitMax_.get(axis); }

// Set all axis limits at once
void Axes::expandLimits(bool noShrink)
{
    for (auto axis = 0; axis < 3; ++axis)
    {
        if ((min_[axis] > limitMin_[axis]) || (!noShrink))
            setToLimit(axis, true);
        if ((max_[axis] < limitMax_[axis]) || (!noShrink))
            setToLimit(axis, false);
    }
}

// Return coordinate at centre of current axes
Vec3<double> Axes::coordCentre() const { return coordCentre_; }

// Return coordinate at minimum of specified axis
Vec3<double> Axes::coordMin(int axis) const { return coordMin_[axis]; }

// Return coordinate at maximum of specified axis
Vec3<double> Axes::coordMax(int axis) const { return coordMax_[axis]; }

// Set whether axis is inverted
void Axes::setInverted(int axis, bool b)
{
    inverted_[axis] = b;

    updateCoordinates();

    ++version_;
}

// Return whether axis is inverted
bool Axes::inverted(int axis) const { return inverted_.get(axis); }

// Set whether axis is logarithmic
void Axes::setLogarithmic(int axis, bool b)
{
    logarithmic_[axis] = b;

    // Update minima / maxima
    parentView_.updateAxisLimits();

    // Update and clamp axis values according to data
    clamp(axis);

    updateCoordinates();

    ++version_;

    parentView_.recalculateView();
}

// Toggle whether axis is logarithmic
void Axes::toggleLogarithmic(int axis) { setLogarithmic(axis, !logarithmic_[axis]); }

// Return whether axis is logarithmic
bool Axes::logarithmic(int axis) const { return logarithmic_.get(axis); }

// Set whether axis is visible
void Axes::setVisible(int axis, bool b) { visible_[axis] = b; }

// Return whether specified axis is visible
bool Axes::visible(int axis) const { return visible_.get(axis); }

// Set stretch factor for axis
void Axes::setStretch(int axis, double value)
{
    stretch_[axis] = value;

    updateCoordinates();

    ++version_;
}

// Return stretch factor for axis
double Axes::stretch(int axis) const { return stretch_.get(axis); }

// Set fractional position flag for axis
void Axes::setPositionIsFractional(int axis, bool b)
{
    positionIsFractional_[axis] = b;

    ++version_;
}

// Return fractional position flag for axis
bool Axes::positionIsFractional(int axis) const { return positionIsFractional_.get(axis); }

// Set axis position (in real surface-space coordinates)
void Axes::setPositionReal(int axis, int dir, double value)
{
    positionReal_[axis].set(dir, value);

    updateCoordinates();

    ++version_;
}

// Set axis position to axis limit (in real surface-space coordinates)
void Axes::setPositionRealToLimit(int axis, int dir, bool minLim)
{
    if (minLim)
        setPositionReal(axis, dir, limitMin_[dir]);
    else
        setPositionReal(axis, dir, limitMax_[dir]);
}

// Return axis position (in real surface-space coordinates)
Vec3<double> Axes::positionReal(int axis) const { return positionReal_[axis]; }

// Set axis position (in fractional axis coordinates)
void Axes::setPositionFractional(int axis, int dir, double value)
{
    // Clamp range to limits
    if (value > 1.0)
        value = 1.0;
    else if (value < 0.0)
        value = 0.0;

    positionFractional_[axis].set(dir, value);

    updateCoordinates();

    ++version_;
}

// Return axis position (in fractional axis coordinates)
Vec3<double> Axes::positionFractional(int axis) const { return positionFractional_[axis]; }

// Set autoscaling method employed
void Axes::setAutoScale(int axis, Axes::AutoScaleMethod method) { autoScale_[axis] = method; }

// Return autoscaling method employed
Axes::AutoScaleMethod Axes::autoScale(int axis) const { return autoScale_[axis]; }

/*
 * Data Transforms
 */

// Return supplied data x value in local axes coordinates
double Axes::transformX(double x) const
{
    if (inverted_.x && logarithmic_.x)
        return log10(max_.x / x) * stretch_.x;
    else if (inverted_.x)
        return ((max_.x - x) + min_.x) * stretch_.x;
    else if (logarithmic_.x)
        return log10(x) * stretch_.x;
    else
        return x * stretch_.x;
}

// Transform entire array of values into local axes coordinates
void Axes::transformX(std::vector<double> &xArray) const
{
    if (inverted_.x && logarithmic_.x)
        for (auto n = 0; n < xArray.size(); ++n)
            xArray[n] = log10(max_.x / xArray[n]) * stretch_.x;
    else if (inverted_.x)
        for (auto n = 0; n < xArray.size(); ++n)
            xArray[n] = ((max_.x - xArray[n]) + min_.x) * stretch_.x;
    else if (logarithmic_.x)
        for (auto n = 0; n < xArray.size(); ++n)
            xArray[n] = log10(xArray[n]) * stretch_.x;
    else
        std::transform(xArray.begin(), xArray.end(), xArray.begin(), [=](auto value) { return value * stretch_.x; });
}

// Return supplied data y value in local axes coordinates
double Axes::transformY(double y) const
{
    if (inverted_.y && logarithmic_.y)
        return (max_.y / y <= 0.0 ? log10(max_.y / y) * stretch_.y : 0.0);
    else if (inverted_.y)
        return ((max_.y - y) + min_.y) * stretch_.y;
    else if (logarithmic_.y)
        return (y <= 0.0 ? log10(y) * stretch_.y : 0.0);
    else
        return y * stretch_.y;
}

// Transform entire array of values into local axes coordinates
void Axes::transformY(std::vector<double> &yArray) const
{
    if (inverted_.y && logarithmic_.y)
        for (auto n = 0; n < yArray.size(); ++n)
        {
            yArray[n] = log10(max_.y / yArray[n]) * stretch_.y;
        }
    else if (inverted_.y)
        for (auto n = 0; n < yArray.size(); ++n)
            yArray[n] = ((max_.y - yArray[n]) + min_.y) * stretch_.y;
    else if (logarithmic_.y)
        for (auto n = 0; n < yArray.size(); ++n)
        {
            yArray[n] = log10(yArray[n]) * stretch_.y;
        }
    else
        std::transform(yArray.begin(), yArray.end(), yArray.begin(), [=](auto value) { return value * stretch_.y; });
}

// Return supplied data z value in local axes coordinates
double Axes::transformZ(double z) const
{
    if (inverted_.z && logarithmic_.z)
        return log10(max_.z / z) * stretch_.z;
    else if (inverted_.z)
        return ((max_.z - z) + min_.z) * stretch_.z;
    else if (logarithmic_.z)
        return log10(z) * stretch_.z;
    else
        return z * stretch_.z;
}

// Transform entire array of values into local axes coordinates
void Axes::transformZ(std::vector<double> &zArray) const
{
    if (inverted_.z && logarithmic_.z)
        for (auto n = 0; n < zArray.size(); ++n)
        {
            zArray[n] = log10(max_.z / zArray[n]) * stretch_.z;
        }
    else if (inverted_.z)
        for (auto n = 0; n < zArray.size(); ++n)
            zArray[n] = ((max_.z - zArray[n]) + min_.z) * stretch_.z;
    else if (logarithmic_.z)
        for (auto n = 0; n < zArray.size(); ++n)
        {
            zArray[n] = log10(zArray[n]) * stretch_.z;
        }
    else
        std::transform(zArray.begin(), zArray.end(), zArray.begin(), [=](auto value) { return value * stretch_.z; });
}

// Transform a 2D array of values into local axes coordinates
void Axes::transformX(Array2D<double> &xArray) const
{
    std::transform(xArray.begin(), xArray.end(), xArray.begin(), [this](auto x) { return transformX(x); });
}

// Transform a 2D array of values into local axes coordinates
void Axes::transformY(Array2D<double> &yArray) const
{
    std::transform(yArray.begin(), yArray.end(), yArray.begin(), [this](auto y) { return transformY(y); });
}

// Transform a 2D array of values into local axes coordinates
void Axes::transformZ(Array2D<double> &zArray) const
{
    std::transform(zArray.begin(), zArray.end(), zArray.begin(), [this](auto z) { return transformZ(z); });
}

/*
 * Ticks / Labels / Gridlines
 */

// Recalculate tick deltas for specified axis
void Axes::calculateTickDeltas(int axis)
{
    const auto nBaseValues = 5, maxIterations = 10, maxTicks = 10;
    auto power = 1, baseValueIndex = 0, minTicks = maxTicks / 2;
    int nTicks, iteration;
    auto baseValues = std::vector<int>{1, 2, 3, 4, 5};

    baseValueIndex = 0;
    power = int(log10((max_[axis] - min_[axis]) / maxTicks) - 1);
    iteration = 0;

    if ((max_[axis] - min_[axis]) > 1.0e-10)
    {
        do
        {
            // Calculate current tickDelta
            tickDelta_[axis] = baseValues[baseValueIndex] * pow(10.0, power);

            // Get first tickmark value
            tickFirst_[axis] = int(min_[axis] / tickDelta_[axis]) * tickDelta_[axis];

            // How many ticks now fit between the firstTick and max value?
            // Add 1 to get total ticks for this delta (i.e. including firstTick)
            nTicks = int((max_[axis] - min_[axis]) / tickDelta_[axis]);
            ++nTicks;

            // Check n...
            if (nTicks > maxTicks)
            {
                ++baseValueIndex;
                if (baseValueIndex == nBaseValues)
                    ++power;
                baseValueIndex = baseValueIndex % nBaseValues;
            }
            else if (nTicks < minTicks)
            {
                --baseValueIndex;
                if (baseValueIndex == -1)
                {
                    --power;
                    baseValueIndex += nBaseValues;
                }
            }

            ++iteration;
            if (iteration == maxIterations)
                break;

        } while ((nTicks > maxTicks) || (nTicks < minTicks));
    }
    else
    {
        tickFirst_[axis] = min_[axis];
        tickDelta_[axis] = 1.0;
    }
}

// Set axis tick direction
void Axes::setTickDirection(int axis, int dir, double value)
{
    tickDirection_[axis].set(dir, value);

    ++version_;
}

// Return axis tick direction
Vec3<double> Axes::tickDirection(int axis) const
{
    if (useBestFlatView_ && parentView_.isFlatView())
        switch (parentView_.viewType())
        {
            case (View::FlatXYView):
                return (axis == 0 ? Vec3<double>(0.0, inverted_.y ? 1.0 : -1.0, 0.0)
                                  : Vec3<double>(inverted_.x ? 1.0 : -1.0, 0.0, 0.0));
            case (View::FlatXZView):
                return (axis == 0 ? Vec3<double>(0.0, 0.0, inverted_.z ? 1.0 : -1.0)
                                  : Vec3<double>(inverted_.x ? 1.0 : -1.0, 0.0, 0.0));
            case (View::FlatZYView):
                return (axis == 1 ? Vec3<double>(0.0, 0.0, inverted_.z ? 1.0 : -1.0)
                                  : Vec3<double>(0.0, inverted_.y ? 1.0 : -1.0, 0.0));
            default:
                break;
        }

    // Safety catch
    return tickDirection_[axis];
}

// Set axis tick size (relative to font size)
void Axes::setTickSize(int axis, double size)
{
    tickSize_[axis] = size;

    ++version_;
}

// Return axis tick size (relative to font size)
double Axes::tickSize(int axis) const { return tickSize_.get(axis); }

// Set position of first tick delta on axes
void Axes::setFirstTick(int axis, double value)
{
    tickFirst_[axis] = value;

    ++version_;
}

// Return position of first tick delta on axes
double Axes::tickFirst(int axis) const { return tickFirst_.get(axis); }

// Set tick delta for axes
void Axes::setTickDelta(int axis, double value)
{
    tickDelta_[axis] = value;

    ++version_;
}

// Return tick delta for axes
double Axes::tickDelta(int axis) const { return tickDelta_.get(axis); }

// Set whether to calculate ticks automatically
void Axes::setAutoTicks(int axis, bool b)
{
    autoTicks_[axis] = b;

    ++version_;
}

// Return whether to calculate ticks automatically
bool Axes::autoTicks(int axis) const { return autoTicks_.get(axis); }

// Set number of minor ticks in major tick intervals
void Axes::setMinorTicks(int axis, int value)
{
    minorTicks_[axis] = value;

    ++version_;
}

// Return number of minor ticks in major tick intervals
int Axes::minorTicks(int axis) const { return minorTicks_.get(axis); }

/*
 * Labels
 */

// Determine suitable label format for the supplied axis
void Axes::determineLabelFormat(int axis)
{
    // Set axis value format based on tick delta
    auto logTick = int(log10(tickDelta_[axis]));
    if (abs(logTick) > 3)
    {
        numberFormat_[axis].setType(NumberFormat::FormatType::Scientific);
        numberFormat_[axis].setUseENotation(true);
        numberFormat_[axis].setNDecimals(3);
    }
    else if (logTick <= 0)
    {
        auto nDecimals = abs(logTick);

        numberFormat_[axis].setType(NumberFormat::FormatType::Decimal);
        numberFormat_[axis].setUseENotation(false);
        numberFormat_[axis].setNDecimals(nDecimals);

        // Check here that all tick values are now different
        while (true)
        {
            // Loop over labels and check that they are all different
            auto allDifferent = true;
            QString tickLabel, oldLabel;
            auto nTicks = (max_[axis] - min_[axis]) / tickDelta_[axis];
            double axisValue = tickFirst_[axis];
            for (auto n = 0; n < nTicks; ++n)
            {
                // Print the current label value
                tickLabel = numberFormat_[axis].format(axisValue);
                if (tickLabel == oldLabel)
                {
                    allDifferent = false;
                    break;
                }

                axisValue += tickDelta_[axis];
                oldLabel = tickLabel;
            }

            // If all are different, we're done
            if (allDifferent)
                break;

            // Increase decimals by one
            ++nDecimals;

            // If we're now at the threshold where we use exponential notation, set it and break now.
            if (nDecimals > 3)
            {
                numberFormat_[axis].setType(NumberFormat::FormatType::Scientific);
                numberFormat_[axis].setUseENotation(true);
                numberFormat_[axis].setNDecimals(3);
                break;
            }
            else
                numberFormat_[axis].setNDecimals(nDecimals);
        }
    }
    else
    {
        numberFormat_[axis].setType(NumberFormat::FormatType::Integer);
        numberFormat_[axis].setUseENotation(false);
        numberFormat_[axis].setNDecimals(0);
    }
}

// Return number format for specified axis
const NumberFormat &Axes::numberFormat(int axis) const { return numberFormat_[axis]; }

// Set number format for specified axis
void Axes::setNumberFormat(int axis, const NumberFormat &numberFormat) { numberFormat_[axis] = numberFormat; }

// Return whether to determine number format automatically for the specified axis
bool Axes::autoNumberFormat(int axis) const { return autoNumberFormat_.get(axis); }

// Set whether to determine number format automatically for the specified axis
void Axes::setAutoNumberFormat(int axis, bool b) { autoNumberFormat_[axis] = b; }

// Set orientation of labels for specified axis
void Axes::setLabelOrientation(int axis, int component, double value)
{
    labelOrientation_[axis].set(component, value);

    ++version_;
}

// Return orientation of labels for specified axis
Vec3<double> Axes::labelOrientation(int axis) const
{
    if (useBestFlatView_ && parentView_.isFlatView())
        switch (parentView_.viewType())
        {
            case (View::FlatXYView):
                return (axis == 0 ? Vec3<double>(0.0, 0.0, 0.2) : Vec3<double>(0.0, 0.0, 0.2));
            case (View::FlatXZView):
                return (axis == 0 ? Vec3<double>(270.0, 0.0, 0.2) : Vec3<double>(270.0, 0.0, 0.2));
            case (View::FlatZYView):
                return (axis == 1 ? Vec3<double>(90.0, 0.0, 0.2) : Vec3<double>(90.0, 0.0, 0.2));
            default:
                break;
        }

    // Safety catch
    return labelOrientation_[axis];
}

// Set axis label text anchor position for specified axis
void Axes::setLabelAnchor(int axis, TextPrimitive::TextAnchor anchor)
{
    labelAnchor_[axis] = anchor;

    ++version_;
}

// Return axis label text anchor position for specified axis
TextPrimitive::TextAnchor Axes::labelAnchor(int axis) const
{
    if (useBestFlatView_ && parentView_.isFlatView())
        switch (parentView_.viewType())
        {
            case (View::FlatXYView):
                return (axis == 0 ? (inverted_.y ? TextPrimitive::BottomMiddleAnchor : TextPrimitive::TopMiddleAnchor)
                                  : (inverted_.x ? TextPrimitive::MiddleLeftAnchor : TextPrimitive::MiddleRightAnchor));
            case (View::FlatXZView):
                return (axis == 0 ? (inverted_.z ? TextPrimitive::BottomMiddleAnchor : TextPrimitive::TopMiddleAnchor)
                                  : (inverted_.x ? TextPrimitive::MiddleLeftAnchor : TextPrimitive::MiddleRightAnchor));
            case (View::FlatZYView):
                return (axis == 1 ? (inverted_.z ? TextPrimitive::MiddleLeftAnchor : TextPrimitive::MiddleRightAnchor)
                                  : (inverted_.y ? TextPrimitive::BottomMiddleAnchor : TextPrimitive::TopMiddleAnchor));
            default:
                break;
        }

    // Safety catch
    return labelAnchor_[axis];
}

// Set title for specified axis
void Axes::setTitle(int axis, std::string_view title)
{
    title_[axis] = title;

    ++version_;
}

// Return title for specified axis
std::string_view Axes::title(int axis) const { return title_[axis]; }

// Set orientation of titles for specified axis
void Axes::setTitleOrientationNEW(int axis, int component, double value)
{
    titleOrientation_[axis].set(component, value);

    ++version_;
}

// Return orientation of titles for specified axis
Vec3<double> Axes::titleOrientation(int axis) const
{
    if (useBestFlatView_ && parentView_.isFlatView())
        switch (parentView_.viewType())
        {
            case (View::FlatXYView):
                return (axis == 0 ? Vec3<double>(0.0, 0.0, 0.0) : Vec3<double>(0.0, 270.0, 0.0));
            case (View::FlatXZView):
                return (axis == 0 ? Vec3<double>(270.0, 0.0, 0.0) : Vec3<double>(270.0, 0.0, 90.0));
            case (View::FlatZYView):
                return (axis == 1 ? Vec3<double>(0.0, 90.0, 90.0) : Vec3<double>(0.0, 90.0, 0.0));
            default:
                break;
        }

    // Safety catch
    return titleOrientation_[axis];
}

// Set title distance from axis
void Axes::setTitleDistance(int axis, double distance) { titleDistances_.set(axis, distance); }

// Return title distance from axis
double Axes::titleDistance(int axis) const { return titleDistances_.get(axis); }

// Set title horizontal offset
void Axes::setTitleHorizontalOffset(int axis, double offset) { titleHorizontalOffsets_.set(axis, offset); }

// Return title horizontal offset
double Axes::titleHorizontalOffset(int axis) const { return titleHorizontalOffsets_.get(axis); }

// Set axis title text anchor position for specified axis
void Axes::setTitleAnchor(int axis, TextPrimitive::TextAnchor anchor)
{
    titleAnchor_[axis] = anchor;

    ++version_;
}

// Return axis title text anchor position for specified axis
TextPrimitive::TextAnchor Axes::titleAnchor(int axis) const
{
    if (useBestFlatView_ && parentView_.isFlatView())
        switch (parentView_.viewType())
        {
            case (View::FlatXYView):
                return (axis == 0 ? (inverted_.y ? TextPrimitive::BottomMiddleAnchor : TextPrimitive::TopMiddleAnchor)
                                  : (inverted_.x ? TextPrimitive::TopMiddleAnchor : TextPrimitive::BottomMiddleAnchor));
            case (View::FlatXZView):
                return (axis == 0 ? (inverted_.z ? TextPrimitive::BottomMiddleAnchor : TextPrimitive::TopMiddleAnchor)
                                  : (inverted_.x ? TextPrimitive::TopMiddleAnchor : TextPrimitive::BottomMiddleAnchor));
            case (View::FlatZYView):
                return (axis == 1 ? (inverted_.z ? TextPrimitive::TopMiddleAnchor : TextPrimitive::BottomMiddleAnchor)
                                  : (inverted_.y ? TextPrimitive::BottomMiddleAnchor : TextPrimitive::TopMiddleAnchor));
            default:
                break;
        }

    // Safety catch
    return titleAnchor_[axis];
}

/*
 * Style Overrides
 */

// Set whether to use best tick/label orientation for flat views
void Axes::setUseBestFlatView(bool b)
{
    useBestFlatView_ = b;

    ++version_;
}

// Return whether to use best tick/label orientation for flat views
bool Axes::useBestFlatView() const { return useBestFlatView_; }

// Set whether to automatically place titles at a sensible position after label text
void Axes::setAutoPositionTitles(bool b)
{
    autoPositionTitles_ = b;

    ++version_;
}

// Return whether to automatically place titles at a sensible position after label text
bool Axes::autoPositionTitles() const { return autoPositionTitles_; }

/*
 * GridLines
 */

// Set whether gridlines cover entire volume or just at axis lines
void Axes::setGridLinesFull(int axis, bool b)
{
    gridLinesFull_[axis] = b;

    ++version_;
}

// Return whether gridlines cover entire volume or just at axis lines
bool Axes::gridLinesFull(int axis) const { return gridLinesFull_.get(axis); }

// Set whether gridLines at major tick intervals are active for specified axis
void Axes::setGridLinesMajor(int axis, bool on)
{
    gridLinesMajor_[axis] = on;

    ++version_;
}

// Return whether gridLines at major tick intervals are active for specified axis
bool Axes::gridLinesMajor(int axis) const { return gridLinesMajor_.get(axis); }

// Set whether gridLines at minor tick intervals are active for specified axis
void Axes::setGridLinesMinor(int axis, bool on)
{
    gridLinesMinor_[axis] = on;

    ++version_;
}

// Return whether gridLines at minor tick intervals are active for specified axis
bool Axes::gridLinesMinor(int axis) const { return gridLinesMinor_.get(axis); }

/*
 * Version
 */

// Return version of axws definitions
int Axes::version() const { return version_; }

/*
 * GL
 */

// Update axes primitives
void Axes::updateAxisPrimitives()
{
    // Check whether we need to regenerate the axes primitives / data
    if (version_ == primitiveVersion_)
        return;

    QString s;
    FTBBox boundingBox;
    Vec3<double> centre;
    double delta, value, clipPlaneDelta = 0.0001;
    Vec3<double> u, v1, v2, tickDir, adjustment;
    Matrix4 labelTransform, titleTransform;
    std::vector<double> tickPositions[3];
    std::vector<bool> tickIsMajor[3];

    // Make sure coordinates are up-to-date
    updateCoordinates();

    // Get view rotation inverse
    Matrix4 viewRotationInverse = parentView_.viewRotationInverse();

    // Set clip coordinates
    for (auto axis = 0; axis < 3; ++axis)
    {
        if (logarithmic_[axis])
        {
            clipMin_[axis] = (inverted_[axis] ? 0.0 : log10(min_[axis])) * stretch_[axis] - clipPlaneDelta;
            clipMax_[axis] =
                (inverted_[axis] ? log10(max_[axis] / min_[axis]) : log10(max_[axis])) * stretch_[axis] + clipPlaneDelta;
        }
        else
        {
            clipMin_[axis] = (min_[axis] * stretch_[axis]) - clipPlaneDelta;
            clipMax_[axis] = (max_[axis] * stretch_[axis]) + clipPlaneDelta;
        }
    }

    auto textColour = fontInstance_.colour();
    GLfloat colour[4] = {textColour.redF(), textColour.greenF(), textColour.blueF(), textColour.alphaF()};

    // Construct axes
    for (auto axis = 0; axis < 3; ++axis)
    {
        // Clear old axis primitives
        axisPrimitives_[axis].forgetAll();
        labelPrimitives_[axis].clear();
        titlePrimitives_[axis].clear();

        // Normalise tickDirection
        tickDir = tickDirection(axis);
        tickDir.normalise();

        // Create tick label transformation matrix
        labelTransform.setIdentity();
        // -- 1) Apply axial rotation along label left-to-right direction
        if (parentView_.viewType() == View::FlatZYView)
            labelTransform.applyPreRotationY(labelOrientation(axis).x);
        else
            labelTransform.applyPreRotationX(labelOrientation(axis).x);
        // -- 2) Perform in-plane rotation
        if (parentView_.viewType() == View::FlatZYView)
            labelTransform.applyPreRotationX(labelOrientation(axis).y);
        else if (parentView_.viewType() == View::FlatXZView)
            labelTransform.applyPreRotationY(labelOrientation(axis).y);
        else
            labelTransform.applyPreRotationZ(labelOrientation(axis).y);

        // Create axis title transformation matrix
        titleTransform.setIdentity();
        titleTransform.applyPreRotationX(titleOrientation(axis).x);
        titleTransform.applyPreRotationY(titleOrientation(axis).y);
        titleTransform.applyPreRotationZ(titleOrientation(axis).z);

        // Add axis labels
        if (logarithmic_[axis])
        {
            // For the log axis, the associated surface data coordinate will already be in log form
            if (max_[axis] < 0.0)
            {
                Messenger::print("Axis range is inappropriate for a log scale ({} < x < {}). Axis will not be drawn.\n",
                                 min_[axis], max_[axis]);
                return;
            }

            // Draw a line from min to max range, passing through the defined position
            axisPrimitives_[axis].line(coordMin_[axis], coordMax_[axis], colour);

            // Grab logged min/max values for convenience, enforcing sensible minimum
            double min = log10(min_[axis] <= 0.0 ? 1.0e-10 : min_[axis]);

            // Plot tickmarks - Start at floored (ceiling'd) integer of logAxisMin (logAxisMax), and go from there.
            auto nMinorTicks = minorTicks_[axis] > 8 ? 8 : minorTicks_[axis];
            auto count = 0;
            double power = floor(min);
            double value = pow(10, power);
            auto u = coordMin_[axis];
            while (value <= max_[axis])
            {
                // If the current value is in range, plot a tick
                u[axis] = (inverted_[axis] ? log10(max_[axis] / value) : log10(value)) * stretch_[axis];
                if (log10(value) >= min)
                {
                    // Tick mark
                    axisPrimitives_[axis].line(u, u + tickDir * tickSize_[axis] * (count == 0 ? 1.0 : 0.5), colour);
                    tickPositions[axis].push_back(u[axis]);
                    tickIsMajor[axis].push_back(count == 0);

                    // Tick label
                    if (count == 0)
                    {
                        // Get formatted value text
                        s = numberFormat_[axis].format(value);

                        labelPrimitives_[axis].add(&fontInstance_, s, u + tickDir * tickSize_[axis], labelAnchor(axis),
                                                   tickDir * labelOrientation(axis).z, labelTransform,
                                                   parentView_.labelPointSize(),
                                                   parentView_.isFlatView() ? false : parentView_.flatLabelsIn3D());
                    }
                }

                // Increase tick counter, value, and power if necessary
                ++count;
                if (count > nMinorTicks)
                {
                    count = 0;
                    power = power + 1.0;
                    value = pow(10, power);
                }
                else
                    value += pow(10, power);
            }
        }
        else
        {
            // Calculate autoticks if requested
            if (autoTicks_[axis])
                calculateTickDeltas(axis);

            // Autoformat the axia labels if requested
            if (autoNumberFormat_[axis])
                determineLabelFormat(axis);

            // Draw a line from min to max limits, passing through the defined position
            axisPrimitives_[axis].line(coordMin_[axis], coordMax_[axis], colour);

            // Check tickDelta
            if (((max_[axis] - min_[axis]) / tickDelta_[axis]) > 1e6)
                return;

            // Plot tickmarks
            auto count = 0;
            delta = tickDelta_[axis] / (minorTicks_[axis] + 1);
            value = tickFirst_[axis];
            u = coordMin_[axis];
            u.set(axis, (inverted_[axis] ? (max_[axis] - tickFirst_[axis]) + min_[axis] : tickFirst_[axis]) * stretch_[axis]);
            while (value <= max_[axis])
            {
                // Draw tick here, only if value >= min_
                if (value >= min_[axis])
                {
                    tickPositions[axis].push_back(u[axis]);

                    if (count % (minorTicks_[axis] + 1) == 0)
                    {
                        axisPrimitives_[axis].line(u, u + tickDir * tickSize_[axis], colour);

                        // Get formatted label text
                        s = numberFormat_[axis].format(value);

                        labelPrimitives_[axis].add(&fontInstance_, s, u + tickDir * tickSize_[axis], labelAnchor(axis),
                                                   tickDir * labelOrientation(axis).z, labelTransform,
                                                   parentView_.labelPointSize(),
                                                   parentView_.isFlatView() ? false : parentView_.flatLabelsIn3D());

                        tickIsMajor[axis].push_back(true);

                        count = 0;
                    }
                    else
                    {
                        axisPrimitives_[axis].line(u, u + tickDir * tickSize_[axis] * 0.5, colour);
                        tickIsMajor[axis].push_back(false);
                    }
                }
                u.add(axis, delta * (inverted_[axis] ? -stretch_[axis] : stretch_[axis]));
                value += delta;
                ++count;
            }
        }

        // Add axis title
        // -- Set basic position (corresponding to offset position of tick labels combined with title label axis
        // position to start with...)
        u = coordMin_[axis];
        if (logarithmic_[axis])
        {
            value = log10(min_[axis]) + log10(max_[axis] / min_[axis]) * titleHorizontalOffsets_[axis];
            u.set(axis, (inverted_[axis] ? log10(max_[axis]) - value : value) * stretch_[axis]);
        }
        else
        {
            value = min_[axis] + (max_[axis] - min_[axis]) * titleHorizontalOffsets_[axis];
            u.set(axis, (inverted_[axis] ? (max_[axis] - value) + min_[axis] : value) * stretch_[axis]);
        }
        // -- Next step depends on whether we are automatically adjusting label positions
        if ((useBestFlatView_ && parentView_.isFlatView()) || autoPositionTitles_)
        {
            Cuboid cuboid = labelPrimitives_[axis].boundingCuboid(fontInstance_, viewRotationInverse, parentView_.textZScale());
            // Project tick direction onto cuboid width/height
            // TODO This does not account for the fact that the bounding cuboid may only partly extend over the end
            // of ths axis tick mark (e.g. as with in-plane rotations/TopMiddle anchors)...
            Vec3<double> extent = cuboid.maxima() - cuboid.minima();
            extent.multiply(tickDir.x, tickDir.y, tickDir.z);
            // -- Add on extra distance from tick mark
            u += tickDir * (tickSize_[axis]);
            // -- Create adjustment vector. Start by adding space between tickmark, label text, and title text
            adjustment = tickDir * (labelOrientation(axis).z + 0.2);
            // -- Add on label extent in the tickmark direction - we must undo the scaling on the bounding box
            // arising from display scales etc.
            adjustment += (tickDir * extent.magnitude()) /
                          (fontInstance_.fontBaseHeight() * parentView_.labelPointSize() / parentView_.textZScale());
            // -- Scaling will be done by the title point size in TextPrimitive, but all our adjustments were done
            // with label point size, so scale it...
            adjustment *= parentView_.labelPointSize() / parentView_.titlePointSize();
        }
        else
            adjustment = tickDir * titleDistances_[axis];

        // -- Add primitive
        titlePrimitives_[axis].add(&fontInstance_, QString::fromStdString(title_[axis]), u, titleAnchor(axis), adjustment,
                                   titleTransform, parentView_.titlePointSize(),
                                   parentView_.isFlatView() ? false : parentView_.flatLabelsIn3D());
    }

    // GridLines
    gridLineMinorPrimitives_[0].initialise(GL_LINES, false);
    gridLineMajorPrimitives_[0].initialise(GL_LINES, false);
    gridLineMinorPrimitives_[1].initialise(GL_LINES, false);
    gridLineMajorPrimitives_[1].initialise(GL_LINES, false);
    gridLineMinorPrimitives_[2].initialise(GL_LINES, false);
    gridLineMajorPrimitives_[2].initialise(GL_LINES, false);

    // The 'axis' variable indicates the vector we are drawing lines along, and the relevant primitive store them in
    for (auto axis = 0; axis < 3; ++axis)
    {
        // Check to see if there is anything to draw for this direction
        if ((!gridLinesMajor_[axis]) && (!gridLinesMinor_[axis]))
            continue;

        // Determine the indices of the remaining two axes to use
        int ortho1 = (axis + 1) % 3;
        int ortho2 = (axis + 2) % 3;

        // Double loop now, over the two sets of tickmarks that are orthogonal to 'axis'
        for (auto i1 = 0; i1 < tickPositions[ortho1].size(); ++i1)
        {
            for (auto i2 = 0; i2 < tickPositions[ortho2].size(); ++i2)
            {
                // Set basic vector info
                // The 'axis' will define its own component, with the other two coming from the tickmark
                // positions of the other axes
                v1[axis] = coordMin_[axis][axis];
                v1[ortho1] = tickPositions[ortho1][i1];
                v1[ortho2] = tickPositions[ortho2][i2];
                v2[axis] = coordMax_[axis][axis];
                v2[ortho1] = tickPositions[ortho1][i1];
                v2[ortho2] = tickPositions[ortho2][i2];

                // If we are only drawing lines in the planes orthogonal to the axis, break if we have moved
                // away from it... Otherwise, we change either the i1 or i2 components of v1 and v2 to position
                // the gridline with the axis line itself
                if (!gridLinesFull_[axis])
                {
                    // If both values are non-zero, we are done so we can break out of the loop
                    if ((i1 != 0) && (i2 != 0))
                        break;

                    // If the i1 counter is zero, change the values of the ortho1 axis to that of the 'axis'
                    // axis, and vice versa
                    if (i1 == 0)
                    {
                        v1[ortho1] = coordMin_[axis][ortho1];
                        v2[ortho1] = coordMax_[axis][ortho1];
                    }
                    else
                    {
                        v1[ortho2] = coordMin_[axis][ortho2];
                        v2[ortho2] = coordMax_[axis][ortho2];
                    }
                }

                // Add line to the relevant primitive
                if (tickIsMajor[ortho1][i1] && tickIsMajor[ortho2][i2])
                {
                    if (gridLinesMajor_[axis])
                        gridLineMajorPrimitives_[axis].line(v1, v2, colour);
                }
                else if (gridLinesMinor_[axis])
                    gridLineMinorPrimitives_[axis].line(v1, v2, colour);
            }
        }
    }

    primitiveVersion_ = version_;
}

// Return clip plane lower Y value
Vec3<double> Axes::clipMin() const { return clipMin_; }

// Return clip plane upper Y value
Vec3<double> Axes::clipMax() const { return clipMax_; }

// Flag primitives as invalid
void Axes::setPrimitivesInvalid() { primitiveVersion_ = -1; }

// Return axis primitive for axis specified
Primitive &Axes::axisPrimitive(int axis)
{
    // Make sure primitives are up to date first...
    updateAxisPrimitives();

    return axisPrimitives_[axis];
}

// Return axis label primitive list for axis specified
TextPrimitiveList &Axes::labelPrimitive(int axis)
{
    // Make sure primitives are up to date first...
    updateAxisPrimitives();

    return labelPrimitives_[axis];
}

// Return axis title primitive list for axis specified
TextPrimitiveList &Axes::titlePrimitive(int axis)
{
    // Make sure primitives are up to date first...
    updateAxisPrimitives();

    return titlePrimitives_[axis];
}

// Return minor gridline primitive for axis specified
Primitive &Axes::gridLineMinorPrimitive(int axis)
{
    // Make sure primitives are up to date first...
    updateAxisPrimitives();

    return gridLineMinorPrimitives_[axis];
}

// Return major gridline primitive for axis specified
Primitive &Axes::gridLineMajorPrimitive(int axis)
{
    // Make sure primitives are up to date first...
    updateAxisPrimitives();

    return gridLineMajorPrimitives_[axis];
}

// Set major gridline style
void Axes::setGridLineMajorStyle(int axis, const LineStyle &style) { gridLineMajorStyle_[axis] = style; }

// Return major GridLine style
const LineStyle &Axes::gridLineMajorStyle(int axis) const { return gridLineMajorStyle_[axis]; }

// Set minor gridline style
void Axes::setGridLineMinorStyle(int axis, const LineStyle &style) { gridLineMinorStyle_[axis] = style; }

// Return minor GridLine style
const LineStyle &Axes::gridLineMinorStyle(int axis) const { return gridLineMinorStyle_[axis]; }
