/*
	*** Mime Tree Widget Item
	*** src/gui/widgets/mimetreewidgetitem.h
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

#ifndef DUQ_MIMETREEWIDGETITEM_H
#define DUQ_MIMETREEWIDGETITEM_H

#include <QTreeWidgetItem>
#include "gui/widgets/mimestrings.h"
#include "templates/list.h"

// Forward declarations
/* none */

// Mime Tree Widget Item
class MimeTreeWidgetItem : public QTreeWidgetItem
{
	public:
	// Constructor
	MimeTreeWidgetItem(QTreeWidgetItem* parent, int type);


	/*
	 * Mime Data
	 */
	private:
	// Mime string data
	MimeStrings mimeStrings_;

	public:
	// Add mime data of specified type
	void addMimeString(MimeString::MimeStringType type, QString data);
	// Return mime strings
	MimeStrings& mimeStrings();
};

#endif

