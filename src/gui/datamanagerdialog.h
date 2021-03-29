// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "genericitems/list.h"
#include "gui/referencepoint.h"
#include "gui/ui_datamanagerdialog.h"
#include "templates/list.h"
#include <QDialog>

Q_DECLARE_METATYPE(const ReferencePoint *)

// Forward Declarations
class Dissolve;

// Data Manager Dialog
class DataManagerDialog : public QDialog
{
    Q_OBJECT

    public:
    DataManagerDialog(QWidget *parent, Dissolve &dissolve, List<ReferencePoint> &referencePoints);
    ~DataManagerDialog();

    private:
    // Reference to Dissolve
    Dissolve &dissolve_;
    // List of current ReferencePoints
    List<ReferencePoint> &referencePoints_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::DataManagerDialog ui_;

    private:
    // Add GenericItems to table
    void addItems(const std::map<std::string, GenericItem::Type> &items);
    // Update the specified table of GenericItems, optionally filtering them by name and description
    void filterTable(QString filterText);
    // Update ReferencePoint table row
    void referencePointRowUpdate(int row, const ReferencePoint *refPoint, bool createItems);
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
