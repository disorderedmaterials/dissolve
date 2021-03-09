// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/lock.h"
#include "gui/ui_modulecontrolwidget.h"
#include "templates/variantpointer.h"

// Forward Declarations
class ConfigurationRefListKeyword;
class ConfigurationRefListKeywordWidget;
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
    ModuleControlWidget(QWidget *parent = nullptr);
    ~ModuleControlWidget();

    private:
    // Lock for widget refresh
    Lock refreshLock_;

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
    // Return target Module for the widget
    Module *module() const;

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
    // Keyword widget for Configuration editing
    ConfigurationRefListKeywordWidget *configurationsWidget_;
    // Additional controls widget for the Module (if any)
    ModuleWidget *moduleWidget_;

    private slots:
    void on_ModuleControlsButton_clicked(bool checked);
    void on_ModuleOutputButton_clicked(bool checked);
    // Keyword data for Module has been modified
    void keywordDataModified();

    signals:
    // Notify that the Module's data has been modified in some way
    void dataModified();
};
