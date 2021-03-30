// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/models/dataManagerModel.h"
#include "gui/referencepoint.h"
#include "gui/ui_datamanagerdialog.h"
#include "templates/list.h"
#include <QDialog>
#include <vector>

Q_DECLARE_METATYPE(const ReferencePoint *)

// Forward Declarations
class Dissolve;
class GenericItem;

// Data Manager Dialog
class DataManagerDialog : public QDialog
{
    Q_OBJECT

    public:
    DataManagerDialog(QWidget *parent, Dissolve &dissolve, std::vector<ReferencePoint> &referencePoints);
    ~DataManagerDialog();

    private:
    // Reference to Dissolve
    Dissolve &dissolve_;
    // List of current ReferencePoints
    std::vector<ReferencePoint> &referencePoints_;
    // ReferencePoint Model
    DataManagerModel refModel_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::DataManagerDialog ui_;

    private:
    // Append GenericItems to table under specified source
    void addItemsToTable(QTableWidget *table, List<GenericItem> &items, const QString locationName,
                         const QString locationIconResource);
    // Update the specified table of GenericItems, optionally filtering them by name and description
    void filterTable(QTableWidget *table, GenericItem *current, QString filter);
    // Return currently-selected ReferencePoint
    ReferencePoint *currentReferencePoint() const;
    // Update controls
    void updateControls();

    private slots:
    // Simulation Data
    void on_SimulationDataFilterEdit_textChanged(const QString &text);
    // Reference Points
    void on_ReferencePointRemoveButton_clicked(bool checked);
    void on_ReferencePointCreateButton_clicked(bool checked);
    void on_ReferencePointOpenButton_clicked(bool checked);
    void on_ReferencePointsTable_currentItemChanged(QTableWidgetItem *currentItem, QTableWidgetItem *previousItem);

    // Dialog
    void on_CloseButton_clicked(bool checked);
};
