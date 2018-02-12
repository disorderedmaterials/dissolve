/*
	*** Data
	*** src/classes/data.h
	Copyright T. Youngs 2012-2018

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

#ifndef DUQ_DATA_H
#define DUQ_DATA_H

#include "base/charstring.h"
#include "base/xydata.h"
#include "classes/weights.h"
#include "templates/listitem.h"
#include "modules/partials/partials.h"

// Forward Declarations
class ProcessPool;

// Data
class Data : public ListItem<Data>
{
	public:
	// Constructor
	Data();
	// Destructor
	~Data();


	/*
	 * Name
	 */
	private:
	// Name of the Data
	CharString name_;
	// Nice name (generated from name_) used for output files
	CharString niceName_;

	public:
	// Set name of the Data
	void setName(const char* name);
	// Return name of the Data
	const char* name();
	// Return nice name of the Data
	const char* niceName();


	/*
	 * Data
	 */
	public:
	// DataType enum
	enum DataType { NeutronData, nDataTypes };
	// Convert text string to DataType
	static DataType dataType(const char* s);
	// Convert DataType to text string
	static const char* dataType(DataType dt);

	private:
	// Type of supplied data
	Data::DataType type_;
	// XY reference data
	XYData data_;
	// X value from which to calculate and subtract average Y level, if any
	double subtractAverageLevel_;
	// Normalisation style of laoded neutron data
	PartialsModule::NormalisationType neutronNormalisation_;
	// Weights matrix for scattering data
	Weights scatteringWeights_;

	public:
	// Type of supplied data
	void setType(Data::DataType dt);
	// Return type of supplied data
	Data::DataType type();
	// Load XY reference data from specified file
	bool loadData(ProcessPool& procPool, const char* filename, int xcol, int ycol);
	// Return XY reference data
	XYData& data();
	// Set X value from which to calculate and subtract average Y level, if any
	void setSubtractAverageLevel(double xMin);
	// Return X value from which to calculate and subtract average Y level, if any
	double subtractAverageLevel();
	// Set normalisation type for neutron data
	void setNeutronNormalisation(PartialsModule::NormalisationType normalisation);
	// Return normalisation type for neutron data
	PartialsModule::NormalisationType neutronNormalisation();
	// Return weights matrix for scattering data
	Weights& scatteringWeights();


	/*
	 * Associated Module
	 */
	private:
	// Name of associated Module for data, if any
	CharString associatedModuleName_;
	// Associated Module for data, if any
	Module* associatedModule_;

	public:
	// Set associated  name
	void setAssociatedModuleName(const char* name);
	// Return whether an associated Module name has been set
	bool hasAssociatedModuleName() const;
	// Return associated Module name
	const char* associatedModuleName() const;
	// Set associated Module
	void setAssociatedModule(Module* module);
	// Return associated Module
	Module* associatedModule();


	/*
	 * Set up
	 */
	private:
	// Whether the data has already been set up
	bool setUp_;

	public:
	// Perform necessary tasks (normalisation etc.) on supplied data
	bool setUp(GenericList& processingModuleData);
	// Return whether data is set up
	bool isSetUp() const;


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from root to all other processes
	bool broadcast(ProcessPool& procPool, int rootRank);
};

#endif
