/*
	*** Target Data
	*** src/gui/uchroma/classes/targetdata.h
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

#ifndef DISSOLVE_UCHROMATARGETDATA_H
#define DISSOLVE_UCHROMATARGETDATA_H

#include "gui/uchroma/classes/targetprimitive.h"

// Forward Declarations
class Collection;
class ViewPane;

// TargetData
class TargetData : public ListItem<TargetData>
{
	public:
	// Constructor / Destructor
	TargetData(ViewPane& parent);
	~TargetData();
	// Copy constructor
	TargetData(const TargetData& source);
	// Assignment operator
	void operator=(const TargetData& source);


	/*
	 * Parent
	 */
	private:
	// Parent ViewPane
	ViewPane& parent_;


	/*
	 * Target Collection and Derived Data
	 */
	public:
	// Generated Data Enum
	enum GeneratedDataType { FitResultsData, nGeneratedDataTypes };

	private:
	// Target collection
	Collection* collection_;
	// Generated Data
	Collection** generatedData_;

	private:
	// Clear and nullify all data / pointers
	void clear();
	// Add new additional data
	Collection* addData(TargetData::GeneratedDataType type);
	// Return specified collection data type (if it exists)
	Collection* data(TargetData::GeneratedDataType type);

	public:
	// Initialise with specified collection
	void initialise(Collection* collection);
	// Return target collection
	Collection* collection();


	/*
	 * Display Data
	 */
	private:
	// List of additional data
	List<TargetPrimitive> displayPrimitives_;

	public:
	// Add target primitive for specified collection
	void addDisplayPrimitive(Collection* collection);
	// Return first display primitive in list
	TargetPrimitive* displayPrimitives();
};

#endif
