/*
	*** Data Collection
	*** src/gui/view/render/collection.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/viewer/render/collection.h"
#include "gui/viewer/render/view.h"
#include "math/interpolator.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include <limits>

// Static Members
template<class Collection> RefList<Collection,int> ObjectStore<Collection>::objects_;
template<class Collection> int ObjectStore<Collection>::objectCount_ = 0;
template<class Collection> int ObjectStore<Collection>::objectType_ = ObjectInfo::UChromaCollectionObject;
template<class Collection> const char* ObjectStore<Collection>::objectTypeName_ = "UChromaCollection";

// Constructor
Collection::Collection() : ListItem<Collection>(), ObjectStore<Collection>(this)
{
	// Set variable defaults
	dataSets_.clear();
	name_ = "Empty Collection";
	dataMin_.zero();
	dataMax_.set(10.0, 10.0, 10.0);
	dataVersion_ = 0;
	identifier_ = -1;

	// Transform
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

	// Update
	limitsAndTransformsVersion_ = -1;

	// Display
	visible_ = true;
	displayData_.clear();
	displayDataGeneratedAt_ = -1;
	displayStyle_ = Collection::LineXYStyle;
	displaySurfaceShininess_ = 128.0;
	displayStyleVersion_ = 0;
}

// Destructor
Collection::~Collection()
{
}

// Copy constructor
Collection::Collection(const Collection& source) : ObjectStore<Collection>(this)
{
	(*this) = source;
}

// Assignment operator
void Collection::operator=(const Collection& source)
{
	// Basic Data
	name_ = source.name_;
	dataSets_ = source.dataSets_;
	dataMin_ = source.dataMin_;
	dataMax_ = source.dataMax_;
	dataVersion_ = 0;

	// Transforms
	transformMin_ = source.transformMin_;
	transformMax_ = source.transformMax_;
	transformMinPositive_ = source.transformMinPositive_;
	transformMaxPositive_ = source.transformMaxPositive_;
	transforms_[0] = source.transforms_[0];
	transforms_[1] = source.transforms_[1];
	transforms_[2] = source.transforms_[2];
	limitsAndTransformsVersion_ = source.limitsAndTransformsVersion_;

	// Group
	groupName_ = source.groupName_;

	// Update
	limitsAndTransformsVersion_ = -1;

	// Display
	visible_ = source.visible_;
	colour_ = source.colour_;
	displayData_.clear();
	displayDataGeneratedAt_ = -1;
	displayStyle_ = source.displayStyle_;
	displaySurfaceShininess_ = source.displaySurfaceShininess_;
	displayLineStyle_ = source.displayLineStyle_;
	displayStyleVersion_ = 0;
}

/*
 * Data
 */

// Set name
void Collection::setName(const char* name)
{
	name_ = name;
}

// Return name
const char* Collection::name()
{
	return name_;
}

// Set identifier
void Collection::setIdentifier(int identifier)
{
	identifier_ = identifier;
}

// Return identifier
int Collection::identifier()
{
	return identifier_;
}

// Add dataset
DataSet* Collection::addDataSet()
{
	// Create new dataset
	DataSet* dataSet = dataSets_.add();
	dataSet->setParent(this);

	++dataVersion_;

	return dataSet;
}

// Add dataset at specified z
DataSet* Collection::addDataSet(double z)
{
	DataSet* dataSet = dataSets_.add();
	dataSet->setParent(this);
	setDataSetZ(dataSet, z);

	return dataSet;
}

// Add dataset from supplied Data1D
DataSet* Collection::addDataSet(Data1D& data, double z)
{
	DataSet* dataSet = dataSets_.add();
	dataSet->setParent(this);

	dataSet->setData(data);
	setDataSetZ(dataSet, z);

	return dataSet;
}

// Add dataset from supplied Data1D
DataSet* Collection::addDataSetWithReference(Data1D* data, double z)
{
	DataSet* dataSet = dataSets_.add();
	dataSet->setParent(this);

	dataSet->setSourceData(data->objectTag());
	setDataSetZ(dataSet, z);

	return dataSet;
}

