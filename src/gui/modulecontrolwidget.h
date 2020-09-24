// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "gui/ui_modulecontrolwidget.h"
#include "templates/variantpointer.h"

// Forward Declarations
class Dissolve;
class DissolveWindow;
class Module;
class ModuleWidget;
class ModuleBlock;

// Module Control Widget
class ModuleControlWidget : public QWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    ModuleControlWidget(QWidget *parent);
    ~ModuleControlWidget();

    private:
    // Whether the widget is currently refreshing
    bool refreshing_;

    /*
     * Setup
     */
    public:
    // Set up links to main window
    void setUp(DissolveWindow *dissolveWindow);

    /*
     * Module Target
     */
    private:
    // Pointer to Dissolve
    Dissolve *dissolve_;
    // Associated Module
    Module *module_;

    private slots:
    // Run the set-up stage of the associated Module
    void setUpModule();

    public:
    // Set target Module to display
    void setModule(Module *module, Dissolve *dissolve);

    signals:
    void updateModuleWidget(int flags);

    /*
     * Update
     */
    public:
    // Update controls within widget
    void updateControls();
    // Disable sensitive controls
    void disableSensitiveControls();
    // Enable sensitive controls
    void enableSensitiveControls();

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::ModuleControlWidget ui_;

    private slots:
    // Keyword data for Module has been modified
    void keywordDataModified();

    public slots:
    void on_NameEdit_editingFinished();
    void on_NameEdit_returnPressed();
    void on_EnabledButton_clicked(bool checked);
    void on_FrequencySpin_valueChanged(int value);
    void on_ConfigurationTargetList_itemChanged(QListWidgetItem *item);

    signals:
    // Notify that the Module's data has been modified in some way
    void dataModified();
};
