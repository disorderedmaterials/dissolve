/*
	*** XY Data
	*** src/base/xydata.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_XYDATA_H
#define DISSOLVE_XYDATA_H

#include "base/charstring.h"
#include "math/broadeningfunction.h"
#include "math/windowfunction.h"
#include "templates/array.h"
#include "templates/objectstore.h"

#define OPTOLERANCE 1.0e-6

// Forward Declarations
class ProcessPool;

// XYData
class XYData : public ListItem<XYData>, public ObjectStore<XYData>, public GenericItemBase
{
	public:
	// Constructor
	XYData();
	// Destructor
	~XYData();
	// Copy Constructor
	XYData(const XYData& source);
	// Clear data
	void clear();
	// Clear interpolation arrays
	void clearInterpolationArrays();
	

	/*
	 * Data
	 */
	private:
	// Array of x values
	Array<double> x_;
	// Array of y values
	Array<double> y_;
	// Z value
	double z_;
	// Name
	CharString name_;

	public:
	// Reset arrays to zero
	void reset();
	// Initialise arrays to specified size
	void initialise(int size);
	// Create new X data and empty Y data
	void createEmpty(double xDelta, double xMax, bool halfBins = true);
	// Copy existing X and Y data
	void copyData(XYData& source);
	// Copy existing X data and generate empty Y
	void templateFrom(const XYData& source);
	// Return current array size
	int arraySize();
	// Set data point 
	void setPoint(int index, double x, double y);
	// Return number of defined datapoints
	int nPoints() const;
	// Set x value
	void setX(int index, double x);
	// Add to x value
	void addX(int index, double delta);
	// Return x value specified
	double x(int index) const;
	// Return x Array
	Array<double>& arrayX();
	// Return x Array (const)
	const Array<double>& constArrayX() const;
	// Set y value
	void setY(int index, double y);
	// Add to y value
	void addY(int index, double delta);
	// Add to y array
	bool addY(const Array<double>& source, double factor = 1.0);
	// Multiply y value
	void multiplyY(int index, double factor);
	// Return y value specified
	double y(int index) const;
	// Return y Array
	Array<double>& arrayY();
	// Return y Array (const)
	const Array<double>& constArrayY() const;
	// Set z value
	void setZ(double z);
	// Add to z value
	void addZ(double delta);
	// Return z value
	double& z();
	// Return z value (const)
	double constZ() const;
	// Add new data point
	void addPoint(double x, double y);
	// Set name of data
	void setName(const char* name);
	// Return name of data
	const char* name() const;


	/*
	 * Operators
	 */
	public:
	// Assignment Operator
	void operator=(const XYData& source);
	// Operator +
	XYData operator+(const XYData& source) const;
	// Operator +=
	void operator+=(const XYData& source);
	// Operator +=
	void operator+=(const double dy);
	// Operator -
	XYData operator-(const XYData& source) const;
	// Operator -=
	void operator-=(const XYData& source);
	// Operator -=
	void operator-=(const double dy);
	// Operator *=
	void operator*=(const double factor);
	// Operator /=
	void operator/=(const double factor);


	/*
	 * Limits
	 */
	public:
	// Return minumum x value in data
	double xMin() const;
	// Return maxumum x value in data
	double xMax() const;
	// Return first x value in data
	double xFirst() const;
	// Return last x value in data
	double xLast() const;
	// Return minumum y value in data
	double yMin() const;
	// Return maxumum y value in data
	double yMax() const;
	// Trim data to X-range specified
	void trim(double minX, double maxX);


	/*
	 * Gradients
	 */
	public:
	// Return gradient of last n points, and average y value if requested
	double lastGradient(int nSamples, double* yMean = NULL) const;


	/*
	 * Integration
	 */
	public:
	// Compute integral of the data
	double integral();
	// Compute absolute integral of the data
	double absIntegral();


	/*
	 * Filtering
	 */
	public:
	// Apply median filter to data
	void medianFilter(int length);
	// Rebin data onto uniform x axis
	void rebin(double deltaX = -1.0);
	// Perform moving average smoothing
	void movingAverage(int avgSize);
	// Apply Kolmogorov–Zurbenko filter
	void kolmogorovZurbenkoFilter(int k, int m);


	/*
	 * Manipulation
	 */
	public:
	// Perform point-wise convolution of this data with the supplied BroadeningFunction
	void convolve(BroadeningFunction function);
	// Perform point-wise convolution of this data with the supplied BroadeningFunction, normalising to the original integral of the function
	void convolveNormalised(BroadeningFunction function);
	// Add interpolated data
	void addInterpolated(XYData& source, double weighting = 1.0);
	// Subtract average level from data, forming average from supplied x value
	double subtractAverage(double xStart);


	/*
	 * Similarity
	 */
	public:
	// Return RMSE of current data with (interpolated) reference data
	double rmse(XYData ref) const;
	// Return percentage error between this and reference data
	double error(XYData ref) const;


	/*
	 * File I/O
	 */
	public:
	// Load data from specified file
	bool load(const char* filename);
	// Load data from specified LineParser, using columns specified
	bool load(LineParser& parser, int xcol = 0, int ycol = 1);
	// Save data to specified file
	bool save(const char* filename) const;
	// Save data and interpolation to specified file
	bool saveWithInterpolation(const char* filename);
	// Dump contents
	void dump();


	/*
	 * Fourier Transforms
	 */
	private:
	// Make some checks before doing transform
	bool checkBeforeTransform();

	public:
	// Perform Fourier sine transform of current distribution function, over range specified, and with specified window and broadening functions applied
	bool sineFT(double normFactor, double wMin, double wStep, double wMax, WindowFunction windowFunction = WindowFunction(), BroadeningFunction broadening = BroadeningFunction());


	/*
	 * Interpolation
	 */
	public:
	// Interpolation Schemes
	enum InterpolationScheme
	{
		NoInterpolation,
		SplineInterpolation,
		/* ConstrainedSplineInterpolation, */  // Removed for now as it produces spurious features in some fits.
		LinearInterpolation,
		ThreePointInterpolation
	};

	private:
	// Array of parameters for interpolations (if created)
	Array<double> interpolationA_, interpolationB_, interpolationC_, interpolationD_, interpolationH_;
	// Interval of last interpolated point
	int interpolationInterval_;
	// Interpolation scheme currently employed
	InterpolationScheme interpolationScheme_;

	private:
	// Prepare natural spline interpolation of data
	void interpolateSpline();
	// Prepare constrained natural spline interpolation of data
	void interpolateConstrainedSpline();
	// Prepare linear interpolation of data
	void interpolateLinear();
	// Prepare three-point interpolation of data
	void interpolateThreePoint();

	public:
	// Calculate interpolation of current data using the supplied scheme
	void interpolate(InterpolationScheme scheme);
	// Return interpolated y value for supplied x
	double interpolated(double xValue);
	// Return interpolated y value for supplied x, specifying containing interval
	double interpolated(double xValue, int interval);
	// Approximate data at specified x value using three-point interpolation
	double approximate(double xValue) const;
	// Expand the current data, adding n interpolated points in-between the original values
	void expand(int nExtraPoints);


	/*
	 * GenericItemBase Implementations
	 */
	public:
	// Return class name
	static const char* itemClassName();
	// Write data through specified LineParser
	bool write(LineParser& parser);
	// Read data through specified LineParser
	bool read(LineParser& parser);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data
	bool broadcast(ProcessPool& procPool, int rootRank = 0);
	// Check item equality
	bool equality(ProcessPool& procPool);
};

#endif
