// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineparser.h"
#include "gui/gui.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/producers.h"
#include "gui/modulecontrolwidget.h"
#include "keywords/procedure.h"
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

    // Set the icon label
    ui_.ModuleIconLabel->setPixmap(
        QPixmap(QString(":/modules/icons/modules_%1.svg").arg(QString::fromStdString(std::string(module_->type())).toLower())));

    // Set up any target keyword widgets
    auto targets = module_->keywords().targetKeywords();
    if (!targets.empty())
    {
        ui_.NoTargetsLabel->setVisible(false);
        for (auto *keyword : targets)
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

    // Treat any ProcedureKeywords as special cases
    auto procedures = module_->keywords().allOfType<ProcedureKeyword>();
    if (procedures.empty())
        ui_.ProcedureWidgetButton->setVisible(false);
    else
    {
        procedureWidget_ = new ProcedureWidget();
        procedureWidget_->setUp(dissolveWindow, procedures.front()->data());
        ui_.ModuleControlsStack->addWidget(procedureWidget_);
        procedureWidgetStackIndex_ = ui_.ModuleControlsStack->count() - 1;
    }

    // Create any additional controls offered by the Module
    moduleWidget_ = ModuleWidgetProducer::create(module_, dissolve_);
    if (moduleWidget_ == nullptr)
    {
        Messenger::printVerbose("Module '{}' did not provide a valid controller widget.\n", module->type());
        ui_.ModuleWidgetButton->setVisible(false);
    }
    else
    {
        ui_.ModuleControlsStack->addWidget(moduleWidget_);
        moduleWidgetStackIndex_ = ui_.ModuleControlsStack->count() - 1;
        moduleWidget_->updateControls(ModuleWidget::RecreateRenderablesFlag);
    }

    updateControls();
}

// Return target Module for the widget
Module *ModuleControlWidget::module() { return module_; }

/*
 * Update
 */

// Update controls within widget
void ModuleControlWidget::updateControls(Flags<ModuleWidget::UpdateFlags> updateFlags)
{
    Locker refreshLocker(refreshLock_);

    // Ensure module name and icon status are up to date
    ui_.ModuleNameLabel->setText(QString("%1 (%2)").arg(QString::fromStdString(std::string(module_->name())),
                                                        QString::fromStdString(std::string(module_->type()))));
    ui_.ModuleIconLabel->setEnabled(module_->isEnabled());

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
    ui_.TargetsGroup->setEnabled(false);
    ui_.ModuleKeywordsWidget->setEnabled(false);
    if (moduleWidget_)
        moduleWidget_->preventEditing();
}

// Allow editing
void ModuleControlWidget::allowEditing()
{
    ui_.TargetsGroup->setEnabled(true);
    ui_.ModuleKeywordsWidget->setEnabled(true);
    if (moduleWidget_)
        moduleWidget_->allowEditing();
}

/*
 * UI
 */

void ModuleControlWidget::on_ModuleControlsButton_clicked(bool checked)
{
    if (checked)
        ui_.ModuleControlsStack->setCurrentIndex(0);
}

void ModuleControlWidget::on_ModuleWidgetButton_clicked(bool checked)
{
    if (checked)
        ui_.ModuleControlsStack->setCurrentIndex(moduleWidgetStackIndex_);
}

void ModuleControlWidget::on_ProcedureWidgetButton_clicked(bool checked)
{
    if (checked)
        ui_.ModuleControlsStack->setCurrentIndex(procedureWidgetStackIndex_);
}

// Prepare widget for deletion
void ModuleControlWidget::prepareForDeletion()
{
    // Nullify the module - this will flag to the update functions that they shouldn't proceed
    module_ = nullptr;

    deleteLater();
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
    Flags<ModuleWidget::UpdateFlags> widgetUpdateFlags;

    // Handle specific flags for the module widget
    if (keywordSignals.isSet(KeywordBase::KeywordSignal::RecreateRenderables))
    {
        widgetUpdateFlags.setFlag(ModuleWidget::RecreateRenderablesFlag);
        keywordSignals -= KeywordBase::KeywordSignal::RecreateRenderables;
    }

    // Clear module data?
    if (keywordSignals.isSet(KeywordBase::KeywordSignal::ClearModuleData))
    {
        dissolve_.processingModuleData().removeWithPrefix(module_->name());
        keywordSignals -= KeywordBase::KeywordSignal::ClearModuleData;
    }

    // Call the module's setUp() function if any other flags are still set
    if (keywordSignals.anySet())
        module_->setUp(dissolve_, dissolve_.worldPool(), keywordSignals);

    // Update the module widget
    if (moduleWidget_)
        moduleWidget_->updateControls(widgetUpdateFlags);
}

// Global data mutated
void ModuleControlWidget::globalDataMutated(int mutationFlags)
{
    // If we have no valid module, don't try to update keyword data
    if (!module_)
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
