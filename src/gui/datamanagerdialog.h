/*
    *** Data Manager Dialog
    *** src/gui/datamanagerdialog.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "gui/referencepoint.h"
#include "gui/ui_datamanagerdialog.h"
#include "templates/list.h"
#include <QDialog>

Q_DECLARE_METATYPE(ReferencePoint *)

// Forward Declarations
class Dissolve;
class GenericItem;

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
    // Append GenericItems to table under specified source
    void addItemsToTable(QTableWidget *table, List<GenericItem> &items, const QString locationName,
                         const QString locationIconResource);
    // Update the specified table of GenericItems, optionally filtering them by name and description
    void filterTable(QTableWidget *table, GenericItem *current, QString filter);
    // Update ReferencePoint table row
    void referencePointRowUpdate(int row, ReferencePoint *refPoint, bool createItems);
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
