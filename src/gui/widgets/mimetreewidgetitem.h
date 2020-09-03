/*
    *** Mime Tree Widget Item
    *** src/gui/widgets/mimetreewidgetitem.h
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

#pragma once

#include "gui/widgets/mimestrings.h"
#include "templates/list.h"
#include <QTreeWidgetItem>

// Forward declarations
/* none */

// Mime Tree Widget Item
class MimeTreeWidgetItem : public QTreeWidgetItem
{
    public:
    MimeTreeWidgetItem(QTreeWidget *parent, int type);
    MimeTreeWidgetItem(QTreeWidgetItem *parent, int type);

    /*
     * Mime Data
     */
    private:
    // Mime string data
    MimeStrings mimeStrings_;

    public:
    // Add mime data of specified type
    void addMimeString(MimeString::MimeStringType type, std::string_view data);
    // Return mime strings
    MimeStrings &mimeStrings();
};