// Add dataset, copying from supplied DataSet
void Collection::addDataSet(DataSet* source)
{
	// Create new dataset
	DataSet* dataSet = addDataSet();

	// Copy existing dataset data, overwriting parent just to make sure
	(*dataSet) = (*source);
	dataSet->setParent(this);

	++dataVersion_;
}

// Add dataset, copying from supplied DisplayDataSet
void Collection::addDataSet(const Array<double>& abscissa, DisplayDataSet* source, bool pruneEmpty)
{
	// Create new dataset
	DataSet* dataSet = addDataSet();

	// Grab references
	const Array<double>& y = source->y();
	const Array<DisplayDataSet::DataPointType>& yType = source->yType();

	// Check sizes
	if (abscissa.nItems() != y.nItems())
	{
		Messenger::print("Collection::addDataSet() : Abscissa and y arrays are of different sizes.\n");
		return;
	}

	// Loop over points, adding points as we go (and pruning truly empty points if necessary)
	for (int n=0; n<abscissa.nItems(); ++n)
	{
		if (pruneEmpty && (yType.constAt(n) == DisplayDataSet::NoPoint)) continue;
		dataSet->addPoint(abscissa.constAt(n), y.constAt(n));
	}

	++dataVersion_;
}

// Copy datasets from specified source collection
void Collection::copyDataSets(Collection* source)
{
	for (DataSet* dataSet = source->dataSets(); dataSet != NULL; dataSet = dataSet->next) addDataSet(dataSet);
}

// Return number of datasets
int Collection::nDataSets()
{
	return dataSets_.nItems();
}

// Remove dataset
void Collection::removeDataSet(DataSet* dataSet)
{
	dataSets_.remove(dataSet);

	++dataVersion_;
}

// Set z value of specified dataset
void Collection::setDataSetZ(DataSet* target, double z)
{
	bool minBad = true, maxBad = true;
	int dummy = 0;

	// Check that this DataSet is owned by the collection
	if (!dataSets_.contains(target))
	{
		Messenger::print("Internal Error : Tried to set the Z value of a dataset using the wrong collection.\n");
		return;
	}
	target->setZ(z);

	// Adjust position of the dataset in the list if necessary
	do
	{
		// Shift item if necessary
		if (target->prev && (target->prev->z() > target->z()))
		{
			// Shift specified target up the list
			dataSets_.shiftUp(target);
			minBad = (target->prev ? (target->prev->z() > target->z()) : false);
		}
		else minBad = false;
		if (target->next && (target->next->z() < target->z()))
		{
			// Move specified target down the list
			dataSets_.shiftDown(target);
			maxBad = (target->next ? (target->next->z() < target->z()) : false);
		}
		else maxBad = false;
		if (++dummy == 10) break;
		
	} while (minBad || maxBad);

	++dataVersion_;
}

// Set data for specified dataset (from source DataSet)
void Collection::setDataSetData(DataSet* target, DataSet& source)
{
	// Check that this DataSet is owned by the collection
	if (!dataSets_.contains(target))
	{
		Messenger::print("Internal Error : Tried to set the data of a dataset using the wrong collection.\n");
		return;
	}

	(*target) = source;

	++dataVersion_;
}

// Return first dataset in list
DataSet* Collection::dataSets() const
{
	return dataSets_.first();
}

// Return named dataset
DataSet* Collection::dataSet(const char* name)
{
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next) if (DissolveSys::sameString(dataSet->name(), name)) return dataSet;
	return NULL;
}

// Return last dataset in list
DataSet* Collection::lastDataSet()
{
	return dataSets_.last();
}

// Return nth dataset in list
DataSet* Collection::dataSet(int index)
{
	return dataSets_[index];
}

// Return index of specified dataset
int Collection::dataSetIndex(const char* name)
{
	int index = 0;
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next, ++index) if (dataSet->name() == name) return index;
	return -1;
}

