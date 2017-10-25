/*
	*** XY Data
	*** src/base/xydata.h
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_XYDATA_H
#define DUQ_XYDATA_H

#include "base/charstring.h"
#include "base/function.h"
#include "templates/array.h"

#define OPTOLERANCE 1.0e-6

// Forward Declarations
class ProcessPool;

// XYData
class XYData : public ListItem<XYData>
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
	
	private:
	// Whether to use local FFT code over DFT
	static bool useFFT_;


	/*
	 * Data
	 */
	private:
	// Array of x values
	Array<double> x_;
	// Array of y values
	Array<double> y_;
	// Name
	CharString name_;
	
	private:
	// Reset arrays to zero
	void reset();
	
	public:
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
	 * General Functions
	 */
	public:
	// Return minumum x value in data
	double xMin();
	// Return maxumum x value in data
	double xMax();
	// Return first x value in data
	double xFirst();
	// Return last x value in data
	double xLast();
	// Return minumum y value in data
	double yMin();
	// Return maxumum y value in data
	double yMax();
	// Compute integral of the data
	double integral();
	// Compute absolute integral of the data
	double absIntegral();
	// Apply median filter to data
	void medianFilter(int length);
	// Convolute this data with the supplied data
	bool convolute(XYData& data);
	// Convolute this data with the supplied data, by products
	bool convoluteProduct(XYData& data);
	// Trim data to X-range specified
	void trim(double minX, double maxX);
	// Rebin data onto uniform x axis
	void rebin(double deltaX = -1.0);
	// Smooth data
	void smooth(int avgSize, int skip = 0);
	// Add interpolated data
	void addInterpolated(XYData& source, double weighting = 1.0);
	// Return RMSE of current data with (interpolated) reference data
	double rmse(XYData ref);
	// Return percentage error between this and reference data
	double error(XYData ref);


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
	public:
	// Window Functions
	enum WindowFunction
	{
		NoWindow,		/* Don't use any window */
		BartlettWindow,		/* Bartlett (triangular) window */
		HannWindow,		/* von Hann (Hanning) window */
		LanczosWindow,		/* Lanczos window */
		NuttallWindow,		/* Nuttall window (continuous first derivatives over range) */
		SineWindow,		/* Sine Window */
		nWindowFunctions	/* Number of defined WindowFunctions */
	};
	// Convert text string to WindowFunction
	static WindowFunction windowFunction(const char* s);
	// Covert WindowFunction to text string
	static const char* windowFunction(XYData::WindowFunction wf);

	private:
	// Make some checks before doing transform
	bool checkBeforeTransform();
	// Return value of window function at specified position (in range 0 - 1.0)
	double window(XYData::WindowFunction wf, double pos);

	public:
	// Perform plain Fourier transform of real data
	bool fourierTransformReal(bool forwardTransform = true, XYData::WindowFunction wf = XYData::NoWindow);
	// Perform Fourier sine transform of current distribution function, over range specified, and with specified broadening functions (applied in Q space) and window applied (if requested)
	bool broadenedSineFT(double normFactor, double wMin, double wStep, double wMax, const Function& wBroadening = Function::unity(), const Function& wDependentBroadening = Function::unity(), XYData::WindowFunction wf = XYData::NoWindow);
	// Fourier transform current data, applying line-width broadening in real-space using the modified Lorch function
	bool transformLorch(double atomicDensity, double step, double rMax, double beta, double delta0, bool qToR);
	// Calculate S(Q) correlation function
	bool correlateSQ(double atomicDensity);


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
	double interpolated(double xvalue);
	// Return interpolated y value for supplied x, specifying containing interval
	double interpolated(double xvalue, int interval);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data
	bool broadcast(ProcessPool& procPool, int rootRank = 0);
};

#endif
