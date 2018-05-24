/*
	*** Axes Description
	*** src/gui/uchroma/classes/axes.h
	Copyright T. Youngs 2013-2018

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_UCHROMAAXES_H
#define DISSOLVE_UCHROMAAXES_H

#include "gui/uchroma/classes/numberformat.h"
#include "gui/uchroma/classes/displaydataset.h"
#include "gui/uchroma/render/primitive.h"
#include "gui/uchroma/render/textprimitivelist.h"
#include "gui/uchroma/render/linestyle.h"
#include "base/charstring.h"
#include "templates/objectstore.h"
#include "templates/vector3.h"
#include "templates/vector4.h"
#include "templates/array.h"

// Forward Declarations
class ViewPane;
class UChromaBase;

// Axes
class Axes : public ObjectStore<Axes>
{
	public:
	// Constructor / Destructor
	Axes(ViewPane& parent);
	~Axes();
	// Copy constructor
	Axes(const Axes& source);
	// Assignment operator
	void operator=(const Axes& source);


	/*
	 * Parent
	 */
	private:
	// Parent ViewPane in which these axes are displayed
	ViewPane& parent_;


	/*
	 * Definition
	 */
	public:
	// Autoscaling for axes
	enum AutoScaleMethod { NoAutoScale, ExpandingAutoScale, FullAutoScale, nAutoScaleMethods };
	// Convert text string to AutoScaleMethod
	static AutoScaleMethod autoScaleMethod(const char* s);
	// Convert AutoScaleMethod to text string
	static const char* autoScaleMethod(AutoScaleMethod scale);

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
	// Version at which coordinate extremes/centre were last generated
	int coordinateVersion_;

	private:
	// Recalculate centre coordinate of axes
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
	Vec3<double> coordCentre();
	// Return coordinate at minimum of specified axis
	Vec3<double> coordMin(int axis);
	// Return coordinate at maximum of specified axis
	Vec3<double> coordMax(int axis);
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
	Axes::AutoScaleMethod autoScale(int axis);


	/*
	 * Data Transforms
	 */
	public:
	// Return supplied data x value in local axes coordinates
	double transformX(double x) const;
	// Transform entire array of values into local axes coordinates
	void transformX(Array<double>& xArray) const;
	// Return supplied data y value in local axes coordinates
	double transformY(double y) const;
	// Transform entire array of values into local axes coordinates
	void transformY(Array< double >& yArray, Array<DisplayDataSet::DataPointType>& typeArray) const;
	// Return supplied data z value in local axes coordinates
	double transformZ(double z) const;


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
	double tickSize(int axis);
	// Set position of first tick delta on specified axis
	void setFirstTick(int axis, double value);
	// Return position of first tick delta on specified axis
	double tickFirst(int axis);
	// Set tick delta for specified axis
	void setTickDelta(int axis, double value);
	// Return tick delta for specified axis
	double tickDelta(int axis);
	// Set whether to calculate ticks automatically for specified axis
	void setAutoTicks(int axis, bool b);
	// Return whether to calculate ticks automatically for specified axis
	bool autoTicks(int axis);
	// Set number of minor ticks in major tick intervals for specified axis
	void setMinorTicks(int axis, int value);
	// Return number of minor ticks in major tick intervals for specified axis
	int minorTicks(int axis);


	/*
	 * Labels
	 */
	private:
	// Number formats for labels
	NumberFormat numberFormat_[3];
	// Orientation of axis labels (axial rot, in-plane rot, distance)
	Vec3<double> labelOrientation_[3];
	// Axis label text anchor positions
	TextPrimitive::TextAnchor labelAnchor_[3];
	// Axis titles
	CharString title_[3];
	// Orientation of axis titles (axial rot, in-plane rot, distance, h-offset)
	Vec4<double> titleOrientation_[3];
	// Axis title text anchor positions
	TextPrimitive::TextAnchor titleAnchor_[3];

	public:
	// Return number format for specified axis
	NumberFormat& numberFormat(int axis);
	// Set orientation of labels for specified axis
	void setLabelOrientation(int axis, int component, double value);
	// Return orientation of labels for specified axis
	Vec3<double> labelOrientation(int axis) const;
	// Set axis label text anchor position for specified axis
	void setLabelAnchor(int axis, TextPrimitive::TextAnchor anchor);
	// Return axis label text anchor position for specified axis
	TextPrimitive::TextAnchor labelAnchor(int axis) const;
	// Set title for specified axis
	void setTitle(int axis, const char* title);
	// Return title for specified axis
	const char* title(int axis) const;
	// Set orientation of titles for specified axis
	void setTitleOrientation(int axis, int component, double value);
	// Return orientation of titles for specified axis
	Vec4<double> titleOrientation(int axis) const;
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
	bool useBestFlatView();
	// Set whether to automatically place titles at a sensible position after label text
	void setAutoPositionTitles(bool b);
	// Return whether to automatically place titles at a sensible position after label text
	bool autoPositionTitles();


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
	// Version of axis definitions
	int axesVersion_;
	// Version of axis properties affecting data display
	int displayVersion_;

	public:
	// Return version of axis definitions
	int axesVersion();
	// Return version of axis properties affecting data display
	int displayVersion() const;


	/*
	 * GL
	 */
	private:
	// Y axis clip plane position
	GLdouble clipPlaneYMin_, clipPlaneYMax_;
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
	GLdouble clipPlaneYMin();
	// Return clip plane upper Y value
	GLdouble clipPlaneYMax();
	// Flag primitives as invalid
	void setPrimitivesInvalid();
	// Return axis primitive for axis specified
	Primitive& axisPrimitive(int axis);
	// Return axis label primitive list for axis specified
	TextPrimitiveList& labelPrimitive(int axis);
	// Return axis title primitive list for axis specified
	TextPrimitiveList& titlePrimitive(int axis);
	// Return minor gridline primitive for axis specified
	Primitive& gridLineMajorPrimitive(int axis);
	// Return major gridline primitive for axis specified
	Primitive& gridLineMinorPrimitive(int axis);
	// Set major gridline style
	void setGridLineMajorStyle(int axis, LineStyle style);
	// Return major gridline style
	LineStyle& gridLineMajorStyle(int axis);
	// Set minor gridline style
	void setGridLineMinorStyle(int axis, LineStyle style);
	// Return minor gridline style
	LineStyle& gridLineMinorStyle(int axis);
};

#endif
