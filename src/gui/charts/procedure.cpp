/*
    *** Procedure Chart
    *** src/gui/charts/procedure.cpp
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

#include "gui/charts/procedure.h"
#include "gui/charts/proceduremetrics.h"
#include "gui/charts/procedurenode.h"
#include "gui/stockcolours.h"
#include "gui/widgets/mimestrings.h"
#include "procedure/nodes/sequence.h"
#include "procedure/procedure.h"
#include <QApplication>
#include <QDrag>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QWidget>

ProcedureChart::ProcedureChart(Procedure *procedure, const CoreData &coreData) : ChartBase(), coreData_(coreData)
{
    refreshing_ = false;

    // Target Procedure
    procedure_ = procedure;

    updateContentBlocks();

    recalculateLayout();

    // Create the insertion widget if we don't already have one
    // 	insertionWidget_ = new ProcedureChartInsertionBlock(this, dissolveWindow_);
    // 	insertionWidget_->setVisible(false);

    updateControls();
}

ProcedureChart::~ProcedureChart() {}

/*
 * QWidget Reimplementations
 */

// Paint event
void ProcedureChart::paintEvent(QPaintEvent *event)
{
    // Draw suitable connecting lines between widgets, illustrating the execution path of the code
    QPainter painter(this);

    // Draw the background before we do anything else
    QBrush backgroundBrush = QBrush(Qt::black, QPixmap(":/images/images/squares.jpg"));
    painter.fillRect(QRect(0, 0, width(), height()), backgroundBrush);

    // Set up some QPens
    QPen solidPen(Qt::black);
    solidPen.setWidth(metrics_.blockBorderWidth());
    QPen dottedPen(Qt::gray);
    dottedPen.setWidth(metrics_.blockBorderWidth());
    dottedPen.setStyle(Qt::DotLine);
    dottedPen.setCapStyle(Qt::RoundCap);

    // TEST - Highlight all hotspots
    if (false)
    {
        ListIterator<ChartHotSpot> hotSpotIterator(hotSpots_);
        while (ChartHotSpot *hotSpot = hotSpotIterator.iterate())
            painter.fillRect(hotSpot->geometry(), QBrush(QColor(200, 200, 0, 50)));
    }
}

/*
 * Chart Blocks
 */

// Update the content block widgets against the current target data for the supplied SequenceNode
void ProcedureChart::updateContentBlocks(const SequenceProcedureNode *sequence,
                                         RefList<ProcedureChartNodeBlock> &oldSequenceWidgets, int &indentLevel)
{
    // Create a temporary list that will store our widgets to be 'reused'
    RefList<ProcedureChartNodeBlock> newSequenceWidgets;

    // Iterate through the nodes in this sequence, searching for their widgets in the oldWidgetsList
    ListIterator<ProcedureNode> nodeIterator(sequence->sequence());
    while (ProcedureNode *node = nodeIterator.iterate())
    {
        // Does this node have an existing widget?
        ProcedureChartNodeBlock *block = nodeBlock(node);
        if (block)
        {
            // Widget already exists, so remove the reference from nodeWidgets_ and add it to the new list
            newSequenceWidgets.append(block);
            oldSequenceWidgets.remove(block);
        }
        else
        {
            // No current widget, so must create one
            block = new ProcedureChartNodeBlock(this, node, coreData_);
            connect(block, SIGNAL(dataModified()), this, SLOT(chartDataModified()));
            connect(block, SIGNAL(keywordsToggled()), this, SLOT(recalculateLayout()));
            // 			connect(mcmBlock, SIGNAL(remove(QString)), this, SLOT(removeModule(QString)));
            newSequenceWidgets.append(block);
            chartBlocks_.append(block);
        }

        // Set the colour of the widget according to the current indent level
        block->setDisplayColour(StockColours::stockColourByIndex(indentLevel));

        // If the node has a branch, deal with it here
        if (node->hasBranch())
        {
            ++indentLevel;

            updateContentBlocks(node->branch(), block->branchWidgets(), indentLevel);

            --indentLevel;
        }
    }

    // Any widgets remaining in oldSequenceWidgets are no longer used, and can thus be deleted
    for (ProcedureChartNodeBlock *block : oldSequenceWidgets)
    {
        chartBlocks_.remove(block);
        delete block;
    }

    // Copy the new list
    oldSequenceWidgets = newSequenceWidgets;
}

