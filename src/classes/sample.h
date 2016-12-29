/*
	*** Sample Definition
	*** src/classes/sample.h
	Copyright T. Youngs 2012-2016

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

#ifndef DUQ_SAMPLE_H
#define DUQ_SAMPLE_H

#include "modules/modulelist.h"
#include "base/data2d.h"
#include "base/charstring.h"
#include "templates/list.h"
#include "templates/genericlist.h"

// Forward Declarations
class Species;

// Sample Definition
class Sample : public ListItem<Sample>
{
	public:
	// Constructor
	Sample();
	// Copy Constructor
	Sample(const Sample& source);
	// Assignment Operator
	void operator=(const Sample& source);


	/*
	 * Basic Information
	 */
	private:
	// Descriptive name
	CharString name_;

	public:
	// Set name of Sample
	void setName(const char* name);
	// Return name of Sample
	const char* name() const;


	/*
	 * Reference Data
	 */
	private:
	// Whether reference data exists
	bool hasReferenceData_;
	// Filename of reference data (if present)
	CharString referenceDataFileName_;
	// Reference data
	Data2D referenceData_;
	// Calculated data
	Data2D calculatedData_;

	public:
	// Return whether reference data exists
	bool hasReferenceData();
	// Load reference data
	bool loadReferenceData(const char* filename);
	// Return reference data filename (if any)
	CharString& referenceDataFileName();
	// Return reference data
	Data2D& referenceData();
	// Return calculated data
	Data2D& calculatedData();


	/*
	 * Modules
	 */
	private:
	// List of Modules associated to this Sample
	ModuleList modules_;
	// Variables set by Modules
	GenericList moduleData_;

	public:
	// Add Module (or an instance of it) to the Sample
	Module* addModule(Module* masterInstance, bool autoAddDependents);
	// Return number of Modules associated to this Sample
	int nModules() const;
	// Return list of Modules associated to this Sample
	RefList<Module,bool>& modules();
	// Return list of variables set by Modules
	GenericList& moduleData();


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool);
};

#endif
