// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/models/atomTypeFilterProxy.h"
#include "gui/models/atomTypeModel.h"
#include "gui/ui_selectAtomTypeWidget.h"
#include <QWidget>

// Forward Declarations
class AtomType;

// Select AtomType Widget
class SelectAtomTypeWidget : public QWidget
{
    Q_OBJECT

    public:
    SelectAtomTypeWidget(QWidget *parent);
    ~SelectAtomTypeWidget() override = default;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SelectAtomTypeWidget ui_;
    // Model for the AtomType list
    AtomTypeModel atomTypeModel_;
    // Filter proxy for the AtomType model
    AtomTypeFilterProxy atomTypeFilterProxy_;

    /*
     * Data
     */
    public:
    // Set filter element
    void setFilterElement(Elements::Element Z);
    // Clear filter element
    void clearFilterElement();
    // Set target AtomType data
    void setAtomTypes(const std::vector<std::shared_ptr<AtomType>> &atomTypes,
                      OptionalReferenceWrapper<const CoreData> coreData = std::nullopt);
    // Reset widget
    void reset();

    /*
     * Signals / Slots
     */
    private slots:
    void selectionChanged(const QItemSelection &, const QItemSelection &);
    void on_AtomTypeList_doubleClicked(const QModelIndex &index);

    signals:
    void atomTypeSelectionChanged(bool isValid);
    void atomTypeDoubleClicked();

    public:
    // Return the currently-selected AtomType
    const std::shared_ptr<AtomType> currentAtomType() const;
};