// Return unique name based on supplied basename
const char* Collection::uniqueDataSetName(const char* baseName)
{
	static CharString testName;
	
	testName = baseName;
	int index = 0;
	do
	{
		// Add on suffix (if index > 0)
		if (index > 0) testName.sprintf("%s (%i)", baseName, index);
		++index;
	} while (dataSet(testName));

	return testName;
}

// Return number of slices with no data present
int Collection::nEmptyDataSets()
{
	int count = 0;
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next) if (dataSet->data().nValues() < 2) ++count;
	return count;
}

// Clear dataset data from collection
void Collection::clearDataSets()
{
	dataSets_.clear();
	displayData_.clear();

	++dataVersion_;
}

// Return total number of points across all datasets
int Collection::nValues()
{
	int nPoints = 0;
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next) nPoints += dataSet->data().nValues();
	return nPoints;
}

// Return data minima, calculating if necessary
Vec3<double> Collection::dataMin()
{
	// Make sure limits and transform are up to date
	updateLimitsAndTransforms();

	return dataMin_;
}

// Return data maxima, calculating if necessary
Vec3<double> Collection::dataMax()
{
	// Make sure limits and transform are up to date
	updateLimitsAndTransforms();

	return dataMax_;
}

// Increase data version (i.e. notify that data has been changed)
void Collection::notifyDataChanged()
{
	++dataVersion_;
}

// Return version counter for changes to data
int Collection::dataVersion()
{
	return dataVersion_;
}

/*
 * Transforms
 */

// Return transformed data minima, calculating if necessary
Vec3<double> Collection::transformMin()
{
	// Make sure limits and transform are up to date
	updateLimitsAndTransforms();

	return transformMin_;
}

// Return transformed data maxima, calculating if necessary
Vec3<double> Collection::transformMax()
{
	// Make sure limits and transform are up to date
	updateLimitsAndTransforms();

	return transformMax_;
}

// Return transformed positive data minima, calculating if necessary
Vec3<double> Collection::transformMinPositive()
{
	// Make sure limits and transform are up to date
	updateLimitsAndTransforms();

	return transformMinPositive_;
}

// Return transformed positive data maxima, calculating if necessary
Vec3<double> Collection::transformMaxPositive()
{
	// Make sure limits and transform are up to date
	updateLimitsAndTransforms();

	return transformMaxPositive_;
}

// Set transform equation for data
void Collection::setTransformEquation(int axis, const char* transformEquation)
{
	transforms_[axis].setEquation(transformEquation);

	// Make sure limits and transform are up to date
	if (transforms_[axis].enabled())
	{
		++dataVersion_;
		updateLimitsAndTransforms();
	}
}

// Return transform equation for data
const char* Collection::transformEquation(int axis)
{
	return transforms_[axis].text();
}

// Return whether specified transform equation is valid
bool Collection::transformEquationValid(int axis)
{
	return transforms_[axis].valid();
}

// Set whether specified transform is enabled
void Collection::setTransformEnabled(int axis, bool enabled)
{
	transforms_[axis].setEnabled(enabled);

	++dataVersion_;

	// Make sure limits and transform are up to date
	updateLimitsAndTransforms();
}

// Return whether specified transform is enabled
bool Collection::transformEnabled(int axis)
{
	return transforms_[axis].enabled();
}

/*
 * Data Operations
 */

// Add to specified axis value`
void Collection::addConstantValue(int axis, double value)
{
	// Loop over datasets
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next) dataSet->addConstantValue(axis, value);

	++dataVersion_;

	updateLimitsAndTransforms();
}

// Calculate min/max y value over specified x range
void Collection::yRangeOverX(double xMin, double xMax, double& yMin, double& yMax)
{
	bool first = true;
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next)
	{
		const Data1D& data = dataSet->transformedData();
		for (int n=0; n<data.nValues(); ++n)
		{
			if (data.constXAxis(n) < xMin) continue;
			else if (data.constXAxis(n) > xMax) break;

			if (first)
			{
				yMin = data.constValue(n);
				yMax = yMin;
				first = false;
			}
			else
			{
				if (data.constValue(n) < yMin) yMin = data.constValue(n);
				else if (data.constValue(n) > yMax) yMax = data.constValue(n);
			}
		}
	}
}

