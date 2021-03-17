// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/charts/chartblock.h"
#include "gui/charts/ui_moduleblock.h"
#include "templates/reflist.h"

// Forward Declarations
class Dissolve;
class KeywordWidgetBase;
class Module;

// Module Block Widget
class ModuleBlock : public QWidget, public ChartBlock
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    ModuleBlock(QWidget *parent, Module *module, Dissolve &dissolve);
    ~ModuleBlock();

    private:
    // Reference to Dissolve
    Dissolve &dissolve_;

    /*
     * Module Target
     */
    private:
    // Displayed Module
    Module *module_;

    public:
    // Return displayed Module
    Module *module() const;

    /*
     * Controls
     */
    private:
    // Main form declaration
    Ui::ModuleBlockWidget ui_;
    // Whether the widget is currently refreshing
    bool refreshing_;
    // QRect representing drag handle
    QRect dragHandleRect_;
    // Colour to use for drawing
    QColor displayColour_;

    public:
    // Return suitable QPixmap for supplied Module
    static QPixmap modulePixmap(const Module *module);
    // Return suitable QPixmap for supplied Module type
    static QPixmap modulePixmap(QString moduleType);

    public slots:
    void on_RemoveButton_clicked(bool checked);
    void on_NameEdit_editingFinished();
    void on_NameEdit_returnPressed();
    void on_EnabledButton_clicked(bool checked);
    void on_FrequencySpin_valueChanged(int value);

    signals:
    void remove(const QString &blockIdentifier);

    /*
     * QWidget Reimplementations
     */
    protected:
    // Paint event
    void paintEvent(QPaintEvent *event);

    /*
     * Type (ChartBlock Reimplementations)
     */
    public:
    // Return type of this block
    const QString blockType() const;

    /*
     * Widget (ChartBlock Reimplementations)
     */
    public:
    // Return underlying widget
    QWidget *widget();
    // Return width of underlying widget
    int widgetWidth() const;
    // Return height of underlying widget
    int widgetHeight() const;
    // Return whether the supplied point (on the parent chart) allows a drag operation to begin
    bool isDragPoint(QPoint point) const;

    /*
     * Update
     */
    private:
    // Update configuration labels and tooltips
    void updateConfigurationLabels();

    private slots:
    // Configuration targets for the Module have been modified
    void configurationKeywordEdited(int flags);

    /*
     * Update (ChartBlock Reimplementations)
     */
    public:
    // Update controls within widget
    void updateControls();
    // Disable sensitive controls
    void disableSensitiveControls();
    // Enable sensitive controls
    void enableSensitiveControls();

    /*
     * Signals / Slots
     */
    signals:
    // Notify that the Module's data has been modified in some way
    void dataModified();
};
