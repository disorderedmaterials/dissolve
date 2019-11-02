/*
	*** Mime Data
	*** src/gui/widgets/mimedata.h
	Copyright T. Youngs 2012-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_MIMESTRINGS_H
#define DISSOLVE_MIMESTRINGS_H

#include <QMimeData>
#include "templates/listitem.h"
#include "templates/list.h"

// Forward declarations
/* none */

// Mime String Object
class MimeString : public ListItem<MimeString>
{
	public:
	// Mime Strings Data Type
	enum MimeStringType
	{
		NoMimeType,
		LocalType,			/* Local block identifier */
		ModuleType,			/* Module type */
		nMimeStringTypes
	};
	// Constructor
	MimeString(MimeString::MimeStringType type = NoMimeType, QString data = QString());

	private:
	// Type of data contained in string
	MimeStringType type_;
	// String data
	QString data_;

	public:
	// Return type of data contained in string
	MimeStringType type() const;
	// Return string data
	QString data() const;
};

// Mime Strings Data
class MimeStrings : public QMimeData
{
	public:
	// Constructor
	MimeStrings();
	// Destructor
	~MimeStrings();
	// Copy Constructor
	MimeStrings(const MimeStrings& source);
	// Assignment Operator
	void operator=(const MimeStrings& source);


	/*
	 * Reimplementations
	 */
	public:
	bool hasFormat(const QString& mimeType) const;
	QStringList formats() const;
	QVariant retrieveData(const QString& mimeType, QVariant::Type type) const;


	/*
	 * Mime Data
	 */
	private:
	// List of mime strings
	List<MimeString> strings_;

	public:
	// Add mime string
	void add(MimeString::MimeStringType type, QString data);
	// Add mime strings from source MimeStrings
	void add(MimeStrings& sourceStrings);
	// Return whether the specified MimeString data is present
	bool hasData(MimeString::MimeStringType type) const;
	// Return the data for the specified type
	QString data(MimeString::MimeStringType type) const;
	// Return mime strings
	List<MimeString>& strings();
};

#endif

