// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/delegates/null.h"

NullDelegate::NullDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

// Create editor
QWidget *NullDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return nullptr;
}
