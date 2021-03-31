// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/pairbroadeningfunction.h"
#include <QComboBox>
#include <QHBoxLayout>

PairBroadeningFunctionKeywordWidget::PairBroadeningFunctionKeywordWidget(QWidget *parent, KeywordBase *keyword,
                                                                         const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Set deltas on spinboxes
    ui_.GaussianFWHMSpin->setSingleStep(0.01);

    // Connect signals / slots
    connect(ui_.NoneRadio, SIGNAL(toggled(bool)), this, SLOT(functionRadioChanged(bool)));
    connect(ui_.GaussianRadio, SIGNAL(toggled(bool)), this, SLOT(functionRadioChanged(bool)));
    connect(ui_.GaussianFWHMSpin, SIGNAL(valueChanged(double)), this, SLOT(functionParameterChanged(double)));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<PairBroadeningFunctionKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into PairBroadeningFunctionKeyword.\n", keyword->name());
    else
    {
        // Set current information
        updateWidgetValues(coreData_);
    }
}

/*
 * Widgets
 */

// Function type radio changed
void PairBroadeningFunctionKeywordWidget::functionRadioChanged(bool checked)
{
    if (refreshing_)
        return;

    updateKeywordData();

    updateWidgetValues(coreData_);

    emit(keywordValueChanged(keyword_->optionMask()));
}

// Parameter value changed
void PairBroadeningFunctionKeywordWidget::functionParameterChanged(double value)
{
    if (refreshing_)
        return;

    updateKeywordData();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void PairBroadeningFunctionKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void PairBroadeningFunctionKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Grab the target PairBroadeningFunction
    auto &pairBroadeningFunction = keyword_->data();

    // Radio button
    switch (pairBroadeningFunction.function())
    {
        case (PairBroadeningFunction::NoFunction):
            ui_.NoneRadio->setChecked(true);
            break;
        case (PairBroadeningFunction::GaussianFunction):
            ui_.GaussianRadio->setChecked(true);
            break;
        default:
            break;
    }

    // Parameters
    ui_.GaussianFWHMSpin->setValue(pairBroadeningFunction.gaussianFWHM());

    // Set summary text
    setSummaryText(
        QString::fromStdString(std::string(PairBroadeningFunction::functionType(pairBroadeningFunction.function()))));

    refreshing_ = false;
}

// Update keyword data based on widget values
void PairBroadeningFunctionKeywordWidget::updateKeywordData()
{
    // Get widget data
    PairBroadeningFunction pairBroadeningFunction;

    // Function type
    if (ui_.NoneRadio->isChecked())
        pairBroadeningFunction.setFunction(PairBroadeningFunction::NoFunction);
    else if (ui_.GaussianRadio->isChecked())
        pairBroadeningFunction.setFunction(PairBroadeningFunction::GaussianFunction);

    // Parameters
    pairBroadeningFunction.setGaussianFWHM(ui_.GaussianFWHMSpin->value());

    keyword_->setData(pairBroadeningFunction);
}
