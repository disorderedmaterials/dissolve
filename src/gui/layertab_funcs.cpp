// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "gui/getmodulelayernamedialog.h"
#include "gui/gui.h"
#include "gui/layertab.h"
#include "gui/maintabswidget.hui"
#include "main/dissolve.h"
#include <QMessageBox>

LayerTab::LayerTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title,
                   ModuleLayer *layer)
    : ListItem<LayerTab>(), MainTab(dissolveWindow, dissolve, parent, QString("Layer: %1").arg(title), this)
{
    ui_.setupUi(this);

    Locker refreshLocker(refreshLock_);

    moduleLayer_ = layer;

    // Set up ModuleEditor
    ui_.ModuleListPanel->setUp(dissolveWindow, moduleLayer_);
}

LayerTab::~LayerTab()
{
    // Remove the Configuration represented in this tab
    dissolve_.removeProcessingLayer(moduleLayer_);
}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType LayerTab::type() const { return MainTab::LayerTabType; }

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
 * ModuleLayer Target
 */

// Return displayed ModuleLayer
ModuleLayer *LayerTab::moduleLayer() const { return moduleLayer_; }

/*
 * Widgets
 */

void LayerTab::on_ShowPaletteButton_clicked(bool checked)
{
    // Show / hide the module palette
    ui_.ModuleListPanel->setModulePaletteVisible(checked);

    // Set correct text on our button
    ui_.ShowPaletteButton->setText(checked ? "Hide Palette" : "Show Palette");
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

    dissolveWindow_->setModified();
}

void LayerTab::on_FrequencySpin_valueChanged(int value)
{
    if (refreshLock_.isLocked() || (!moduleLayer_))
        return;

    moduleLayer_->setFrequency(value);

    dissolveWindow_->setModified();
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

    ui_.EnabledButton->setChecked(moduleLayer_->enabled());
    ui_.FrequencySpin->setValue(moduleLayer_->frequency());

    ui_.ModuleListPanel->updateControls();
}

// Disable sensitive controls within tab
void LayerTab::disableSensitiveControls()
{
    ui_.EnabledButton->setEnabled(false);
    ui_.FrequencySpin->setEnabled(false);
    ui_.ModuleListPanel->disableSensitiveControls();
}

// Enable sensitive controls within tab
void LayerTab::enableSensitiveControls()
{
    ui_.EnabledButton->setEnabled(true);
    ui_.FrequencySpin->setEnabled(true);
    ui_.ModuleListPanel->enableSensitiveControls();
}
