// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/charts/moduleblock.h"
#include "gui/charts/modulelist.h"
#include "gui/gui.h"
#include "gui/modulecontrolwidget.h"
#include "gui/modulelisteditor.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/registry.h"
#include "templates/variantpointer.h"

ModuleListEditor::ModuleListEditor(QWidget *parent) : QWidget(parent)
{
    ui_.setupUi(this);

    chartWidget_ = nullptr;

    refreshing_ = false;
}

ModuleListEditor::~ModuleListEditor() {}

/*
 * Setup
 */

// Setup up the ModuleListEditor for the specified Module list
bool ModuleListEditor::setUp(DissolveWindow *dissolveWindow, ModuleLayer *moduleLayer, Configuration *localConfiguration)
{
    dissolveWindow_ = dissolveWindow;
    moduleLayer_ = moduleLayer;
    localConfiguration_ = localConfiguration;

    // Create a ModuleListChart widget and set its source list
    chartWidget_ = new ModuleListChart(moduleLayer_, dissolveWindow_->dissolve(), localConfiguration_);
    chartWidget_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    ui_.ChartScrollArea->setWidget(chartWidget_);
    ui_.ChartScrollArea->setWidgetResizable(true);
    ui_.ChartScrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    connect(chartWidget_, SIGNAL(dataModified()), dissolveWindow_, SLOT(setModified()));
    connect(chartWidget_, SIGNAL(fullUpdate()), dissolveWindow_, SLOT(fullUpdate()));
    connect(chartWidget_, SIGNAL(dataModified()), this, SLOT(chartWidgetDataModified()));
    connect(chartWidget_, SIGNAL(requiredSizeChanged()), this, SLOT(chartWidgetSizeChanged()));
    connect(chartWidget_, SIGNAL(blockRemoved(const QString &)), this, SLOT(moduleDeleted(const QString &)));
    connect(chartWidget_, SIGNAL(blockSelectionChanged(const QString &)), this, SLOT(moduleSelectionChanged(const QString &)));

    // Add MimeTreeWidgetItems for each Module, adding them to a parent category item
    for (const auto &[categoryName, modules] : ModuleRegistry::categoryMap())
    {
        // Create a top-level category item
        auto *categoryItem = new MimeTreeWidgetItem(ui_.AvailableModulesTree, 1000);
        categoryItem->setText(0, QString::fromStdString(std::string(categoryName)));
        categoryItem->setFlags(Qt::ItemIsEnabled);

        // Create items for the Modules
        for (auto &[moduleType, moduleBrief] : modules)
        {

            auto *item = new MimeTreeWidgetItem(categoryItem, 1000);
            item->setIcon(0, ModuleBlock::modulePixmap(QString::fromStdString(std::string(moduleType))));
            item->setText(0, QString::fromStdString(std::string(moduleType)));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
            item->setToolTip(0, QString::fromStdString(std::string(moduleBrief)));
            item->addMimeString(MimeString::ModuleType, moduleType);
        }
    }

    ui_.AvailableModulesTree->sortByColumn(0, Qt::AscendingOrder);
    ui_.AvailableModulesTree->expandAll();
    ui_.AvailableModulesTree->resizeColumnToContents(0);
    ui_.AvailableModulesTree->resizeColumnToContents(1);

    // Hide available modules group initially
    ui_.ModulePaletteGroup->setVisible(false);

    // Set the currently selected module to the first one in the list
    if (!moduleLayer_->modules().empty())
        chartWidget_->setCurrentModule(moduleLayer_->modules().front().get());

    updateControls();

    return true;
}

/*
 * Update
 */

// Update controls in tab
void ModuleListEditor::updateControls()
{
    if (!chartWidget_)
        return;

    refreshing_ = true;

    chartWidget_->updateControls();
    ModuleControlWidget *mcw = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->currentWidget());
    if (mcw)
        mcw->updateControls();

    refreshing_ = false;
}

// Disable sensitive controls within tab
void ModuleListEditor::disableSensitiveControls()
{
    ui_.AvailableModulesTree->setEnabled(false);
    chartWidget_->disableSensitiveControls();
    for (auto n = 0; n < ui_.ModuleControlsStack->count(); ++n)
    {
        ModuleControlWidget *mcw = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->widget(n));
        if (mcw)
            mcw->disableSensitiveControls();
    }
}

