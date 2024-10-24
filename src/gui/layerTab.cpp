// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/layerTab.h"
#include "base/lineParser.h"
#include "gui/getModuleLayerNameDialog.h"
#include "gui/gui.h"
#include "gui/mainTabsWidget.h"
#include "gui/moduleControlWidget.h"
#include "main/dissolve.h"
#include <QMessageBox>

LayerTab::LayerTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title,
                   ModuleLayer *layer)
    : MainTab(dissolveWindow, dissolve, parent, QString("Layer: %1").arg(title), this)
{
    ui_.setupUi(this);

    moduleLayer_ = layer;

    // Set the module list model and connect signals
    ui_.ModulesTable->setModel(&moduleLayerModel_);
    moduleLayerModel_.setData(moduleLayer_, &dissolve.coreData());
    connect(ui_.ModulesTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(moduleSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(&moduleLayerModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)), this,
            SLOT(layerDataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)));
    connect(&moduleLayerModel_, SIGNAL(moduleNameChanged(const QModelIndex &, const QString &, const QString &)), this,
            SLOT(moduleNameChanged(const QModelIndex &, const QString &, const QString &)));
    ui_.ModulesTable->resizeColumnsToContents();

    if (!moduleLayer_->modules().empty())
        ui_.ModulesTable->selectRow(0);

    // Set up the available modules tree
    ui_.AvailableModulesTree->setModel(&modulePaletteModel_);
    ui_.AvailableModulesTree->expandAll();
    ui_.AvailableModulesTree->setVisible(false);
}

// Return displayed ModuleLayer
ModuleLayer *LayerTab::moduleLayer() const { return moduleLayer_; }

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType LayerTab::type() const { return MainTab::TabType::Layer; }

// Raise suitable dialog for entering / checking new tab name
QString LayerTab::getNewTitle(bool &ok)
{
    // Get a new, valid name for the layer
    GetModuleLayerNameDialog nameDialog(this, dissolve_.coreData().processingLayers());
    ok = nameDialog.get(moduleLayer_, QString::fromStdString(std::string(moduleLayer_->name())));

    if (ok)
    {
        // Rename our layer, and flag that our data has been modified
        moduleLayer_->setName(qPrintable(nameDialog.newName()));

        dissolveWindow_->setModified();
    }

    return nameDialog.newName();
}

// Return whether the title of the tab can be changed
bool LayerTab::canChangeTitle() const { return true; }

// Return whether the tab can be closed (after any necessary user querying, etc.)
bool LayerTab::canClose() const
{
    // Check that we really want to delete this tab
    QMessageBox queryBox;
    queryBox.setText(QString("Really delete the layer '%1'?\nThis cannot be undone!")
                         .arg(QString::fromStdString(std::string(moduleLayer_->name()))));
    queryBox.setInformativeText("Proceed?");
    queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    queryBox.setDefaultButton(QMessageBox::No);
    auto ret = queryBox.exec();

    if (ret != QMessageBox::Yes)
        return false;

    return true;
}

/*
 * Widgets
 */

// Return ModuleControlWidget for the specified Module (if it exists)
ModuleControlWidget *LayerTab::getControlWidget(const Module *module, bool setAsCurrent)
{
    for (auto n = 1; n < ui_.ModuleControlsStack->count(); ++n)
    {
        auto *w = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->widget(n));
        if (w && (w->module() == module))
        {
            if (setAsCurrent)
                ui_.ModuleControlsStack->setCurrentIndex(n);
            return w;
        }
    }
    return nullptr;
}

// Remove ModuleControlWidget for the specified Module (if it exists)
void LayerTab::removeControlWidget(const Module *module)
{
    for (auto n = 1; n < ui_.ModuleControlsStack->count(); ++n)
    {
        auto *w = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->widget(n));
        if (w && (w->module() == module))
        {
            if (ui_.ModuleControlsStack->currentIndex() == n)
                ui_.ModuleControlsStack->setCurrentIndex((n + 1) < ui_.ModuleControlsStack->count() ? n + 1 : n - 1);
            ui_.ModuleControlsStack->removeWidget(w);
            w->prepareForDeletion();
            return;
        }
    }
}

void LayerTab::on_ShowAvailableModulesButton_clicked(bool checked)
{
    // Toggle the visibility of the available modules tree
    ui_.AvailableModulesTree->setVisible(!ui_.AvailableModulesTree->isVisible());

    // Set correct text on our button
    ui_.ShowAvailableModulesButton->setText(ui_.AvailableModulesTree->isVisible() ? "Hide Available Modules"
                                                                                  : "Show Available Modules");
}

