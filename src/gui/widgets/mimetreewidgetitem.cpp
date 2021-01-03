// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
