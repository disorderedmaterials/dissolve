/*
	*** Object Data
	*** src/gui/objectdata_funcs.cpp
	Copyright T. Youngs 2012-2017

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

#include "gui/objectdata.h"

// Constructor
ObjectData::ObjectData()
{
}

/*
 * Reimplementations
 */

bool ObjectData::hasFormat(const QString& mimeType) const
{
	if (mimeType == "duq/objectdata") return true;

	return false;
}

QStringList ObjectData::formats() const
{
	return QStringList() << "duq/objectdata";
}

QVariant ObjectData::retrieveData(const QString& mimeType, QVariant::Type type) const
{
	if (mimeType == "duq/objectdata") return objects_;

	return QVariant();
}

/*
 * ObjectData
 */

// Add object item
void ObjectData::addObject(const char* objectName)
{
	objects_ << objectName;
}

// Return object list
QStringList ObjectData::objects()
{
	return objects_;
}
