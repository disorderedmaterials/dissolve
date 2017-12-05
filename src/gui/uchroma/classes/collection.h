/*
	*** Data Collection
	*** src/gui/uchroma/classes/collection.h
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

#ifndef DUQ_UCHROMACOLLECTION_H
#define DUQ_UCHROMACOLLECTION_H

#include "gui/uchroma/classes/dataset.h"
#include "gui/uchroma/classes/displaydataset.h"
#include "gui/uchroma/classes/transformer.h"
#include "gui/uchroma/classes/colourdef.h"
#include "gui/uchroma/render/linestyle.h"
#include "gui/uchroma/render/primitivelist.h"
#include "templates/objectstore.h"

// Forward Declarations
class FitKernel;
class UChromaBase;

class Collection : public ListItem<Collection>, public ObjectStore<Collection>
{
	public:
	// Constructor
	Collection();
	// Destructor
	~Collection();
	// Copy constructor
	Collection(const Collection& source);
	// Assignment operator
	void operator=(const Collection& source);
	

	/*
	 * Link to UChromaBase
	 */
	private:
	// Link to UChromaBase
	static UChromaBase* uChromaBase_;

	public:
	// Set link to UChromaBase
	static void setUChromaBase(UChromaBase* uChromaBase);


	/*
	 * Data
	 */
	private:
	// Name of collection
	CharString name_;
	// List of datasets
	List<DataSet> dataSets_;
	// Root directory for datafiles
	QDir dataFileDirectory_;
	// Extreme values of raw data
	Vec3<double> dataMin_, dataMax_;
	// Identifier
	int identifier_;
	// Version counter for changes to data
	int dataVersion_;

	public:
	// Set name of collection
	void setName(const char* title);
	// Return name
	const char* name();
	// Return collection locator
	const char* locator(const char* currentPath = NULL);
	// Set identifier
	void setIdentifier(int identifier);
	// Return identifier
	int identifier();
	// Add dataset
	DataSet* addDataSet();
	// Add dataset at specified z
	DataSet* addDataSet(double z);
	// Add dataset from supplied XYData
	DataSet* addDataSet(XYData& data, double z = 0.0);
	// Add dataset from supplied XYData
	DataSet* addDataSetWithReference(XYData* data, double z = 0.0);
	// Add dataset, copying from supplied DataSet
	void addDataSet(DataSet* source);
	// Add dataset, copying from supplied DisplayDataSet
	void addDataSet(const Array<double>& abscissa, DisplayDataSet* source, bool pruneEmpty = true);
	// Copy datasets from specified source collection
	void copyDataSets(Collection* source);
	// Return number of datasets
	int nDataSets();
	// Remove dataset
	void removeDataSet(DataSet* dataSet);
	// Set z value of specified dataset
	void setDataSetZ(DataSet* target, double z);
	// Set data for specified dataset (from source DataSet)
	void setDataSetData(DataSet* target, DataSet& source);
	// Return first dataset in list
	DataSet* dataSets() const;
	// Return named dataset
	DataSet* dataSet(const char* name);
	// Return last dataset in list
	DataSet* lastDataSet();
	// Return nth dataset in list
	DataSet* dataSet(int index);
	// Return index of specified dataset
	int dataSetIndex(const char* name);
	// Return unique name based on supplied basename
	const char* uniqueDataSetName(const char* baseName);
	// Return number of dataset with no data present
	int nEmptyDataSets();
	// Clear dataset data from collection
	void clearDataSets();
	// Return total number of points across all dataset
	int nDataPoints();
	// Set root directory for datafiles
	void setDataFileDirectory(QDir directory);
	// Return root directory for datafiles
	QDir dataFileDirectory();
	// Append dataset to collection
	bool appendDataSet(const char* fileName);
	// Load specified dataset
	bool loadDataSet(DataSet* dataSet);
	// Refresh (load or re-copy) data for all datasets
	int refreshDataSets();
	// Return data minima, calculating if necessary
	Vec3<double> dataMin();
	// Return data maxima, calculating if necessary
	Vec3<double> dataMax();
	// Increase data version (i.e. notify that data has been changed)
	void notifyDataChanged();
	// Return version counter for changes to data
	int dataVersion();


	/*
	 * Transform
	 */
	private:
	// Extreme values of transformed data 
	Vec3<double> transformMin_, transformMax_;
	// Extreme positive values of transformed data
	Vec3<double> transformMinPositive_, transformMaxPositive_;
	// Transform for data
	Transformer transforms_[3];
	// Interpolation flags
	Vec3<bool> interpolate_, interpolateConstrained_;
	// Interpolation step sizes
	Vec3<double> interpolationStep_;

	public:
	// Return transformed data minima, calculating if necessary
	Vec3<double> transformMin();
	// Return transformed data maxima, calculating if necessary
	Vec3<double> transformMax();
	// Return transformed positive data minima
	Vec3<double> transformMinPositive();
	// Return transformed positive data maxima
	Vec3<double> transformMaxPositive();
	// Set transform equation for data
	void setTransformEquation(int axis, const char* transformEquation);
	// Return transform equation for data
	const char* transformEquation(int axis);
	// Return whether specified transform equation is valid
	bool transformEquationValid(int axis);
	// Set whether specified transform is enabled
	void setTransformEnabled(int axis, bool enabled);
	// Return whether specified transform is enabled
	bool transformEnabled(int axis);
	// Set whether interpolation is enabled
	void setInterpolate(int axis, bool enabled);
	// Return whether interpolation is enabled
	bool interpolate(int axis);
	// Set whether interpolation is constrained
	void setInterpolateConstrained(int axis, bool enabled);
	// Return whether interpolation is constrained
	bool interpolateConstrained(int axis);
	// Set interpolation step size
	void setInterpolationStep(int axis, double step);
	// Return interpolation step size
	double interpolationStep(int axis);


	/*
	 * Data Operations
	 */
	public:
	// Add to specified axis value`
	void addConstantValue(int axis, double value);


	/*
	 * Associated Data
	 */
	public:
	// Collection Type
	enum CollectionType
	{
		MasterCollection,
		FitCollection,
		ExtractedCollection,
		CurrentSliceCollection
	};

	private:
	// Parent Collection
	Collection* parent_;
	// Type of this collection
	CollectionType type_;
	// List of fits
	List<Collection> fits_;
	// List of extracted slices
	List<Collection> slices_;
	// Current slice data
	Collection* currentSlice_;
	// FitKernel (if a FitCollection)
	FitKernel* fitKernel_;

	private:
	// Return axis bin value of closest point to supplied value
	int closestBin(int axis, double value);
	// Get slice at specified axis and bin
	void getSlice(int axis, int bin);

	public:
	// Locate collection using parts specified
	Collection* locateCollection(const QStringList& parts, int offset);
	// Return next logical collection in lists
	Collection* nextCollection(bool descend);
	// Return previous logical collection in lists
	Collection* previousCollection(bool descend);
	// Set parent Collection
	void setParent(Collection* parent);
	// Return parent Collection
	Collection* parent();
	// Return type of this collection
	CollectionType type();
	// Return icon string reflecting this Collection's type / status
	const char* iconString(bool isUsed = true);
	// Return locally-unique fit name based on basename provided
	const char* uniqueFitName(const char* baseName);
	// Add fit to Collection
	Collection* addFit(const char* name, int position = -1);
	// Remove specified fit from list
	void removeFit(Collection* collection);
	// Return fit data
	Collection* fits();
	// Return fit with name specified
	Collection* fit(const char* name);
	// Return locally-unique slice name based on basename provided
	const char* uniqueSliceName(const char* baseName);
	// Add slice to Collection
	Collection* addSlice(const char* name, int position = -1);
	// Remove specified slice
	void removeSlice(Collection* collection);
	// Return slices
	Collection* slices();
	// Return slice with name specified
	Collection* slice(const char* name);
	// Update current slice based on specified axis and bin
	void updateCurrentSlice(int axis, double axisValue);
	// Extract current slice based on specified axis and bin
	void extractCurrentSlice(int axis, double axisValue);
	// Return current slice
	Collection* currentSlice();
	// Add FitKernel, if one does not exist
	void addFitKernel();
	// Return FitKernel
	FitKernel* fitKernel();


	/*
	 * Update
	 */
	private:
	// Data version at which limits and transforms were last updated
	int limitsAndTransformsVersion_;

	private:
	// Update data limits and transform data
	void updateLimitsAndTransforms();


	/*
	 * Group
	 */
	private:
	// Name of group that this Collection is associated to (if any)
	CharString group_;

	public:
	// Set group that this Collection is associated to
	void setGroup(const char* groupName);
	// Return whether this Collection is associated to a group
	bool hasGroup() const;
	// Return group name that this Collection is associated to
	const char* group() const;


	/*
	 * Display
	 */
	public:
	// Display types enum
	enum DisplayStyle { LineXYStyle, LineZYStyle, GridStyle, SurfaceStyle, UnlitSurfaceStyle, nDisplayStyles };
	// Convert text string to DisplayStyle
	static DisplayStyle displayStyle(const char* s);
	// Convert DisplayStyle to text string
	static const char* displayStyle(DisplayStyle kwd);

	private:
	// Colour definition for display
	ColourDefinition colour_;
	// Whether data is visible
	bool visible_;
	// Transformed data to display
	List<DisplayDataSet> displayData_;
	// Data version at which displayData_ was last generated
	int displayDataGeneratedAt_;
	// Abscissa values for display data
	Array<double> displayAbscissa_;
	// Display style of data
	DisplayStyle displayStyle_;
	// Line style
	LineStyle displayLineStyle_;
	// Surface shininess
	double displaySurfaceShininess_;
	// Style version
	int displayStyleVersion_;

	private:
	// Generate display data
	void updateDisplayData();

	public:
	// Return local colour definition for display
	ColourDefinition& colour();
	// Return colour definition to use for display (from group if defined)
	const ColourDefinition& displayColour() const;
	// Set whether data is visible
	void setVisible(bool visible);
	// Return hether data is visible
	bool visible();
	// Return transformed display abscissa for data
	const Array<double>& displayAbscissa();
	// Return transformed data to display
	List<DisplayDataSet>& displayData();
	// Set display style of data
	void setDisplayStyle(DisplayStyle style);
	// Return display style of data
	DisplayStyle displayStyle();
	// Return line style
	LineStyle& displayLineStyle();
	// Set surface shininess
	void setDisplaySurfaceShininess(double shininess);
	// Return surface shininess
	double displaySurfaceShininess();
	// Return style version
	int displayStyleVersion();


	/*
	 * File
	 */
	public:
	// Export data to plain text file
	bool exportData(const char* fileName);
};

#endif