/*
 * Update
 */

// Update data limits and transform data
void Collection::updateLimitsAndTransforms()
{
	if (dataVersion_ == limitsAndTransformsVersion_) return;

	dataMin_ = 0.0;
	dataMax_ = 0.0;
	if (dataSets_.nItems() > 0)
	{
		// Grab first dataset and set initial values
		DataSet* dataSet = dataSets_.first();
		dataMin_.set(dataSet->data().constXAxis().firstValue(), dataSet->data().minValue(), dataSet->z());
		dataMax_.set(dataSet->data().constXAxis().lastValue(), dataSet->data().maxValue(), dataSet->z());
		double mmin, mmax;
		for (dataSet = dataSet->next; dataSet != NULL; dataSet = dataSet->next)
		{
			mmin = dataSet->data().constXAxis().firstValue();
			mmax = dataSet->data().constXAxis().lastValue();
			if (mmin < dataMin_.x) dataMin_.x = mmin;
			if (mmax > dataMax_.x) dataMax_.x = mmax;
			mmin = dataSet->data().minValue();
			mmax = dataSet->data().maxValue();
			if (mmin < dataMin_.y) dataMin_.y = mmin;
			if (mmax > dataMax_.y) dataMax_.y = mmax;
			if (dataSet->z() < dataMin_.z) dataMin_.z = dataSet->z();
			else if (dataSet->z() > dataMax_.z) dataMax_.z = dataSet->z();
		}
	}

	// Loop over dataSets_ list, updating transforms as we go
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next) dataSet->transform(transforms_[0], transforms_[1], transforms_[2]);

	transformMin_ = 0.0;
	transformMax_ = 0.0;
	transformMinPositive_ = 0.1;
	transformMaxPositive_ = -1.0;
	if (dataSets_.nItems() == 0) return;
	
	// Grab first dataset and set initial values
	DataSet* dataSet = dataSets_.first();
	transformMin_.set(dataSet->transformedData().constXAxis().firstValue(), dataSet->transformedData().minValue(), dataSet->transformedZ());
	transformMax_.set(dataSet->transformedData().constXAxis().lastValue(), dataSet->transformedData().maxValue(), dataSet->transformedZ());
	double mmin, mmax;
	for (dataSet = dataSet->next; dataSet != NULL; dataSet = dataSet->next)
	{
		mmin = dataSet->transformedData().constXAxis().firstValue();
		mmax = dataSet->transformedData().constXAxis().lastValue();
		if (mmin < transformMin_.x) transformMin_.x = mmin;
		if (mmax > transformMax_.x) transformMax_.x = mmax;
		mmin = dataSet->transformedData().minValue();
		mmax = dataSet->transformedData().maxValue();
		if (mmin < transformMin_.y) transformMin_.y = mmin;
		if (mmax > transformMax_.y) transformMax_.y = mmax;
		if (dataSet->transformedZ() < transformMin_.z) transformMin_.z = dataSet->transformedZ();
		else if (dataSet->transformedZ() > transformMax_.z) transformMax_.z = dataSet->transformedZ();
	}

	// Now determine minimum positive limits
	for (dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next)
	{
		// Loop over XY points in data, searching for first positive, non-zero value
		const Data1D& data = dataSet->transformedData();
		for (int n=0; n<data.nValues(); ++n)
		{
			// X
			if (data.constXAxis(n) > 0.0)
			{
				if (data.constXAxis(n) < transformMinPositive_.x) transformMinPositive_.x = data.constXAxis(n);
				if (data.constXAxis(n) > transformMaxPositive_.x) transformMaxPositive_.x = data.constXAxis(n);
			}
			// Y
			if (data.constValue(n) > 0.0)
			{
				if (data.constValue(n) < transformMinPositive_.y) transformMinPositive_.y = data.constValue(n);
				if (data.constValue(n) > transformMaxPositive_.y) transformMaxPositive_.y = data.constValue(n);
			}
		}
		
		// Z
		if (dataSet->z() > 0.0)
		{
			if (dataSet->z() < transformMinPositive_.z) transformMinPositive_.z = dataSet->z();
			if (dataSet->z() > transformMaxPositive_.z) transformMaxPositive_.z = dataSet->z();
		}
	}

	// Check maximum positive values (since all datapoints might have been negative
	if (transformMaxPositive_.x < 0.0) transformMaxPositive_.x = 1.0;
	if (transformMaxPositive_.y < 0.0) transformMaxPositive_.y = 1.0;
	if (transformMaxPositive_.z < 0.0) transformMaxPositive_.z = 1.0;

	// Update version
	limitsAndTransformsVersion_ = dataVersion_;
}

