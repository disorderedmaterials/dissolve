// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QtWidgets/QStyledItemDelegate>

class NullDelegate : public QStyledItemDelegate
{
    public:
    NullDelegate(QObject *parent = 0);
    // Reimplemented virtual function to create editing widget
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
