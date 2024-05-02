// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/lock.h"
#include "gui/ui_moduleControlWidget.h"
#include "modules/widget.h"

// Forward Declarations
class Dissolve;
class DissolveWindow;
class KeywordsWidget;
class Module;
class ModuleWidget;
class ProcedureWidget;

// Module Control Widget
class ModuleControlWidget : public QWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    ModuleControlWidget(DissolveWindow *dissolveWindow, Module *module);
    ~ModuleControlWidget() = default;

    private:
    // Lock for widget refresh
    Lock refreshLock_;

    /*
     * Module Target
     */
    private:
    // Reference to Dissolve
    Dissolve &dissolve_;
    // Associated Module
    Module *module_;

    public:
    // Return target Module for the widget
    Module *module();

    /*
     * Update
     */
    public:
    // Update controls within widget
    void updateControls(Flags<ModuleWidget::UpdateFlags> updateFlags = {});
    // Disable editing
    void preventEditing();
    // Allow editing
    void allowEditing();

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::ModuleControlWidget ui_;
    // Additional controls widget for the Module (if any)
    ModuleWidget *moduleWidget_{nullptr};
    // Procedure widget for the module (if any)
    ProcedureWidget *procedureWidget_{nullptr};
    // Keyword widgets for the module (if any)
    std::vector<KeywordsWidget *> keywordWidgets_;
    // Map of button widgets to related stack indices
    std::map<QPushButton *, int> controlStackMap_;

    private Q_SLOTS:
    void switchControlStackPage(QPushButton *button);
    void keywordGroupButtonClicked(bool checked);
    void on_ModuleWidgetButton_clicked(bool checked);
    void on_ProcedureWidgetButton_clicked(bool checked);

    public:
    // Prepare widget for deletion
    void prepareForDeletion();

    public Q_SLOTS:
    // Local keyword data changed
    void localKeywordChanged(int signalMask);
    // Global data mutated
    void globalDataMutated(int mutationFlags);

    Q_SIGNALS:
    // Notify that the Module's data has been modified in some way
    void dataModified();
    // Notify that the active status of a module has changed
    void statusChanged();
};
