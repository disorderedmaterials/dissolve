/*
    *** Procedure Editor
    *** src/gui/procedureeditor.h
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

#include "gui/ui_procedureeditor.h"
#include "templates/refdatalist.h"

// Forward Declarations
class CoreData;
class LineParser;
class MimeTreeWidgetItem;
class Procedure;
class ProcedureChart;
class ModulePalette;

// Procedure Editor
class ProcedureEditor : public QWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    ProcedureEditor(QWidget *parent = nullptr);
    ~ProcedureEditor();

    /*
     * Procedure Target
     */
    private:
    // Procedure to be edited
    Procedure *procedure_;
    // Module categories and their associated MimeTreeWidgetItems
    RefDataList<MimeTreeWidgetItem, QString> moduleCategories_;

    public:
    // Set up to display / edit the target Procedure
    void setUp(Procedure *procedure, const CoreData &coreData);

    /*
     * Update
     */
    private:
    // Whether the widget is currently refreshing
    bool refreshing_;

    public:
    // Update controls
    void updateControls();
    // Disable sensitive controls within widget
    void disableSensitiveControls();
    // Enable sensitive controls within widget
    void enableSensitiveControls();

    /*
     * Controls
     */
    private:
    // Main form declaration
    Ui::ProcedureEditor ui_;
    // Pointer to created ProcedureChart
    ProcedureChart *chart_;

    private slots:
    // 	void on_AvailableModulesTree_itemDoubleClicked(QTreeWidgetItem* item);

    /*
     * State
     */
    public:
    // Write widget state through specified LineParser
    bool writeState(LineParser &parser) const;
    // Read widget state through specified LineParser
    bool readState(LineParser &parser);

    /*
     * Signals / Slots
     */
    private slots:
    // Data displayed in the chart has been modified
    void chartDataModified();
    // Required size of the chart widget has changed
    void chartSizeChanged();

    signals:
    // Notify that the Procedure's data has changed
    void dataModified();
};
