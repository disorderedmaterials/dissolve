// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/charts/chart.hui"
#include "gui/charts/proceduremetrics.h"
#include <QWidget>

// Forward Declarations
class CoreData;
class Procedure;
class ProcedureChartNodeBlock;
class ProcedureNode;
class SequenceProcedureNode;

// Procedure Chart - Manages display of a sequence of nodes in a Procedure
class ProcedureChart : public ChartBase
{
    public:
    ProcedureChart(Procedure *procedure, const CoreData &coreData);
    ~ProcedureChart();

    /*
     * CoreData Reference
     */
    private:
    // CoreData reference
    const CoreData &coreData_;

    /*
     * Target Procedure
     */
    private:
    // Target Procedure for display
    Procedure *procedure_;

    /*
     * QWidget Reimplementations
     */
    protected:
    // Paint event
    void paintEvent(QPaintEvent *event);

    /*
     * Chart Blocks
     */
    private:
    // Widgets for our root sequence Procedure nodes
    RefList<ProcedureChartNodeBlock> rootSequenceNodeWidgets_;

    private:
    // Create / own chart block widgets for the specified sequence
    void updateContentBlocks(const SequenceProcedureNode *sequenceNode, RefList<ProcedureChartNodeBlock> &oldSequenceWidgets,
                             int &indentLevel);
    // Find ProcedureChartNodeBlock displaying specified ProcedureNode anywhere in the heirarchy of nodes
    ProcedureChartNodeBlock *nodeBlock(ProcedureNode *node);
    // Find ProcedureChartNodeBlock displaying specified ProcedureNode in the supplied list
    ProcedureChartNodeBlock *nodeBlock(ProcedureNode *node, const RefList<ProcedureChartNodeBlock> &list);

    protected:
    // Update the content block widgets against the current target data
    void updateContentBlocks();

    /*
     * Drag / Drop
     */
    protected:
    // TODO

    /*
     * Widget Layout
     */
    private:
    // Chart metrics
    ProcedureChartMetrics metrics_;

    private:
    // Calculate geometries for the widgets in the supplied sequence list
    void calculateGeometries(RefList<ProcedureChartNodeBlock> &nodeWidgets, QSize &requiredSize, int &indentLevel);
    // Calculate new widget geometry according to the layout requirements, returning the entire area required
    QSize calculateNewWidgetGeometry(QSize currentSize);
};
