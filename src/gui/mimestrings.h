/*
	*** Mime Data
	*** src/gui/mimedata.h
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

#ifndef DUQ_MIMESTRINGS_H
#define DUQ_MIMESTRINGS_H

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
		UChromaCollectionBlockType,		/* UChroma-format 'Collection' block */
		nMimeStringTypes
	};
	// Constructor
	MimeString(MimeString::MimeStringType type, QString data);

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
	List<MimeString> mimeStrings_;

	public:
	// Add mime string
	void add(MimeString::MimeStringType type, QString data);
	// Add mime strings from source MimeStrings
	void add(MimeStrings& sourceStrings);
	// Return mime strings
	List<MimeString>& mimeStrings();
};

#endif

