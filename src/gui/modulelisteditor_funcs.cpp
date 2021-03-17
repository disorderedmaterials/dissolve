// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "gui/charts/moduleblock.h"
#include "gui/charts/modulelist.h"
#include "gui/gui.h"
#include "gui/modulecontrolwidget.h"
#include "gui/modulelisteditor.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
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
    moduleCategories_.clear();
    ListIterator<Module> moduleIterator(dissolveWindow->dissolve().masterModules());
    while (const Module *module = moduleIterator.iterate())
    {
        // Check that the category is not 'HIDDEN' (in which case we don't show it)
        if (DissolveSys::sameString("HIDDEN", module->category()))
            continue;

        // Find category for this Module (if it exists) or create a new one
        MimeTreeWidgetItem *categoryItem = nullptr;
        RefDataListIterator<MimeTreeWidgetItem, QString> categoryIterator(moduleCategories_);
        while ((categoryItem = categoryIterator.iterate()))
            if (DissolveSys::sameString(module->category(), qPrintable(categoryIterator.currentData())))
                break;
        if (categoryItem == nullptr)
        {
            categoryItem = new MimeTreeWidgetItem((QTreeWidget *)nullptr, 1000);
            categoryItem->setText(0, QString::fromStdString(std::string(module->category())));
            categoryItem->setFlags(Qt::ItemIsEnabled);
            moduleCategories_.append(categoryItem, QString::fromStdString(std::string(module->category())));
        }

        // Create item for the Module
        MimeTreeWidgetItem *item = new MimeTreeWidgetItem(categoryItem, 1000);
        item->setIcon(0, ModuleBlock::modulePixmap(module));
        item->setText(0, QString::fromStdString(std::string(module->type())));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
        item->setData(0, Qt::UserRole, VariantPointer<const Module>(module));
        item->setToolTip(0, QString::fromStdString(std::string(module->brief())));
        item->addMimeString(MimeString::ModuleType, module->type());
    }

    // Populate the available Modules tree with the categories we now have
    ui_.AvailableModulesTree->clear();
    RefDataListIterator<MimeTreeWidgetItem, QString> categoryIterator(moduleCategories_);
    while (MimeTreeWidgetItem *categoryItem = categoryIterator.iterate())
        ui_.AvailableModulesTree->addTopLevelItem(categoryItem);
    ui_.AvailableModulesTree->sortByColumn(0, Qt::AscendingOrder);
    ui_.AvailableModulesTree->expandAll();
    ui_.AvailableModulesTree->resizeColumnToContents(0);
    ui_.AvailableModulesTree->resizeColumnToContents(1);

    // Hide available modules group initially
    ui_.ModulePaletteGroup->setVisible(false);

    // Set the currently selected module to the first one in the list
    chartWidget_->setCurrentModule(moduleLayer_->modules().first());

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

    // Get the Module associated to the double-clicked item
    const Module *module = VariantPointer<const Module>(item->data(0, Qt::UserRole));
    if (!module)
        return;

    // Create a new instance of the Module
    Module *newInstance = dissolveWindow_->dissolve().createModuleInstance(module->type());
    newInstance->setConfigurationLocal(localConfiguration_);

    // Set Configuration targets as appropriate
    if (newInstance->nRequiredTargets() != Module::ZeroTargets)
    {
        if (localConfiguration_)
            newInstance->addTargetConfiguration(localConfiguration_);
        else
        {
            ListIterator<Configuration> configIterator(dissolveWindow_->dissolve().configurations());
            while (Configuration *cfg = configIterator.iterate())
            {
                newInstance->addTargetConfiguration(cfg);
                if ((newInstance->nRequiredTargets() != Module::OneOrMoreTargets) &&
                    (newInstance->nRequiredTargets() == newInstance->nTargetConfigurations()))
                    break;
            }
        }
    }

    moduleLayer_->own(newInstance);

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
