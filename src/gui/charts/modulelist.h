// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/charts/chart.hui"
#include "gui/charts/modulelistmetrics.h"
#include <QWidget>

// Forward Declarations
class Configuration;
class Dissolve;
class Module;
class ModuleBlock;
class ModuleInsertionBlock;
class ModuleList;

// ModuleList Chart - Manages display of a sequence of modules from a ModuleList
class ModuleListChart : public ChartBase
{
    public:
    ModuleListChart(ModuleList *moduleList, Dissolve &dissolve, Configuration *localConfiguration = nullptr);
    ~ModuleListChart();

    /*
     * Dissolve Reference
     */
    private:
    // Dissolve reference
    Dissolve &dissolve_;

    /*
     * Target ModuleList
     */
    private:
    // Target ModuleList for display
    ModuleList *moduleList_;
    // Configuration in which the layer exists (if any)
    Configuration *localConfiguration_;

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
    // Widgets for our Module blocks
    RefList<ModuleBlock> moduleBlockWidgets_;
    // Insertion block
    ModuleInsertionBlock *insertionBlock_;

    private:
    // Find ModuleBlock displaying specified Module
    ModuleBlock *moduleBlock(Module *module);

    protected:
    // Update the content block widgets against the current target data
    void updateContentBlocks();

    public:
    // Set the currently-selected Module
    void setCurrentModule(Module *module);
    // Return the currently-selected molecule
    Module *currentModule() const;

    /*
     * Block Interaction
     */
    protected:
    // Return whether to accept the dragged object (described by its mime info)
    bool acceptDraggedObject(const MimeStrings *strings);
    // Handle hover over specified hotspot
    bool handleHotSpotHover(const ChartHotSpot *hotSpot);
    // Handle the drop of an object (described by its mime info)
    void handleDroppedObject(const MimeStrings *strings);
    // Return mime info for specified block (owned by this chart)
    MimeStrings mimeInfo(ChartBlock *block);
    // Specified block has been double clicked
    void blockDoubleClicked(ChartBlock *block);
    // The chart has requested removal of one of its blocks
    void blockRemovalRequested(const QString &blockIdentifier);
    // Block selection has changed
    void blockSelectionChanged(ChartBlock *block);

    /*
     * Widget Layout
     */
    private:
    // Chart metrics
    ModuleListChartMetrics metrics_;

    private:
    // Calculate new widget geometry according to the layout requirements, returning the entire area required
    QSize calculateNewWidgetGeometry(QSize currentSize);
};
