// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/isotopes.h"
#include <QComboBox>
#include <QModelIndex>
#include <QObject>
#include <QtWidgets/QItemDelegate>

// Delegate class, providing list of Isotopes for one element in a QComboBox
class IsotopeComboDelegate : public QItemDelegate
{
    Q_OBJECT

    public:
    IsotopeComboDelegate(QObject *parent);
    virtual ~IsotopeComboDelegate() = default;
    // Reimplemented virtual function to create editing widget
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    // Set the data to appear when the editor is called
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    // Extract data from editor, and place back into source model
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    // Resize editor
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /*
     * Helper Functions
     */
    public:
    // Return text for Isotope specified
    static QString textForIsotope(Sears91::Isotope isotope);
};
