/*
	*** Edit State Data
	*** src/gui/uchroma/session/editstatedata.h
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

#ifndef DUQ_UCHROMAEDITSTATEDATA_H
#define DUQ_UCHROMAEDITSTATEDATA_H

#include "base/messenger.h"
#include "gui/uchroma/classes/collection.h"
#include "gui/uchroma/render/linestyle.h"
#include "templates/list.h"

// Forward Declarations
/* none */

// Edit State Data
class EditStateData : public ListItem<EditStateData>
{
	public:
	// Constructor
	EditStateData();


	/*
	 * Data
	 */
	public:
	// Data Type
	enum EditStateDataType { IntegerData, DoubleData, StringData, XYDataData, LineStyleData, CollectionData, nDataTypes };

	private:
	// Name of contained data
	CharString name_;
	// Type of contained data
	EditStateDataType type_;
	// Associated integer data
	int dataI_;
	// Associated double data
	double dataD_;
	// Associated string data
	CharString dataS_;
	// Associated Collection data
	Collection dataCollection_;
	// Associated LineStyle data
	LineStyle dataLineStyle_;
	// Associated XYData data
	XYData dataXYData_;

	public:
	// Set data (int)
	void set(const char* name, int value);
	// Set data (double)
	void set(const char* name, double value);
	// Set data (const char*)
	void set(const char* name, const char* value);
	// Set data (from Collection*)
	void set(const char* name, Collection* value);
	// Set data (from XYData*)
	void set(const char* name, XYData* value);
	// Set data (from LineStyle&)
	void set(const char* name, LineStyle& value);
	// Return whether name of contained data matches that supplied
	bool isName(const char* name);
	// Return type of contained data
	EditStateDataType type();
	// Return integer value
	int asInt();
	// Return double value
	double asDouble();
	// Return string value
	const char* asString();
	// Return reference to XYData value
	XYData& asXYData();
	// Return reference to Collection value
	Collection& asCollection();
	// Return LineStyle value
	LineStyle asLineStyle();
};

#endif
