/*
	*** EditStateDataData
	*** src/gui/uchroma/session/editstatedata.cpp
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

#include "gui/uchroma/session/editstatedata.h"
#include "gui/uchroma/classes/collection.h"
#include "base/messenger.h"
#include "base/sysfunc.h"

// Constructor
EditStateData::EditStateData() : ListItem<EditStateData>()
{
	type_ = EditStateData::nDataTypes;
	name_ = "";
	dataI_ = 0;
	dataD_ = 0.0;
	dataS_ = "";
}

/*
 * Data
 */

// Set data (int)
void EditStateData::set(const char* name, int value)
{
	name_ = name;
	type_ = EditStateData::IntegerData;
	dataI_ = value;
}

// Set data (double)
void EditStateData::set(const char* name, double value)
{
	name_ = name;
	type_ = EditStateData::DoubleData;
	dataD_ = value;
}

// Set data (QString)
void EditStateData::set(const char* name, const char* value)
{
	name_ = name;
	type_ = EditStateData::StringData;
	dataS_ = value;
}

// Set data (from Collection*)
void EditStateData::set(const char* name, Collection* value)
{
	name_ = name;
	type_ = EditStateData::CollectionData;
	dataCollection_ = (*value);
}

// Set data (from XYData*)
void EditStateData::set(const char* name, XYData* value)
{
	name_ = name;
	type_ = EditStateData::XYDataData;
	dataXYData_ = (*value);
}

// Set data (from LineStyle&)
void EditStateData::set(const char* name, LineStyle& value)
{
	name_ = name;
	type_ = EditStateData::LineStyleData;
	dataLineStyle_ = value;
}

// Return whether name of contained data matches that supplied
bool EditStateData::isName(const char* name)
{
	return DUQSys::sameString(name_, name);
}

// Return type of contained data
EditStateData::EditStateDataType EditStateData::type()
{
	return type_;
}

// Return integer value
int EditStateData::asInt()
{
	if (type_ == EditStateData::IntegerData) return dataI_;
	else printf("Internal Error - Can't convert type %i into an integer.\n", type_);
	return 0;
}

// Return double value
double EditStateData::asDouble()
{
	if (type_ == EditStateData::DoubleData) return dataD_;
	else printf("Internal Error - Can't convert type %i into a double.\n", type_);
	return 0.0;
}

// Return string value
const char* EditStateData::asString()
{
	if (type_ == EditStateData::StringData) return dataS_.get();
	else printf("Internal Error - Can't convert type %i into a QString.\n", type_);
	return "NULL";
}

// Return XYData value
XYData& EditStateData::asXYData()
{
	if (type_ == EditStateData::XYDataData) return dataXYData_;
	else printf("Internal Error - Can't convert type %i into a XYData.\n", type_);
	return dataXYData_;
}

// Return Collection value
Collection& EditStateData::asCollection()
{
	if (type_ == EditStateData::CollectionData) return dataCollection_;
	else printf("Internal Error - Can't convert type %i into a Collection.\n", type_);
	return dataCollection_;
}

// Return LineStyle value
LineStyle EditStateData::asLineStyle()
{
	if (type_ == EditStateData::IntegerData) return dataLineStyle_;
	else printf("Internal Error - Can't convert type %i into a LineStyle.\n", type_);
	return dataLineStyle_;
}
