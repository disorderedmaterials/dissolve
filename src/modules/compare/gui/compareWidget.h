// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/compare/gui/ui_compareWidget.h"
#include "modules/widget.h"
#include <QPushButton>
#include <QTableWidget>

// Forward Declarations
class CompareModule;
class DataViewer;

// Module Widget
class CompareModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    CompareModuleWidget(QWidget *parent, CompareModule *module, Dissolve &dissolve);
    ~CompareModuleWidget() override = default;

    private:
    // Associated Module
    CompareModule *module_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::CompareModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *graph_;
    // Vector of buttons
    std::vector<QPushButton *> buttons_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;

    /*
     * Widgets / Functions
     */

    private Q_SLOTS:
    void on_Data1DButton_clicked(bool checked);
};
