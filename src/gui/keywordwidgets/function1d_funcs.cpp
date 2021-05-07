// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/function1d.h"
#include <QComboBox>
#include <QHBoxLayout>

Q_DECLARE_METATYPE(Functions::Function1D);

#define MaxParams 6

Function1DKeywordWidget::Function1DKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    refreshing_ = true;

    // Grab widget pointers for convenience
    spins_ = {ui_.Parameter0Spin, ui_.Parameter1Spin, ui_.Parameter2Spin,
              ui_.Parameter3Spin, ui_.Parameter4Spin, ui_.Parameter5Spin};
    labels_ = {ui_.Parameter0Label, ui_.Parameter1Label, ui_.Parameter2Label,
               ui_.Parameter3Label, ui_.Parameter4Label, ui_.Parameter5Label};

    // Set deltas on spinboxes
    for (auto *spin : spins_)
        spin->setSingleStep(0.01);

    // Connect signals / slots
    connect(ui_.FunctionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(functionCombo_currentIndexChanged(int)));
    for (auto *spin : spins_)
        connect(spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<Function1DKeyword *>(keyword);
    if (!keyword_)
        throw(std::runtime_error(fmt::format("Couldn't cast base keyword '{}' into Function1DKeyword.\n", keyword->name())));

    // Get relevant function types to show in combo
    auto availableFunctions = Functions::matchingFunction1D(keyword_->functionProperties());
    if (availableFunctions.empty())
    {
        ui_.FunctionCombo->addItem("<None Available>");
        ui_.FunctionCombo->setEnabled(false);
    }
    else
    {
        auto i = 0;
        for (auto func : availableFunctions)
        {
            ui_.FunctionCombo->addItem(QString::fromStdString(Functions::function1D().keyword(func)),
                                       QVariant::fromValue(func));
            if (func == keyword_->data().type())
                ui_.FunctionCombo->setCurrentIndex(i);
            ++i;
        }
    }
}

/*
 * Widgets
 */

// Function type combo changed
void Function1DKeywordWidget::functionCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    updateKeywordData();

    updateWidgetValues(coreData_);

    emit(keywordValueChanged(keyword_->optionMask()));
}

// Parameter value changed
void Function1DKeywordWidget::parameterSpin_valueChanged(double value)
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
void Function1DKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void Function1DKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Grab the target Function1D
    auto &function = keyword_->data();

    // Summary text on KeywordDropDown button
    setSummaryText(QString::fromStdString(
        function.nParameters() == 0
            ? Functions::function1D().keyword(function.type())
            : fmt::format("{} ({})", Functions::function1D().keyword(function.type()), function.parameterSummary())));

    // Widgets
    ui_.FunctionCombo->setCurrentIndex(ui_.FunctionCombo->findData(QVariant::fromValue(function.type())));

    const auto nParams = function.nParameters();
    for (auto n = 0; n < MaxParams; ++n)
    {
        spins_[n]->setValue(nParams > n ? function.parameters()[n] : 0.0);
        labels_[n]->setText(nParams > n ? QString::fromStdString(function.parameterName(n)) : "N/A");
        spins_[n]->setVisible(nParams > n);
        labels_[n]->setVisible(nParams > n);
    }

    refreshing_ = false;
}

// Update keyword data based on widget values
void Function1DKeywordWidget::updateKeywordData()
{
    // Grab the target Function1D
    auto &function = keyword_->data();

    // Get current data from widgets
    auto func = ui_.FunctionCombo->currentData().value<Functions::Function1D>();
    std::vector<double> newParams;
    for (auto n = 0; n < Functions::functionDefinition1D(func).nParameters(); ++n)
        newParams.push_back(spins_[n]->value());

    // Set new data
    function.set(func, newParams);
    keyword_->hasBeenSet();
}
