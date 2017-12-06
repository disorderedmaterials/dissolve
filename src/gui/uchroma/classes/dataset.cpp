/*
	*** DataSet
	*** src/gui/uchroma/classes/dataset.cpp
	Copyright T. Youngs 2013-2017

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
const char* DataSourceKeywords[] = { "File", "Internal", "XYData" };

// Convert text string to SettingsKeyword
DataSet::DataSource DataSet::dataSource(const char* s)
{
	for (int n=0; n<nDataSources; ++n) if (DUQSys::sameString(s, DataSourceKeywords[n])) return (DataSet::DataSource) n;
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
	sourceXYData_ = NULL;
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
	sourceXYData_ = source.sourceXYData_;
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

// Return source XYData, if any
const char* DataSet::sourceXYData() const
{
	return sourceXYData_.get();
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
		data_.arrayX().clear();
		data_.arrayY().clear();

		// Check file exists
		if (!QFile::exists(sourceDir.absoluteFilePath(sourceFileName_.get())))
		{
			Messenger::error("File '%s' not found.\n", qPrintable(sourceFileName_));
			return false;
		}

		// Read in the data
		return data_.load(qPrintable(sourceDir.absoluteFilePath(sourceFileName_.get())));
	}
	else if (dataSource_ == DataSet::XYDataSource)
	{
		// Locate the data...
		XYData* sourceData = XYData::findObject(sourceXYData_);
		if (sourceData)
		{
			// Store the current z, copy the data, then re-set z
			double z = data_.z();
			setData(*sourceData);
			data_.setZ(z);
		}
		else
		{
			Messenger::warn("Couldn't locate data '%s' for display.\n", sourceXYData_.get());
			data_.arrayX().clear();
			data_.arrayY().clear();

			notifyParent();

			return false;
		}
	}

	return true;
}

// Set data from supplied XYData
void DataSet::setData(XYData& source)
{
	data_ = source;

	notifyParent();
}

// Set data from supplied XYData
void DataSet::setSourceData(const char* xyDataObjectName)
{
	sourceXYData_ = xyDataObjectName;

	dataSource_ = DataSet::XYDataSource;

	refreshData();
}

// Return data
const XYData& DataSet::data() const
{
	return data_;
}

// Return X array from data
const Array<double>& DataSet::x() const
{
	return data_.constArrayX();
}

// Return Y array from data
const Array<double>& DataSet::y() const
{
	return data_.constArrayY();
}

// Return z value from data
double DataSet::z() const
{
	return data_.constZ();
}

// Transform original data with supplied transformers
void DataSet::transform(Transformer& xTransformer, Transformer& yTransformer, Transformer& zTransformer)
{
	// X
	if (xTransformer.enabled()) transformedData_.arrayX() = xTransformer.transformArray(data_.arrayX(), data_.arrayY(), data_.z(), 0);
	else transformedData_.arrayX() = data_.arrayX();

	// Y
	if (yTransformer.enabled()) transformedData_.arrayY() = yTransformer.transformArray(data_.arrayX(), data_.arrayY(), data_.z(), 1);
	else transformedData_.arrayY() = data_.arrayY();

	// Z
	if (zTransformer.enabled()) transformedData_.setZ(zTransformer.transform(0.0, 0.0, data_.z()));
	else transformedData_.setZ(data_.z());
}

// Return transformed data
XYData& DataSet::transformedData()
{
	return transformedData_;
}

/*
 * Data Operations
 */

// Reset (zero) data
void DataSet::resetData()
{
	data_.arrayX() = 0.0;

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
	data_.setX(index, newX);

	notifyParent();
}

// Set y value
void DataSet::setY(int index, double newY)
{
	data_.setY(index, newY);

	notifyParent();
}

// Set z data
void DataSet::setZ(double z)
{
	data_.setZ(z);

	notifyParent();
}

// Add to specified axis value
void DataSet::addConstantValue(int axis, double value)
{
	if (axis == 0) data_.arrayX() += value;
	else if (axis == 1) data_.arrayY() += value;
	else if (axis == 2) data_.addZ(value);

	notifyParent();
}

// Calculate average y value over x range specified
double DataSet::averageY(double xMin, double xMax) const
{
	double result = 0.0;
	int nAdded = 0;
	for (int n=0; n<data_.nPoints(); ++n)
	{
		if (data_.x(n) < xMin) continue;
		else if (data_.x(n) > xMax) break;
		++nAdded;
		result += data_.y(n);
	}
	return (nAdded == 0 ? 0.0 : result / nAdded);
}