// Enable sensitive controls within tab
void ModuleListEditor::enableSensitiveControls()
{
    ui_.AvailableModulesTree->setEnabled(true);
    chartWidget_->enableSensitiveControls();
    for (auto n = 0; n < ui_.ModuleControlsStack->count(); ++n)
    {
        ModuleControlWidget *mcw = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->widget(n));
        if (mcw)
            mcw->enableSensitiveControls();
    }
}

// Show / hide module palette
void ModuleListEditor::setModulePaletteVisible(bool visible) { ui_.ModulePaletteGroup->setVisible(visible); }

/*
 * Widget Functions
 */

// Find the ModuleControlWidget for this Module in the stack, if it exists
ModuleControlWidget *ModuleListEditor::controlWidgetForModule(Module *module) const
{
    for (auto n = 0; n < ui_.ModuleControlsStack->count(); ++n)
    {
        ModuleControlWidget *mcw = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->widget(n));
        if (mcw && (mcw->module() == module))
            return mcw;
    }

    return nullptr;
}

// Return the index of the ModuleControlWidget for this Module in the stack, if it exists
int ModuleListEditor::widgetIndexForModule(Module *module) const
{
    for (auto n = 0; n < ui_.ModuleControlsStack->count(); ++n)
    {
        ModuleControlWidget *mcw = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->widget(n));
        if (mcw && (mcw->module() == module))
            return n;
    }

    return -1;
}

void ModuleListEditor::moduleDeleted(const QString &blockIdentifier)
{
    // Check for empty block identifier
    if (blockIdentifier.isEmpty())
        return;

    // Get Module by unique name
    Module *module = dissolveWindow_->dissolve().findModuleInstance(qPrintable(blockIdentifier));
    if (!module)
        return;

    // Find the ModuleControlWidget for this Module in the stack, if it exists
    auto mcw = controlWidgetForModule(module);
    if (mcw)
    {
        ui_.ModuleControlsStack->removeWidget(mcw);
        mcw->deleteLater();
    }
}

void ModuleListEditor::moduleSelectionChanged(const QString &blockIdentifier)
{
    // Check for empty block identifier
    if (blockIdentifier.isEmpty())
    {
        ui_.ModuleControlsStack->setCurrentIndex(0);
        return;
    }

    // Get Module by unique name
    Module *module = dissolveWindow_->dissolve().findModuleInstance(qPrintable(blockIdentifier));
    if (!module)
    {
        ui_.ModuleControlsStack->setCurrentIndex(0);
        return;
    }

    // Find the ModuleControlWidget for this Module in the stack, if it exists
    auto widgetIndex = widgetIndexForModule(module);
    if (widgetIndex == -1)
    {
        // Create a new widget to display this Module
        ModuleControlWidget *mcw = new ModuleControlWidget;
        mcw->setModule(module, &dissolveWindow_->dissolve());
        connect(mcw, SIGNAL(dataModified()), dissolveWindow_, SLOT(setModified()));
        widgetIndex = ui_.ModuleControlsStack->addWidget(mcw);
    }

    ui_.ModuleControlsStack->setCurrentIndex(widgetIndex);
}

void ModuleListEditor::on_AvailableModulesTree_itemDoubleClicked(QTreeWidgetItem *item)
{
    if (!moduleLayer_)
        return;

    // Create a new instance of the Module
    dissolveWindow_->dissolve().createModuleInstance(item->text(0).toStdString(), moduleLayer_);

    updateControls();

    // Flag that the current data has changed
    dissolveWindow_->setModified();
}

void ModuleListEditor::chartWidgetDataModified()
{
    // We don't know which ModuleBlock sent the signal, so just update the visible controls widget
    ModuleControlWidget *mcw = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->currentWidget());
    if (mcw)
        mcw->updateControls();
}

// Required size of the chart widget has changed
void ModuleListEditor::chartWidgetSizeChanged()
{
    ui_.ChartScrollArea->setMinimumWidth(chartWidget_->width());
    ui_.ChartScrollArea->updateGeometry();
}

void ModuleListEditor::controlsWidgetDataModified()
{
    // The controls widget must correspond to the selected module in the chart, so update it
    if (chartWidget_->selectedBlock())
        chartWidget_->selectedBlock()->updateControls();
}
