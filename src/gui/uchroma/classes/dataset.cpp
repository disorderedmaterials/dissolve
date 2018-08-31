/*
	*** DataSet
	*** src/gui/uchroma/classes/dataset.cpp
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

#include "gui/uchroma/classes/dataset.h"
#include "gui/uchroma/classes/collection.h"

// Data Sources
const char* DataSourceKeywords[] = { "File", "Internal", "Data1D" };

// Convert text string to SettingsKeyword
DataSet::DataSource DataSet::dataSource(const char* s)
{
	for (int n=0; n<nDataSources; ++n) if (DissolveSys::sameString(s, DataSourceKeywords[n])) return (DataSet::DataSource) n;
	return nDataSources;
}

// Convert SettingsKeyword to text string
const char* DataSet::dataSource(DataSet::DataSource kwd)
{
	return DataSourceKeywords[kwd];
}

// Constructor
DataSet::DataSet() : ListItem<DataSet>()
{
	sourceFileName_ = "";
	dataSource_ = DataSet::InternalSource;
	name_ = "New DataSet";
	parent_ = NULL;
	sourceData1D_ = NULL;
}

// Destructor
DataSet::~DataSet()
{
}

// Copy constructor
DataSet::DataSet(const DataSet& source)
{
	(*this) = source;
}

// Assignment operator
void DataSet::operator=(const DataSet& source)
{
	sourceFileName_ = source.sourceFileName_;
	name_ = source.name_;
	data_ = source.data_;
	transformedData_ = source.transformedData_;
	dataSource_ = source.dataSource_;
	sourceData1D_ = source.sourceData1D_;
}

/*
 * Parent Collection
 */

// Set parent
void DataSet::setParent(Collection* parent)
{
	parent_ = parent;
}

// Return parent
Collection* DataSet::parent()
{
	return parent_;
}

// Notify parent that data has changed
void DataSet::notifyParent()
{
	if (parent_) parent_->notifyDataChanged();
}

/* 
 * Data
 */

// Set source of data
void DataSet::setDataSource(DataSet::DataSource source)
{
	dataSource_ = source;
}

// Return source of data
DataSet::DataSource DataSet::dataSource()
{
	return dataSource_;
}

// Set source filename
void DataSet::setSourceFileName(const char* fileName)
{
	sourceFileName_ = fileName;
}

// Return source filename
const char* DataSet::sourceFileName()
{
	return sourceFileName_.get();
}

// Return source Data1D, if any
const char* DataSet::sourceData1D() const
{
	return sourceData1D_.get();
}

// Set associated data name
void DataSet::setName(const char* name)
{
	name_ = name;
}

// Return name
const char* DataSet::name()
{
	return name_;
}

// Refresh source data (if not internal)
bool DataSet::refreshData(QDir sourceDir)
{
	if (dataSource_ == DataSet::InternalSource) return false;
	else if (dataSource_ == DataSet::FileSource)
	{
		// Clear any existing data
		data_.x().clear();
		data_.y().clear();

		// Check file exists
		if (!QFile::exists(sourceDir.absoluteFilePath(sourceFileName_.get())))
		{
			Messenger::error("File '%s' not found.\n", sourceFileName_.get());
			return false;
		}

		// Read in the data
		return data_.load(qPrintable(sourceDir.absoluteFilePath(sourceFileName_.get())));
	}
	else if (dataSource_ == DataSet::Data1DSource)
	{
		// Locate the data...
		Data1D* sourceData = Data1D::findObject(sourceData1D_);
		if (sourceData) setData(*sourceData);
		else
		{
			Messenger::printVerbose("Couldn't locate data '%s' for display.\n", sourceData1D_.get());
			data_.x().clear();
			data_.y().clear();

			notifyParent();

			return false;
		}
	}

	return true;
}

// Set data from supplied Data1D
void DataSet::setData(Data1D& source)
{
	data_ = source;

	notifyParent();
}

// Set data from supplied Data1D
void DataSet::setSourceData(const char* xyDataObjectName)
{
	sourceData1D_ = xyDataObjectName;

	dataSource_ = DataSet::Data1DSource;

	refreshData();
}

// Return data
const Data1D& DataSet::data() const
{
	return data_;
}

// Return X array from data
const Array<double>& DataSet::x() const
{
	return data_.constX();
}

// Return Y array from data
const Array<double>& DataSet::y() const
{
	return data_.constY();
}

// Return z value from data
double& DataSet::z()
{
	return z_;
}

// Transform original data with supplied transformers
void DataSet::transform(Transformer& xTransformer, Transformer& yTransformer, Transformer& zTransformer)
{
	// X
	if (xTransformer.enabled()) transformedData_.x() = xTransformer.transformArray(data_.x(), data_.y(), z_, 0);
	else transformedData_.x() = data_.x();

	// Y
	if (yTransformer.enabled()) transformedData_.y() = yTransformer.transformArray(data_.x(), data_.y(), z_, 1);
	else transformedData_.y() = data_.y();

	// Z
	if (zTransformer.enabled()) transformedZ_ = zTransformer.transform(0.0, 0.0, z_);
	else transformedZ_ = z_;
}

// Return transformed data
const Data1D& DataSet::transformedData() const
{
	return transformedData_;
}

// Return transformed z value
double DataSet::transformedZ() const
{
	return transformedZ_;
}

/*
 * Data Operations
 */

// Reset (zero) data
void DataSet::resetData()
{
	data_.x() = 0.0;

	notifyParent();
}

// Initialise data to specified number of points
void DataSet::initialiseData(int nPoints)
{
	data_.initialise(nPoints);

	notifyParent();
}

// Add point to data
void DataSet::addPoint(double x, double y)
{
	data_.addPoint(x, y);

	notifyParent();
}

// Set x value
void DataSet::setX(int index, double newX)
{
	data_.x(index) = newX;

	notifyParent();
}

// Set y value
void DataSet::setY(int index, double newY)
{
	data_.y(index) = newY;

	notifyParent();
}

// Set z data
void DataSet::setZ(double z)
{
	z_ = z;

	notifyParent();
}

// Add to specified axis value
void DataSet::addConstantValue(int axis, double value)
{
	if (axis == 0) data_.x() += value;
	else if (axis == 1) data_.y() += value;
	else if (axis == 2) z_ += value;

	notifyParent();
}

// Calculate average y value over x range specified
double DataSet::averageY(double xMin, double xMax) const
{
	double result = 0.0;
	int nAdded = 0;
	for (int n=0; n<data_.nDataPoints(); ++n)
	{
		if (data_.constX(n) < xMin) continue;
		else if (data_.constX(n) > xMax) break;
		++nAdded;
		result += data_.constY(n);
	}
	return (nAdded == 0 ? 0.0 : result / nAdded);
}
