/*
    *** Mime Tree Widget Item
    *** src/gui/widgets/mimetreewidgetitem.cpp
    Copyright T. Youngs 2012-2020

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

#include "gui/widgets/mimetreewidgetitem.h"

MimeTreeWidgetItem::MimeTreeWidgetItem(QTreeWidget *parent, int type) : QTreeWidgetItem(parent, type) {}
MimeTreeWidgetItem::MimeTreeWidgetItem(QTreeWidgetItem *parent, int type) : QTreeWidgetItem(parent, type) {}

/*
 * Mime Data
 */

// Add mime data of specified type
void MimeTreeWidgetItem::addMimeString(MimeString::MimeStringType type, std::string_view data) { mimeStrings_.add(type, data); }

// Return mime strings
MimeStrings &MimeTreeWidgetItem::mimeStrings() { return mimeStrings_; }