void LayerTab::on_LayerEnabledButton_clicked(bool checked)
{
    if (refreshLock_.isLocked() || (!moduleLayer_))
        return;

    if (checked)
    {
        tabWidget_->setTabIcon(page_, QIcon(":/general/icons/layer.svg"));
        moduleLayer_->runControlFlags().removeFlag(ModuleLayer::RunControlFlag::Disabled);
    }
    else
    {
        tabWidget_->setTabIcon(page_, QIcon(":/general/icons/layer_disabled.svg"));
        moduleLayer_->runControlFlags().setFlag(ModuleLayer::RunControlFlag::Disabled);
    }

    updateModuleList();

    dissolveWindow_->setModified();
}

void LayerTab::on_LayerFrequencySpin_valueChanged(int value)
{
    if (refreshLock_.isLocked() || (!moduleLayer_))
        return;

    moduleLayer_->setFrequency(value);

    dissolveWindow_->setModified();
}

void LayerTab::on_RunControlEnergyStabilityCheck_clicked(bool checked)
{
    if (refreshLock_.isLocked() || (!moduleLayer_))
        return;

    moduleLayer_->runControlFlags().setState(ModuleLayer::RunControlFlag::EnergyStability, checked);

    dissolveWindow_->setModified();
}

void LayerTab::on_RunControlSizeFactorsCheck_clicked(bool checked)
{
    if (refreshLock_.isLocked() || (!moduleLayer_))
        return;

    moduleLayer_->runControlFlags().setState(ModuleLayer::RunControlFlag::SizeFactors, checked);

    dissolveWindow_->setModified();
}

void LayerTab::moduleSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
    auto modelIndices = current.indexes();

    // If there is no selected index, show the default page on the stack
    if (modelIndices.empty())
    {
        ui_.ModuleControlsStack->setCurrentIndex(0);
        return;
    }

    // Get the selected module
    auto *module = moduleLayer_->modules()[modelIndices.front().row()].get();
    if (!module)
    {
        ui_.ModuleControlsStack->setCurrentIndex(0);
        return;
    }

    Locker refreshLocker(refreshLock_);

    // See if our stack already contains a control widget for the module - if not, create one
    auto *mcw = getControlWidget(module, true);
    if (!mcw)
    {
        // Create a new widget to display this Module
        mcw = new ModuleControlWidget(dissolveWindow_, module);
        connect(mcw, SIGNAL(dataModified()), dissolveWindow_, SLOT(setModified()));
        connect(mcw, SIGNAL(statusChanged()), this, SLOT(updateModuleList()));
        ui_.ModuleControlsStack->setCurrentIndex(ui_.ModuleControlsStack->addWidget(mcw));

        // If we're currently running, don;t allow editing in our new widget
        if (dissolveWindow_->dissolveIterating())
            mcw->preventEditing();
    }
    else
        mcw->updateControls();
}

void LayerTab::layerDataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)
{
    dissolveWindow_->setModified({DissolveSignals::ModulesMutated});
}

void LayerTab::moduleNameChanged(const QModelIndex &index, const QString &oldName, const QString &newName)
{
    auto *module = moduleLayerModel_.data(index, Qt::UserRole).value<Module *>();
    assert(module);

    // Find the control widget for the module and update it
    auto *mcw = getControlWidget(module);
    if (mcw)
        mcw->updateControls(ModuleWidget::RecreateRenderablesFlag);

    // Rename processing module data
    dissolve_.processingModuleData().renamePrefix(oldName.toStdString(), newName.toStdString());

    // Trigger another redraw of the widget in order to update any graph controls
    if (mcw)
        mcw->updateControls();
}

// Update the module list
void LayerTab::updateModuleList()
{
    // Refresh the module list
    std::optional<QModelIndex> selectedIndex;
    if (!ui_.ModulesTable->selectionModel()->selection().indexes().empty())
        selectedIndex = ui_.ModulesTable->selectionModel()->selection().indexes().front();
    moduleLayerModel_.reset();
    if (selectedIndex)
        ui_.ModulesTable->selectRow(selectedIndex.value().row());
}

