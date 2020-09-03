/*
    *** Module Chart Block
    *** src/gui/charts/moduleblock.h
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
