/*
	*** Object Data
	*** src/gui/objectdata.hui
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

#ifndef DUQ_OBJECTDATA_H
#define DUQ_OBJECTDATA_H

#include <QMimeData>
#include "base/charstring.h"
#include "templates/list.h"

// Forward declarations
/* none */

// Object Data
class ObjectData : public QMimeData
{
	public:
	// Constructor
	ObjectData();


	/*
	 * Reimplementations
	 */
	public:
	bool hasFormat(const QString& mimeType) const;
	QStringList formats() const;
	QVariant retrieveData(const QString& mimeType, QVariant::Type type) const;


	/*
	 * ObjectData
	 */
	private:
	// List of objects
	QStringList objects_;

	public:
	// Add object item
	void addObject(const char* objectName);
	// Return object list
	QStringList objects();
};

#endif

