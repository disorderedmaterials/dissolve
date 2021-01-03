// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
