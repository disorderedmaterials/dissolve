// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/ff/library.h"
#include "gui/ui_selectconfigurationwidget.h"
#include "templates/list.h"
#include <QWidget>

// Forward Declarations
class Dissolve;
class Configuration;

// Select Configuration Widget
class SelectConfigurationWidget : public QWidget
{
    Q_OBJECT

    public:
    SelectConfigurationWidget(QWidget *parent);
    ~SelectConfigurationWidget();

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SelectConfigurationWidget ui_;
    // Whether the widget is refreshing
    bool refreshing_;

    /*
     * Data
     */
    private:
    // CoreData containing available Configuration
    const CoreData *coreData_;
    // Minimum number of Configuration in a valid selection
    int minimumSelectionSize_;
    // Maximum number of Configuration in a valid selection (-1 for no limit)
    int maximumSelectionSize_;

    public:
    // Set CoreData containing available Configuration
    void setCoreData(const CoreData *coreData);
    // Reset widget, applying specified min and max limits to selection
    void reset(int minSize, int maxSize);

    /*
     * Update
     */
    private:
    // Update the list of Configuration
    void updateConfigurationList();

    /*
     * Signals / Slots
     */
    private slots:
    void on_SelectNoneButton_clicked(bool checked);
    void on_SelectAllButton_clicked(bool checked);
    void on_ConfigurationList_itemSelectionChanged();
    void on_ConfigurationList_itemDoubleClicked(QListWidgetItem *item);

    signals:
    void speciesSelectionChanged(bool isValid);
    void speciesDoubleClicked();

    private:
    // Return whether number of selected items is valid
    bool isSelectionValid() const;

    public:
    // Return number of species currently selected
    int nSelected() const;
    // Return the currently-selected Configuration
    RefList<Configuration> currentConfiguration() const;
};
