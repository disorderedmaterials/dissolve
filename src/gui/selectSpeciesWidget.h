// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/library.h"
#include "gui/models/speciesFilterProxy.h"
#include "gui/models/speciesModel.h"
#include "gui/ui_selectSpeciesWidget.h"
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
    ~SelectSpeciesWidget() override = default;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SelectSpeciesWidget ui_;
    // Whether the widget is refreshing
    bool refreshing_;
    // Model for the Species list
    SpeciesModel speciesModel_;
    // Filter proxy for the Species model
    SpeciesFilterProxy speciesFilterProxy_;

    /*
     * Data
     */
    private:
    // Minimum number of Species in a valid selection
    int minimumSelectionSize_;
    // Maximum number of Species in a valid selection (if specified)
    std::optional<int> maximumSelectionSize_;

    public:
    // Set flags for the filter proxy
    void setFilterProxyFlags(const Flags<SpeciesFilterProxy::FilterFlags> &flags);
    // Set target Species data
    void setSpecies(const std::vector<std::unique_ptr<Species>> &species);
    // Reset widget, applying specified min and max limits to selection
    void reset(int minSize, std::optional<int> maxSize = std::nullopt);

    /*
     * Signals / Slots
     */
    private slots:
    void selectionChanged(const QItemSelection &, const QItemSelection &);
    void on_SelectNoneButton_clicked(bool checked);
    void on_SelectAllButton_clicked(bool checked);
    void on_SpeciesList_doubleClicked(const QModelIndex &index);

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
    std::vector<const Species *> selection() const;
};
