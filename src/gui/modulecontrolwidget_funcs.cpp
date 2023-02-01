// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineparser.h"
#include "gui/gui.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/producers.h"
#include "gui/keywordwidgets/widget.hui"
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
    connect(dissolveWindow, SIGNAL(dataMutated(int)), this, SLOT(globalDataMutated(int)));

    // Set the icon label
    ui_.ModuleIconLabel->setPixmap(
        QPixmap(QString(":/modules/icons/modules_%1.svg").arg(QString::fromStdString(std::string(module_->type())).toLower())));

    // Set up keyword widgets, one group per stack page
    auto &&[keywordIndex, keywordMap] = module_->keywords().keywordOrganisation();
    std::string_view currentGroupName = "";
    QPushButton *firstButton = nullptr;
    for (auto &[groupName, sectionName] : keywordIndex)
    {
        if (currentGroupName == groupName)
            continue;

        // Create a button for the group
        auto &&[b, alignRight] = KeywordsWidget::buttonForGroup(groupName);
        b->setCheckable(true);
        b->setAutoExclusive(true);
        connect(b, SIGNAL(clicked(bool)), this, SLOT(keywordGroupButtonClicked(bool)));
        if (alignRight)
            ui_.KeywordGroupRightButtonsLayout->insertWidget(0, b);
        else
            ui_.KeywordGroupButtonsLayout->insertWidget(-1, b);

        // Add a new KeywordsWidget, set it up, and add it to the stack
        auto *w = new KeywordsWidget();
        w->setUp(groupName, keywordMap, dissolve_.coreData());
        connect(w, SIGNAL(keywordChanged(int)), this, SLOT(localKeywordChanged(int)));
        keywordWidgets_.push_back(w);
        ui_.ModuleControlStack->addWidget(w);

        // Map our button to the new stack page
        controlStackMap_[b] = ui_.ModuleControlStack->count() - 1;

        currentGroupName = groupName;
        if (!firstButton)
            firstButton = b;
    }

    // If we added at least one button, check it now
    if (firstButton)
        firstButton->setChecked(true);

    // Treat any ProcedureKeywords as special cases
    auto procedures = module_->keywords().allOfType<ProcedureKeyword>();
    if (procedures.empty())
        ui_.ProcedureWidgetButton->setVisible(false);
    else
    {
        procedureWidget_ = new ProcedureWidget();
        procedureWidget_->setUp(dissolveWindow, procedures.front()->data());
        ui_.ModuleControlStack->addWidget(procedureWidget_);
        controlStackMap_[ui_.ProcedureWidgetButton] = ui_.ModuleControlStack->count() - 1;

        if (keywordWidgets_.empty())
            ui_.ProcedureWidgetButton->setChecked(true);
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
        ui_.ModuleControlStack->addWidget(moduleWidget_);
        controlStackMap_[ui_.ModuleWidgetButton] = ui_.ModuleControlStack->count() - 1;
        moduleWidget_->updateControls(ModuleWidget::RecreateRenderablesFlag);

        if (keywordWidgets_.empty() && !ui_.ProcedureWidgetButton->isVisible())
            ui_.ModuleWidgetButton->setChecked(true);
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

    // Update keywords
    for (auto &kw : keywordWidgets_)
        kw->updateControls();

    // Update additional controls (if they exist)
    if (moduleWidget_)
        moduleWidget_->updateControls(updateFlags);
}

// Disable editing
void ModuleControlWidget::preventEditing()
{
    for (auto &kw : keywordWidgets_)
        kw->setEnabled(false);
    if (moduleWidget_)
        moduleWidget_->preventEditing();
}

// Allow editing
void ModuleControlWidget::allowEditing()
{
    for (auto &kw : keywordWidgets_)
        kw->setEnabled(true);
    if (moduleWidget_)
        moduleWidget_->allowEditing();
}

/*
 * UI
 */

void ModuleControlWidget::switchControlStackPage(QPushButton *button)
{
    auto it = controlStackMap_.find(button);
    if (it != controlStackMap_.end())
        ui_.ModuleControlStack->setCurrentIndex(it->second);
}

void ModuleControlWidget::keywordGroupButtonClicked(bool checked)
{
    if (checked)
        switchControlStackPage(qobject_cast<QPushButton *>(sender()));
}

void ModuleControlWidget::on_ModuleWidgetButton_clicked(bool checked)
{
    if (checked)
        switchControlStackPage(ui_.ModuleWidgetButton);
}

void ModuleControlWidget::on_ProcedureWidgetButton_clicked(bool checked)
{
    if (checked)
        switchControlStackPage(ui_.ProcedureWidgetButton);
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

    // Module keywords
    for (auto &kw : keywordWidgets_)
        kw->updateControls(mutationFlags);
}
