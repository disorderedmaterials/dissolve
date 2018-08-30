/*
	*** DataSet
	*** src/gui/uchroma/classes/dataset.h
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

#ifndef DISSOLVE_UCHROMADATASET_H
#define DISSOLVE_UCHROMADATASET_H

#include "math/xydata.h"
#include "gui/uchroma/classes/transformer.h"
#include "templates/listitem.h"
#include <QDir>

// Forward Declarations
class Collection;

// DataSet
class DataSet : public ListItem<DataSet>
{
	public:
	// Constructor / Destructor
	DataSet();
	~DataSet();
	// Copy constructor
	DataSet(const DataSet& source);
	// Assignment operator
	void operator=(const DataSet& source);
	// Data Sources
	enum DataSource { FileSource, InternalSource, XYDataSource, nDataSources };
	// Convert text string to DataSource
	static DataSource dataSource(const char* s);
	// Convert DataSource to text string
	static const char* dataSource(DataSource kwd);


	/*
	 * Parent Collection
	 */
	private:
	// Parent collection
	Collection* parent_;

	private:
	// Notify parent that data has changed
	void notifyParent();

	public:
	// Set parent
	void setParent(Collection* parent);
	// Return parent
	Collection* parent();


	/*
	 * Data
	 */
	private:
	// Source of data
	DataSource dataSource_;
	// Source file, if one is specified (not including path)
	CharString sourceFileName_;
	// Name
	CharString name_;
	// Data
	XYData data_;
	// Transformed data
	XYData transformedData_;
	// Z value of data
	double z_;
	// Transformed Z value of data
	double transformedZ_;
	// Source XYData object name
	CharString sourceXYData_;

	public:
	// Set source of data
	void setDataSource(DataSet::DataSource source);
	// Return source of data
	DataSet::DataSource dataSource();
	// Set source filename
	void setSourceFileName(const char* fileName);
	// Return source filename
	const char* sourceFileName();
	// Set name
	void setName(const char* name);
	// Return name
	const char* name();
	// Set data from supplied XYData
	void setData(XYData& source);
	// Set data from supplied XYData
	void setSourceData(const char* xyDataObjectName);
	// Return source XYData, if any
	const char* sourceXYData() const;
	// Refresh source data (if not internal)
	bool refreshData(QDir sourceDir = QDir());
	// Return data
	const XYData& data() const;
	// Return X array from data
	const Array<double>& x() const;
	// Return Y array from data
	const Array<double>& y() const;
	// Return z value
	double& z();
	// Transform original data with supplied transformers
	void transform(Transformer& xTransformer, Transformer& yTransformer, Transformer& zTransformer);
	// Return transformed data
	const XYData& transformedData() const;
	// Return transformed z value
	double transformedZ() const;


	/*
	 * Data Operations
	 */
	public:
	// Reset (zero) data
	void resetData();
	// Initialise data to specified number of points
	void initialiseData(int nPoints);
	// Add point to data
	void addPoint(double x, double y);
	// Set x value
	void setX(int index, double newX);
	// Set y value
	void setY(int index, double newY);
	// Set z data
	void setZ(double z);
	// Add to specified axis value`
	void addConstantValue(int axis, double value);
	// Calculate average y value over x range specified
	double averageY(double xMin, double xMax) const;
};

#endif
