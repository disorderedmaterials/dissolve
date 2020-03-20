/*
    *** Module Control Widget
    *** src/gui/modulecontrolwidget.h
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
    // Constructor / Destructor
    ModuleControlWidget(QWidget *parent = nullptr);
    ~ModuleControlWidget();

    private:
    // Lock for widget refresh
    Lock refreshLock_;

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
    // Return target Module for the widget
    Module *module() const;

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