/*
 * Group
 */

// Set group that this Collection is associated to
void Collection::setGroupName(const char* groupName)
{
	groupName_ = groupName;
}

// Return whether this Collection is associated to a group
bool Collection::hasGroupName() const
{
	return (!groupName_.isEmpty());
}

// Return group name that this Collection is associated to
const char* Collection::groupName() const
{
	return groupName_.get();
}

/*
 * Display
 */

// Suface Style Keywords
const char* SurfaceStyleKeywords[] = { "LineXY", "LineZY", "Grid", "Surface", "UnlitSurface" };

// Convert text string to DisplayStyle
Collection::DisplayStyle Collection::displayStyle(const char* s)
{
	for (int n=0; n<nDisplayStyles; ++n) if (DissolveSys::sameString(s, SurfaceStyleKeywords[n])) return (Collection::DisplayStyle) n;
	return nDisplayStyles;
}

// Convert DisplayStyle to text string
const char* Collection::displayStyle(Collection::DisplayStyle kwd)
{
	return SurfaceStyleKeywords[kwd];
}

// Generate display data
void Collection::updateDisplayData()
{
	if (dataVersion_ == displayDataGeneratedAt_) return;

	// Make sure transforms are up to date
	updateLimitsAndTransforms();

	// Clear old displayData_ and create temporary Data1D list for display data construction
	List<Data1D> transformedData;
	displayData_.clear();

	// Loop over slices, apply any transforms (X or Y) and check limits
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next)
	{
		// Check for slice with no points...
		if (dataSet->data().nValues() == 0) continue;

		// Z
		double z = dataSet->transformedZ();

		// Add new item to transformedData and displayData_ arrays
		Data1D* surfaceDataSet = transformedData.add();
		DisplayDataSet* displayDataSet = displayData_.add();
		displayDataSet->setZ(z);

		// Copy raw data arrays
		Array<double> array[2];
		array[0] = dataSet->transformedData().constXAxis();
		array[1] = dataSet->transformedData().constValues();

		// Now add data to surfaceDataSet
		for (int n=0; n<array[0].nItems(); ++n) surfaceDataSet->addPoint(array[0].constAt(n), array[1].constAt(n));
	}

	// Construct common x scale for data, and create y value data
	// We have already pruned out those slices with no data points, so no checks for this are necessary
	int n, nFinished = 0, nTransformedDataSets = transformedData.nItems();
	Data1D** data = transformedData.array();
	double lowest;

	// -- Set up initial array indices and nFinished count
	Array<int> i(nTransformedDataSets);
	for (n=0; n<nTransformedDataSets; ++n)
	{
		if (data[n]->nValues() == 0)
		{
			i[n] = -1;
			++nFinished;
		}
		else i[n] = 0;
	}
	DisplayDataSet** slices = displayData_.array();
	displayAbscissa_.clear();

	// -- Loop over all datasets simultaneously, seeking next lowest point in their x data
	while (nFinished != nTransformedDataSets)
	{
		// Find lowest point of current x values
		lowest = std::numeric_limits<double>::max();
		for (n = 0; n< nTransformedDataSets; ++n)
		{
			// If we have exhausted this slice's data, move on
			if (i[n] == -1) continue;
			if (data[n]->xAxis(i[n]) <= lowest) lowest = data[n]->xAxis(i[n]);
		}

		// Now have lowest x value, so add new x point to abscissa...
		displayAbscissa_.add(lowest);

		// ...and add y values/flags from slice data to new displayData_
		for (n = 0; n< nTransformedDataSets; ++n)
		{
			// If we have exhausted this slice's data, add a dummy value.
			// Otherwise, check how close the X-value is to 'lowest'
			if (i[n] == -1) slices[n]->addDummy();
			else if (fabs(data[n]->xAxis(i[n]) - lowest) < 1.0e-5)
			{
				slices[n]->add(data[n]->value(i[n]), DisplayDataSet::RealPoint);
				++i[n];
				if (i[n] == data[n]->nValues())
				{
					i[n] = -1;
					++nFinished;
				}
			}
			else slices[n]->addDummy();
		}
	}

	// Finally, interpolate values over dummy points (where the dummy points are surrounded by actual values)
	int lastReal, m, o;
	double yWidth, xWidth, position;
	for (n = 0; n < nTransformedDataSets; ++n)
	{
		lastReal = -1;
		const Array<DisplayDataSet::DataPointType>& yType = slices[n]->yType();
		const Array<double>& y = slices[n]->y();
		for (m = 0; m < displayAbscissa_.nItems(); ++m)
		{
			// If this point is a real value, interpolate up to here from the last real point (if one exists and it is more than one element away)
			if (yType.constAt(m) == DisplayDataSet::RealPoint)
			{
				if (lastReal == -1) lastReal = m;
				else if ((m - lastReal) == 1) lastReal = m;
				else
				{
					// Interpolate from 'lastReal' index up to here
					xWidth = displayAbscissa_[m] - displayAbscissa_[lastReal]; 
					yWidth = y.constAt(m) - y.constAt(lastReal);
					for (o = lastReal+1; o<=m; ++o)
					{
						position = (displayAbscissa_[o] - displayAbscissa_[lastReal]) / xWidth;
						slices[n]->setY(o, y.constAt(lastReal) + yWidth*position, DisplayDataSet::InterpolatedPoint);
					}
				}
				lastReal = m;
			}
		}	
	}
	
	// Store new version 
	displayDataGeneratedAt_ = dataVersion_;
}

