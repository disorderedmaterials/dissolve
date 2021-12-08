// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "gui/gui.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/modulecontrolwidget.h"
#include "main/dissolve.h"
#include "module/module.h"
#include "modules/widget.h"
#include "modules/widgetproducer.h"

ModuleControlWidget::ModuleControlWidget(QWidget *parent)
{
    // Set up user interface
    ui_.setupUi(this);

    dissolve_ = nullptr;
    module_ = nullptr;
    moduleWidget_ = nullptr;

    // Set event filtering so that we do not blindly accept mouse wheel events in the frequency spin (problematic since we
    // will exist in a QScrollArea)
    ui_.FrequencySpin->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.FrequencySpin));
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
        throw(std::runtime_error("Can't create a ModuleControlWidget without both Module and Dissolve pointers.\n"));

    // Set the icon label
    ui_.ModuleIconLabel->setPixmap(
        QPixmap(QString(":/modules/icons/modules_%1.svg").arg(QString::fromStdString(std::string(module_->type())).toLower())));

    // Set up any target keyword widgets
    if (!module_->keywords().targetsGroup().empty())
    {
        ui_.NoTargetsLabel->setVisible(false);
        for (auto *keyword : module_->keywords().targetsGroup())
        {
            // Create the label
            auto *nameLabel = new QLabel(QString::fromStdString(std::string(keyword->name())));
            nameLabel->setToolTip(QString::fromStdString(std::string(keyword->description())));
            nameLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

            ui_.TargetsLayout->addWidget(nameLabel);
        }
        ui_.TargetsLayout->addStretch(2);
    }

    // Create any additional controls offered by the Module
    moduleWidget_ = ModuleWidgetProducer::create(module_, *dissolve_);
    if (moduleWidget_ == nullptr)
        Messenger::printVerbose("Module '{}' did not provide a valid controller widget.\n", module->type());
    else
    {
        ui_.ModuleControlsStack->addWidget(moduleWidget_);
        ui_.ModuleOutputButton->setEnabled(true);
        moduleWidget_->updateControls(ModuleWidget::UpdateType::RecreateRenderables);
    }

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

    if (moduleWidget_)
        moduleWidget_->updateControls(ModuleWidget::UpdateType::Normal);
}

/*
 * Update
 */

// Update controls within widget
void ModuleControlWidget::updateControls()
{
    if ((!module_) || (!dissolve_))
        return;

    Locker refreshLocker(refreshLock_);

    // Ensure module name is up to date
    ui_.ModuleNameLabel->setText(QString("%1 (%2)").arg(QString::fromStdString(std::string(module_->uniqueName())),
                                                        QString::fromStdString(std::string(module_->type()))));

    // Set 'enabled' button status
    ui_.EnabledButton->setChecked(module_->isEnabled());
    ui_.ModuleIconLabel->setEnabled(module_->isEnabled());

    // Set frequency spin
    ui_.FrequencySpin->setValue(module_->frequency());

    // Update additional controls (if they exist)
    if (moduleWidget_)
        moduleWidget_->updateControls(ModuleWidget::UpdateType::Normal);
}

// Disable sensitive controls
void ModuleControlWidget::disableSensitiveControls()
{
    if (moduleWidget_)
        moduleWidget_->disableSensitiveControls();
}

// Enable sensitive controls
void ModuleControlWidget::enableSensitiveControls()
{
    if (moduleWidget_)
        moduleWidget_->enableSensitiveControls();
}

/*
 * UI
 */

void ModuleControlWidget::on_ModuleControlsButton_clicked(bool checked)
{
    if (checked)
        ui_.ModuleControlsStack->setCurrentIndex(0);
}

void ModuleControlWidget::on_ModuleOutputButton_clicked(bool checked)
{
    if (checked)
        ui_.ModuleControlsStack->setCurrentIndex(1);
}

void ModuleControlWidget::on_EnabledButton_clicked(bool checked)
{
    if (refreshLock_.isLocked())
        return;

    module_->setEnabled(checked);

    ui_.ModuleIconLabel->setEnabled(checked);

    emit(statusChanged());
    emit(dataModified());
}

void ModuleControlWidget::on_FrequencySpin_valueChanged(int value)
{
    if (refreshLock_.isLocked())
        return;

    module_->setFrequency(value);

    emit(dataModified());
}

// Keyword data for Module has been modified
void ModuleControlWidget::keywordDataModified() { emit(dataModified()); }

// Target keyword data changed
void ModuleControlWidget::targetKeywordDataChanged(int flags)
{
    if (refreshLock_.isLocked())
        return;

    // Always emit the 'dataModified' signal
    emit(dataModified());

    // Set-up of encompassing class required?
    if (flags & KeywordBase::ModificationRequiresSetUpOption)
        setUpModule();
}
