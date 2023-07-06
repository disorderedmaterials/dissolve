// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/units.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "gui/configurationTab.h"
#include "gui/getConfigurationNameDialog.h"
#include "gui/gui.h"
#include "gui/helpers/comboPopulator.h"
#include "gui/keywordWidgets/producers.h"
#include "main/dissolve.h"
#include <QMessageBox>

ConfigurationTab::ConfigurationTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent,
                                   const QString title, Configuration *cfg)
    : MainTab(dissolveWindow, dissolve, parent, QString("Configuration: %1").arg(title), this),
      procedureModel_(cfg->generator())
{
    ui_.setupUi(this);

    Locker refreshLocker(refreshLock_);

    configuration_ = cfg;

    // Set target for ConfigurationViewer
    ui_.ViewerWidget->setConfiguration(configuration_);

    // Set-up the generator procedure editor
    ui_.GeneratorWidget->setUp(dissolveWindow_, configuration_->generator());

    boxIcon_ = addStatusBarIcon(":/tabs/icons/tabs_configuration.svg", true);
    boxIcon_->installEventFilter(this);
}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType ConfigurationTab::type() const { return MainTab::TabType::Configuration; }

// Raise suitable dialog for entering / checking new tab name
QString ConfigurationTab::getNewTitle(bool &ok)
{
    // Get a new, valid name for the Configuration
    GetConfigurationNameDialog nameDialog(this, dissolve_.coreData());
    ok = nameDialog.get(configuration_, QString::fromStdString(std::string(configuration_->name())));

    if (ok)
    {
        // Rename our Configuration, and flag that our data has been modified
        configuration_->setName(qPrintable(nameDialog.newName()));

        dissolveWindow_->setModified();
    }

    return nameDialog.newName();
}

// Return whether the title of the tab can be changed
bool ConfigurationTab::canChangeTitle() const { return true; }

// Return whether the tab can be closed (after any necessary user querying, etc.)
bool ConfigurationTab::canClose() const
{
    // Check that we really want to delete this tab
    QMessageBox queryBox;
    queryBox.setText(QString("Really delete the configuration '%1'?\nThis cannot be undone!")
                         .arg(QString::fromStdString(std::string(configuration_->name()))));
    queryBox.setInformativeText("Proceed?");
    queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    queryBox.setDefaultButton(QMessageBox::No);
    auto ret = queryBox.exec();

    if (ret != QMessageBox::Yes)
        return false;

    return true;
}

/*
 * Statusbar
 */

// Add icon to status bar
QLabel *ConfigurationTab::addStatusBarIcon(QString resource, bool permanent)
{
    const auto iconSize = ui_.statusBar->font().pointSize() * 1.75;
    auto *label = new QLabel;
    label->setPixmap(QPixmap(resource));
    label->setMaximumSize(QSize(iconSize, iconSize));
    label->setScaledContents(true);
    if (permanent)
        ui_.statusBar->addPermanentWidget(label);
    else
        ui_.statusBar->addWidget(label);
    return label;
}


/*
 * Configuration Target
 */

// Return displayed Configuration
Configuration *ConfigurationTab::configuration() const { return configuration_; }

/*
 * Update
 */

// Update controls in tab
void ConfigurationTab::updateControls()
{
    Locker refreshLocker(refreshLock_);

    // Viewer
    ui_.ViewerWidget->postRedisplay();
}

// Prevent editing within tab
void ConfigurationTab::preventEditing()
{
    ui_.GeneratorWidget->setEnabled(false);
}

// Allow editing within tab
void ConfigurationTab::allowEditing()
{
    ui_.GeneratorWidget->setEnabled(true);
}

/*
 * Signals / Slots
 */

void ConfigurationTab::on_GenerateButton_clicked(bool checked)
{
    auto proceed = true;

    // If the configuration already contains something, check first
    if (configuration_->nAtoms() > 0)
    {
        QMessageBox queryBox;
        queryBox.setText(QString("This will erase the current contents of the Configuration '%1'.")
                             .arg(QString::fromStdString(std::string(configuration_->name()))));
        queryBox.setInformativeText("Proceed?");
        queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        queryBox.setDefaultButton(QMessageBox::No);
        proceed = queryBox.exec() == QMessageBox::Yes;
    }

    if (!proceed)
        return;

    // Clear the messages
    dissolveWindow_->clearMessages();

    // Make sure the potential map is up to date
    dissolve_.regeneratePairPotentials();

    // Initialise the content
    configuration_->initialiseContent({dissolve_.worldPool(), dissolve_.potentialMap()});

    // Update
    updateControls();
    dissolveWindow_->updateStatusBar();
}

/*
 * Event filer
 */

bool ConfigurationTab::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Enter)
    {
        Messenger::print("Enter");
    }
    else if (event->type() == QEvent::Leave)
        Messenger::print("Leave");
    return QObject::eventFilter(obj, event);
}
