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
#include "gui/modulecontrolwidget.h"
#include "gui/modulewidget.h"
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

    refreshing_ = false;
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

// Update controls within widget
void ModuleControlWidget::updateControls()
{
    if ((!module_) || (!dissolve_))
        return;

    refreshing_ = true;

    // Set unique name and status of icon frame
    ui_.ModuleNameLabel->setText(QString::fromStdString(std::string(module_->uniqueName())));
    ui_.ModuleIconLabel->setEnabled(module_->isEnabled());

    // Update Configuration list and HeaderFrame tooltip
    /*	ui_.ConfigurationTargetList->clear();
        CharString toolTip("Targets: ");
        ListIterator<Configuration> configIterator(dissolve_->constConfigurations());
        while (Configuration *cfg = configIterator.iterate())
        {
            QListWidgetItem *item = new QListWidgetItem(cfg->name(), ui_.ConfigurationTargetList);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setData(Qt::UserRole, VariantPointer<Configuration>(cfg));

            if (module_->isTargetConfiguration(cfg))
            {
                item->setCheckState(Qt::Checked);

                if (configIterator.isFirst())
                    toolTip.strcatf("%s", cfg->name());
                else
                    toolTip.strcatf(", %s", cfg->name());
            }
            else
                item->setCheckState(Qt::Unchecked);
        }
        ui_.ConfigurationTargetGroup->setVisible((!module_->configurationLocal()) && (module_->nRequiredTargets() !=
       Module::ZeroTargets)); ui_.HeaderFrame->setToolTip(toolTip.get()); */
    printf("FIX THE SELETION OF CONFIGURATIONS WITH A KEYWORD!\n");

    // Update keywords
    ui_.ModuleKeywordsWidget->updateControls();

    refreshing_ = false;
}

// Disable sensitive controls
void ModuleControlWidget::disableSensitiveControls()
{
    ui_.ModuleKeywordsWidget->setEnabled(false);
    // 	ui_.ConfigurationTargetGroup->setEnabled(false);
    printf("FIX THE DISABLING OF CONFIGURATION CONTROL\n");
}

// Enable sensitive controls
void ModuleControlWidget::enableSensitiveControls()
{
    ui_.ModuleKeywordsWidget->setEnabled(true);
    // 	ui_.ConfigurationTargetGroup->setEnabled(true);
    printf("FIX THE ENABLING OF CONFIGURATION CONTROL\n");
}

/*
 * UI
 */

// Keyword data for Module has been modified
void ModuleControlWidget::keywordDataModified() { emit(dataModified()); }
