// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/lineparser.h"
#include "gui/gui.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/producers.h"
#include "gui/modulecontrolwidget.h"
#include "module/module.h"
#include "modules/widget.h"
#include "modules/widgetproducer.h"

ModuleControlWidget::ModuleControlWidget(DissolveWindow *dissolveWindow, Module *module) : dissolve_(dissolveWindow->dissolve())
{
    // Set up user interface
    ui_.setupUi(this);

    module_ = module;
    moduleWidget_ = nullptr;
    assert(module_);

    // Connect signals
    connect(ui_.ModuleKeywordsWidget, SIGNAL(keywordChanged(int)), this, SLOT(localKeywordChanged(int)));
    connect(dissolveWindow, SIGNAL(dataMutated(int)), this, SLOT(globalDataMutated(int)));

    // Set event filtering so that we do not blindly accept mouse wheel events in the frequency spin (problematic since we
    // will exist in a QScrollArea)
    ui_.FrequencySpin->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.FrequencySpin));

    // Set the icon label
    ui_.ModuleIconLabel->setPixmap(
        QPixmap(QString(":/modules/icons/modules_%1.svg").arg(QString::fromStdString(std::string(module_->type())).toLower())));

    // Set up any target keyword widgets
    if (!module_->keywords().targetsGroup().empty())
    {
        ui_.NoTargetsLabel->setVisible(false);
        for (auto *keyword : module_->keywords().targetsGroup())
        {
            // Try to create a suitable widget
            auto [widget, base] = KeywordWidgetProducer::create(keyword, dissolve_.coreData());
            if (!widget || !base)
                throw(std::runtime_error(fmt::format("No widget created for keyword '{}'.\n", keyword->name())));

            // Connect it up
            connect(widget, SIGNAL(keywordDataChanged(int)), this, SLOT(localKeywordChanged(int)));

            // Create the label
            auto *nameLabel = new QLabel(QString::fromStdString(std::string(keyword->name())));
            nameLabel->setToolTip(QString::fromStdString(std::string(keyword->description())));
            nameLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

            ui_.TargetsLayout->addWidget(nameLabel);
            ui_.TargetsLayout->addWidget(widget);

            targetKeywordWidgets_.push_back(base);
        }
        ui_.TargetsLayout->addStretch(2);
    }

    // Set up our keyword widget
    ui_.ModuleKeywordsWidget->setUp(module_->keywords(), dissolve_.coreData());

    // Create any additional controls offered by the Module
    moduleWidget_ = ModuleWidgetProducer::create(module_, dissolve_);
    if (moduleWidget_ == nullptr)
        Messenger::printVerbose("Module '{}' did not provide a valid controller widget.\n", module->type());
    else
    {
        ui_.ModuleControlsStack->addWidget(moduleWidget_);
        ui_.ModuleOutputButton->setEnabled(true);
        moduleWidget_->updateControls(ModuleWidget::RecreateRenderablesFlag);
    }

    updateControls();
}

// Return target Module for the widget
Module *ModuleControlWidget::module() const { return module_; }

/*
 * Update
 */

// Update controls within widget
void ModuleControlWidget::updateControls(Flags<ModuleWidget::UpdateFlags> updateFlags)
{
    Locker refreshLocker(refreshLock_);

    // Ensure module name is up to date
    ui_.ModuleNameLabel->setText(QString("%1 (%2)").arg(QString::fromStdString(std::string(module_->uniqueName())),
                                                        QString::fromStdString(std::string(module_->type()))));

    // Set 'enabled' button status
    ui_.EnabledButton->setChecked(module_->isEnabled());
    ui_.ModuleIconLabel->setEnabled(module_->isEnabled());

    // Set frequency spin
    ui_.FrequencySpin->setValue(module_->frequency());

    // Update tqrget keywords
    for (auto w : targetKeywordWidgets_)
        w->updateValue();

    // Update keywords
    ui_.ModuleKeywordsWidget->updateControls();

    // Update additional controls (if they exist)
    if (moduleWidget_)
        moduleWidget_->updateControls(updateFlags);
}

// Disable editing
void ModuleControlWidget::preventEditing()
{
    ui_.FrequencySpin->setEnabled(false);
    ui_.EnabledButton->setEnabled(false);
    ui_.TargetsGroup->setEnabled(false);
    ui_.ModuleKeywordsWidget->setEnabled(false);
    if (moduleWidget_)
        moduleWidget_->preventEditing();
}

// Enable editing
void ModuleControlWidget::enableEditing()
{
    ui_.FrequencySpin->setEnabled(true);
    ui_.EnabledButton->setEnabled(true);
    ui_.TargetsGroup->setEnabled(true);
    ui_.ModuleKeywordsWidget->setEnabled(true);
    if (moduleWidget_)
        moduleWidget_->enableEditing();
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

// Target keyword data changed
void ModuleControlWidget::localKeywordChanged(int signalMask)
{
    if (refreshLock_.isLocked())
        return;

    // Always emit the 'dataModified' signal
    emit(dataModified());

    // Determine flags
    Flags<KeywordBase::KeywordSignal> keywordSignals(signalMask);

    // Call the module's setUp() function with it
    if (keywordSignals.anySet())
        module_->setUp(dissolve_, dissolve_.worldPool(), keywordSignals);

    // Handle specific flags for the module widget
    if (moduleWidget_)
        moduleWidget_->updateControls(Flags<ModuleWidget::UpdateFlags>(signalMask));
}

// Global data mutated
void ModuleControlWidget::globalDataMutated(int mutationFlags)
{
    // If we have no valid parent, don't try to update keyword data
    if (!parent())
        return;

    Flags<DissolveSignals::DataMutations> dataMutations(mutationFlags);
    if (!dataMutations.anySet())
        return;

    // Target keywords
    for (auto w : targetKeywordWidgets_)
        w->updateValue(dataMutations);

    // Control keywords
    ui_.ModuleKeywordsWidget->updateControls(mutationFlags);
}