// Find ProcedureChartNodeBlock displaying specified ProcedureNode anywhere in the heirarchy of nodes
ProcedureChartNodeBlock *ProcedureChart::nodeBlock(ProcedureNode *node) { return nodeBlock(node, rootSequenceNodeWidgets_); }

// Find ProcedureChartNodeBlock displaying specified ProcedureNode in the supplied list
ProcedureChartNodeBlock *ProcedureChart::nodeBlock(ProcedureNode *node, const RefList<ProcedureChartNodeBlock> &list)
{
    for (ProcedureChartNodeBlock *block : list)
    {
        if (block->node() == node)
            return block;

        // Search the branch list of this node
        ProcedureChartNodeBlock *branchBlock = nodeBlock(node, block->branchWidgets());
        if (branchBlock)
            return branchBlock;
    }

    return nullptr;
}

// Update the content block widgets against the current target data
void ProcedureChart::updateContentBlocks()
{
    if (!procedure_)
        return;

    // Set initial indent level
    auto indentLevel = 0;

    // Start with the root sequence node of the Procedure - we deal recursively with the rest
    updateContentBlocks(&procedure_->rootSequence(), rootSequenceNodeWidgets_, indentLevel);
}

/*
 * Drag / Drop
 */

// TODO

/*
 * Widget Layout
 */

// Calculate geometries for the widgets in the supplied sequence list
void ProcedureChart::calculateGeometries(RefList<ProcedureChartNodeBlock> &nodeWidgets, QSize &requiredSize, int &indentLevel)
{
    // Precalculate some useful metrics
    const auto leftIndent = indentLevel * metrics_.indentWidth();

    // Loop over widgets in this sequence
    for (ProcedureChartNodeBlock *block : nodeWidgets)
    {
        // Set basic position of the block, accounting for the indent
        block->setNewPosition(leftIndent, requiredSize.height());

        // Try to give our block its preferred (minimum) size
        QSize minSize = block->widget()->minimumSizeHint();
        block->setNewSize(minSize.width(), minSize.height());

        // Update the maximum width if necessary
        auto blockWidth = block->widgetWidth() + leftIndent;
        if (blockWidth > requiredSize.width())
            requiredSize.setWidth(blockWidth);

        // Increase the required height
        requiredSize.setHeight(requiredSize.height() + block->widgetHeight() + metrics_.blockVerticalSpacing());

        // If this block has sub-blocks (i.e. the node has a branch), recurse in to it
        if (block->branchWidgets().nItems() > 0)
        {
            // Increase the indent level
            ++indentLevel;

            // Lay out the branch widgets
            calculateGeometries(block->branchWidgets(), requiredSize, indentLevel);

            // Decrease the indent level
            --indentLevel;
        }
    }
}

// Calculate new widget geometry according to the layout requirements
QSize ProcedureChart::calculateNewWidgetGeometry(QSize currentSize)
{
    /*
     * ProcedureNode layout is a single vertical column, with branches incrementally indented
     */

    // Set initial indent level and widget position
    auto indentLevel = 0;
    QSize requiredSize(0, 0);

    // Begin by calling the layout function for the root sequence - we recurse from there
    calculateGeometries(rootSequenceNodeWidgets_, requiredSize, indentLevel);

    // Set the widths of all widgets so their right edges are aligned
    for (ChartBlock *block : chartBlocks_)
        block->setNewRightEdge(requiredSize.width());

    // Finalise minimum size hint - we just need to add on the surrounding margins
    requiredSize += QSize(2 * metrics_.chartMargin(), 2 * metrics_.chartMargin());

    return requiredSize;
}

/*
 * State I/O
 */

// Write widget state through specified LineParser
bool ProcedureChart::writeState(LineParser &parser) const { return true; }

// Read widget state through specified LineParser
bool ProcedureChart::readState(LineParser &parser) { return true; }
