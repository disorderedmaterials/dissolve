// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/charts/chartblock.h"
#include "gui/charts/ui_procedurenode.h"
#include "templates/reflist.h"

// Forward Declarations
class CoreData;
class KeywordWidgetBase;
class ProcedureNode;

// Procedure Charte Node Block Widget
class ProcedureChartNodeBlock : public QWidget, public ChartBlock
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    ProcedureChartNodeBlock(QWidget *parent, ProcedureNode *node, const CoreData &coreData);
    ~ProcedureChartNodeBlock();

    /*
     * Node Target
     */
    private:
    // Displayed node
    ProcedureNode *node_;

    // Widgets that exist in the branch of our Procedure node
    RefList<ProcedureChartNodeBlock> branchWidgets_;

    public:
    // Return displayed node
    ProcedureNode *node() const;
    // Return RefList of widgets that exist in the branch of our Procedure node
    RefList<ProcedureChartNodeBlock> &branchWidgets();

    /*
     * Controls
     */
    private:
    // Main form declaration
    Ui::ProcedureChartNodeWidget ui_;
    // Whether the widget is currently refreshing
    bool refreshing_;
    // QRect representing drag handle
    QRect dragHandleRect_;
    // Colour to use for drawing
    QColor displayColour_;

    public:
    // Set display colour for widget
    void setDisplayColour(QColor colour);
    // Set whether the keywords widget is expanded or not, and whether this is permanent
    void setKeywordsExpanded(bool expanded, bool permanent = false);
    // Hide the remove button
    void hideRemoveButton();

    public slots:
    void on_ToggleKeywordsButton_clicked(bool checked);
    void on_RemoveButton_clicked(bool checked);

    signals:
    void keywordsToggled();
    void remove(void *nodePointer);

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
    private slots:
    // Keyword data for node has been modified
    void keywordDataModified();

    signals:
    // Notify that the node's keyword data has been modified
    void dataModified();
};
