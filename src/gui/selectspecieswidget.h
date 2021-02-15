// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/ff/library.h"
#include "gui/ui_selectspecieswidget.h"
#include "templates/list.h"
#include <QWidget>

// Forward Declarations
class Dissolve;
class Species;

// Select Species Widget
class SelectSpeciesWidget : public QWidget
{
    Q_OBJECT

    public:
    SelectSpeciesWidget(QWidget *parent);
    ~SelectSpeciesWidget();

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SelectSpeciesWidget ui_;
    // Whether the widget is refreshing
    bool refreshing_;

    /*
     * Data
     */
    private:
    // CoreData containing available Species
    const CoreData *coreData_;
    // Minimum number of Species in a valid selection
    int minimumSelectionSize_;
    // Maximum number of Species in a valid selection (-1 for no limit)
    int maximumSelectionSize_;

    public:
    // Set CoreData containing available Species
    void setCoreData(const CoreData *coreData);
    // Reset widget, applying specified min and max limits to selection
    void reset(int minSize, int maxSize);

    /*
     * Update
     */
    private:
    // Update the list of Species
    void updateSpeciesList();

    /*
     * Signals / Slots
     */
    private slots:
    void on_SelectNoneButton_clicked(bool checked);
    void on_SelectAllButton_clicked(bool checked);
    void on_SpeciesList_itemSelectionChanged();
    void on_SpeciesList_itemDoubleClicked(QListWidgetItem *item);

    signals:
    void speciesSelectionChanged(bool isValid);
    void speciesDoubleClicked();

    private:
    // Return whether number of selected items is valid
    bool isSelectionValid() const;

    public:
    // Return number of species currently selected
    int nSelected() const;
    // Return the currently-selected Species
    RefList<Species> currentSpecies() const;
};