void LayerTab::on_ModulesTable_customContextMenuRequested(const QPoint &pos)
{
    auto index = ui_.ModulesTable->indexAt(pos);
    if (!index.isValid())
        return;
    auto module = moduleLayerModel_.data(index, Qt::UserRole).value<Module *>();
    assert(module);

    QMenu menu;
    menu.setFont(font());
    menu.setEnabled(!dissolveWindow_->dissolveIterating());

    // Construct the context menu
    auto *enableModule = menu.addAction("&Enable this");
    enableModule->setEnabled(!module->isEnabled());
    auto *disableModule = menu.addAction("&Disable this");
    disableModule->setEnabled(module->isEnabled());
    menu.addSeparator();
    auto *enableOnlyModule = menu.addAction("Enable &only this");
    menu.addSeparator();
    auto *clearData = menu.addAction("&Clear associated data");
    menu.addSeparator();
    auto *deleteModule = menu.addAction("&Delete");
    deleteModule->setIcon(QIcon(":/general/icons/cross.svg"));

    auto *action = menu.exec(ui_.ModulesTable->mapToGlobal(pos));
    if (action == enableModule)
        module->setEnabled(true);
    else if (action == disableModule)
        module->setEnabled(false);
    else if (action == enableOnlyModule)
        for (auto &m : moduleLayer_->modules())
            m->setEnabled(m.get() == module);
    else if (action == clearData)
    {
        Renderable::invalidateAll();
        Renderable::setSourceDataAccessEnabled(false);
        dissolve_.processingModuleData().removeWithPrefix(module->name());
        Renderable::setSourceDataAccessEnabled(true);
    }
    else if (action == deleteModule)
    {
        // Remove the module's data, the module control widget, then the module itself
        dissolve_.processingModuleData().removeWithPrefix(module->name());
        removeControlWidget(module);
        moduleLayerModel_.removeRows(index.row(), 1, QModelIndex());
    }

    // Update required objects
    if (action == enableModule || action == disableModule || action == enableOnlyModule || action == deleteModule)
    {
        updateModuleList();
        dissolveWindow_->setModified({DissolveSignals::ModulesMutated});
    }
    updateControls();
}

void LayerTab::on_AvailableModulesTree_doubleClicked(const QModelIndex &index)
{
    moduleLayerModel_.appendNew(modulePaletteModel_.data(index, Qt::DisplayRole).toString());
}

// Remove all module control widgets
void LayerTab::removeModuleControlWidgets()
{
    // Remove all stack pages but the first (which corresponds to the "No Module Selected" widget)
    while (ui_.ModuleControlsStack->count() > 1)
    {
        auto *w = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->widget(1));
        ui_.ModuleControlsStack->removeWidget(w);
        w->prepareForDeletion();
    }
}

/*
 * Update
 */

// Update controls in tab
void LayerTab::updateControls()
{
    if (!moduleLayer_)
        return;

    Locker refreshLocker(refreshLock_);

    ui_.LayerEnabledButton->setChecked(!moduleLayer_->runControlFlags().isSet(ModuleLayer::RunControlFlag::Disabled));
    ui_.LayerFrequencySpin->setValue(moduleLayer_->frequency());

    ui_.RunControlEnergyStabilityCheck->setChecked(
        moduleLayer_->runControlFlags().isSet(ModuleLayer::RunControlFlag::EnergyStability));
    ui_.RunControlSizeFactorsCheck->setChecked(moduleLayer_->runControlFlags().isSet(ModuleLayer::RunControlFlag::SizeFactors));

    updateModuleList();

    auto *mcw = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->currentWidget());
    if (mcw)
        mcw->updateControls();
}

// Prevent editing within tab
void LayerTab::preventEditing()
{
    ui_.LayerEnabledButton->setEnabled(false);
    ui_.LayerFrequencySpin->setEnabled(false);
    ui_.RunControlGroup->setEnabled(false);

    ui_.ModulesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_.ModulesTable->setDragDropMode(QAbstractItemView::NoDragDrop);
    ui_.AvailableModulesTree->setEnabled(false);
    for (auto n = 0; n < ui_.ModuleControlsStack->count(); ++n)
    {
        auto *mcw = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->widget(n));
        if (mcw)
            mcw->preventEditing();
    }
}

// Allow editing within tab
void LayerTab::allowEditing()
{
    ui_.LayerEnabledButton->setEnabled(true);
    ui_.LayerFrequencySpin->setEnabled(true);
    ui_.RunControlGroup->setEnabled(true);
    ui_.AvailableModulesTree->setEnabled(true);
    ui_.ModulesTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    ui_.ModulesTable->setDragDropMode(QAbstractItemView::DragDrop);
    for (auto n = 0; n < ui_.ModuleControlsStack->count(); ++n)
    {
        auto *mcw = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->widget(n));
        if (mcw)
            mcw->allowEditing();
    }
}
