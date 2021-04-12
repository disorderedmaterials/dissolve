// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
