// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/charts/ui_moduleinsertionblock.h"
#include "templates/reflist.h"

// Forward Declarations
class Dissolve;
class DissolveWindow;

// Module Insertion Block Widget
class ModuleInsertionBlock : public QWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    ModuleInsertionBlock(QWidget *parent);
    ~ModuleInsertionBlock();

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::ModuleInsertionWidget ui_;

    /*
     * QWidget Reimplementations
     */
    protected:
    // Paint event
    void paintEvent(QPaintEvent *event);

    /*
     * Widget Functions
     */
    public:
    // Return underlying widget
    QWidget *widget();
    // Update controls within widget
    void updateControls();
    // Disable sensitive controls
    void disableSensitiveControls();
    // Enable sensitive controls
    void enableSensitiveControls();

    /*
     * Geometry
     */
    public:
    // Return width of underlying widget
    int widgetWidth() const;
    // Return height of underlying widget
    int widgetHeight() const;
    // Set underlying widget geometry
    void setWidgetGeometry(int left, int top, int width, int height);

    /*
     * Icons
     */
    public:
    // Set icons to reflect insertion of specified Module
    void setDisplayModuleInsertion(QString moduleType);
    // Set icons to reflect moving existing Module to new position
    void setDisplayModuleMove();
};
