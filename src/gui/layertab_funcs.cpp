// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/lineparser.h"
#include "gui/getmodulelayernamedialog.h"
#include "gui/gui.h"
#include "gui/layertab.h"
#include "gui/maintabswidget.hui"
#include "gui/modulecontrolwidget.h"
#include "main/dissolve.h"
#include <QMessageBox>

LayerTab::LayerTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title,
                   ModuleLayer *layer)
    : MainTab(dissolveWindow, dissolve, parent, QString("Layer: %1").arg(title), this)
{
    ui_.setupUi(this);

    moduleLayer_ = layer;

    // Set the module list model and connect signals
    ui_.ModulesList->setModel(&moduleLayerModel_);
    moduleLayerModel_.setData(moduleLayer_, dissolve);
    connect(ui_.ModulesList->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(moduleSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(&moduleLayerModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)), this,
            SLOT(layerDataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)));
    connect(&moduleLayerModel_, SIGNAL(moduleNameChanged(const QModelIndex &, const QString &, const QString &)), this,
            SLOT(moduleNameChanged(const QModelIndex &, const QString &, const QString &)));

    if (moduleLayer_->modules().size() >= 1)
    {
        auto firstIndex = moduleLayerModel_.index(0, 0);
        ui_.ModulesList->selectionModel()->setCurrentIndex(firstIndex, QItemSelectionModel::ClearAndSelect);
    }

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
    GetModuleLayerNameDialog nameDialog(this, dissolve_.processingLayers());
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

void LayerTab::on_ShowAvailableModulesButton_clicked(bool checked)
{
    // Toggle the visibility of the available modules tree
    ui_.AvailableModulesTree->setVisible(!ui_.AvailableModulesTree->isVisible());

    // Set correct text on our button
    ui_.ShowAvailableModulesButton->setText(ui_.AvailableModulesTree->isVisible() ? "Hide Available Modules"
                                                                                  : "Show Available Modules");
}

void LayerTab::on_EnabledButton_clicked(bool checked)
{
    if (refreshLock_.isLocked() || (!moduleLayer_))
        return;

    moduleLayer_->setEnabled(checked);
    if (checked)
        tabWidget_->setTabIcon(page_, QIcon(":/tabs/icons/tabs_modulelayer.svg"));
    else
        tabWidget_->setTabIcon(page_, QIcon(":/tabs/icons/tabs_modulelayer_disabled.svg"));

    updateModuleList();

    dissolveWindow_->setModified();
}

void LayerTab::on_FrequencySpin_valueChanged(int value)
{
    if (refreshLock_.isLocked() || (!moduleLayer_))
        return;

    moduleLayer_->setFrequency(value);

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

    // See if our stack already contains a control widget for the module - if not, create one
    auto *mcw = getControlWidget(module, true);
    if (!mcw)
    {
        // Create a new widget to display this Module
        mcw = new ModuleControlWidget;
        mcw->setModule(module, &dissolveWindow_->dissolve());
        connect(mcw, SIGNAL(dataModified()), dissolveWindow_, SLOT(setModified()));
        connect(mcw, SIGNAL(statusChanged()), this, SLOT(updateModuleList()));
        ui_.ModuleControlsStack->setCurrentIndex(ui_.ModuleControlsStack->addWidget(mcw));

        if (dissolveWindow_->dissolveIterating())
            mcw->disableSensitiveControls();
    }
    else
        mcw->updateControls();
}

void LayerTab::layerDataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)
{
    dissolveWindow_->setModified();
}

void LayerTab::moduleNameChanged(const QModelIndex &index, const QString &oldName, const QString &newName)
{
    auto *module = moduleLayerModel_.data(index, Qt::UserRole).value<Module *>();
    assert(module);

    // Find the control widget for the module and update it
    auto *mcw = getControlWidget(module);
    if (mcw)
        mcw->updateControls();

    // Rename processing module data
    dissolve_.processingModuleData().renamePrefix(oldName.toStdString(), newName.toStdString());
}

// Update the module list
void LayerTab::updateModuleList()
{
    // Refresh the module list
    std::optional<QModelIndex> selectedIndex;
    if (!ui_.ModulesList->selectionModel()->selection().indexes().empty())
        selectedIndex = ui_.ModulesList->selectionModel()->selection().indexes().front();
    moduleLayerModel_.reset();
    if (selectedIndex)
        ui_.ModulesList->selectionModel()->select(selectedIndex.value(), QItemSelectionModel::ClearAndSelect);
}

void LayerTab::on_ModulesList_customContextMenuRequested(const QPoint &pos)
{
    auto index = ui_.ModulesList->indexAt(pos);
    if (!index.isValid())
        return;
    auto module = moduleLayerModel_.data(index, Qt::UserRole).value<Module *>();
    assert(module);

    QMenu menu;
    menu.setFont(font());

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
    deleteModule->setIcon(QIcon(":/general/icons/general_cross.svg"));

    auto *action = menu.exec(ui_.ModulesList->mapToGlobal(pos));
    if (action == enableModule)
        module->setEnabled(true);
    else if (action == disableModule)
        module->setEnabled(false);
    else if (action == enableOnlyModule)
        for (auto &m : moduleLayer_->modules())
            m->setEnabled(m.get() == module);
    else if (action == clearData)
        dissolve_.processingModuleData().removeWithPrefix(module->uniqueName());
    else if (action == deleteModule)
    {
        // Remove the module's data, then the module
        dissolve_.processingModuleData().removeWithPrefix(module->uniqueName());
        moduleLayerModel_.removeRows(index.row(), 1, QModelIndex());
    }

    // Update required objects
    if (action == enableModule || action == disableModule || action == enableOnlyModule || action == deleteModule)
    {
        updateModuleList();
        dissolveWindow_->setModified();
    }
    updateControls();
}

void LayerTab::on_AvailableModulesTree_doubleClicked(const QModelIndex &index)
{
    moduleLayerModel_.appendNew(modulePaletteModel_.data(index, Qt::DisplayRole).toString());
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

    ui_.EnabledButton->setChecked(moduleLayer_->isEnabled());
    ui_.FrequencySpin->setValue(moduleLayer_->frequency());

    auto *mcw = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->currentWidget());
    if (mcw)
        mcw->updateControls();
}

// Disable sensitive controls within tab
void LayerTab::disableSensitiveControls()
{
    ui_.EnabledButton->setEnabled(false);
    ui_.FrequencySpin->setEnabled(false);
    ui_.ModulesList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_.ModulesList->setDragDropMode(QAbstractItemView::NoDragDrop);
    ui_.AvailableModulesTree->setEnabled(false);
    for (auto n = 0; n < ui_.ModuleControlsStack->count(); ++n)
    {
        auto *mcw = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->widget(n));
        if (mcw)
            mcw->disableSensitiveControls();
    }
}

// Enable sensitive controls within tab
void LayerTab::enableSensitiveControls()
{
    ui_.EnabledButton->setEnabled(true);
    ui_.FrequencySpin->setEnabled(true);
    ui_.AvailableModulesTree->setEnabled(true);
    ui_.ModulesList->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    ui_.ModulesList->setDragDropMode(QAbstractItemView::DragDrop);
    for (auto n = 0; n < ui_.ModuleControlsStack->count(); ++n)
    {
        auto *mcw = dynamic_cast<ModuleControlWidget *>(ui_.ModuleControlsStack->widget(n));
        if (mcw)
            mcw->enableSensitiveControls();
    }
}