// Return local colour definition for display
ColourDefinition& Collection::colour()
{
	return colour_;
}

// Set whether data is visible
void Collection::setVisible(bool visible)
{
	visible_ = visible;
}

// Return hether data is visible
bool Collection::visible()
{
	return visible_;
}

// Return transformed display abscissa for data
const Array<double>& Collection::displayAbscissa()
{
	updateDisplayData();

	return displayAbscissa_;
}

// Return transformed data to display
List<DisplayDataSet>& Collection::displayData()
{
	updateDisplayData();

	return displayData_;
}

// Set display style of data
void Collection::setDisplayStyle(DisplayStyle style)
{
	displayStyle_ = style;

	++displayStyleVersion_;
}

// Return display style of data
Collection::DisplayStyle Collection::displayStyle()
{
	return displayStyle_;
}

// Return line style
LineStyle& Collection::displayLineStyle()
{
	return displayLineStyle_;
}

// Set surface shininess
void Collection::setDisplaySurfaceShininess(double shininess)
{
	displaySurfaceShininess_ = shininess;
}

// Return surface shininess
double Collection::displaySurfaceShininess()
{
	return displaySurfaceShininess_;
}

// Return style version
int Collection::displayStyleVersion()
{
	return displayStyleVersion_;
}

/*
 * File
 */

// Export data to plain text file
bool Collection::exportData(const char* fileName)
{
	LineParser parser;
	parser.openOutput(qPrintable(fileName), true);

	if (!parser.isFileGoodForWriting()) return false;

	parser.writeLineF("# Exported data: '%s'\n", name_.get());

	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next)
	{
		parser.writeLineF("# Z = %e\n", dataSet->z());
		for (int n=0; n<dataSet->data().nValues(); ++n)
		{
			const Array<double>& x = dataSet->data().constXAxis();
			const Array<double>& y = dataSet->data().constValues();
			parser.writeLineF("%e  %e\n", x.constAt(n), y.constAt(n));
		}
		parser.writeLineF("\n");
	}

	parser.closeFiles();

	return true;
}
