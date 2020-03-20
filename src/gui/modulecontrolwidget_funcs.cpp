/*
    *** Module Control Widget - Functions
    *** src/gui/modulecontrolwidget_funcs.cpp
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

#include "base/lineparser.h"
#include "classes/configuration.h"
#include "gui/charts/moduleblock.h"
#include "gui/gui.h"
#include "gui/keywordwidgets/configurationreflist.h"
#include "gui/modulecontrolwidget.h"
#include "gui/modulewidget.h"
#include "keywordwidgets/configurationreflist.h"
#include "main/dissolve.h"
#include "module/module.h"
#include <QGridLayout>
#include <QLabel>

ModuleControlWidget::ModuleControlWidget(QWidget *parent)
{
    // Set up user interface
    ui_.setupUi(this);

    dissolve_ = nullptr;
    module_ = nullptr;
    configurationsWidget_ = nullptr;
}

ModuleControlWidget::~ModuleControlWidget() {}

/*
 * SetUp
 */

// Set up links to main window
void ModuleControlWidget::setUp(DissolveWindow *dissolveWindow)
{
    connect(ui_.ModuleKeywordsWidget, SIGNAL(dataModified()), this, SLOT(keywordDataModified()));
    connect(ui_.ModuleKeywordsWidget, SIGNAL(setUpRequired()), this, SLOT(setUpModule()));
}

/*
 * Module Target
 */

// Set target Module to display
void ModuleControlWidget::setModule(Module *module, Dissolve *dissolve)
{
    module_ = module;
    dissolve_ = dissolve;
    if ((!module_) || (!dissolve_))
    {
        ui_.ModuleKeywordsWidget->setUp(KeywordList(), CoreData());
        return;
    }

    // Set the icon
    ui_.ModuleIconLabel->setPixmap(ModuleBlock::modulePixmap(module_));

    // Create a suitable keyword widget for our Configuration targets
    auto *cfgKeyword = module_->keywords().find("Configuration");
    if (cfgKeyword)
    {
        configurationsWidget_ = new ConfigurationRefListKeywordWidget(NULL, cfgKeyword, dissolve_->coreData());
        connect(configurationsWidget_, SIGNAL(keywordValueChanged(int)), this, SLOT(configurationKeywordEdited(int)));
        auto *layout = new QHBoxLayout;
        layout->setMargin(0);
        layout->addWidget(configurationsWidget_);
        ui_.ConfigurationsPlaceholderWidget->setLayout(layout);
        if (module_->nRequiredTargets() == Module::ZeroTargets)
            configurationsWidget_->setEnabled(false);
    }

    // Set up our keywords widget
    ui_.ModuleKeywordsWidget->setUp(module_->keywords(), dissolve_->constCoreData());

    updateControls();
}

// Return target Module for the widget
Module *ModuleControlWidget::module() const { return module_; }

// Run the set-up stage of the associated Module
void ModuleControlWidget::setUpModule()
{
    if ((!module_) || (!dissolve_))
        return;

    // Run the Module's set-up stage
    module_->setUp(*dissolve_, dissolve_->worldPool());

    emit(updateModuleWidget(ModuleWidget::ResetGraphDataTargetsFlag));
}

/*
 * Update
 */

// Update basic Module controls
void ModuleControlWidget::updateBasicControls()
{
    Lock refreshLocker(refreshLock_);

    // Set unique name and status of icon frame
    ui_.ModuleNameLabel->setText(QString::fromStdString(std::string(module_->uniqueName())));
    ui_.ModuleIconLabel->setEnabled(module_->isEnabled());

    // Update configurations info
    ui_.ConfigurationsLabel->setText(QString::number(module_->nTargetConfigurations()));
    if (configurationsWidget_)
        configurationsWidget_->updateValue();
}

// Update controls within widget
void ModuleControlWidget::updateControls()
{
    if ((!module_) || (!dissolve_))
        return;

    Lock refreshLocker(refreshLock_);

    updateBasicControls();

    // Update keywords
    ui_.ModuleKeywordsWidget->updateControls();
}

// Disable sensitive controls
void ModuleControlWidget::disableSensitiveControls()
{
    ui_.ModuleKeywordsWidget->setEnabled(false);
    ui_.ConfigurationsPlaceholderWidget->setEnabled(false);
}

// Enable sensitive controls
void ModuleControlWidget::enableSensitiveControls()
{
    ui_.ModuleKeywordsWidget->setEnabled(true);
    if (module_ && (module_->nRequiredTargets() != Module::ZeroTargets))
        ui_.ConfigurationsPlaceholderWidget->setEnabled(true);
}

/*
 * UI
 */

// Configuration targets for the Module have been modified
void ModuleControlWidget::configurationKeywordEdited(int flags) { updateBasicControls(); }

// Keyword data for Module has been modified
void ModuleControlWidget::keywordDataModified() { emit(dataModified()); }
